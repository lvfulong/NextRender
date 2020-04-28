#pragma once

#include <thread>
#include <Mutex>
#include <Queue>

class WorkerThread;
class WorkerThreadJob;

class WorkerThreadPool
{
public:

    WorkerThreadPool();

    ~WorkerThreadPool();

    bool Create(unit32_t threadNum, uint32_t stackSize/*ThreadPriority*/);

    void Destory();

    void DispatchThreadJob(WorkerThreadJob* job);

private:

    std::mutex m_Mutex;

    std::deque<WorkerThread *> m_Threads;

    std::deque<WorkerThread *> m_WaitingThreads;
};