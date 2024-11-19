#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(int nThreads)
    : m_nThreads(nThreads), m_stop(false)
{
    for (int i = 0; i < nThreads; i++)
    {
        m_threads.emplace_back([this]
            {
                std::function<void()> task;
                while (1)
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_cv.wait(lock, [this] {
                        return !m_taskQueue.empty() || m_stop;
                        });
                    if (m_stop)
                        return;
                    task = move(m_taskQueue.front());
                    m_taskQueue.pop();
                    lock.unlock();
                    m_countWorking++;
                    m_cvUpdate.notify_one();
                    task();
                    m_countWorking--;
                    m_cvUpdate.notify_one();
                    if (m_taskQueue.empty() && m_countWorking == 0)
                        m_cvEmpty.notify_one();
                }
            });
    }
}

ThreadPool::~ThreadPool()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_stop = true;
    lock.unlock();
    m_cv.notify_all();
    for (auto& thread : m_threads)
        thread.join();
}

void ThreadPool::waitForTasks()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_taskQueue.empty() && m_countWorking == 0)
        return;
    m_cvEmpty.wait(lock, [this] {
        return m_taskQueue.empty() && m_countWorking == 0;
        });
}

void ThreadPool::waitForUpdate()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_taskQueue.empty() && m_countWorking == 0)
        return;
    m_cvUpdate.wait(lock);
}

int ThreadPool::getQueueSize()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_taskQueue.size();
}