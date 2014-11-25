#include "breakpoint.h"
#include "win32/windows.h"

namespace xtrap
{
    void DebuggerBreakpoint()
    {
        if (IsDebuggerPresent())
        {
            DebugBreak();
        }
    }
}