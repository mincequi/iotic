#include "CoapThing.h"

CoapThing::CoapThing(const std::string& id) : _id(id) {
}

uvw_iot::ThingPtr CoapThing::from(const CoapMessage& message) {
    std::string id((char*)message.token.data(), message.token.size());

    if (!id.empty()) {
        return ThingPtr(new CoapThing(id));
    }
    return nullptr;
}

void CoapThing::fetchProperties() {
    CoapMessage msg;
    msg.code = CoapMessage::Code::Get;
    // No-op
}
