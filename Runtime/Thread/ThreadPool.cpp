#include "ThreadPool.h"
#include <assert.h>

WorkerThreadPool::WorkerThreadPool()
{

}

WorkerThreadPool::~WorkerThreadPool()
{

}

void WorkerThreadPool::DispatchThreadJob(WorkerThreadJob* job)
{
    assert(job != nullptr);

    if (m_Dying)
    {
        job->Abandon();
        return;
    }

    WorkerThread *thread = nullptr;

    {
        std::lock_guard lock(m_Mutex);

        if (m_WaitingThreads.empty())
        {
            m_WaitingJobs.push_back(job);
            return;
        }

        thread = m_WaitingThreads.back();
        m_WaitingThreads.pop_back();
    }

    thread->DoJob(job);
}