// http_client.cpp
#include "HttpClient.h"

HttpClient::HttpClient(std::size_t thread_count)
	: thread_pool_(thread_count) {}

std::shared_ptr<HttpCore> HttpClient::get_core(const std::string& user_id) {
	std::lock_guard<std::mutex> lock(mutex_);
	auto it = core_map_.find(user_id);
	if (it != core_map_.end()) return it->second;

	auto core = std::make_shared<HttpCore>(thread_pool_.get_io_context());
	core_map_.emplace(user_id, core);
	return core;
}
