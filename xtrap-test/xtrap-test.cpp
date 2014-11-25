#include "stdafx.h"
#include <iostream>

#include "xtrap/all.h"

#define RUN_TEST(test) \
    { \
        std::cout << "--- " #test << std::endl; \
        test(); \
        std::cout << std::endl; \
    }

void throw_std_exception()
{
    try
    {
        throw std::exception("std exception");
    }
    catch (const std::exception& e)
    {
        std::cout << xtrap::format_exception(e);
    }
}

void throw_xtrap_exception()
{
    try
    {
        throw xtrap::Exception("My exception");
    }
    catch (const xtrap::Exception& e)
    {
        std::cout << xtrap::format_exception(e);
    }
}

void boost_throw_xtrap_exception()
{
    try
    {
        XTRAP_THROW_EXCEPTION("Explicit throw exception");
    }
    catch (const xtrap::Exception& e)
    {
        std::cout << xtrap::format_exception(e);
    }
}

void save_exception_info()
{
    xtrap::ExceptionInfo info;
    try
    {
        throw std::exception("runtime error");
    }
    catch (...)
    {
        info = xtrap::ExceptionInfo::CurrentException();
    }

    std::cout << info << std::endl;
}


void RunTests()
{
    RUN_TEST(throw_std_exception);
    RUN_TEST(throw_xtrap_exception);
    RUN_TEST(boost_throw_xtrap_exception);
    RUN_TEST(save_exception_info);
}

int _tmain(int argc, _TCHAR* argv[])
{
    std::cout << "===  Without stack trace trap" << std::endl;
    RunTests();

    xtrap::RegisterExceptionStackTraceTrap();

    std::cout << "=== With stack trace trap" << std::endl;
    RunTests();

    return 0;
}
