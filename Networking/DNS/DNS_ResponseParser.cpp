//============================================================================
// Name        : DNS_ResponseParser.cpp
// Created on  : December 07, 2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : DNS_ResponseParser
//============================================================================

#include "DNS_ResponseParser.h"

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <atomic>

#include <cmath>
#include <numeric>
#include <numbers>
#include <ostream>

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <array>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <span>
#include <concepts>
#include <variant>

#include <netinet/in.h>

namespace DNS
{
    enum class QueryResponseFlag {
        Query = 0,
        Response = 1
    };

    enum class RecordType : uint16_t {
        A = 1,
        NS = 2,
        CNAME = 5,
        SOA = 6,
        WKS = 11,
        PTR = 12,
        MX = 15,
        AAAA = 28,
        SRV = 33,
        ANY = 255
    };

    enum class ResourceClass : uint16_t {
        Reserved = 0x0000,
        IN = 0x0001,
        Unassigned = 0x0002,
        CH = 0x0003,
        HS = 0x0004
    };

    // TODO: unsigned short --> some type ?? Byte
    struct DNSHeader final {
        /** Identification number: **/
        uint16_t id{0};

        uint8_t rd: 1 {0};     // Recursion desired
        uint8_t tc: 1 {0};     // Truncation flag:
        uint8_t aa: 1 {0};     // Authoritative Answer Flag
        uint8_t opcode: 4 {0}; // Purpose of message
        uint8_t qr: 1 {0};     // Query/response flag: 0 - query and 1 - response

        uint8_t rc: 4 {0}; // Response code:
        uint8_t cd: 1 {0}; // checking disabled
        uint8_t ad: 1 {0}; // authenticated data
        uint8_t z: 1 {0}; // its z! reserved
        uint8_t ra: 1 {0}; // recursion available

        /** Number of question entries: **/
        uint16_t questionCount{0};

        /** Number of answer entries: **/
        uint16_t answerCount{0};

        /** Number of authority entries: **/
        uint16_t authorityCount{0};

        /** Number of resource entries: **/
        uint16_t additionalCount{0};

        [[nodiscard]]
        uint16_t getId() const noexcept {
            return static_cast<uint16_t>(htons(id));
        }

        [[nodiscard]]
        uint16_t getQuestionsCount() const noexcept {
            return static_cast<uint16_t>(htons(questionCount));
        }

        [[nodiscard]]
        uint16_t getAnswersCount() const noexcept {
            return static_cast<uint16_t>(htons(answerCount));
        }

        [[nodiscard]]
        uint16_t getAuthorityCount() const noexcept {
            return static_cast<uint16_t>(htons(authorityCount));
        }

        [[nodiscard]]
        uint16_t getAdditionalCount() const noexcept {
            return static_cast<uint16_t>(htons(additionalCount));
        }

        [[nodiscard]]
        uint16_t getResponseCode() const noexcept {
            return static_cast<uint16_t>(htons(rc));
        }

        [[nodiscard]]
        inline bool isRecursionDesired() const noexcept {
            return static_cast<bool>(rd);
        }

        [[nodiscard]]
        inline bool isRecursionAvailable() const noexcept {
            return static_cast<bool>(ra);
        }

        [[nodiscard]]
        inline bool isAuthoritative() const noexcept {
            return static_cast<bool>(aa);
        }

        [[nodiscard]]
        inline bool isTruncated() const noexcept {
            return static_cast<bool>(tc);
        }

        [[nodiscard]]
        inline bool isQuery() const noexcept {
            return not static_cast<bool>(qr);
        }
    };

#pragma pack(push, 1)
    struct QuestionParams final {
        uint16_t qType{0};
        uint16_t qClass{0};
    };
#pragma pack(pop)


#pragma pack(push, 1)
    struct AnswerParams final {
        uint16_t type{0};
        uint16_t cls{0};
        uint32_t ttl{0};
        uint16_t data_len{0};
    };
#pragma pack(pop)


#pragma pack(push, 1)
    struct SOARecordData  final {
        uint32_t serial {0};
        uint32_t refresh {0};
        uint32_t retry {0};
        uint32_t expire {0};
        uint32_t minimum {0};

    };
#pragma pack(pop)

    struct SOAResource final {
        std::string masterName {};
        std::string responsibleName {};
        SOARecordData params {};
    };

    struct AAAAResource final {
        std::string data {};
    };

    struct DNSQuestion final {
        std::string name{};
        QuestionParams params{};

        [[nodiscard]]
        RecordType getType() const noexcept {
            return static_cast<RecordType>(htons(params.qType));
        }

        [[nodiscard]]
        ResourceClass getClass() const noexcept {
            return static_cast<ResourceClass>(htons(params.qClass));
        }
    };

    struct DNSAnswer final {
        std::string name {};
        AnswerParams params {};
        std::variant<std::string,
                AAAAResource,
                SOAResource> resourceData {};

        [[nodiscard]]
        RecordType getType() const noexcept {
            return static_cast<RecordType>(htons(params.type));
        }

        [[nodiscard]]
        ResourceClass getClass() const noexcept {
            return static_cast<ResourceClass>(htons(params.cls));
        }

        [[nodiscard]]
        uint32_t getTTL() const noexcept {
            return static_cast<uint32_t>(htonl(params.ttl));
        }

        [[nodiscard]]
        uint16_t getDataLength() const noexcept {
            return static_cast<uint16_t>(htons(params.data_len));
        }
    };

    struct DNSMessage {
        DNSHeader header {};
        DNSQuestion question {};
        std::vector<DNS::DNSAnswer> answers {};
    };

    size_t extractName(const std::vector<uint8_t> &bytes, size_t &offset, std::string &output) {
        const uint16_t labelLength = bytes[offset];
        if (0 == labelLength)
            return 0;
        offset++;

        if (labelLength >= 192) {
            size_t jumpTo = bytes[offset++];
            return extractName(bytes, jumpTo, output);
        } else {
            output.append(reinterpret_cast<const char *>(&bytes[offset]), labelLength).push_back('.');
            size_t tmp = offset + labelLength;
            auto steps = extractName(bytes, tmp, output);
            offset = tmp;
            return steps + labelLength + 1;
        }
    };
}

namespace Encoding
{
    static constexpr uint8_t hexCode(unsigned char symbol) noexcept {
        if (symbol >= '0' && symbol <= '9')
            return symbol - '0';
        if (symbol >= 'A' && symbol <= 'F')
            return symbol - 'A' + 10;
        if (symbol >= 'a' && symbol <= 'f')
            return symbol - 'a' + 10;
        return 0;
    }

    static constexpr uint8_t hex2UChar(std::string_view hexValue) noexcept {
        return 16 * hexCode(hexValue[0]) + hexCode(hexValue[1]);
    }

    std::vector<uint8_t> hex2Bytes(std::string_view hexString) noexcept {
        std::vector<uint8_t> bytes;
        bytes.reserve(hexString.length()/2);
        for (size_t length = hexString.length(), i = 0; i < length; i += 2)
            bytes.push_back(hex2UChar(hexString.substr(i, 2)));
        return bytes;
    }
}

namespace Printing
{
    using namespace DNS;

    [[nodiscard]]
    std::string recordType2String(RecordType type) noexcept {
        switch (type) {
            case RecordType::A :     return std::string{"A"};
            case RecordType::NS :    return std::string{"NS"};
            case RecordType::CNAME : return std::string{"CNAME"};
            case RecordType::SOA :   return std::string{"SOA"};
            case RecordType::WKS :   return std::string{"WKS"};
            case RecordType::PTR :   return std::string{"PTR"};
            case RecordType::MX :    return std::string{"MX"};
            case RecordType::AAAA :  return std::string{"AAAA"};
            case RecordType::SRV :   return std::string{"SRV"};
            case RecordType::ANY :   return std::string{"ANY"};
            default: return std::string{"UNKNOWN"};
        }
    }

    [[nodiscard]]
    std::string resourceClass2String(ResourceClass type) noexcept {
        switch (type) {
            case ResourceClass::Reserved :   return std::string{"Reserved"};
            case ResourceClass::IN :         return std::string{"IN"};
            case ResourceClass::Unassigned : return std::string{"Unassigned"};
            case ResourceClass::CH :         return std::string{"CH"};
            case ResourceClass::HS :         return std::string{"HS"};
            default: return std::string{"UNKNOWN"};
        }
    }

    [[nodiscard]]
    std::string opcode2String(uint8_t opcode) noexcept {
        switch (opcode) {
            case 0:  return std::string{"QUERY"};
            case 1:  return std::string{"IQUERY"};
            case 2:  return std::string{"STATUS"};
            default: return std::string{"UNKNOWN"};
        }
    }

    [[nodiscard]]
    std::string responseCode2String(uint8_t responseCode) noexcept {
        switch (responseCode) {
            case 0:  return std::string{"NOERROR"};
            case 1:  return std::string{"FORMERR"};
            case 2:  return std::string{"SERVFAIL"};
            case 3:  return std::string{"NXDOMAIN"};
            case 4:  return std::string{"NOTIMP"};
            case 5:  return std::string{"REFUSED"};
            default: return std::string{"UNKNOWN"};
        }
    }

    void printHeader(const DNSHeader& dnsHeader)
    {
        std::cout << ";; ->>HEADER<<- opcode: "
                  << opcode2String(dnsHeader.opcode)
                  << ", status: " << responseCode2String(dnsHeader.getResponseCode())
                  << ", id: " << dnsHeader.getId()<< "\n";

        std::cout << ";; flags:"
                  << (!dnsHeader.isQuery() ? " qr" : "")
                  << (dnsHeader.isAuthoritative()? " aa" : "")
                  << (dnsHeader.isRecursionDesired()? " rd" : "")
                  << (dnsHeader.isRecursionAvailable()? " ra" : "")
                  << (dnsHeader.isTruncated()? " tc" : "");

        std::cout << "; QUERY: " << dnsHeader.getQuestionsCount()
                  << ", ANSWER: " << dnsHeader.getAnswersCount()
                  << ", AUTHORITY: " << dnsHeader.getAuthorityCount()
                  << ", ADDITIONAL: " << dnsHeader.getAdditionalCount() << "\n";
    }


    void printQuestionSection(const DNSQuestion& dnsQuestion) {
        std::cout << "\n;; QUESTION SECTION:\n;" << dnsQuestion.name
                  << "\t\t" << resourceClass2String(dnsQuestion.getClass()) << "\t"
                  << recordType2String(dnsQuestion.getType()) << "\n";
    }

    [[nodiscard]]
    std::string answerDataToStrAAAA(const AAAAResource& obj) {
        std::string humanFriendlyStr;
        unsigned char tmp[2]{};
        char label[4]{};
        constexpr std::string_view hex = "0123456789abcdef";
        for (size_t b = 0, pos = 0, i = 0; i < obj.data.length(); ++i) {
            tmp[++b % 2] = obj.data[i];
            if (b % 2 == 0 && (tmp[0] || tmp[1])) {
                label[0] = hex[tmp[1] >> 4]; label[1] = hex[tmp[1] & 0xf];
                label[2] = hex[tmp[0] >> 4]; label[3] = hex[tmp[0] & 0xf];
                for (int i = 0; i < std::ssize(label) && '0' == label[i]; ++i, ++pos) {}
                humanFriendlyStr.append(label, pos, 4 - pos).push_back(':');
                pos = 0;
            }
            else if (b % 2 == 0 && humanFriendlyStr.length() >= 2 && humanFriendlyStr[humanFriendlyStr.size() -2] != ':') {
                humanFriendlyStr.push_back(':');
            }
        }
        if (':' == humanFriendlyStr.back())
            humanFriendlyStr.pop_back();
        return humanFriendlyStr;
    }

    /** Visitor: Encapsulates DNS resource print logic: **/
    struct ResourceVisitor {
        void operator()(const std::string& s) {
            std::cout << s << "\n";
        }
        void operator()(const SOAResource& obj) {
            std::cout << obj.masterName << " " << obj.responsibleName << "  "
                      << htonl(obj.params.serial) << "  "
                      << htonl(obj.params.refresh) << "  "
                      << htonl(obj.params.retry) << "  "
                      << htonl(obj.params.expire) << "  "
                      << htonl(obj.params.minimum) << "  "
                      << "]\n";
        }
        void operator()(const AAAAResource& obj) {
            std::cout << answerDataToStrAAAA(obj) << "\n";
        }
    };

    void printAnswerSection(const std::vector<DNSAnswer>& answers) {
        std::cout << "\n;; ANSWER SECTION:\n";
        size_t minLen = std::numeric_limits<size_t>::max();
        for (const auto& answer: answers) {
            // FIXME: dirty hack to align print
            minLen = std::min(minLen, answer.name.length());
            std::cout << answer.name << "\t" << ((answer.name.length() < 2 * minLen) ? "\t" : "")
                      << answer.getTTL() << "\t"
                      << resourceClass2String(answer.getClass()) << "\t"
                      << recordType2String(answer.getType()) << "\t";
            std::visit(ResourceVisitor{}, answer.resourceData);
        }
    }

    void printDnsMessage(const DNS::DNSMessage& message) {
        Printing::printHeader(message.header);
        Printing::printQuestionSection(message.question);
        Printing::printAnswerSection(message.answers);
    }
}

// https://pastebin.com/nm90xNhs
// https://gist.github.com/fffaraz/9d9170b57791c28ccda9255b48315168

void DNS_ResponseParser::TestAll()
{
    [[maybe_unused]]
    std::string_view hexData = "a01d81800001000100000000076578616d706c6503636f6d0000010001c00c0001000100001bbc00045db"
                               "8d822";
    [[maybe_unused]]
    std::string_view hexData2 = "4be885000001000000010001076578616d706c6503636f6d0000010001c00c0006000100000e10003504"
                                "646e733103703035056e736f6e65036e6574000a686f73746d6173746572c032621672610000a8c00000"
                                "1c200012750000000e1000002904d0000000000000";
    [[maybe_unused]]
    std::string_view hexData3 = "ef2781800001000100000001076578616d706c6503636f6d0000010001c00c000100010000001e00045d"
                                "b8d8220000291000000000000000";
    [[maybe_unused]]
    std::string_view google = "256d8180000100060000000106676f6f676c6503636f6d0000010001c00c000100010000001e000440e9a16"
                              "5c00c000100010000001e000440e9a171c00c000100010000001e000440e9a18bc00c000100010000001e00"
                              "0440e9a164c00c000100010000001e000440e9a166c00c000100010000001e000440e9a18a00002904d0000"
                              "000000000";

    [[maybe_unused]]
    std::string_view googleAAA = "58438180000100040000000106676f6f676c6503636f6d00001c0001c00c001c00010000001e00102a00145"
                                 "040100c020000000000000066c00c001c00010000001e00102a00145040100c020000000000000071c00c00"
                                 "1c00010000001e00102a00145040100c02000000000000008ac00c001c00010000001e00102a00145040100"
                                 "c02000000000000008b00002904d0000000000000";


    [[maybe_unused]]
    std::string_view test1 = "9b4c84000001000200000000037777770a636c6f7564666c61726503636f6d0000010001c00c000"
                             "100010000012c000468107c60c00c000100010000012c000468107b60";
    [[maybe_unused]]
    std::string_view test2 = "7ebd84000001000200000000037777770a636c6f7564666c61726503636f6d00001c0001c00c001c00010000012c"
                             "001026064700000000000000000068107c60c00c001c00010000012c001026064700000000000000000068107b60";
    // CNAME:
    [[maybe_unused]]
    std::string_view test3 = "762081800001000200000000037777770773706f7469667903636f6d0000010001c00c0005000100000102001f12"
                             "656467652d7765622d73706c69742d67656f096475616c2d67736c62c010c02d000100010000006c000423bae019";
    [[maybe_unused]]
    std::string_view test4 = "619381800001000100000000076578616d706c6503636f6d00001c0001c00c001c00"
                             "0100001bf9001026062800022000010248189325c81946";

    [[maybe_unused]]
    std::string_view soa = "af978180000100010000000106676f6f676c6503636f6d0000060001c00c000600010000000a0026036e7331c"
                           "00c09646e732d61646d696ec00c19ddcc490000038400000384000007080000003c00002904d0000000000000";

    const std::vector<uint8_t> bytes = Encoding::hex2Bytes(soa);

    //-------------------------------------------------------------------------------------------------------//

#if 1
    DNS::DNSMessage message {};
    memcpy(&message.header, bytes.data(), sizeof(DNS::DNSHeader));
    size_t offset = sizeof(DNS::DNSHeader);

    DNS::extractName(bytes, offset, message.question.name);
    memcpy(&message.question.params, &bytes[++offset] , sizeof(DNS::QuestionParams));
    offset += sizeof(DNS::QuestionParams);

    const uint16_t answersCount = message.header.getAnswersCount();
    message.answers.reserve(answersCount);

    /** Extract and parse DNS answers: **/
    for (uint16_t n = 0; n < answersCount; ++n) {
        DNS::DNSAnswer& dnsAnswer = message.answers.emplace_back();
        DNS::extractName(bytes, offset, dnsAnswer.name);
        memcpy(&dnsAnswer.params, &bytes[offset] , sizeof(DNS::AnswerParams));
        offset += sizeof(DNS::AnswerParams);

        uint32_t dataLen {dnsAnswer.getDataLength()};
        const DNS::RecordType type = dnsAnswer.getType();
        if (DNS::RecordType::A == type) {
            /** Address resource record. **/
            auto& resource = dnsAnswer.resourceData.emplace<std::string>();
            while (dataLen--)
                resource.append(std::to_string(bytes[offset++])).push_back('.');
            resource.pop_back();
        } else if (DNS::RecordType::CNAME == type) {
            /** Canonical Name resource record.: **/
            DNS::extractName(bytes, offset, dnsAnswer.resourceData.emplace<std::string>());
        } else if (DNS::RecordType::SOA == type) {
            /** Start Of Authority resource record. **/
            DNS::SOAResource& soaResource = dnsAnswer.resourceData.emplace<DNS::SOAResource >();
            DNS::extractName(bytes, offset, soaResource.masterName);
            DNS::extractName(bytes, offset, soaResource.responsibleName);
            memcpy(&soaResource.params, &bytes[offset] , sizeof(DNS::SOARecordData));
        } else if (DNS::RecordType::AAAA == type) {
            /** AAAA IPv6 resource record. **/
            DNS::AAAAResource& resource = dnsAnswer.resourceData.emplace<DNS::AAAAResource>();
            while (dataLen--)
                resource.data.push_back(bytes[offset++]);
        } else {
            /** TODO: All another.... didn't support everything - I hope it's enough for the test task :-)) **/
            auto& resource = std::get<std::string>(dnsAnswer.resourceData);
            while (dataLen--)
                resource.push_back(bytes[offset++]);
        }
    }

    Printing::printDnsMessage(message);

    /*
    for (size_t i = offset; i < bytes.size(); i++) {
        const auto b = bytes[i];
        std::cout << "[" << i++ << "]: " << b << "   " << (int) b << std::endl;
    }

    for (int i = offset; i < (dataLen + offset); ++i) {
        const auto b = bytes[i];
        std::cout << "[" << i << "]: " << b << "   " << (int) b << std::endl;
    }
    */

#endif

#if 0
    for (size_t i = 0; const auto b: bytes) {
        //std::cout << "[" << i++ << "]: " << b << "   " << (int) b << std::endl;
    }


    /*
    {
        size_t offset = 12;
        const auto name = extractAnswerNameEx(bytes, offset);
        std::cout << "Name: " << name << "    Offset: " << offset << std::endl;
    }

    {
        size_t offset = 33;
        const auto name = extractAnswerNameEx(bytes, offset);
        std::cout << "Name: " << name << "    Offset: " << offset << std::endl;
    }*/

    /*
    {
        size_t offset = 45;
        const auto name = extractAnswerNameEx(bytes, offset);
        std::cout << "Name: " << name << "    Offset: " << offset << std::endl;
    }*/

    {
        size_t offset = 76;
        const auto name = extractAnswerNameEx(bytes, offset);
        std::cout << "Name: " << name << "    Offset: " << offset << std::endl;
    }

#endif


#if 0
    for (size_t i = 0; const auto b: bytes) {
        //std::cout << "[" << i++ << "]: " << b << "   " << (int) b << std::endl;
    }
#endif
    /*
    std::string data;
    data.reserve(256);
    uint16_t labelLength = bytes[offset++];

    while (labelLength > 0 && 255 >= labelLength)
    {
        std::cout << "labelLength = " << labelLength << std::endl;
        if (labelLength >= 192) {
            localOffset = bytes[offset++];
            std::cout << "jumpToPos = " << localOffset << std::endl;
            labelLength = bytes[localOffset];

        }
        else {
            std::cout << "SEQ: " << labelLength << ", offset: " << offset << std::endl;
            data.append(reinterpret_cast<const char*>(&bytes[offset]), labelLength).push_back('.');
            offset += labelLength;
            labelLength = bytes[offset++];
        }
    }*/


    /*
    while (labelLength > 0 && 255 >= labelLength)
    {

        if (labelLength >= 192) {
            size_t jumpToPos = bytes[offset++];
            std::cout << "jumpToPos = " << jumpToPos << std::endl;
            const auto name = extractSequencedDNSName(bytes, jumpToPos);
            data.append(name);
            std::cout << "JUMP NAME: " << name << std::endl;
            std::cout << "NEX POS:  " << jumpToPos << std::endl;
            labelLength = 0;
        } else {
            data.append(reinterpret_cast<const char *>(&bytes[offset]), labelLength).push_back('.');
            offset += labelLength;
            labelLength = bytes[offset++];
        }
    }
    data.shrink_to_fit();
    return data;
     */


    /*
    std::string res;
    unsigned char tmp[2] {};
    constexpr std::string_view hex = "0123456789abcdef";
    for (uint16_t b = 0; const unsigned char s: answers.front().data) {
        tmp[++b % 2] = s;
        if (b % 2 == 0 && (tmp[0] || tmp[1])) {
            res.append(1, hex[tmp[1] >> 4]).append(1, hex[tmp[1] & 0xf])
               .append(1, hex[tmp[0] >> 4]).append(1, hex[tmp[0] & 0xf]).push_back(':');
        }
    }
    bool leading = true;
    size_t pos = 0;
    for (size_t i = 0; i < res.length(); ++i) {
        if ('0' == res[i]) {
            if (!leading)
                res[pos++] = res[i];
        } else {
            leading = (':' == res[i]);
            res[pos++] = res[i];
        }
    }
    res.resize(pos);
    */

    //std::cout << answerDataToStrAAAA() << std::endl;


    // 2606:2800:220:1:248:1893:25c8:1946
    /*
    for (const auto& answer: answers)
    {
        std::string res;
        unsigned char tmp[2]{};
        char label[4]{};
        constexpr std::string_view hex = "0123456789abcdef";
        for (uint16_t b = 0, pos = 0; const unsigned char s: answer.data) {
            tmp[++b % 2] = s;
            if (b % 2 == 0 && (tmp[0] || tmp[1])) {
                label[0] = hex[tmp[1] >> 4]; label[1] = hex[tmp[1] & 0xf];
                label[2] = hex[tmp[0] >> 4]; label[3] = hex[tmp[0] & 0xf];
                for (size_t i = 0; i < std::size(label) && '0' == label[i]; ++i, ++pos) {}
                res.append(label, pos, 4 -pos).push_back(':');
                std::cout << std::string(label, pos, 4) << std::endl;
                pos = 0;
            }
            else if (b % 2 == 0 && res.length() >= 2 && res[res.size() -2] != ':') {
                res.push_back(':');
            }
        }
        if (':' == res.back())
            res.pop_back();

        // std::cout << res << std::endl;
    }*/

}
