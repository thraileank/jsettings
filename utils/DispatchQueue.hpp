#pragma once

#include <queue>
#include <thread>
#include <any>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace JSettings {
    //TODO make generic dispatch queue
    class DispatchQueue {
    public:
        DispatchQueue() : quit_(false) {};
        ~DispatchQueue() {
            stop();
        }

        void dispatch(const std::function<void(void)>& task) {
            std::unique_lock<std::mutex> lock(lock_);
            tasks_.push(task);
            lock.unlock();
            cv_.notify_one();
        }

        void start() {
            workerThread_ = std::thread(&DispatchQueue::work, this);
        }

    private:
        void stop() {
            quit_ = true;
            cv_.notify_all();
            if (workerThread_.joinable()) {
                workerThread_.join();
            }
        }

        void work() {
            std::unique_lock<std::mutex> lock(lock_);
            do {
                cv_.wait(lock, [this]{
                    return (!tasks_.empty() || quit_);
                });

                if (!tasks_.empty()) {
                    auto task = std::move(tasks_.front());
                    tasks_.pop();
                    lock.unlock();
                    task();
                    lock.lock();
                }
            } while(!(quit_ && tasks_.empty())); // It's critical to store all parameters before shutdown
        }

    private:
        std::thread workerThread_;
        bool quit_;
        std::mutex lock_;
        std::condition_variable cv_;
        std::queue<std::function<void(void)>> tasks_;
    };
};
