// http_session.cpp
#include "HttpSession.h"
#include "NetDefine.h"
#include "Log/LogManager.h"

#include <boost/beast/version.hpp>
#include <iostream>

namespace CommonModule {
	namespace Net{

		static CommonModule::Logger::ptr g_logger = LOG_ROOT();

		HttpSession::HttpSession(boost::asio::io_context& io_context, const std::string& host, const std::string& port,
			boost::beast::http::request<boost::beast::http::string_body> req, ResponseHandler handler)
			: _resolver(io_context),
			_stream(io_context),
			_request(std::move(req)),
			_host(host),
			_port(port),
			_handler(std::move(handler)),
			_requestId(req.base()["X-Request-ID"]) {
		}

		void HttpSession::run() {
			_resolver.async_resolve(_host, _port, std::bind(&HttpSession::on_resolve, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		}

		void HttpSession::on_resolve(boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
			if (ec) {
				LOG_INFO(g_logger) << "[RequestId：" << _requestId << "]  " << "on_resolve failed" << " [ " << ec.message() << " ]";
				return;
			}

			_stream.async_connect(results, std::bind(&HttpSession::on_connect, shared_from_this(), std::placeholders::_1));
		}

		void HttpSession::on_connect(boost::system::error_code ec) {
			if (ec) {
				LOG_INFO(g_logger) << "[RequestId：" << _requestId << "]  " << "Connect failed" << " [ " << ec.message() << " ]";
				return;
			}

			boost::beast::http::async_write(_stream, _request, std::bind(&HttpSession::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		}

		void HttpSession::on_write(boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
			if (ec) {
				LOG_INFO(g_logger) << "[RequestId：" << _requestId << "]  " << "Send failed" << " [ " << ec.message() << " ]";
				return;
			}

			boost::beast::http::async_read(_stream, _buffer, _response, std::bind(&HttpSession::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		}

		void HttpSession::on_read(boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
			if (ec) {
				LOG_INFO(g_logger) << "[RequestId：" << _requestId << "]  " << "Read failed" << " [ " << ec.message() << " ]";
				return;
			}

			if (_handler) {
				HttpResponse response;
				response.statusCode = _response.result_int();
				response.strReason = _response.reason();
				response.strBody = _response.body();
				if (auto it = _response.find("X-Request-ID"); it != _response.end()) {
					response.strRequestId = it->value(); // 提取值
				}
				for (const auto& field : _response.base()) { // 遍历所有头字段
					response.headers[field.name_string()] = field.value();
				}
				_handler(std::move(response));
			}

			boost::system::error_code shutdown_ec;
			_stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, shutdown_ec);
		}

	}
}
