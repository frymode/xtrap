#include "symbol_server.h"

#include <mutex>
#include <dbghelp.h>
#include <boost/format.hpp>

namespace xtrap
{
    namespace
    {
        std::string ExtractFileName(const std::string& path)
        {
            return path;
        }

        SymbolServer* _symbol_server_instance = nullptr;
        std::mutex _symbol_server_mutex;

        std::string FormatFrameInfo(DWORD64 frame, DWORD64 displacement, PSYMBOL_INFO symbol, PIMAGEHLP_LINE64 line)
        {
            std::string str_frame;

            if (line)
            {
                str_frame += boost::str(boost::format("%1%(%2%) : ") % ExtractFileName(line->FileName) % line->LineNumber);
            }

            if (symbol) 
            {
                str_frame += boost::str(boost::format("%1% [%2%+%3%]") % symbol->Name % boost::io::group(std::hex, std::uppercase, frame) % displacement);
            } 
            else 
            {
                str_frame += boost::str(boost::format("(No symbol) [%1%]") % boost::io::group(std::hex, std::uppercase, frame));
            }

            return str_frame;
        }

        std::string FormatFrameInfo(HANDLE process, DWORD64 frame, std::string* symbol_name = nullptr)
        {
            const int kMaxNameLength = 256;
            char buffer[sizeof(SYMBOL_INFO) + kMaxNameLength * sizeof(wchar_t)];
            memset(buffer, 0, sizeof(buffer));

            DWORD64 displacement = 0;
            PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(&buffer[0]);
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            symbol->MaxNameLen = kMaxNameLength - 1;
            BOOL has_symbol = SymFromAddr(process, frame, &displacement, symbol);

            DWORD line_displacement = 0;
            IMAGEHLP_LINE64 line = {};
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            BOOL has_line = SymGetLineFromAddr64(process, frame, &line_displacement, &line);

            if (symbol_name)
            {
                if (has_symbol)
                {
                    symbol_name->assign(symbol->Name);
                }
                else
                {
                    symbol_name->clear();
                }
            }
            return FormatFrameInfo(frame, displacement, has_symbol ? symbol : NULL, has_line ? &line : NULL);
        }
    }

    SymbolServer* SymbolServer::instance()
    {
        if (!_symbol_server_instance)
        {
            std::unique_lock<std::mutex> lock(_symbol_server_mutex);
            if (!_symbol_server_instance)
            {
                _symbol_server_instance = new SymbolServer();
            }
        }
        return _symbol_server_instance;
    }

    DWORD SymbolServer::init_error() const 
    {
        return _init_error;
    }

    SymbolServer::SymbolServer() 
        : _init_error(ERROR_SUCCESS)
    {
        filter_stubs = true;
        _stack_startup_entries.push_back("__tmainCRTStartup");
        _stack_throw_entries.push_back("CxxThrowException");
        _stack_throw_entries.push_back("_CxxThrowException");
        _stack_throw_entries.push_back("RaiseException");

        DWORD options = SymGetOptions();
        options |= SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES;
        SymSetOptions(options);

        if (!SymInitialize(GetCurrentProcess(), NULL, TRUE)) 
        {
            _init_error = GetLastError();
        }
    }

    bool SymbolServer::DumpBacktraceData(Backtrace::BacktraceData* backtrace_data, PCONTEXT context, HANDLE thread /* = 0 */, int skip /* = 0 */)
    {
        if (_init_error != ERROR_SUCCESS)
        {
            return false;
        }

        std::unique_lock<std::mutex> lock(_symbol_server_mutex);

        STACKFRAME64 sf = {};
        sf.AddrPC.Offset = context->Eip;
        sf.AddrPC.Mode = AddrModeFlat;
        sf.AddrStack.Offset = context->Esp;
        sf.AddrStack.Mode = AddrModeFlat;
        sf.AddrFrame.Offset = context->Ebp;
        sf.AddrFrame.Mode = AddrModeFlat;

        HANDLE process = GetCurrentProcess();
        if (!thread)
        {
            thread = GetCurrentThread();
        }

        for (;;)
        {
            SetLastError(0);
            BOOL stack_walk_ok = StackWalk64(IMAGE_FILE_MACHINE_I386, process, thread, &sf, context, 0, &SymFunctionTableAccess64, &SymGetModuleBase64, 0);
            if (!stack_walk_ok || !sf.AddrFrame.Offset)
            {
                break;
            }

            if (skip)
            {
                --skip;
            }
            else
            {
                DWORD64 frame = sf.AddrPC.Offset;
                backtrace_data->push_back((void*)frame);
            }
        }

        return true;
    }

    bool SymbolServer::DumpContext(BacktraceStack* stack, PCONTEXT context, HANDLE thread /* = 0 */, int skip /* = 0 */)
    {
        if (_init_error != ERROR_SUCCESS)
        {
            return false;
        }

        std::unique_lock<std::mutex> lock(_symbol_server_mutex);

        STACKFRAME64 sf = {};
        sf.AddrPC.Offset    = context->Eip;
        sf.AddrPC.Mode      = AddrModeFlat;
        sf.AddrStack.Offset = context->Esp;
        sf.AddrStack.Mode   = AddrModeFlat;
        sf.AddrFrame.Offset = context->Ebp;
        sf.AddrFrame.Mode   = AddrModeFlat;

        HANDLE process = GetCurrentProcess();
        if (!thread)
        {
            thread = GetCurrentThread();
        }

        for (;;) 
        {
            SetLastError(0);
            BOOL stack_walk_ok = StackWalk64(IMAGE_FILE_MACHINE_I386, process, thread, &sf, context, 0, &SymFunctionTableAccess64, &SymGetModuleBase64, 0);
            if (!stack_walk_ok || !sf.AddrFrame.Offset)
            {
                break;
            }

            if (skip) 
            {
                --skip;
            }
            else 
            {
                DWORD64 frame = sf.AddrPC.Offset;

                std::string symbol_name;
                std::string str_frame = FormatFrameInfo(process, frame, &symbol_name);

                if (filter_stubs)
                {
                    if (std::find(_stack_startup_entries.begin(), _stack_startup_entries.end(), symbol_name) != _stack_startup_entries.end())
                    {
                        break;
                    }
                    else if (std::find(_stack_throw_entries.begin(), _stack_throw_entries.end(), symbol_name) != _stack_throw_entries.end())
                    {
                        continue;
                    }
                }

                stack->push_back(str_frame);
            }
        }

        return true;
    }

    BacktraceStack SymbolServer::DumpContext(PCONTEXT context, HANDLE thread /* = 0 */, int skip /* = 0 */)
    {
        BacktraceStack stack;
        DumpContext(&stack, context, thread);
        return stack;
    }

    bool SymbolServer::DumpTrace(BacktraceStack* stack, const void* const* trace,  int count)
    {
        if (_init_error != ERROR_SUCCESS)
        {
            return false;
        }

        std::unique_lock<std::mutex> lock(_symbol_server_mutex);

        HANDLE process = GetCurrentProcess();
        for (int i = 0; i < count; ++i) 
        {
            DWORD_PTR frame = reinterpret_cast<DWORD_PTR>(trace[i]);

            std::string symbol_name;
            std::string str_frame = FormatFrameInfo(process, frame, &symbol_name);

            if (filter_stubs)
            {
                if (std::find(_stack_startup_entries.begin(), _stack_startup_entries.end(), symbol_name) != _stack_startup_entries.end())
                {
                    break;
                }
                else if (std::find(_stack_throw_entries.begin(), _stack_throw_entries.end(), symbol_name) != _stack_throw_entries.end())
                {
                    continue;
                }
            }

            stack->push_back(str_frame);
        }

        return true;
    }

    BacktraceStack SymbolServer::DumpTrace(const void* const* trace,  int count)
    {
        BacktraceStack stack;
        DumpTrace(&stack, trace, count);
        return stack;
    }
}