#pragma once

#include "exception.h"
#include <iostream>

namespace xtrap
{
    //
    // Like std::exception_ptr but with stack trace attached
    //
    class ExceptionInfo
    {
    public:
        static ExceptionInfo CurrentException();

        ExceptionInfo();
        ExceptionInfo(const std::exception_ptr& error, const Backtrace& backtrace);
        explicit ExceptionInfo(const std::exception_ptr& error);

        template <typename E>
        explicit ExceptionInfo(const E& except)
        {
            _error = std::make_exception_ptr(except);
            _backtrace = Backtrace(2);
        }
        
        void Rethrow() const;
        const std::exception_ptr& error() const { return _error; }
        const Backtrace& backtrace() const { return _backtrace; }

        operator bool() const { return (bool)_error; }

        const std::exception* exception_object() const;
        const boost::exception* boost_exception_object() const;

        std::string to_string() const;
        std::string exception_details() const;

    private:
        std::exception_ptr _error;
        Backtrace _backtrace;
    };

    std::ostream& operator << (std::ostream& s, const ExceptionInfo& info);
}