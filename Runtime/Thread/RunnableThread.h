#pragma once

#include <thread>

class RunnableThread
{
public:

    RunnableThread();

    ~RunnableThread();

    bool Kill(bool shouldWait = true);

    void WaitForCompletion();

    const std::thread::id GetThreadID() const;

    const std::string *GetThreadName() const;

private:

    std::thread m_Thread;
};