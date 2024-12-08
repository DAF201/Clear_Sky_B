#ifndef EVENT_SYSTEM_HPP
#define EVENT_SYSTEM_HPP

#ifndef TOOL_HPP
#include <math.h>
#endif
#include <stdio.h>
#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <map>
#include <sys/epoll.h>
#include <signal.h>
#include <atomic>
namespace event_system
{
    typedef int fd;

    std::mutex event_system_mutex;

    std::atomic<bool> event_system_alive = true;

    static const int urgency_multiplier = 50;
    static const int importance_multiplier = 120;

    enum LEVEL
    {
        NOT = 1,
        SLIGHTLY = 2,
        SOME = 4,
        VERY = 8,
        EXTREME = 16
    };

    inline int priority_calculator(LEVEL urgency, LEVEL importance)
    {
        return floor(pow(urgency * urgency_multiplier, 0.5) / 10 + pow(importance * importance_multiplier, 0.5) / 10);
    }

    struct task
    {
        LEVEL urgency;
        LEVEL importance;

        // you made your handler so you need to decide how to handle the param and return value not driver class
        std::function<void(void *, void *)> func;
        void *return_ptr;
        void *param_ptr;

        // FUTURE USE
        int repeat;

        task() {}

        task(std::function<void(void *, void *)> task_body, void *return_ptr, void *param_ptr, LEVEL urgency, LEVEL importance, int repeat = 1)
        {
            this->func = task_body;
            this->return_ptr = return_ptr;
            this->param_ptr = param_ptr;
            this->urgency = urgency;
            this->importance = importance;
            this->repeat = repeat;
        }

        void exec()
        {
            try
            {
                func(return_ptr, param_ptr);
            }
            catch (...)
            {
                std::rethrow_exception(std::current_exception());
            }
        }
    };

    struct compare_task
    {
        inline bool operator()(const task &t1, const task &t2)
        {
            // Return true if t1 has lower priority than t2 (for max-heap behavior)
            return priority_calculator(t1.urgency, t1.importance) < priority_calculator(t2.urgency, t2.importance);
        }
    };

    std::priority_queue<task, std::vector<task>, compare_task> task_heap;
    std::map<fd, task> task_map;

    class executor
    {
    private:
        void work()
        {
            task t;
            while (event_system_alive)
            {
                if (task_heap.empty())
                {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
                else
                {
                    event_system_mutex.lock();
                    t = task_heap.top();
                    t.exec();
                    task_heap.pop();
                    event_system_mutex.unlock();
                }
            }
        }

    public:
        void start()
        {
            std::thread t = std::thread(&executor::work, this);
            t.detach();
        }
    };

    class listener
    {
    private:
        fd epoll_fd;
        epoll_event ev;
        executor ex;
        void listen()
        {
            int num_of_events;
            epoll_event events_buffer[1024];
            while (event_system_alive)
            {
                num_of_events = epoll_wait(epoll_fd, events_buffer, 1024, 500);

                if (num_of_events == 0)
                    continue;

                event_system_mutex.lock();
                for (int i = 0; i < num_of_events; i++)
                {

                    // task max trigger check, if reached max repeat, remove from map and epoll
                    if (task_map[events_buffer[i].data.fd].repeat == 0)
                    {
                        task_map.erase(events_buffer[i].data.fd);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events_buffer[i].data.fd, &ev);
                        continue;
                    }

                    if (task_map[events_buffer[i].data.fd].repeat != INT32_MAX)
                        task_map[events_buffer[i].data.fd].repeat--;

                    // find the task, push to heap
                    task_heap.push(task_map[events_buffer[i].data.fd]);
                }
                event_system_mutex.unlock();
            }
        }

    public:
        listener()
        {
            epoll_fd = epoll_create1(0);
            printf("epoll fd: %d\n", epoll_fd);
            signal(SIGUSR1, listener::handler);
            ex = executor();
            ex.start();
        }

        void add_event(fd target_fd, int trigger_event, task t)
        {
            ev.data.fd = target_fd;
            ev.events = trigger_event;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, target_fd, &ev) < 0)
                throw std::runtime_error("failed to register event!");

            if (task_map.count(target_fd) == 0)
                task_map[target_fd] = t;
        }

        static void handler(int signal)
        {
            if (signal == SIGUSR1)
                event_system_alive = false;
        }

        void start()
        {
            std::thread t = std::thread(&listener::listen, this);
            t.detach();
        }

        void stop()
        {
            kill(getgid(), SIGUSR1);
        }

        ~listener()
        {
            event_system_alive = false;
            close(epoll_fd);
        }
    };

} // namespace event_system

#endif