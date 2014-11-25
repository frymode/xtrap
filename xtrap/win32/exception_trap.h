#pragma once

#include <functional>

#include "untyped_exception.h"
#include "../backtrace_stack.h"

namespace xtrap
{
    void RegisterExceptionStackTraceTrap();
    bool IsExceptionStackTrapRegistered();

    typedef std::function<void(const UntypedException& error, const BacktraceStack& stacktrace)> FatalErrorHandler;
    void RegisterUnhandledExceptionTrap(FatalErrorHandler error_handler);

    BacktraceStack GetCurrentExceptionStackTrace();
    BacktraceStack GetCurrentExceptionStackTrace(const type_info& exception_type, const void* exception_object = nullptr);

    template <typename T>
    BacktraceStack GetCurrentExceptionStackTrace(const T& exception_object)
    {
        const type_info& type = typeid(exception_object);
        return GetCurrentExceptionStackTrace(type, &exception_object);
    }

    PEXCEPTION_POINTERS GetCurrentExceptionStackTracePointers();
    PEXCEPTION_POINTERS GetCurrentExceptionStackTracePointers(const type_info& exception_type, const void* exception_object = nullptr);

    template <typename T>
    PEXCEPTION_POINTERS GetCurrentExceptionStackTracePointers(const T& exception_object)
    {
        const type_info& type = typeid(exception_object);
        return GetExceptionStackTracePointers(type, &exception_object);
    }
}