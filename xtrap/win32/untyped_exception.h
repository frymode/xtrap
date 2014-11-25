#pragma once

#include <typeinfo>
#include <string>

#include "windows.h"

namespace xtrap
{
    class UntypedException 
    {
    public:
        UntypedException(const EXCEPTION_RECORD& er);

        template <typename T>
        T* exception_cast() const
        {
            const type_info& ti = typeid(T);
            return reinterpret_cast<T*>(FindException(ti));
        }

        void* FindException(const std::type_info& ti) const;
        std::string to_string() const;

    private:
        void* _exception_object;
        const void* _exception_types;
    };
}