#include <exception>
#include <stdio.h>
#include <assert.h>
#include "ThreadPool.h"
ThreadPool::ThreadPool(const std::string& nameArg): name_(nameArg), maxQueueSize_(0), running_(false)
{
}

ThreadPool::~ThreadPool()
{
    if (running_)
    {
        stop();
    }
}

void ThreadPool::start(int numThreads)
{
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(numThreads);

    for (int i = 0; i < numThreads; ++i)
    {
        char id[32] = { 0 };
        snprintf(id, sizeof id, "%d", i + 1);
        threads_.emplace_back(new thread(std::bind(&ThreadPool::runInThread, this), name_ + id));
    }
    if (numThreads == 0 && threadInitCallback_)
    {
        threadInitCallback_();
    }
}

void ThreadPool::stop()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
        notEmpty_.notify_all();
    }
    for (auto& thr : threads_)
    {
        thr->join();
    }
}

size_t ThreadPool::queueSize() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

int ThreadPool:run(task func)
{
  std::lock_guard<std::mutex>lock(mutex_);
  return queue_.size();
}
