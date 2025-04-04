#pragma once
#include "CommonModule.h"

#include <atomic>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace CommonModule {

	class COMMONMODULE_API ThreadPool {
	public:
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
		static ThreadPool& instance() {
			static ThreadPool ins;
			return ins;
		}
		using Task = std::packaged_task<void()>;
		~ThreadPool() {
			stop();
		}
		template <class F, class... Args>
		auto commit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
			using RetType = decltype(f(args...));
			if (_stop.load())
				return std::future<RetType>{};
			auto task = std::make_shared<std::packaged_task<RetType()>>(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...));
			std::future<RetType> ret = task->get_future();
			{
				std::lock_guard<std::mutex> cv_mt(_cvMutex);
				_tasks.emplace([task] { (*task)(); });
			}
			_cvLock.notify_one();
			return ret;
		}

		int idleThreadCount() {
			return _thread_num;
		}
	private:
		ThreadPool(unsigned int num = 5)
			: _stop(false) {
				{
					if (num < 1)
						_thread_num = 1;
					else
						_thread_num = num;
				}
				start();
		}
		void start() {
			for (int i = 0; i < _thread_num; ++i) {
				_vecThreadPool.emplace_back([this]() {
					while (!this->_stop.load()) {
						Task task;
						{
							std::unique_lock<std::mutex> cv_mt(_cvMutex);
							this->_cvLock.wait(cv_mt, [this] {
								return this->_stop.load() || !this->_tasks.empty();
								});
							if (this->_tasks.empty())
								return;
							task = std::move(this->_tasks.front());
							this->_tasks.pop();
						}
						this->_thread_num--;
						task();
						this->_thread_num++;
					}
				});
			}
		}
		void stop() {
			_stop.store(true);
			_cvLock.notify_all();
			for (auto& td : _vecThreadPool) {
				if (td.joinable()) {
					std::cout << "join thread " << td.get_id() << std::endl;
					td.join();
				}
			}
		}
	private:
		std::mutex               _cvMutex;
		std::condition_variable  _cvLock;
		std::atomic_bool         _stop;
		std::atomic_int          _thread_num;
		std::queue<Task>         _tasks;
		std::vector<std::thread> _vecThreadPool;
	};
}