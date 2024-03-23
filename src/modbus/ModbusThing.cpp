#include "ModbusThing.h"

#include <common/Logger.h>

#include <uvw/loop.h>
#include <uvw/tcp.h>

ModbusThing::ModbusThing(const ThingInfo& info) :
    Thing(info) {
}

ModbusThing::~ModbusThing() {
    LOG_S(2) << host() << "> destroyed";
}

void ModbusThing::connect() {
    if (!_tcpClient) {
        _tcpClient = uvw::loop::get_default()->resource<uvw::tcp_handle>();
        // Set this as user data
        _tcpClient->data(shared_from_this());
        // Set error handler
        _tcpClient->on<uvw::error_event>([](const uvw::error_event& error, uvw::tcp_handle& tcpClient) {
            auto self = tcpClient.data<ModbusThing>();
            switch (-error.code()) {
            case ECONNREFUSED:
            case ETIMEDOUT:
                LOG_S(1) << self->host() << "> error: " << error.what();
                break;
            default:
                LOG_S(1) << self->host() << "> error: " << error.what();
                break;
            }

            //TODO: we might need to change the order here
            tcpClient.close();
            // Release ourselves from tcpClient
            tcpClient.data(nullptr);
            self->stateSubscriber().on_next(State::Failed);
        });
        // Set connect handler
        _tcpClient->on<uvw::connect_event>([](const uvw::connect_event&, uvw::tcp_handle& tcpClient) {
            auto self = tcpClient.data<ModbusThing>();
            LOG_S(INFO) << self->host() << "> connected";
            self->stateSubscriber().on_next(State::Ready);
        });

        // Start reading after writing
        _tcpClient->on<uvw::write_event>([](const uvw::write_event&, uvw::tcp_handle& tcpClient) {
            tcpClient.read();
        });

        _tcpClient->on<uvw::data_event>([](const uvw::data_event& event, uvw::tcp_handle& tcpClient) {
            auto self = tcpClient.data<ModbusThing>();
            auto response = ModbusResponse::deserialize((uint8_t*)event.data.get(), event.length);
            if (response.exceptionCode != 0) {
                LOG_S(1) << self->host() << "> exception: " << (int)response.exceptionCode;
                self->_exceptionSubject.get_subscriber().on_next(response.exceptionCode);
            } else if (self->_transactionsUserData.contains(response.transactionId)) {
                LOG_S(1) << self->host() << "> received: " << response.transactionId;
                response.userData = self->_transactionsUserData[response.transactionId];
                self->_holdingRegistersSubject.get_subscriber().on_next(response);
            }
            self->_transactionsUserData.erase(response.transactionId);
        });
    }

    _tcpClient->connect(host(), 502);
}

void ModbusThing::readHoldingRegisters(uint8_t unitId, uint16_t address, uint16_t length, uint16_t userData) {
    _request.unitId = unitId;
    _request.memoryAddress = address;
    _request.memoryLength = length;
    const auto& buffer = _request.serialize();
    _transactionsUserData[_request.transactionId] = userData;
    _tcpClient->write((char*)buffer.data(), buffer.size());
}

dynamic_observable<ModbusResponse> ModbusThing::holdingRegistersObservable() const {
    return _holdingRegistersSubject.get_observable();
}

dynamic_observable<uint8_t> ModbusThing::exceptionObservable() const {
    return _exceptionSubject.get_observable();
}
