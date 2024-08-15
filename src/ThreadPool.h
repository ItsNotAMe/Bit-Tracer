#pragma once

#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool
{
public:
    explicit ThreadPool(int nThreads);
    ~ThreadPool();
    
    template<class F, class... Args>
    void addTask(F&& f, Args&&... args)
    {
        auto task = std::make_shared<std::function<void()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::unique_lock<std::mutex> lock(m_mutex);
        m_taskQueue.push([task]() {(*task)();});
        lock.unlock();
        m_cv.notify_one();
    }

    void waitForTasks();
    void waitForUpdate();
    int getQueueSize();
    int getCountWorking() { return m_countWorking; }
private:
    std::atomic<int> m_countWorking;
    int m_nThreads;
    std::queue<std::function<void()>> m_taskQueue;
    std::vector<std::thread> m_threads;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::condition_variable m_cvUpdate;
    std::condition_variable m_cvEmpty;
    bool m_stop;
};