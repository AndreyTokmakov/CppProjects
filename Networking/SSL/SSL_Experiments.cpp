/**============================================================================
Name        : SSL_Experiments.cpp
Created on  : 27.08.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SSL_Experiments.cpp
============================================================================**/

#include "SSL_Experiments.h"

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

#include <iostream>
#include <string_view>
#include <memory>
#include <vector>
#include <array>
#include <fstream>

namespace
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
    ptrCert509 getCertificateCrt(std::string_view path) noexcept {
        const auto certData = readCertificate(path);
        const auto *data = reinterpret_cast<const unsigned char*>(certData.data());
        return {d2i_X509(nullptr, &data, std::ssize(certData)), CertificateDeleter{}};
    }
}

namespace Tests
{
    void TestCertificate_PEM()
    {
        const std::vector<char8_t> content = readCertificate(
                R"(/home/andtokm/DiskS/ProjectsUbuntu/BoostProjects/data/server.pem)");
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
}

void SSL_Experiments::TestAll()
{
    Tests::TestCertificate_PEM();
}
