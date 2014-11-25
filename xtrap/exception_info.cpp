#include "exception_info.h"
#include "exception.h"

#include "win32/untyped_exception.h"
#include "win32/exception_trap.h"
#include "exception_formatter.h"

namespace xtrap
{
    ExceptionInfo ExceptionInfo::CurrentException()
    {
        std::exception_ptr error = std::current_exception();
        if (!error)
        {
            return ExceptionInfo();
        }

        PEXCEPTION_POINTERS ex_pointers = GetCurrentExceptionStackTracePointers();
        if (ex_pointers)
        {
            return ExceptionInfo(error, Backtrace(ex_pointers));
        }
        else
        {
            return ExceptionInfo(error);
        }
    }

    ExceptionInfo::ExceptionInfo()
        : _error(nullptr),
          _backtrace(Backtrace::empty)
    {
    }

    ExceptionInfo::ExceptionInfo(const std::exception_ptr& error)
        : _error(error),
          _backtrace(Backtrace::empty)
    {
    }

    ExceptionInfo::ExceptionInfo(const std::exception_ptr& error, const Backtrace& backtrace)
        : _error(error),
          _backtrace(backtrace)
    {
    }

    void ExceptionInfo::Rethrow() const
    {
        rethrow_exception(_error);
    }

    std::string ExceptionInfo::exception_details() const
    {
        if (!_error)
        {
            return std::string();
        }

        try
        {
            Rethrow();
        }
        catch (boost::exception& e)
        {
            return format_exception(e, false);
        }
        catch (std::exception& e)
        {
            return format_exception(e, false);
        }
        catch (...)
        {
            return std::string();
        }
    }

    std::string ExceptionInfo::to_string() const
    {
        if (!_backtrace)
        {
            return exception_details();
        }

        std::stringstream s;
        s << *this;
        return s.str();
    }

    std::ostream& operator << (std::ostream& s, const ExceptionInfo& info)
    {
        s << info.exception_details();
        if (info.backtrace())
        {
            s << std::endl << "Stack trace:" << std::endl << info.backtrace().GetStack();
        }

        return s;
    }

}