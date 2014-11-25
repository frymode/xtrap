#include "generic_exceptions.h"

namespace xtrap
{
	namespace
	{
        std::string FormatArgumentMessage(const std::string& arg, const std::string& details)
		{
            std::string msg = "Invalid value of argument '" + arg + "'";
			if (!details.empty())
			{
				msg.append(": ");
				msg.append(details);
			}
			return msg;
		}
	}

    NotImplementedException::NotImplementedException()
    {
    }

    NotImplementedException::NotImplementedException(const std::string& message)
        : Exception(message)
    {
    }

    NotImplementedException::NotImplementedException(const boost::format& format)
        : Exception(format)
    {
    }

    NotImplementedException::~NotImplementedException() throw()
    {
    }



    NotSupportedException::NotSupportedException()
    {
    }

    NotSupportedException::NotSupportedException(const std::string& message)
        : Exception(message)
    {
    }

    NotSupportedException::NotSupportedException(const boost::format& format)
        : Exception(format)
    {
    }

    NotSupportedException::~NotSupportedException() throw()
    {
    }



    InvalidStateException::InvalidStateException()
    {
    }

    InvalidStateException::InvalidStateException(const std::string& message)
        : Exception(message)
    {
    }

    InvalidStateException::InvalidStateException(const boost::format& format)
        : Exception(format)
    {
    }

    InvalidStateException::~InvalidStateException() throw()
    {
    }



    ArgumentException::ArgumentException(const std::string& arg, const std::string& details /* = "" */)
		: Exception(FormatArgumentMessage(arg, details))
    {
		*this << ErrorInfo_Argument(arg);
    }

    ArgumentException::ArgumentException(const std::string& arg, const boost::format& format)
        : Exception(format)
    {
		*this << ErrorInfo_Argument(arg);
    }

    ArgumentException::~ArgumentException() throw()
    {
    }



    ArgumentNullException::ArgumentNullException(const std::string& arg)
        : ArgumentException(arg, "can not be null")
    {
    }

    ArgumentNullException::~ArgumentNullException() throw()
    {
    }



    int SystemError::GetSystemErrorCode()
    {
        return GetLastError();
    }

    SystemError::SystemError()
    {
        *this << ErrorInfo_SystemErrorCode(GetSystemErrorCode());
    }

    SystemError::SystemError(const std::string& message)
        : Exception(message)
    {
        *this << ErrorInfo_SystemErrorCode(GetSystemErrorCode());
    }

    SystemError::SystemError(const boost::format& format)
        : Exception(format)
    {
        *this << ErrorInfo_SystemErrorCode(GetSystemErrorCode());
    }

    SystemError::~SystemError() throw()
    {
    }
}

