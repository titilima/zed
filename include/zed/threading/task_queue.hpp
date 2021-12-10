#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: task_queue.hpp
//      Author: Ziming Li
//     Created: 2021-12-10
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_THREADING_TASK_QUEUE_HPP
#define ZED_THREADING_TASK_QUEUE_HPP

#include <memory>
#include <queue>
#include "../mutex.hpp"
#include "../thread.hpp"
#include "../threading/signal.hpp"

namespace zed {

class task_queue
{
public:
    task_queue(void) = default;

    class task {
    public:
        virtual ~task(void) = default;
        virtual void run(void) = 0;
    };
    void add(task *t);

    using queue_t = std::queue<std::unique_ptr<task>>;
    void take(queue_t &dst);
private:
    zed::mutex m_mutex;
    zed::signal m_signal;
    queue_t m_tasks;
};

class task_thread : public thread<task_thread>
{
public:
    task_thread(void);
    virtual ~task_thread(void);

    using task = task_queue::task;
    void add(task *t) { m_queue.add(t); }
protected:
    virtual void on_enter_loop(void) {}
    virtual void on_leave_loop(void) {}
private:
    void work(void);
    void loop(void);

    bool m_running = true;
    task_queue m_queue;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

namespace detail {

class exit_loop_task final : public task_queue::task
{
public:
    exit_loop_task(bool &running_flag) : m_running_flag(running_flag) {}
private:
    void run(void) override
    {
        m_running_flag = false;
        delete this;
    }

    bool &m_running_flag;
};

} // namespace detail

inline void task_queue::add(task *t)
{
    if (auto _ = m_mutex.guard())
        m_tasks.emplace(t);
    m_signal.notify();
}

inline void task_queue::take(queue_t &dst)
{
    m_signal.wait();
    if (auto _ = m_mutex.guard())
        m_tasks.swap(dst);
    m_signal.reset();
}

inline task_thread::task_thread(void) : thread(this, &task_thread::work)
{
}

inline task_thread::~task_thread(void)
{
    add(new detail::exit_loop_task(m_running));
    join();
}

inline void task_thread::loop(void)
{
    task_queue::queue_t tasks;
    for (;;)
    {
        m_queue.take(tasks);
        while (!tasks.empty())
        {
            task *t = tasks.front().release();
            t->run();

            if (!m_running)
                return;

            tasks.pop();
        }
    }
}

inline void task_thread::work(void)
{
    on_enter_loop();
    loop();
    on_leave_loop();
}

} // namespace zed

#endif // ZED_THREADING_TASK_QUEUE_HPP
