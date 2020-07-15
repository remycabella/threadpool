#include <list>
#include <queue>
#include <mutex>
#include <thread>
#include <future>
#include <functional>
#include <condition_variable>

namespace Utility{

    class ThreadPool {
	public:
	    ~ThreadPool() {
	        for (auto &thd : _threadList) {
	            thd.join();
	        }
	    }
	
	    ThreadPool(int size) : _maxSize(size) {
	        while (size--) {
	            _threadList.emplace_back(
		            std::thread ([this] () {
		                while (true) {
		                    std::unique_lock <std::mutex> guard(_mutex);
		                    if (_jobs.size() == 0 ) {
		                        _emptyCond.wait(guard);
		                    } else {
		                        auto job = _jobs.front();
                                _jobs.pop();
		                        _fullCond.notify_one();
		                        guard.unlock();
		                        job();
		                    }
	                    }
	                })
	            );
	        }
	    }

        template <typename T, typename... Args> 	
	    std::future<bool> addTask(T t, Args... args) {
	        std::unique_lock <std::mutex> guard(_mutex);
	        if (_jobs.size() >= _maxSize) {
	            _fullCond.wait(guard);
	        } else {
                auto task = std::make_shared <std::packaged_task <bool ()>> (std::bind(t, std::forward<Args>(args)...));
                std::future <bool> res = task->get_future();

	            _jobs.push([task]() {
                    (*task)(); 
                });

	            guard.unlock();
	            _emptyCond.notify_one(); 

                return res;
	        }
	    }
	
	private:
	    int _maxSize {0};
	
	    std::mutex _mutex;
	    
	    std::condition_variable _fullCond;
	    
	    std::condition_variable _emptyCond;
	
	    std::queue <std::function<void ()>> _jobs;
	
	    std::list <std::thread> _threadList;
	};    
}

