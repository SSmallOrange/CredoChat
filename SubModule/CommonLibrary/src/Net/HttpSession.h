// http_session.hpp
#pragma once

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <functional>

namespace CommonModule {
	namespace Net {
		struct HttpResponse;

		class HttpSession : public std::enable_shared_from_this<HttpSession> {
		public:
			using ResponseHandler = std::function<void(HttpResponse)>;

			HttpSession(boost::asio::io_context& io_context,
				const std::string& host, const std::string& port,
				boost::beast::http::request<boost::beast::http::string_body> req,
				ResponseHandler handler);

			void run();

		private:
			void on_resolve(boost::system::error_code ec,
				boost::asio::ip::tcp::resolver::results_type results);
			void on_connect(boost::system::error_code ec);
			void on_write(boost::system::error_code ec, std::size_t bytes_transferred);
			void on_read(boost::system::error_code ec, std::size_t bytes_transferred);

			boost::asio::ip::tcp::resolver _resolver;
			boost::beast::tcp_stream _stream;
			boost::beast::flat_buffer _buffer;
			boost::beast::http::request<boost::beast::http::string_body> _request;
			boost::beast::http::response<boost::beast::http::string_body> _response;
			std::string _host;
			std::string _port;
			std::string _requestId;
			ResponseHandler _handler;
		};
	}
}
