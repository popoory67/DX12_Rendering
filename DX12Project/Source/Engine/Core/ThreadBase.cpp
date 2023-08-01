#include "ThreadBase.h"
#include "Global.h"

// It would be much better to be abstracted depending on target platforms.
// More specifically if I would say my idea,
// WindowsThread need to be created by another class that selects and manages platforms.
// However, this project is for DirectX 12, so I didn't think other platforms so far.
#include "WindowsThread.h"
using ProcessPlatform = WindowsThread;

GenericThread* GenericThread::Create(Task* InAction, ThreadPriority InPriority)
{
    GenericThread* newThread = ProcessPlatform::CreateThread();

    if (newThread)
    {
        bool bValid = newThread->CreateInternal(InAction, InPriority);
        if (!bValid)
        {
            SafeDelete(newThread);
        }
    }
    return newThread;
}