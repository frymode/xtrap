#include "backtrace.h"
#include "symbol_server.h"

extern "C" NTSYSAPI USHORT NTAPI RtlCaptureStackBackTrace(IN ULONG FramesToSkip, IN ULONG FramesToCapture, OUT PVOID *BackTrace, OUT PULONG BackTraceHash);

namespace xtrap
{
    const Backtrace Backtrace::empty(0, 0);

    Backtrace::Backtrace(int exclude_frames /* = DEFAULT_EXCLUDE_FIRST_FRAMES_COUNT */, int depth /* = DEFAULT_STACK_DEPTH */)
        : _trace(depth ? new BacktraceData(depth) : nullptr),
          _trace_size(0)
    {
        if (depth)
        {
            _trace_size = CaptureStackBackTrace(exclude_frames, depth, _trace->data(), NULL);
        }
    }

    Backtrace::Backtrace(PEXCEPTION_POINTERS pointers)
        : _trace_size(0),
          _trace(new BacktraceData())
    {
        if (pointers)
        {
            if (SymbolServer::instance()->DumpBacktraceData(_trace.get(), pointers->ContextRecord))
            {
                _trace_size = _trace->size();
            }
        }
    }

    BacktraceStack Backtrace::GetStack() const
    {
        if (!_trace_size)
        {
            return BacktraceStack();
        }
        return SymbolServer::instance()->DumpTrace(_trace->data(), _trace_size);
    }
}