#include "HttpDiscovery.h"

#include <common/Logger.h>
#include <things/http/HttpThingFactory.h>

#include <uvw_net/dns_sd/DnsMessage.h>

HttpDiscovery::HttpDiscovery() {
    _udp->on<uvw::udp_data_event>([this](const uvw::udp_data_event& event, uvw::udp_handle& udp) {
        const auto msg = uvw_net::DnsMessage::fromBuffer((uint8_t*)event.data.get(), event.length);
        if (!msg) {
            LOG_S(1) << "Failed parsing packet with size: " << event.length;
            return;
        }
        for (const auto& a : msg->answers) {
            if (a.type != uvw_net::DnsRecordType::SRV ||
                    !a.name.ends_with("._http._tcp.local.") ||
                    !std::holds_alternative<uvw_net::DnsRecordDataSrv>(a.data)) continue;
            const auto& srv = std::get<uvw_net::DnsRecordDataSrv>(a.data);
            const auto host = srv.target.substr(0, srv.target.find("."));
            LOG_S(INFO) << "thing found> host: " << host << ", port: " << srv.port;
            auto thing = HttpThingFactory::from({ ThingInfo::DiscoveryType::Http, host, host });
            // TODO: shall we get a subscriber each time, we want to emit (or keep it as class member)?
            if (thing) thingDiscoveredSubscriber().on_next(thing);
        }
    });
    assert(0 == _udp->bind("0.0.0.0", 5353, uvw::udp_handle::udp_flags::REUSEADDR));
    assert(0 == uv_udp_set_membership(_udp->raw(), "224.0.0.251", NULL, static_cast<uv_membership>(uvw::udp_handle::membership::JOIN_GROUP)));
    assert(0 == _udp->recv());
}

void HttpDiscovery::start(int /*msec*/) {
    // https://mislove.org/teaching/cs4700/spring11/handouts/project1-primer.pdf
    uvw_net::DnsQuestion question;
    question.name = "_http._tcp.local";
    question.type = uvw_net::DnsRecordType::SRV;
    uvw_net::DnsMessage message;
    message.questions.push_back(question);
    const auto buffer = message.toBuffer();

    _udp->send("224.0.0.251", 5353, (char*)buffer.data(), buffer.size());
}

void HttpDiscovery::stop() {
}
