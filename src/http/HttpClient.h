#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

class HttpClient : public std::enable_shared_from_this<HttpClient> {
public:
    explicit HttpClient(asio::io_context& ioc);

    void connect(const std::string& host, uint16_t port);
    void run(char const* host, char const* port, char const* target);
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type);
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);

private:
    tcp::socket _socket;
    tcp::resolver _resolver;
    beast::tcp_stream _stream;
    beast::flat_buffer _buffer; // (Must persist between reads)
    http::request<http::empty_body> _request;
    http::response<http::string_body> _response;
};
