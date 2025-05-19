#include "AsyncNetPool.h"
#include "HttpClient.h"

// async_thread_pool.cpp
namespace CommonModule {
	namespace Net {

		// --------------------- AsyncThreadPool ---------------------
		AsyncThreadPool::AsyncThreadPool(std::size_t thread_count)
			: work_guard_(boost::asio::make_work_guard(io_context_)) {
			threads_.reserve(thread_count);
			for (std::size_t i = 0; i < thread_count; ++i) {
				threads_.emplace_back([this]() { io_context_.run(); });
			}
		}

		AsyncThreadPool::~AsyncThreadPool() {
			stop();
		}

		boost::asio::io_context& AsyncThreadPool::getIOContext() {
			return io_context_;
		}

		void AsyncThreadPool::run() {
			io_context_.run();
		}

		void AsyncThreadPool::stop() {
			work_guard_.reset();
			io_context_.stop();
			for (auto& thread : threads_) {
				if (thread.joinable()) thread.join();
			}
		}
	}
}
