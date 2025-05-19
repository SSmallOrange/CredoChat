// http_client.cpp
#include "HttpClient.h"
#include "HttpCore.h"
#include "Log/LogManager.h"

namespace CommonModule {
	namespace Net {

		static CommonModule::Logger::ptr g_logger = LOG_ROOT();

		HttpClient::HttpClient(std::size_t thread_count)
			: _threadPool(thread_count) {}

		HttpCore::ptr HttpClient::GetCore(const std::string& strSessionId) {
			std::lock_guard<std::mutex> lock(_mutex);
			auto it = _coreMap.find(strSessionId);
			if (it != _coreMap.end() && it->second->SessionIsEffective()) {
				return it->second;
			}
			return nullptr;
		}

		void HttpClient::Get_Async(const std::string& strUserId, HttpRequest req, ResponseHandler handleFunc) {
			HttpCore::ptr core = GetCore(strUserId);
			if (core == nullptr) {
				LOG_DEBUG(g_logger) << "SessionId is vailed Or Session Not Set SessionInfo";
				return;
			}
			core->MakeRequest(req, handleFunc);
		}

		void HttpClient::Post_Async(const std::string& strUserId, HttpRequest req, ResponseHandler handleFunc) {

		}

		void HttpClient::SetSessionInfo(const SessionInfo& sessionInfo) {
			_coreMap[sessionInfo.strSessionId] = std::make_shared<HttpCore>(_threadPool.getIOContext(), sessionInfo);
		}
	}
}
