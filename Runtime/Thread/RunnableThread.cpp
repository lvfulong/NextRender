#include "RunnableThread.h"
#include <assert.h>

RunnableThread::RunnableThread()
{

}

RunnableThread::~RunnableThread()
{

}

bool RunnableThread::Kill(bool shoudWait/* = true*/)
{

}

void RunnableThread::WaitForCompletion()
{
    assert(m_Thread.joinable());
    m_Thread.join();
}

const std::thread::id RunnableThread::GetThreadID() const
{
    return m_Thread.get_id();
}

const std::string *RunnableThread::GetThreadName() const
{

}