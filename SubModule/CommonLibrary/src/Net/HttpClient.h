// http_client.hpp
#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "HttpCore.h"
#include "AsyncNetPool.h"

class HttpClient {
public:
	explicit HttpClient(std::size_t thread_count = std::thread::hardware_concurrency());

	std::shared_ptr<HttpCore> get_core(const std::string& user_id);

private:
	AsyncThreadPool thread_pool_;
	std::unordered_map<std::string, std::shared_ptr<HttpCore>> core_map_;
	std::mutex mutex_;
};
