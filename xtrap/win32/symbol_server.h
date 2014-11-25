#pragma once

#include "backtrace.h"
#include <boost/noncopyable.hpp>

namespace xtrap
{
    class SymbolServer : boost::noncopyable
    {
    public:
        static SymbolServer* instance();

        bool filter_stubs;
        DWORD init_error() const;

        bool DumpBacktraceData(Backtrace::BacktraceData* backtrace_data, PCONTEXT context, HANDLE thread = 0, int skip = 0);

        bool DumpContext(BacktraceStack* stack, PCONTEXT context, HANDLE thread = 0, int skip = 0);
        BacktraceStack DumpContext(PCONTEXT context, HANDLE thread = 0, int skip = 0);

        bool DumpTrace(BacktraceStack* stack, const void* const* trace,  int count);
        BacktraceStack DumpTrace(const void* const* trace,  int count);

    private:
        SymbolServer();
        DWORD _init_error;
        std::vector<std::string> _stack_startup_entries;
        std::vector<std::string> _stack_throw_entries;
    };
}