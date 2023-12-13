#include "HttpClient.h"

#include <iostream>
//#include <boost/beast/http.hpp>
#include <boost/asio/strand.hpp>

#include <common/Logger.h>

void fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

// Objects are constructed with a strand to ensure that handlers do not execute concurrently.
HttpClient::HttpClient(asio::io_context& ioc)
    : _socket(ioc),
      _resolver(asio::make_strand(ioc)),
      _stream(asio::make_strand(ioc)) {
}

void HttpClient::connect(const std::string& host, uint16_t port) {
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host.c_str()), port);
    _socket.async_connect(endpoint, [](const boost::system::error_code& error) {
        LOG_S(INFO) << "error: " << error.message();
    });
}

// Start the asynchronous operation
void HttpClient::run(char const* host, char const* port, char const* target) {
    // Set up an HTTP GET request message
    _request.method(http::verb::get);
    _request.target(target);
    _request.set(http::field::host, host);
    _request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Look up the domain name
    _resolver.async_resolve(host,
                            port,
                            beast::bind_front_handler(&HttpClient::on_resolve, shared_from_this()));
}

void HttpClient::on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec) return fail(ec, "resolve");

    // Set a timeout on the operation
    _stream.expires_after(std::chrono::seconds(30));

    // Make the connection on the IP address we get from a lookup
    _stream.async_connect(results, beast::bind_front_handler(&HttpClient::on_connect, shared_from_this()));

    std::cout << "succes" << std::endl;
}

void HttpClient::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
    if (ec) return fail(ec, "connect");

    // Set a timeout on the operation
    _stream.expires_after(std::chrono::seconds(30));

    // Send the HTTP request to the remote host
    http::async_write(_stream, _request, beast::bind_front_handler(&HttpClient::on_write, shared_from_this()));
}

void HttpClient::on_write(beast::error_code ec, std::size_t /*bytes_transferred*/) {
    if (ec) return fail(ec, "write");

    // Receive the HTTP response
    http::async_read(_stream, _buffer, _response, beast::bind_front_handler(&HttpClient::on_read, shared_from_this()));
}

void HttpClient::on_read(beast::error_code ec, std::size_t /*bytes_transferred*/) {
    if (ec) return fail(ec, "read");

    // Write the message to standard out
    std::cout << _response << std::endl;

    // Gracefully close the socket
    _stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    // not_connected happens sometimes so don't bother reporting it.
    if(ec && ec != beast::errc::not_connected)
        return fail(ec, "shutdown");

    // If we get here then the connection is closed gracefully
}
