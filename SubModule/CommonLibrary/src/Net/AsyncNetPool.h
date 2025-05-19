#pragma once
// async_thread_pool.hpp
#pragma once
#include <boost/asio.hpp>
#include <vector>
#include <thread>
#include <memory>

namespace CommonModule {
	namespace Net {
		class AsyncThreadPool {
		public:
			explicit AsyncThreadPool(std::size_t thread_count = std::thread::hardware_concurrency());
			~AsyncThreadPool();

			boost::asio::io_context& getIOContext();

			void run();
			void stop();

		private:
			boost::asio::io_context io_context_;
			boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;
			std::vector<std::thread> threads_;
		};
	};
};
