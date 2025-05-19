// http_client.hpp
#pragma once
#include "Utils/singleton.h"
#include <memory>
#include "CommonModule.h"
#include "NetDefine.h"
#include "AsyncNetPool.h"

#include <unordered_map>
#include <string>

namespace CommonModule {
	namespace Net {
		class HttpCore;

		class HttpClient : public Singleton<HttpClient> {
		public:
			void Get_Async(const std::string& strUserId, HttpRequest req, ResponseHandler handleFunc = nullptr);
			void Post_Async(const std::string& strUserId, HttpRequest req, ResponseHandler handleFunc = nullptr);

		public:
			void SetSessionInfo(const SessionInfo& sessionInfo);

		private:
			std::shared_ptr<HttpCore> GetCore(const std::string& strSessionId);

		private:
			friend class Singleton<HttpClient>;
			explicit HttpClient(std::size_t thread_count = std::thread::hardware_concurrency());

		private:
			AsyncThreadPool _threadPool;
			std::unordered_map<std::string, std::shared_ptr<HttpCore>> _coreMap;
			std::mutex _mutex;
		};
	}
}
