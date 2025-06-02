// http_core.cpp
#include "HttpCore.h"

namespace CommonModule {
	namespace Net {
		HttpCore::HttpCore(boost::asio::io_context& io_context, const SessionInfo& sessionInfo)
			: _io_context(io_context), _sessionInfo(sessionInfo){}

		void HttpCore::MakeRequest(HttpRequest req, ResponseHandler handleFunc) {
			const std::string strHost = _sessionInfo.strHost;
			const std::string strPort = _sessionInfo.strPort;

			boost::beast::http::verb method = toHttpVerb(req.strMethod);
			int httpVersion = toHttpVersion(req.strHttpVersion);

			boost::beast::http::request<boost::beast::http::string_body> boostReq{ method, req.strUrl, httpVersion };

			boostReq.set(boost::beast::http::field::host, _sessionInfo.strHost);
			boostReq.set(boost::beast::http::field::user_agent, "CredoChat.v0.1");
			boostReq.set(boost::beast::http::field::content_type, "application/json");
			boostReq.set("X-Request-ID", req.strRequestId);

			if (!_sessionInfo.strToken.empty()) {
				boostReq.set("Token", _sessionInfo.strToken);
			}

			if (!req.strRequestContext.empty()) {
				boostReq.body() = req.strRequestContext;
				boostReq.prepare_payload();
			}

			std::make_shared<HttpSession>(_io_context, strHost, strPort, boostReq, handleFunc)->run();
		}

		void HttpCore::SetSessionInfo(const SessionInfo& sessionInfo) {
			_sessionInfo = sessionInfo;
		}

		void HttpCore::SetToken(const std::string& strToken) {
			_sessionInfo.strToken = strToken;
		}

		inline boost::beast::http::verb HttpCore::toHttpVerb(const std::string& method) {
			static const std::unordered_map<std::string, boost::beast::http::verb> kMethodMap = {
				{"get", boost::beast::http::verb::get},
				{"post", boost::beast::http::verb::post},
				{"put", boost::beast::http::verb::put},
				{"delete", boost::beast::http::verb::delete_},
				{"head", boost::beast::http::verb::head},
				{"patch", boost::beast::http::verb::patch},
				{"options", boost::beast::http::verb::options}
			};

			std::string lower = method;
			std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

			auto it = kMethodMap.find(lower);
			if (it != kMethodMap.end()) {
				return it->second;
			}

			return boost::beast::http::verb::unknown;
		}

		inline int HttpCore::toHttpVersion(const std::string& version) {
			static const std::unordered_map<std::string, int> kversionMap = {
				{"1.0", 10},
				{"1.1", 11},
				{"2.0", 20},
			};

			std::string lower = version;
			std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

			auto it = kversionMap.find(lower);
			if (it != kversionMap.end()) {
				return it->second;
			}

			return -1;
		}

	}
}
