#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include <functional> // Include the <functional> header for std::function
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;

// Function to calculate the square of each element in a subarray
void calculateSquare(const vector<int>& input, vector<int>& output, int start, int end) {
    for (int i = start; i < end; ++i) {
        output[i] = input[i] * input[i];
    }
}

// Thread pool class to manage a pool of worker threads
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads) : stop(false) {
        // Create worker threads and assign them tasks from the queue
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back([this] {
                while (true) {
                    function<void()> task;
                    {
                        // Acquire a lock to access the task queue
                        unique_lock<std::mutex> lock(queueMutex);
                        // Wait until there's a task in the queue or the thread pool is stopped
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        // If the thread pool is stopped and the task queue is empty, exit the thread
                        if (stop && tasks.empty()) return;
                        // Get the next task from the queue
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    // Execute the task
                    task();
                }
                });
        }
    }

    ~ThreadPool() {
        // Signal all worker threads to stop
        {
            unique_lock<mutex> lock(queueMutex);
            stop = true;
        }
        // Notify all threads to wake up and exit
        condition.notify_all();
        // Join all worker threads to wait for their completion
        for (thread& worker : threads) {
            worker.join();
        }
    }

    // Method to enqueue a new task to the thread pool
    template<class F, class... Args>
    void enqueue(F&& f, Args&&... args) {
        {
            // Acquire a lock to access the task queue
            unique_lock<mutex> lock(queueMutex);
            // Add the new task to the queue
            tasks.emplace(bind(forward<F>(f), forward<Args>(args)...));
        }
        // Notify one waiting thread that a new task is available
        condition.notify_one();
    }

private:
    vector<thread> threads; // Container to hold worker threads
    queue<function<void()>> tasks; // Queue to store tasks
    mutex queueMutex; // Mutex to synchronize access to the task queue
    condition_variable condition; // Condition variable for thread synchronization
    bool stop; // Flag to indicate if the thread pool should stop
};

int main() {
    const int numElements = 300000000;
    const int numThreads = thread::hardware_concurrency();
    const int chunkSize = numElements / numThreads;

    vector<int> numbers(numElements); // Vector to hold input numbers
    vector<int> squares(numElements); // Vector to hold output squares

    // Generate input numbers
    for (int i = 0; i < numElements; ++i) {
        numbers[i] = i + 1;
    }

    // Single-threaded computation
    auto startSingle = chrono::high_resolution_clock::now();
    calculateSquare(numbers, squares, 0, numElements);
    auto endSingle = chrono::high_resolution_clock::now();
    chrono::duration<double> singleDuration = endSingle - startSingle;

    // Multi-threaded computation
    auto startMulti = chrono::high_resolution_clock::now();
    ThreadPool threadPool(numThreads); // Create a thread pool with the specified number of threads
    for (int i = 0; i < numThreads; ++i) {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? numElements : (i + 1) * chunkSize;
        // Enqueue tasks to calculate squares of subarrays in parallel
        threadPool.enqueue(calculateSquare, cref(numbers), ref(squares), start, end);
    }
    auto endMulti = chrono::high_resolution_clock::now();
    chrono::duration<double> multiDuration = endMulti - startMulti;

    // Output results and execution times
    cout << "Single-threaded computation took " << singleDuration.count() << " seconds." << endl;
    cout << "Multi-threaded computation took " << multiDuration.count() << " seconds." << endl;

    return 0;
}
