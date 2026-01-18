#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    
    void push(const T& data) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (queue_.size() >= maxSize) {
            queue_.pop(); // Drop oldest
            droppedCount_++;
        }
        queue_.push(data);
        condition_.notify_one();
    }
    
    void push(T&& data) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (queue_.size() >= maxSize) {
            queue_.pop(); // Drop oldest
            droppedCount_++;
        }
        queue_.push(std::move(data));
        condition_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return !queue_.empty(); });
        T data = queue_.front();
        queue_.pop(); // O(1) operation
        return data;
    }

    bool isEmpty() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    
    size_t size() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
    
    size_t getDroppedCount() const {
        return droppedCount_.load();
    }

private:
    static constexpr size_t maxSize = 100;
    std::atomic<size_t> droppedCount_{0};
    std::queue<T> queue_; // O(1) push/pop operations
    mutable std::mutex mutex_;
    std::condition_variable condition_;
};

#endif // THREADSAFEQUEUE_H