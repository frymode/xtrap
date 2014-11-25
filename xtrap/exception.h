#pragma once

#include <exception>
#include <boost/exception/all.hpp>
#include <boost/format.hpp>

#include "backtrace.h"

#define XTRAP_THROW BOOST_THROW_EXCEPTION
#define XTRAP_THROW_EXCEPTION(x) XTRAP_THROW(xtrap::Exception(x))

#define XTRAP_DECLARE_ERROR_INFO(InfoType, ValueType) \
    typedef boost::error_info<struct InfoType##_, ValueType> InfoType

namespace xtrap
{
    typedef boost::errinfo_errno ErrorInfo_SystemErrorCode;
    XTRAP_DECLARE_ERROR_INFO(ErrorInfo_Message, std::string);
    XTRAP_DECLARE_ERROR_INFO(ErrorInfo_Backtrace, Backtrace);

    class Exception : public std::exception, public boost::exception
    {
    public:
        static const int backtrace_skip;

        Exception();
        Exception(const std::string& message);
        Exception(const boost::format& format);

        virtual ~Exception() throw();

        template <typename T>
        const typename T::value_type* get() const
        {
            return boost::get_error_info<T>(*this);
        }

        template <typename T>
        const typename T::value_type try_get(const typename T::value_type& default_value) const
        {
            auto value = get<T>();
            return value ? *value : default_value;
        }

        const int* system_errno() const { return get<ErrorInfo_SystemErrorCode>(); }
        const Backtrace* backtrace() const {  return get<ErrorInfo_Backtrace>();  }
        std::string message() const {  return try_get<ErrorInfo_Message>("");  }

        virtual const char* what() const throw();
        virtual const char* detailed_info() const throw();

    private:
        mutable std::string exception_str_;
    };
}
