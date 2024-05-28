#pragma once
#include <queue>
#include <mutex>

template <typename T>
class ThreadSafeQueue {
private:
    std::queue<T> _queue;
    mutable std::mutex _mutex;

public:

    void Push(T value) {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(std::move(value));
    }

    bool TryPop(T& value) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_queue.empty()) {
            return false;
        }
        value = std::move(_queue.front());
        _queue.pop();
        return true;
    }

    bool Empty() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.empty();
    }

    size_t GetQueueSize() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.size();
    }

    // Destructor to ensure mutex is properly released
    ~ThreadSafeQueue() {
        // Mutex will be automatically released when the object is destroyed
    }
};
