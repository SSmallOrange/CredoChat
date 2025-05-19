#include "NetPool.h"
#include "HttpClient.h"

namespace CommonModule {
	namespace Net {
		// --------------------- NetPool ---------------------
		void NetPool::Get_Async(const std::string& strUserId, HttpRequest req, ResponseHandler handleFunc) const {
			HttpClient::GetInstance()->Get_Async(strUserId, req, handleFunc);
		}

		void NetPool::Post_Async(const std::string& strUserId, HttpRequest req, ResponseHandler handleFunc) const {

		}

		void NetPool::Get_Async(const std::string& strHost, const std::string& strPort, HttpRequest req, ResponseHandler handleFunc) const {

		}

		void NetPool::Post_Async(const std::string& strHost, const std::string& strPort, HttpRequest req, ResponseHandler handleFunc) const {

		}

		void NetPool::SetSessionInfo(const SessionInfo& sessionInfo) {
			HttpClient::GetInstance()->SetSessionInfo(sessionInfo);
		}
	}
}