#pragma once

#include "windows.h"
#include <memory>
#include "../backtrace_stack.h"

namespace xtrap
{
    class Backtrace
    {
    public:
        typedef std::vector<void*> BacktraceData;
        static const Backtrace empty;

        static const int DEFAULT_EXCLUDE_FIRST_FRAMES_COUNT = 1;
        static const int DEFAULT_STACK_DEPTH = 50;

        Backtrace(int exclude_frames = DEFAULT_EXCLUDE_FIRST_FRAMES_COUNT, int depth = DEFAULT_STACK_DEPTH);
        Backtrace(PEXCEPTION_POINTERS pointers);

        operator bool() const { return _trace_size != 0; }
        BacktraceStack GetStack() const;

    protected:
        std::shared_ptr<BacktraceData> _trace;
        size_t _trace_size;
    };

    inline std::ostream& operator << (std::ostream& stream, const Backtrace& backtrace)
    {
        stream << backtrace.GetStack();
        return stream;
    }
}