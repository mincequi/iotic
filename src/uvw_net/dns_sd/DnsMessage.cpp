#include "DnsMessage.h"

#include <cstring>
#include <arpa/inet.h>

#include <magic_enum.hpp>

#include "DnsParser.h"
#include "DnsRecordDataSrv.h"

namespace uvw_net {

std::vector<std::string> split(const std::string& str, char delimiter) {
    size_t begin = 0;
    size_t end;
    std::vector<std::string> labels;

    while ((end = str.find(delimiter, begin)) != std::string::npos) {
        labels.push_back(str.substr(begin, end - begin));
        begin = end + 1;
    }
    labels.push_back(str.substr(begin));

    return labels;
}

class DnsRecordParser {
public:
    DnsRecordParser(const uint8_t* begin, const uint8_t* end) :
        _begin(begin),
        _end(end) {
        assert(_begin < _end);
    }

	std::tuple<size_t, DnsQuestion> parseQuestion(size_t offset) {
		if (_begin + offset + DnsQuestion::minSize > _end) {
			return { 0, {} };
		}
		auto [rsize, name] = DnsParser::parseName(_begin, _begin + offset, _end);
		if (!rsize) {
			return { 0, {} };
		}
		if (_begin + offset + rsize + 4 > _end) {
			return { 0, {} };
		}

		DnsQuestion r;
		r.name = std::move(name);
		r.type = read<DnsRecordType>((uint16_t*)(_begin + offset + rsize));
		rsize += sizeof(uint16_t);
		r.cls = read<DnsRecordClass>((uint16_t *)(_begin + offset + rsize));
		rsize += sizeof(uint16_t);

		return { rsize, std::move(r) };
	}

	std::tuple<size_t, DnsAnswer> parseAnswer(size_t offset) {
		if (_begin + offset + DnsAnswer::minSize > _end) {
			return { 0, {} };
		}
		auto [rsize, name] = DnsParser::parseName(_begin, _begin + offset, _end);
		if (!rsize) {
			return { 0, {} };
		}
		if (_begin + offset + rsize + 10 > _end) {
			return { 0, {} };
		}

		DnsAnswer r;
		r.name = std::move(name);
		r.type = read<DnsRecordType>((uint16_t*)(_begin + offset + rsize));
		rsize += sizeof(uint16_t);
		r.cls = read<DnsRecordClass>((uint16_t *)(_begin + offset + rsize));
		rsize += sizeof(uint16_t);
		r.ttl = ntohl(*(uint32_t *)(_begin + offset + rsize));
		rsize += sizeof(uint32_t);
		const auto dataSize = ntohs(*(uint16_t *)(_begin + offset + rsize));
		rsize += sizeof(uint16_t);
		if (_begin + offset + rsize + dataSize > _end) {
			return { 0, {} };
		}
		// resolve data
		auto data = readDnsRecordData(r.type, _begin + offset + rsize, dataSize);
		if (!data) return { 0, {} };
		r.data = std::move(*data);
		rsize += dataSize;

		return { rsize, std::move(r) };
	}

private:
    template <class T>
    T read(const uint16_t* p) {
        return magic_enum::enum_cast<T>(ntohs(*p)).value_or(T::Invalid);
    }

    std::optional<DnsRecordData> readDnsRecordData(DnsRecordType type, const uint8_t* dataPtr, uint16_t dataSize) {
        switch (type) {
        case DnsRecordType::A: {
            //
            // +0x00 IPv4 (4 bytes)
            //
            if (dataSize != sizeof(AData)) {
                break;
            }
            return *(AData*)dataPtr;
        } break;
            /* TODO
            case DnsRecordType::AAAA: {
                //
                // +0x00 IPv6 (16 bytes)
                //
                AAAAData data;
                if (dataLength != data.size()) {
                    break;
                }
                memcpy(&data[0], dataPtr, data.size());
                return { true, std::move(data) };
            } break;*/
        case DnsRecordType::SOA: {
            //
            // +0x00 Primary name server (VARIANT, 2 bytes at least)
            //	...
            // +0x02 Responsible authority's mailbox (VARIANT, 2 bytes at least)
            //  ...
            // +0x04 Serial number
            // +0x08 Refresh interval
            // +0x0C Retry interval
            // +0x10 Expire limit
            // +0x14 Minimum TTL
            //
            if (dataSize < 0x18) {
                return {};
            }
            size_t offset = 0;

            auto [primaySvrLen, primarySvr] = DnsParser::parseName(_begin, dataPtr + offset, _end);
                    if (!primaySvrLen || dataSize < 0x16 + primaySvrLen) {
                return {};
            }
            offset += primaySvrLen;

            auto [mailboxLen, mailbox] = DnsParser::parseName(_begin, dataPtr + offset, _end);
                    if (!mailboxLen || dataSize != 0x14 + primaySvrLen + mailboxLen) {
                return {};
            }
            offset += mailboxLen;

            // read other fields
            SOAData r;
            r.primaryServer = std::move(primarySvr);
            r.administrator = std::move(mailbox);
            r.serialNo = ntohl(*(uint16_t *)(dataPtr + offset));
            r.refresh = ntohl(*(uint16_t *)(dataPtr + offset + sizeof(uint32_t)));
            r.retry = ntohl(*(uint16_t *)(dataPtr + offset + sizeof(uint32_t) * 2));
            r.expire = ntohl(*(uint16_t *)(dataPtr + offset + sizeof(uint32_t) * 3));
            r.defaultTtl = ntohl(*(uint16_t *)(dataPtr + offset + sizeof(uint32_t) * 4));
            return std::move(r);
        } break;
        case DnsRecordType::MX: {
            //
            // +0x00 Preference
            // +0x02 Exchange Server Name
            //
            MXData data;
            if (dataSize < sizeof(uint16_t) * 2) {
                return {};
            }
            data.preference = ntohs(*(uint16_t *)dataPtr);
            auto [nameSize, name] = DnsParser::parseName(_begin, dataPtr + sizeof(uint16_t), _end);
                    if (nameSize + sizeof(uint16_t) != dataSize) {
                return {};
            }
            data.exchange = std::move(name);
            return std::move(data);
        } break;
        case DnsRecordType::NS:
        case DnsRecordType::CNAME:
        case DnsRecordType::PTR: {
            //
            // +0x00 Host name
            //
            auto [nameSize, name] = DnsParser::parseName(_begin, dataPtr, _end);
                    if (nameSize != dataSize) {
                return {};
            }
            return std::move(name);
        } break;
        case DnsRecordType::TXT: {
            //
            // +0x00 Text length
            // +0x01 Text (VARIANT length)
            //
            // TODO
            if (dataSize <= 1 /*|| dataLength != *(uint8_t *)dataPtr + 1*/) {
                return {};
            }
            TXTData data;
            data.size = *(uint8_t *)dataPtr;
            data.txt = std::string((char *)dataPtr + 1, dataSize - 1);
            return std::move(data);
        } break;
        case DnsRecordType::SRV:
            return DnsRecordDataSrv::fromBuffer(_begin, dataPtr, dataSize);
        default: {
            // Unsupported record type (for now)
            OctectStreamData payload;
            if (dataSize) {
                payload.resize(dataSize);
                if (payload.empty()) {
                    return {};
                }
                memcpy(&payload[0], dataPtr, dataSize);
            }
            return std::move(payload);
        } break;
        }
        return {};
    }


	const uint8_t *_begin = nullptr;
    const uint8_t *_end = nullptr;
};

std::optional<DnsMessage> DnsMessage::fromBuffer(const uint8_t* buf, size_t bufSize) {
    // +------------+
    // | Header     |
    // +------------+
    // | Question   |
    // +------------+
    // | Answer     |
    // +------------+
    // | Authority  |
    // +------------+
    // | Additional |
    // +------------+

    if (bufSize < 12) {
        return {};
    }

    DnsMessage message;
    const auto header = (const DnsHeader*)buf;
    memcpy(&message.header, header, sizeof(DnsHeader));
    message.header.transactionId = ntohs(header->transactionId);

    size_t offset = sizeof(DnsHeader);
    auto questionCount = ntohs(*(uint16_t*)(buf + offset));
    auto anwserCount = ntohs(*(uint16_t*)(buf + offset + sizeof(uint16_t)));
    auto authoriyCount = ntohs(*(uint16_t*)(buf + offset + sizeof(uint16_t) * 2));
    auto additonalCount = ntohs(*(uint16_t*)(buf + offset + sizeof(uint16_t) * 3));
    offset += sizeof(uint16_t) * 4;

    DnsRecordParser parser(buf, buf + bufSize);
    for (auto i = 0; i < questionCount; ++i) {
        auto [qsize, question] = parser.parseQuestion(offset);
                if (!qsize) {
            return {};
        }
        message.questions.emplace_back(std::move(question));
        offset += qsize;
    }
    if (header->isResponse) {
        for (auto i = 0; i < anwserCount; ++i) {
            auto [rsize, record] = parser.parseAnswer(offset);
                    if (!rsize) {
                return {};
            }
            offset += rsize;
            message.answers.emplace_back(std::move(record));
        }
        for (auto i = 0; i < authoriyCount; ++i) {
            auto [rsize, record] = parser.parseAnswer(offset);
                    if (!rsize) {
                return {};
            }
            offset += rsize;
            message.authorityAnwsers.emplace_back(std::move(record));
        }
        for (auto i = 0; i < additonalCount; ++i) {
            auto [rsize, record] = parser.parseAnswer(offset);
                    if (!rsize) {
                return {};
            }
            offset += rsize;
            message.additionalAnwsers.emplace_back(std::move(record));
        }
    }

    return message;
};

std::vector<uint8_t> DnsMessage::toBuffer() const {
    std::vector<uint8_t> buffer;
    buffer.resize(sizeof(DnsHeader) + 4 * 2); // make room for header and counts
    std::memcpy(buffer.data(), &header, sizeof(DnsHeader));
    const auto questionCount = htons(questions.size());
    const auto anwserCount = htons(answers.size());
    const auto authoriyCount = htons(authorityAnwsers.size());
    const auto additonalCount = htons(additionalAnwsers.size());
    std::memcpy(buffer.data() + 4, &questionCount, 2);
    std::memcpy(buffer.data() + 6, &anwserCount, 2);
    std::memcpy(buffer.data() + 8, &authoriyCount, 2);
    std::memcpy(buffer.data() + 10, &additonalCount, 2);

    uint16_t offset = 12;
    for (const auto& q : questions) {
        const auto labels = split(q.name, '.');
        for (const auto& l : labels) {
            buffer.resize(offset + l.size() + 1);
            *(buffer.data() + offset) = (uint8_t)l.size();
            std::memcpy(buffer.data() + offset + 1, l.data(), l.size());
            offset += l.size()+1;
        }
        buffer.resize(offset + 1);
        *(buffer.data() + offset) = (uint8_t)0;
        offset += 1;
        buffer.resize(offset + 4);
        const auto type = htons((uint16_t)q.type);
        const auto cls = htons((uint16_t)q.cls);
        std::memcpy(buffer.data() + offset, &type, 2);
        std::memcpy(buffer.data() + offset + 2, &cls, 2);
    }

    return buffer;
}

} // namespace uvw_net
