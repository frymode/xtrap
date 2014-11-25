#include "exception_trap.h"

#include <boost/thread.hpp>
#include "untyped_exception.h"
#include "symbol_server.h"

static const DWORD EXCEPTION_USER = 0xE06D7363U;

static bool _xtrap_exception_stack_trap_registered;
static xtrap::FatalErrorHandler _xtrap_fatal_error_handler;
static boost::thread_specific_ptr<EXCEPTION_POINTERS> _xtrap_stack_trace_pointers;

static LONG CALLBACK StackTraceVectoredHandler(PEXCEPTION_POINTERS ex_pointers)
{
    PEXCEPTION_POINTERS saved_info = _xtrap_stack_trace_pointers.get();
    if (!saved_info)
    {
        saved_info = new EXCEPTION_POINTERS();
        _xtrap_stack_trace_pointers.reset(saved_info);
    }
    *saved_info = *ex_pointers;
    return EXCEPTION_CONTINUE_SEARCH;
}

static LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* ex_pointers)
{
    if (_xtrap_fatal_error_handler)
    {
        xtrap::BacktraceStack stack = xtrap::SymbolServer::instance()->DumpContext(ex_pointers->ContextRecord);
        xtrap::UntypedException error(*ex_pointers->ExceptionRecord);
        _xtrap_fatal_error_handler(error, stack);
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

namespace xtrap
{
    void RegisterExceptionStackTraceTrap()
    {
        if (!_xtrap_exception_stack_trap_registered)
        {
            AddVectoredExceptionHandler(FALSE, StackTraceVectoredHandler);
            _xtrap_exception_stack_trap_registered = true;
        }
    }

    bool IsExceptionStackTrapRegistered()
    {
        return _xtrap_exception_stack_trap_registered;
    }

    void RegisterExceptionTraps(FatalErrorHandler error_handler /* = NULL */)
    {
        _xtrap_fatal_error_handler = error_handler;
        SetUnhandledExceptionFilter(&UnhandledExceptionHandler);
    }

    BacktraceStack GetCurrentExceptionStackTrace()
    {
        PEXCEPTION_POINTERS ex_pointers = GetCurrentExceptionStackTracePointers();
        if (!ex_pointers)
        {
            return BacktraceStack();
        }

        return SymbolServer::instance()->DumpContext(ex_pointers->ContextRecord);
    }

    BacktraceStack GetCurrentExceptionStackTrace(const type_info& exception_type, const void* exception_object /* = nullptr */)
    {
        PEXCEPTION_POINTERS ex_pointers = GetCurrentExceptionStackTracePointers(exception_type, exception_object);
        if (!ex_pointers)
        {
            return BacktraceStack();
        }

        return SymbolServer::instance()->DumpContext(ex_pointers->ContextRecord);
    }

    PEXCEPTION_POINTERS GetCurrentExceptionStackTracePointers()
    {
        return _xtrap_stack_trace_pointers.get();
    }

    PEXCEPTION_POINTERS GetCurrentExceptionStackTracePointers(const type_info& exception_type, const void* exception_object /* = nullptr */)
    {
        PEXCEPTION_POINTERS ex_pointers = GetCurrentExceptionStackTracePointers();
        if (!ex_pointers)
        {
            return nullptr;
        }

        const EXCEPTION_RECORD& er = *ex_pointers->ExceptionRecord;
        if (er.ExceptionCode != EXCEPTION_USER)
        {
            return nullptr;
        }

        UntypedException untyped(er);
        void* found_exception = untyped.FindException(exception_type);
        if (!found_exception)
        {
            return nullptr;
        }

        if (exception_object != nullptr && found_exception != exception_object)
        {
            return nullptr;
        }

        return ex_pointers;
    }
}