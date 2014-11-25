#pragma once

#include <string>
#include <exception>
#include <boost/exception/exception.hpp>

namespace xtrap
{
    class Exception;

    std::string format_exception(const std::exception& e, bool extract_stack_trace = true);
    std::string format_exception(const boost::exception& e, bool extract_stack_trace = true);
    std::string format_exception(const Exception& e);

    std::string format_boost_exception_details(const std::string& boost_details);
}
