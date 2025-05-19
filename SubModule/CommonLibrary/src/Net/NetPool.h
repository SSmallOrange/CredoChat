#pragma once
#include "CommonModule.h"
#include "NetDefine.h"
#include "Utils/singleton.h"

#include <memory>
// CommonModule::Net::NetPool::GetInstance()->Get(UserId, strRequest, [](struct Response){});

// NET_GET(UserId, strRequest, strResponse)
// NET_PUT()
// NET_POST()
// NET_DEL()

namespace CommonModule {
	namespace Net {
		// NetPool
		class COMMONMODULE_API NetPool : public Singleton<NetPool> {
			using ptr = std::shared_ptr<NetPool>;
			friend class Singleton<NetPool>;

		public:
			void Get_Async(const std::string& strUserId, HttpRequest req, ResponseHandler handleFunc) const;
			void Post_Async(const std::string& strUserId, HttpRequest req, ResponseHandler handleFunc) const;

			void Get_Async(const std::string& strHost, const std::string& strPort, HttpRequest req, ResponseHandler handleFunc) const;
			void Post_Async(const std::string& strHost, const std::string& strPort, HttpRequest req, ResponseHandler handleFunc) const;

		public:
			void SetSessionInfo(const SessionInfo& sessionInfo);

			// 		public:
			// 			void Get_Sync(const std::string& strUserId, const HttpRequest& req, HttpResponse& response) const;
			// 			void Post_Sync(const std::string& strUserId, HttpRequest req, HttpResponse& response) const;
			// 
			// 			void Get_Sync(const std::string& strHost, const std::string& strPort, HttpRequest req, HttpResponse& response) const;
			// 			void Post_Sync(const std::string& strHost, const std::string& strPort, HttpRequest req, HttpResponse& response) const;

		private:
			NetPool() = default;

		private:

		};
	}
}