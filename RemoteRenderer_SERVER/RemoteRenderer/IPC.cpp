#include "IPC.h"

#include <exception>
#include <thread>

#pragma region EventSignal
EventSignal::EventSignal(const char *pTag, bool bResetOnWait) :
    m_pTag(pTag),
    m_bResetOnWait(bResetOnWait),
    m_hEvent(nullptr)
{
    if (!Create())
        throw std::exception("[EventSignal] failed to create.");
}
EventSignal::~EventSignal()
{
    if (m_hEvent)
        CloseHandle(m_hEvent);
}

// [Private]:
bool EventSignal::Create()
{
    m_hEvent = CreateEventA(
        nullptr,
        !m_bResetOnWait,
        false,
        m_pTag
    );

    return m_hEvent;
}

// [Public]:
void EventSignal::Set()
{
    SetEvent(m_hEvent);

}
void EventSignal::Reset()
{
    ResetEvent(m_hEvent);
}
void EventSignal::Wait()
{
    WaitForSingleObjectEx(
        m_hEvent,
        INFINITE,
        false
    );
}
#pragma endregion

#pragma region SharedMemory
SharedMemory::SharedMemory(const char *pTag, unsigned __int64 uSize) :
    m_pTag(pTag),
    m_uSize(uSize),
    m_hMapFile(nullptr),
    m_pBuffer(nullptr)
{
    if (!Create())
        throw std::exception("[SharedMemory] failed to create.");
}

SharedMemory::~SharedMemory()
{
    if (m_pBuffer)
        UnmapViewOfFile(m_pBuffer);

    if (m_hMapFile)
        CloseHandle(m_hMapFile);
}

// [Private]:
bool SharedMemory::Create()
{
    m_hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        nullptr,
        PAGE_READWRITE,
        0,
        m_uSize,
        m_pTag
    );

    if (m_hMapFile == nullptr)
    {
        return false;
    }

    m_pBuffer = MapViewOfFile(
        m_hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        m_uSize
    );

    if (m_pBuffer == nullptr)
    {
        CloseHandle(m_hMapFile);
        return false;
    }

    return true;
}

// [Public]:
unsigned __int64 SharedMemory::GetSize()
{
    return m_uSize;
}

void SharedMemory::Clear()
{
    memset(m_pBuffer, 0, m_uSize);
}
#pragma endregion

#pragma region IPC
IPC::IPC(const char *pTag) :
    m_sTag(pTag),
    m_bStopped(true),
    m_eSignals{ EventSignal((m_sTag + "_SIGNAL_1").c_str(), false),
                EventSignal((m_sTag + "_SIGNAL_2").c_str(), false) },
    m_sMemory((m_sTag + "_MEMORY").c_str(), 8192) { }

// [Private]:
void IPC::InvokeCallbacks()
{
    for (std::function<void()>& fCallback : m_vEventCallbacks)
        fCallback();
}

// [Public]:
void IPC::BindCallback(const std::function<void()> &fCallback)
{
    m_vEventCallbacks.push_back(fCallback);
}

void IPC::Start()
{
    m_bStopped = false;

    std::thread tWorker([this]() {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

        for (;;)
        {
            m_eSignals[1].Wait();

            InvokeCallbacks();

            m_eSignals[1].Reset();
            m_eSignals[0].Set();
        }
    });

    tWorker.detach();
}

void IPC::Stop()
{
    m_bStopped = true;
    ExitThread(0);
}

void IPC::WaitForStop()
{
    while (!m_bStopped)
        Sleep(10);
}

SharedMemory& IPC::GetMemory()
{
    return m_sMemory;
}
#pragma endregion