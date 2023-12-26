#pragma once

#include <asio/io_context.hpp>

#define _asio Asio::instance()

class Asio {
public:
    static Asio* instance();

    asio::io_context& ioc();

private:
    Asio();

    static inline Asio* _instance = nullptr;

    asio::io_context _ioc;
};
