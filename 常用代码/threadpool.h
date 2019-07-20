#ifndef _THREAD_POOL_
#define _THREAD_POOL_
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>
#include <functional>
#include <vector>
#include <memory>
#include <iostream>
class threadpool
{
public:
	threadpool(size_t n) :stop_(false)
	{
		for (int i = 0; i < n; ++i)
		{
			workers.emplace_back(std::thread([ & ] {
				while (!stop_)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(mut);
						while (!stop_ && tasks.empty())
						{
							cv.wait(lock, [ & ] {return stop_ || !tasks.empty(); });
						}
						if (stop_)
							return;
						task = std::move(tasks.front());
						tasks.pop();						
					}
					task();
				}
			}));
		}
	}
	template<typename F,typename... ARGS>
	auto enqueue(F&& fun, ARGS ... args)->std::future<std::result_of_t<F(ARGS...)>>
	{
		using return_type = typename std::result_of_t<F(ARGS...)>;
		std::future<return_type> res;
		{
			std::lock_guard<std::mutex> lock(mut);
			std::shared_ptr<std::packaged_task<return_type()>> ptr(new std::packaged_task<return_type()>(std::bind(fun, args ...)));
			res = ptr->get_future();
			tasks.push([ ptr ] {(*ptr)(); });
		}
		cv.notify_one();		
		return res;
	}
	~threadpool()
	{
		{
			std::lock_guard<std::mutex> lock(mut);
			stop_ = true;
		}
		cv.notify_all();
		for (auto& it : workers)
		{
			it.join();
		}
	}
private:
	bool stop_;
	std::mutex mut;
	std::condition_variable cv;
	std::queue<std::function<void()>> tasks;
	std::vector<std::thread> workers;
};





#endif

