#pragma once

#include "async_result_base.h"

namespace xtrap
{
    template <typename T>
    class AsyncResult : public AsyncResultBase
    {
    public:
        typedef T Data;

        AsyncResult(const T& data)
            : _data(data)
        {
        }

        AsyncResult(T&& data)
            : _data(move(data))
        {
        }

        AsyncResult(const ExceptionInfo& error)
            : AsyncResultBase(error)
        {
        }

        AsyncResult(ExceptionInfo&& error)
            : AsyncResultBase(move(error))
        {
        }

        template <typename E>
        static AsyncResult<T> FromException(const E& exception)
        {
            return AsyncResult<T>(ExceptionInfo(exception));
        }

        const T& data() const
        {
            Validate();
            return _data;
        }

        T& data()
        {
            Validate();
            return _data;
        }

    private:
        T _data;
    };
}