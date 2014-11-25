#include "untyped_exception.h"

#include "../exception.h"
#include "../exception_formatter.h"

namespace xtrap
{
    static const DWORD EXCEPTION_USER = 0xE06D7363U;

    UntypedException::UntypedException(const EXCEPTION_RECORD& er)
    {
         if (er.ExceptionCode == EXCEPTION_USER)
         {
             _exception_object = reinterpret_cast<void*>(er.ExceptionInformation[1]);
             _exception_types = reinterpret_cast<_ThrowInfo*>(er.ExceptionInformation[2])->pCatchableTypeArray;
         }
         else
         {
             _exception_object = nullptr;
             _exception_types = nullptr;
         }
    }

    void* UntypedException::FindException(const std::type_info& ti) const
    {
        if (!_exception_object || !_exception_types)
        {
            return nullptr;
        }

        _CatchableTypeArray* types = reinterpret_cast<_CatchableTypeArray*>(_exception_types);

        for (int i = 0; i < types->nCatchableTypes; ++i)
        {
            _CatchableType& available_type = *types->arrayOfCatchableTypes[i];
            const std::type_info& available_type_info = *reinterpret_cast<std::type_info*>(available_type.pType);
            if (available_type_info == ti)
            {
                char* base_address = reinterpret_cast<char*>(_exception_object);
                base_address += available_type.thisDisplacement.mdisp;
                return base_address;
            }
        }

        return nullptr;
    }

    std::string UntypedException::to_string() const
    {
        auto exception = exception_cast<Exception>();
        if (exception)
        {
            return format_exception(*exception);
        }

        auto std_exception = exception_cast<std::exception>();
        if (std_exception)
        {
            return format_exception(*std_exception);
        }

        auto boost_exception = exception_cast<boost::exception>();
        if (boost_exception)
        {
            return format_exception(*boost_exception);
        }

        return "Unknown error";
    }
}