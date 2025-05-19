// http_core.hpp
#pragma once
#include <string>
#include <memory>
#include "NetDefine.h"
#include "HttpSession.h"
#include "AsyncNetPool.h"

namespace CommonModule {
	namespace Net {
		class HttpCore {
		public:
			using ptr = std::shared_ptr<HttpCore>;

			explicit HttpCore(boost::asio::io_context& io_context, const SessionInfo& sessionInfo = {});

			void MakeRequest(HttpRequest req, ResponseHandler handleFunc = nullptr);

			void SetSessionInfo(const SessionInfo& sessionInfo);
			void SetToken(const std::string& strToken);

		public:
			bool SessionIsEffective() { return !_sessionInfo.strSessionId.empty(); }

		private:
			boost::beast::http::verb toHttpVerb(const std::string& method);
			int toHttpVersion(const std::string& version);

		private:
			boost::asio::io_context& _io_context;
			SessionInfo _sessionInfo;
		};

	}
}
