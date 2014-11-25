#include "exception.h"
#include "exception_formatter.h"
#include "config.h"

#include "win32/exception_trap.h"

namespace xtrap
{
    const int Exception::backtrace_skip = 2;

    Exception::Exception()
    {
#ifdef XTRAP_EXCEPTION_BACKTRACE
        if (!IsExceptionStackTrapRegistered())
        {
            *this << ErrorInfo_Backtrace(Backtrace(backtrace_skip));
        }
#endif
    }

    Exception::Exception(const std::string& message)
    {
#ifdef XTRAP_EXCEPTION_BACKTRACE
        if (!IsExceptionStackTrapRegistered())
        {
            *this << ErrorInfo_Backtrace(Backtrace(backtrace_skip));
        }
#endif
		*this << ErrorInfo_Message(message);
	}

    Exception::Exception(const boost::format& format)
    {
#ifdef XTRAP_EXCEPTION_BACKTRACE
        if (!IsExceptionStackTrapRegistered())
        {
            *this << ErrorInfo_Backtrace(Backtrace(backtrace_skip));
        }
#endif
		*this << ErrorInfo_Message(str(format));
    }

    Exception::~Exception() throw()
    {
    }

    char const* Exception::what() const throw()
    {
        exception_str_ = format_boost_exception_details(detailed_info());
        return exception_str_.c_str();
    }

    const char* Exception::detailed_info() const throw()
    {
        return diagnostic_information_what(*this);
    }
}