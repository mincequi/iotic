#include "Asio.h"

Asio* Asio::instance() {
    if (!_instance) _instance = new Asio();
    return _instance;
}

asio::io_context& Asio::ioc() {
    return _ioc;
}

Asio::Asio() {
}

