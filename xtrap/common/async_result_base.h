#pragma once

#include "../exception_info.h"
#include <type_traits>

namespace xtrap
{
    class AsyncResultBase
    {
    public:
        AsyncResultBase()
        {
        }

        AsyncResultBase(const ExceptionInfo& error)
            : _error(error)
        {
        }

        AsyncResultBase(ExceptionInfo&& error)
            : _error(std::move(error))
        {
        }

        bool succeeded() const { return !_error; }
        const ExceptionInfo& error() const { return _error; }
    
    protected:
        ~AsyncResultBase() { }

        void Validate() const
        {
            if (_error)
            {
                _error.Rethrow();
            }
        }

    private:
        ExceptionInfo _error;
    };
}