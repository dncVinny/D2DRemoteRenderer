#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <vector>
#include <functional>

class EventSignal
{
private:
    const char *m_pTag;
    bool m_bResetOnWait;

    HANDLE m_hEvent;

    bool Create();

public:
    EventSignal(const char *pTag, bool bResetOnWait);
    ~EventSignal();

    void Set();
    void Reset();
    void Wait();
};

class SharedMemory
{
private:
    const char *m_pTag;
    unsigned __int64 m_uSize;
    void *m_pBuffer;

    HANDLE m_hMapFile;

    bool Create();

public:
    SharedMemory(const char *pTag, unsigned __int64 uSize);
    ~SharedMemory();

    unsigned __int64 GetSize();

    void Clear();

    template<class T>
    T* Get()
    {
        return (T*)m_pBuffer;
    }
};

class IPC
{
private:
    std::string m_sTag;
    bool m_bStopped;

    EventSignal m_eSignals[2];
    SharedMemory m_sMemory;

    std::vector<std::function<void()>> m_vEventCallbacks;

    void InvokeCallbacks();

public:
    IPC(const char *pTag);

    void BindCallback(const std::function<void()> &fCallback);

    void Start();
    void Stop();
    void WaitForStop();

    SharedMemory& GetMemory();
};