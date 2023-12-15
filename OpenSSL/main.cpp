//============================================================================
// Name        : OpenSSL.cpp
// Created on  : 15.07.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : OpenSSL C++ project
//============================================================================

#include <optional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <tuple>
#include <cmath>
#include <exception>
#include <random>
#include <thread>
#include <future>
#include <mutex>
#include <atomic>

#include <cmath>
#include <numeric>
#include <numbers>
#include <ostream>

#include <utility>
#include <vector>
#include <any>
#include <list>
#include <deque>
#include <map>
#include <algorithm>
#include <array>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <span>

#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/buffer.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/crypto.h>

#include "CertificateGenerator.h"


namespace
{
    constexpr std::string_view certPath {
            "/home/andtokm/DiskS/Chromium/chromium/src/net/third_party/nist-pkits/certs/GoodCACert.crt"};

    constexpr std::string_view certPath1 {
            "/home/andtokm/DiskS/Chromium/chromium/src/net/third_party/nist-pkits/certs/GoodCACert.crt"};

    constexpr std::string_view validPem {
            "/home/andtokm/DiskS/Projects/ClickHouse/tests/config/dhparam.pem"};

    constexpr std::string_view selfSignedCert {"/home/andtokm/tmp/OpenSSL/certificate.crt"};
}

namespace Types
{
    using ptrBigNumber = std::unique_ptr<BIGNUM, decltype(&::BN_free)>;
    using ptrRSA = std::unique_ptr<RSA, decltype(&::RSA_free)>;
    using ptrBIO = std::unique_ptr<BIO, decltype(&::BIO_free)>;
    using ptrPKEY = std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;
    using ptrAsnInteger = std::unique_ptr<ASN1_INTEGER, decltype(&::ASN1_INTEGER_free)>;

    struct CertificateDeleter {
        void operator()(BIO* bio) const {
            if (bio)
                ::BIO_free(bio);
        }

        void operator()(X509* x509) const {
            if (x509)
                ::X509_free(x509);
        }
    };

    auto x509Deleter = [] (X509* ptr) {
        if (ptr)
            X509_free(ptr);
    };

    using ptrCert509 = std::unique_ptr<X509, CertificateDeleter>;
    using ptrCert509Ex = std::unique_ptr<X509, decltype(&::X509_free)>;
}


namespace Utilities
{
    //template<typename T = char>
    [[nodiscard]]
    std::vector<char8_t> readCertificate(std::string_view path) noexcept {
        std::vector<char8_t> data {};
        if (std::fstream file(path.data(), std::ios::in | std::ios::binary); file.is_open() && file.good()) {
            file.seekg(0, std::ios_base::end);
            const auto bytesLength{file.tellg()};
            file.seekg(0, std::ios_base::beg);

            data.resize(bytesLength);
            file.read(reinterpret_cast<char *>(data.data()), bytesLength);
        }
        return data;
    }

    [[nodiscard]]
    Types::ptrCert509 getCertificateCrt(std::string_view path) noexcept {
        const auto certData = readCertificate(path);
        const auto *data = reinterpret_cast<const unsigned char*>(certData.data());
        return {d2i_X509(nullptr, &data, std::ssize(certData)), Types::CertificateDeleter{}};
    }
}

namespace Experiments
{
    using namespace Utilities;
    using namespace Types;

    void TestReadCertificate() {
        const auto certData = readCertificate(selfSignedCert);
        const auto *data = reinterpret_cast<const unsigned char*>(certData.data());

        ptrCert509 cert {d2i_X509(nullptr, &data, std::ssize(certData)), CertificateDeleter{}};
        if (!cert) {
            std::cout << "Failed to get certificate '" << selfSignedCert << "'\n";

            const size_t code = ERR_get_error();
            std::cout << "Error  : " << code << std::endl;
            std::cout << "Reason : " << ERR_reason_error_string(code) << std::endl;
            std::cout << "Strings: " << ERR_func_error_string(code) << std::endl;
            std::cout << "Lib err: " << ERR_lib_error_string(code) << std::endl;



            ERR_print_errors_fp (stdout);
        }
    }

    void TestCertificate() {
        const ptrCert509 cert { getCertificateCrt(certPath) };
        if (!cert) {
            std::cout << "Unable to parse certificate in memory" << std::endl;
            return;
        }

        int version = ((int) X509_get_version(cert.get())) + 1;
        std::cout << "version = " << version << std::endl;
    }

    X509* openPemFile(std::string_view path)
    {
        BIO* bio_cert = BIO_new_file(path.data(), "rb");
        X509* cert = X509_new();
        PEM_read_bio_X509(bio_cert, &cert, nullptr, nullptr);
        BIO_free(bio_cert);
        return cert;
    }


    void TestCertificate_PEM() {
        // X509* cert = openPemFile(validPem);
        // std::unique_ptr<FILE, decltype(&fclose)> file (fopen(validPem.data(), "r"), fclose);

        const std::vector<char8_t> content = readCertificate(
                R"(/home/andtokm/DiskS/Projects/ClickHouse/contrib/aws/android-build/cacert.pem)");
        const auto *data = reinterpret_cast<const unsigned char*>(content.data());

        std::unique_ptr<X509, CertificateDeleter> certX509 {
            ::d2i_X509(nullptr, &data, std::ssize(content)), CertificateDeleter {}};
        if (!certX509) {
            std::unique_ptr<BIO, CertificateDeleter> bio {
                ::BIO_new_mem_buf(content.data(), content.size()), CertificateDeleter{} };
            if (!bio) {
                std::cout << "BIO_new_mem_buf() failed" << std::endl;
                return;
            }

            certX509.reset(::PEM_read_bio_X509(bio.get(), nullptr, nullptr, nullptr));
            if (!certX509) {
                std::cout << "PEM_read_bio_X509() failed" << std::endl;
                return;
            }
        }

        std::cout << "Version: " << X509_get_version(certX509.get()) +1 << std::endl;

        tm time {};
        ASN1_TIME_to_tm(X509_get_notBefore(certX509.get()), &time);
        std::cout << asctime(&time);

        ASN1_TIME_to_tm(X509_get_notAfter(certX509.get()), &time);
        std::cout << asctime(&time);
    }

    void GetSerialNumber()
    {
        ptrCert509 cert509 { getCertificateCrt(certPath) };
        if (!cert509) {
            std::cout << "Failed to get certificate '" << certPath << "'\n";
            return;
        }

        const ASN1_INTEGER *serial = X509_get_serialNumber(cert509.get());

        ptrBigNumber bigNum {ASN1_INTEGER_to_BN(serial, nullptr), BN_free};

        //char *hex = BN_bn2hex(bigNum.get());

        auto HexDeleter = [](char* ptr) {
            ::OPENSSL_free(ptr);
        };

        std::unique_ptr<char, decltype(HexDeleter)> hex { BN_bn2hex(bigNum.get()), HexDeleter };


        std::cout << hex.get() << std::endl;
        std::cout << serial->length << std::endl;

        std::cout << "We've done\n";
    }
}

namespace Errors {
    using namespace Utilities;
    using namespace Types;

    void InspectErrors()
    {
        ptrCert509 cert509 { getCertificateCrt(selfSignedCert) };
        if (!cert509)
        {
            std::cout << "Failed to get certificate '" << selfSignedCert << "'\n";

            const size_t code = ERR_get_error();
            std::cout << "Error  : " << code << std::endl;
            std::cout << "Reason : " << ERR_reason_error_string(code) << std::endl;
            std::cout << "Strings: " << ERR_func_error_string(code) << std::endl;
            std::cout << "Lib err: " << ERR_lib_error_string(code) << std::endl;

            ERR_print_errors_fp (stdout);
        }
    }
}

namespace Keys
{
    using namespace Utilities;
    using namespace Types;

    // https://riptutorial.com/openssl/example/16737/generate-rsa-key
    // https://www.dynamsoft.com/codepool/how-to-use-openssl-generate-rsa-keys-cc.html
    void Generate_RSA_Keys ()
    {
        constexpr std::string_view publicKey { R"(/home/andtokm/tmp/SSL/public.pem)"};
        constexpr std::string_view privateKey { R"(/home/andtokm/tmp/SSL/private.pem)"};

        /** generate rsa key **/
        ptrBigNumber bigNum {BN_new(), BN_free };
        BN_set_word(bigNum.get(), RSA_F4);

        constexpr int bits { 2048 };
        ptrRSA rsa {RSA_new(),::RSA_free};

        /** here we generate the RSA keys. **/
        RSA_generate_key_ex(rsa.get(), bits,bigNum.get(),nullptr);

        /** save public key **/
        ptrBIO publicKeyBio {BIO_new_file(publicKey.data(), "w+"), BIO_free};
        int ret = PEM_write_bio_RSAPublicKey(publicKeyBio.get(), rsa.get());
        if (ret != 1)
            return;

        /*
        EVP_PKEY *pkey = EVP_PKEY_new();
        EVP_PKEY_assign_RSA(pkey, rsa);

        EVP_PKEY_free(pkey);
        */

        /** save private key **/
        ptrBIO privateKeyBio { BIO_new_file(privateKey.data(), "w+"), BIO_free};
        ret = PEM_write_bio_RSAPrivateKey(privateKeyBio.get(), rsa.get(),
                                          nullptr, nullptr, 0, nullptr, nullptr);
        // TODO: Check 'ret'
    }
}



/// How to create a self-signed PEM file:
/// openssl req -newkey rsa:2048 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem
///

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    Experiments::TestCertificate();
    // Experiments::TestCertificate_PEM();
    // Experiments::TestReadCertificate();
    // Experiments::GetSerialNumber();

    // CertificateGenerator::TestAll();

    // Keys::Generate_RSA_Keys();

    // Errors::InspectErrors();

    return EXIT_SUCCESS;
}

