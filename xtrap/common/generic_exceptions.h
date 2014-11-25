#pragma once

#include "../exception.h"

#define XTRAP_THROW_NOT_IMPLEMENTED() THROW(xutil::NotImplementedException())
#define XTRAP_THROW_NOT_SUPPORTED(msg) THROW(xutil::NotSupportedException(msg))
#define XTRAP_THROW_INVALID_STATE(msg) THROW(xutil::InvalidStateException(msg))
#define XTRAP_THROW_ARGUMENT_ERROR(arg, details) THROW(xutil::ArgumentException(arg, details))
#define XTRAP_THROW_ARGUMENT_NULL(arg) THROW(xutil::ArgumentNullException(arg))

namespace xtrap
{
    class NotImplementedException : public Exception
    {
    public:
        NotImplementedException();
        NotImplementedException(const std::string& message);
        NotImplementedException(const boost::format& format);

        virtual ~NotImplementedException() throw();
    };

    class NotSupportedException : public Exception
    {
    public:
        NotSupportedException();
        NotSupportedException(const std::string& message);
        NotSupportedException(const boost::format& format);

        virtual ~NotSupportedException() throw();
    };

    XTRAP_DECLARE_ERROR_INFO(ErrorInfo_Argument, std::string);

    class ArgumentException : public Exception
    {
    public:
        ArgumentException(const std::string& arg, const std::string& details = "");
        ArgumentException(const std::string& arg, const boost::format& format);
        
		virtual ~ArgumentException() throw();
    };

    class ArgumentNullException : public ArgumentException
    {
    public:
        ArgumentNullException(const std::string& arg);
        
		virtual ~ArgumentNullException() throw();
    };

    class InvalidStateException : public Exception
    {
    public:
        InvalidStateException();
        InvalidStateException(const std::string& message);
        InvalidStateException(const boost::format& format);

		virtual ~InvalidStateException() throw();
    };

    class SystemError : public Exception
    {
    public:
        static int GetSystemErrorCode();

        SystemError();
        SystemError(const std::string& message);
        SystemError(const boost::format& format);

        virtual ~SystemError() throw();
    };
}
