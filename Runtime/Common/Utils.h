#pragma once

class NonCopyable
{
protected:
    constexpr NonCopyable() = default;
    ~NonCopyable() = default;

private:
    NonCopyable(const NonCopyable &) = delete;
    void operator=(const NonCopyable &) = delete;
};
