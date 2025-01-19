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
#include <openssl/opensslv.h>

#include "CertificateGenerator.h"
#include "Encryption.h"
#include "EncryptionRSA.h"


namespace
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    constexpr std::string_view certPath {
            "/home/andtokm/DiskS/Chromium/chromium/src/net/third_party/nist-pkits/certs/GoodCACert.crt"
    };
    constexpr std::string_view certPath1 {
            "/home/andtokm/DiskS/Chromium/chromium/src/net/third_party/nist-pkits/certs/GoodCACert.crt"
    };
    constexpr std::string_view validPem {
            "/home/andtokm/DiskS/Projects/ClickHouse/tests/config/dhparam.pem"
    };

    constexpr std::string_view selfSignedCert {"/home/andtokm/tmp/OpenSSL/certificate.crt"};
    constexpr std::string_view selfSignedCertPem { R"(/home/andtokm/DiskS/Temp/SSL/self_signed/cert.pem)"sv };
    constexpr std::string_view selfSignedKeyPem  { R"(/home/andtokm/DiskS/Temp/SSL/self_signed/key.pem)"sv };

    constexpr std::string_view binancePrivateKey  { R"(/home/andtokm/Documents/Binance/ssh_Key/ed25519.pem)"sv };
}

namespace Types
{
    using ptrBigNumber  = std::unique_ptr<BIGNUM, decltype(&::BN_free)>;
    using ptrRSA        = std::unique_ptr<RSA, decltype(&::RSA_free)>;
    using ptrBIO        = std::unique_ptr<BIO, decltype(&::BIO_free)>;
    using ptrPKEY       = std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;
    using ptrAsnInteger = std::unique_ptr<ASN1_INTEGER, decltype(&::ASN1_INTEGER_free)>;
    using ptrCert509    = std::unique_ptr<X509, decltype(&::X509_free)>;
}


namespace Utilities
{
    [[nodiscard]]
    std::vector<char8_t> readFileAsBytes(std::string_view path) noexcept
    {
        std::vector<char8_t> data {};
        if (std::fstream file(path.data(), std::ios::in | std::ios::binary); file.is_open() && file.good()) {
            file.seekg(0, std::ios_base::end);
            const auto bytesLength{ file.tellg() };
            file.seekg(0, std::ios_base::beg);

            data.resize(bytesLength);
            file.read(reinterpret_cast<char *>(data.data()), bytesLength);
        }
        return data;
    }

    [[nodiscard]]
    std::string readFileToString(std::string_view path) noexcept
    {
        std::string data {};
        if (std::fstream file(path.data(), std::ios::in | std::ios::binary); file.is_open() && file.good())
        {
            file.seekg(0, std::ios_base::end);
            const auto bytesLength{ file.tellg() };
            file.seekg(0, std::ios_base::beg);

            data.resize(bytesLength);
            file.read(data.data(), bytesLength);
        }
        return data;
    }

    [[nodiscard]]
    Types::ptrCert509 getCertificateCrt(std::string_view path) noexcept
    {
        const auto certData = readFileAsBytes(path);
        const auto *data = reinterpret_cast<const unsigned char*>(certData.data());
        return { d2i_X509(nullptr, &data, std::ssize(certData)), ::X509_free };
    }

    void printSslErrors()
    {
        const size_t code = ERR_get_error();
        std::cerr << "Error  : " << code << std::endl;
        std::cerr << "Reason : " << ERR_reason_error_string(code) << std::endl;
        std::cerr << "Strings: " << ERR_func_error_string(code) << std::endl;
        std::cerr << "Lib err: " << ERR_lib_error_string(code) << std::endl;
        ERR_print_errors_fp (stdout);
    }

    void printVersionAndExpirationTime(X509* cert)
    {
        std::cout << "Version: " << X509_get_version(cert) + 1 << std::endl;

        tm time {};
        ASN1_TIME_to_tm(X509_get_notBefore(cert), &time);
        std::cout << asctime(&time);

        ASN1_TIME_to_tm(X509_get_notAfter(cert), &time);
        std::cout << asctime(&time);
    }


    void printCertificateInfo(X509* cert)
    {
        constexpr uint32_t maxLength { 1024 };
        {
            char buffer[maxLength]{};
            X509_NAME_oneline(X509_get_subject_name(cert), buffer, maxLength);
            std::cout << "certificate: " << buffer << std::endl;
        }
        {
            char buffer[maxLength]{};
            X509_NAME_oneline(X509_get_issuer_name(cert), buffer, maxLength);
            std::cout << "issuer     : " << buffer << std::endl;
        }
    }

    constexpr std::string_view sslErrorToString(int64_t errorCode)
    {
        switch (errorCode)
        {
            case X509_V_OK: return "X509_V_OK"sv;
            case X509_V_ERR_UNSPECIFIED: return "X509_V_ERR_UNSPECIFIED"sv;
            case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT: return "ERR_UNABLE_TO_GET_ISSUER_CERT"sv;
            case X509_V_ERR_UNABLE_TO_GET_CRL: return "ERR_UNABLE_TO_GET_CRL"sv;
            case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE: return "ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE"sv;
            case X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE: return "ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE"sv;
            case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY: return "ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY"sv;
            case X509_V_ERR_CERT_SIGNATURE_FAILURE: return "ERR_CERT_SIGNATURE_FAILURE"sv;
            case X509_V_ERR_CRL_SIGNATURE_FAILURE: return "ERR_CRL_SIGNATURE_FAILURE"sv;
            case X509_V_ERR_CERT_NOT_YET_VALID: return "ERR_CERT_NOT_YET_VALID"sv;
            case X509_V_ERR_CERT_HAS_EXPIRED: return "ERR_CERT_HAS_EXPIRED"sv;
            case X509_V_ERR_CRL_NOT_YET_VALID: return "ERR_CRL_NOT_YET_VALID"sv;
            case X509_V_ERR_CRL_HAS_EXPIRED: return "ERR_CRL_HAS_EXPIRED"sv;
            case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:  return "ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD"sv;
            case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD: return "ERR_ERROR_IN_CERT_NOT_AFTER_FIELD"sv;
            case X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD: return "ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD"sv;
            case X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD: return "ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD"sv;
            case X509_V_ERR_OUT_OF_MEM: return "ERR_OUT_OF_MEM"sv;
            case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT: return "ERR_DEPTH_ZERO_SELF_SIGNED_CERT"sv;
            case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN: return "ERR_SELF_SIGNED_CERT_IN_CHAIN"sv;
            case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY: return "ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY"sv;
            case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE: return "ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE"sv;
            case X509_V_ERR_CERT_CHAIN_TOO_LONG: return "ERR_CERT_CHAIN_TOO_LONG"sv;
            case X509_V_ERR_CERT_REVOKED: return "ERR_CERT_REVOKED"sv;
            case X509_V_ERR_NO_ISSUER_PUBLIC_KEY: return "X509_V_ERR_NO_ISSUER_PUBLIC_KEY"sv;
            case X509_V_ERR_PATH_LENGTH_EXCEEDED: return "ERR_PATH_LENGTH_EXCEEDED"sv;
            case X509_V_ERR_INVALID_PURPOSE: return "ERR_INVALID_PURPOSE"sv;
            case X509_V_ERR_CERT_UNTRUSTED: return "ERR_CERT_UNTRUSTED"sv;
            case X509_V_ERR_CERT_REJECTED: return "ERR_CERT_REJECTED"sv;
            case X509_V_ERR_SUBJECT_ISSUER_MISMATCH: return "ERR_SUBJECT_ISSUER_MISMATCH"sv;
            case X509_V_ERR_AKID_SKID_MISMATCH: return "ERR_AKID_SKID_MISMATCH"sv;
            case X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH: return "ERR_AKID_ISSUER_SERIAL_MISMATCH"sv;
            case X509_V_ERR_KEYUSAGE_NO_CERTSIGN: return "ERR_KEYUSAGE_NO_CERTSIGN"sv;
            case X509_V_ERR_UNABLE_TO_GET_CRL_ISSUER: return "X509_V_ERR_UNABLE_TO_GET_CRL_ISSUER"sv;
            case X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION: return "X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION"sv;
            case X509_V_ERR_KEYUSAGE_NO_CRL_SIGN: return "X509_V_ERR_KEYUSAGE_NO_CRL_SIGN"sv;
            case X509_V_ERR_UNHANDLED_CRITICAL_CRL_EXTENSION: return "X509_V_ERR_UNHANDLED_CRITICAL_CRL_EXTENSION"sv;
            case X509_V_ERR_INVALID_NON_CA: return "X509_V_ERR_INVALID_NON_CA"sv;
            case X509_V_ERR_PROXY_PATH_LENGTH_EXCEEDED: return "X509_V_ERR_PROXY_PATH_LENGTH_EXCEEDED"sv;
            case X509_V_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE: return "X509_V_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE"sv;
            case X509_V_ERR_PROXY_CERTIFICATES_NOT_ALLOWED: return "X509_V_ERR_PROXY_CERTIFICATES_NOT_ALLOWED"sv;
            case X509_V_ERR_INVALID_EXTENSION: return "X509_V_ERR_INVALID_EXTENSION"sv;
            case X509_V_ERR_INVALID_POLICY_EXTENSION: return "X509_V_ERR_INVALID_POLICY_EXTENSION"sv;
            case X509_V_ERR_NO_EXPLICIT_POLICY: return "ERR_NO_EXPLICIT_POLICY"sv;
            case X509_V_ERR_DIFFERENT_CRL_SCOPE: return "X509_V_ERR_DIFFERENT_CRL_SCOPE"sv;
            case X509_V_ERR_UNSUPPORTED_EXTENSION_FEATURE: return "X509_V_ERR_UNSUPPORTED_EXTENSION_FEATURE"sv;
            case X509_V_ERR_UNNESTED_RESOURCE: return "X509_V_ERR_UNNESTED_RESOURCE"sv;
            case X509_V_ERR_PERMITTED_VIOLATION: return "X509_V_ERR_PERMITTED_VIOLATION"sv;
            case X509_V_ERR_EXCLUDED_VIOLATION: return "X509_V_ERR_EXCLUDED_VIOLATION"sv;
            case X509_V_ERR_SUBTREE_MINMAX: return "X509_V_ERR_SUBTREE_MINMAX"sv;
            case X509_V_ERR_APPLICATION_VERIFICATION: return "ERR_APPLICATION_VERIFICATION"sv;
            case X509_V_ERR_UNSUPPORTED_CONSTRAINT_TYPE: return "X509_V_ERR_UNSUPPORTED_CONSTRAINT_TYPE"sv;
            case X509_V_ERR_UNSUPPORTED_CONSTRAINT_SYNTAX: return "X509_V_ERR_UNSUPPORTED_CONSTRAINT_SYNTAX"sv;
            case X509_V_ERR_UNSUPPORTED_NAME_SYNTAX: return "X509_V_ERR_UNSUPPORTED_NAME_SYNTAX"sv;
            case X509_V_ERR_CRL_PATH_VALIDATION_ERROR: return "X509_V_ERR_CRL_PATH_VALIDATION_ERROR"sv;
            case X509_V_ERR_SUITE_B_INVALID_VERSION: return "X509_V_ERR_SUITE_B_INVALID_VERSION"sv;
            case X509_V_ERR_SUITE_B_INVALID_ALGORITHM: return "X509_V_ERR_SUITE_B_INVALID_ALGORITHM"sv;
            case X509_V_ERR_SUITE_B_INVALID_CURVE: return "X509_V_ERR_SUITE_B_INVALID_CURVE"sv;
            case X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM: return "X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM"sv;
            case X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED: return "X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED"sv;
            case X509_V_ERR_SUITE_B_CANNOT_SIGN_P_384_WITH_P_256: return "X509_V_ERR_SUITE_B_CANNOT_SIGN_P_384_WITH_P_256"sv;
            case X509_V_ERR_HOSTNAME_MISMATCH: return "X509_V_ERR_HOSTNAME_MISMATCH"sv;
            case X509_V_ERR_EMAIL_MISMATCH: return "X509_V_ERR_EMAIL_MISMATCH"sv;
            case X509_V_ERR_IP_ADDRESS_MISMATCH: return "X509_V_ERR_IP_ADDRESS_MISMATCH"sv;
            case X509_V_ERR_DANE_NO_MATCH: return "X509_V_ERR_DANE_NO_MATCH"sv;
            case X509_V_ERR_EE_KEY_TOO_SMALL: return "X509_V_ERR_EE_KEY_TOO_SMALL"sv;
            case X509_V_ERR_CA_KEY_TOO_SMALL: return "X509_V_ERR_CA_KEY_TOO_SMALL"sv;
            case X509_V_ERR_CA_MD_TOO_WEAK: return "X509_V_ERR_CA_MD_TOO_WEAK"sv;
            case X509_V_ERR_INVALID_CALL: return "X509_V_ERR_INVALID_CALL"sv;
            case X509_V_ERR_STORE_LOOKUP: return "X509_V_ERR_STORE_LOOKUP"sv;
            case X509_V_ERR_NO_VALID_SCTS: return "X509_V_ERR_NO_VALID_SCTS"sv;
            case X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION: return "X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION"sv;
            case X509_V_ERR_OCSP_VERIFY_NEEDED: return "X509_V_ERR_OCSP_VERIFY_NEEDED"sv;
            case X509_V_ERR_OCSP_VERIFY_FAILED: return "X509_V_ERR_OCSP_VERIFY_FAILED"sv;
            case X509_V_ERR_OCSP_CERT_UNKNOWN: return "X509_V_ERR_OCSP_CERT_UNKNOWN"sv;
            case X509_V_ERR_UNSUPPORTED_SIGNATURE_ALGORITHM: return "X509_V_ERR_UNSUPPORTED_SIGNATURE_ALGORITHM"sv;
            case X509_V_ERR_SIGNATURE_ALGORITHM_MISMATCH: return "X509_V_ERR_SIGNATURE_ALGORITHM_MISMATCH"sv;
            case X509_V_ERR_SIGNATURE_ALGORITHM_INCONSISTENCY: return "X509_V_ERR_SIGNATURE_ALGORITHM_INCONSISTENCY"sv;
            case X509_V_ERR_INVALID_CA: return "X509_V_ERR_INVALID_CA"sv;
            case X509_V_ERR_PATHLEN_INVALID_FOR_NON_CA: return "X509_V_ERR_PATHLEN_INVALID_FOR_NON_CA"sv;
            case X509_V_ERR_PATHLEN_WITHOUT_KU_KEY_CERT_SIGN: return "X509_V_ERR_PATHLEN_WITHOUT_KU_KEY_CERT_SIGN"sv;
            case X509_V_ERR_KU_KEY_CERT_SIGN_INVALID_FOR_NON_CA: return "X509_V_ERR_KU_KEY_CERT_SIGN_INVALID_FOR_NON_CA"sv;
            case X509_V_ERR_ISSUER_NAME_EMPTY: return "X509_V_ERR_ISSUER_NAME_EMPTY"sv;
            case X509_V_ERR_SUBJECT_NAME_EMPTY: return "X509_V_ERR_SUBJECT_NAME_EMPTY"sv;
            case X509_V_ERR_MISSING_AUTHORITY_KEY_IDENTIFIER: return "X509_V_ERR_MISSING_AUTHORITY_KEY_IDENTIFIER"sv;
            case X509_V_ERR_MISSING_SUBJECT_KEY_IDENTIFIER: return "X509_V_ERR_MISSING_SUBJECT_KEY_IDENTIFIER"sv;
            case X509_V_ERR_EMPTY_SUBJECT_ALT_NAME: return "X509_V_ERR_EMPTY_SUBJECT_ALT_NAME"sv;
            case X509_V_ERR_EMPTY_SUBJECT_SAN_NOT_CRITICAL: return "X509_V_ERR_EMPTY_SUBJECT_SAN_NOT_CRITICAL"sv;
            case X509_V_ERR_CA_BCONS_NOT_CRITICAL: return "X509_V_ERR_CA_BCONS_NOT_CRITICAL"sv;
            case X509_V_ERR_AUTHORITY_KEY_IDENTIFIER_CRITICAL: return "X509_V_ERR_AUTHORITY_KEY_IDENTIFIER_CRITICAL"sv;
            case X509_V_ERR_SUBJECT_KEY_IDENTIFIER_CRITICAL: return "X509_V_ERR_SUBJECT_KEY_IDENTIFIER_CRITICAL"sv;
            case X509_V_ERR_CA_CERT_MISSING_KEY_USAGE: return "X509_V_ERR_CA_CERT_MISSING_KEY_USAGE"sv;
            case X509_V_ERR_EXTENSIONS_REQUIRE_VERSION_3: return "X509_V_ERR_EXTENSIONS_REQUIRE_VERSION_3"sv;
            case X509_V_ERR_EC_KEY_EXPLICIT_PARAMS: return "X509_V_ERR_EC_KEY_EXPLICIT_PARAMS"sv;
            default: return "ERR_UNKNOWN";
        }
    }
}

namespace Experiments
{
    using namespace Utilities;
    using namespace Types;


    void TestReadCertificate()
    {
        const std::vector<char8_t> content = readFileAsBytes(selfSignedCert);
        const auto *data = reinterpret_cast<const unsigned char*>(content.data());

        ptrCert509 cert { d2i_X509(nullptr, &data, std::ssize(content)), ::X509_free };
        if (!cert) {
            std::cout << "Failed to get certificate '" << selfSignedCert << "'\n";
            printSslErrors();
        }
    }

    void TestCertificate()
    {
        const ptrCert509 cert { getCertificateCrt(certPath) };
        if (!cert) {
            std::cout << "Unable to parse certificate in memory" << std::endl;
            return;
        }

        printVersionAndExpirationTime(cert.get());
    }

    void TestCertificate_PEM_1()
    {
        const ptrBIO bio { ::BIO_new_file(selfSignedCertPem.data(), "rb"),::BIO_free };
        const ptrCert509 certX509 { ::X509_new(), ::X509_free };

        X509* ptrCert = certX509.get();
        PEM_read_bio_X509(bio.get(), &ptrCert, nullptr, nullptr);

        printVersionAndExpirationTime(ptrCert);
    }

    void TestCertificate_PEM_2()
    {
        const std::string content = readFileToString(selfSignedCertPem);
        // const std::string content = readFileToString(binancePrivateKey);

        const unsigned char *data = reinterpret_cast<const unsigned char*>(content.data());

        ptrCert509 certX509 {::d2i_X509(nullptr, &data, std::ssize(content)), ::X509_free};
        if (!certX509)
        {
            ptrBIO bio {::BIO_new_mem_buf(content.data(), content.size()),::BIO_free };
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

        printCertificateInfo(certX509.get());
        printVersionAndExpirationTime(certX509.get());
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
    }
}

namespace Errors
{
    using namespace Utilities;
    using namespace Types;

    void InspectErrors()
    {
        ptrCert509 cert509 { getCertificateCrt(selfSignedCert) };
        if (!cert509)
        {
            std::cout << "Failed to get certificate '" << selfSignedCert << "'\n";
            printSslErrors();
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
        constexpr std::string_view publicKey { R"(/tmp/SSL/public.pem)"};
        constexpr std::string_view privateKey { R"(/tmp/SSL/private.pem)"};

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


void printVersionInfo()
{
    std::cout << "===========================================================================" << std::endl;

#ifdef OPENSSL_VERSION_NUMBER
    printf("OPENSSL_VERSION_NUMBER: %#08lx\n", OPENSSL_VERSION_NUMBER);
#endif

#ifdef LIBRESSL_VERSION_NUMBER
    printf("LIBRESSL_VERSION_NUMBER: %#08lx\n", LIBRESSL_VERSION_NUMBER);
#endif

#ifdef LIBRESSL_VERSION_TEXT
    printf("LIBRESSL_VERSION_TEXT: %s\n", LIBRESSL_VERSION_TEXT);
#endif

#if OPENSSL_VERSION_NUMBER >= 0x1010000f
    std::cout << "NUMBER: " << std::hex << OpenSSL_version_num() << std::endl;
    std::cout << "VERSION: " << OpenSSL_version(OPENSSL_VERSION) << std::endl;
    std::cout << "BUILT_ON: " << OpenSSL_version(OPENSSL_BUILT_ON) << std::endl;
    std::cout << "PLATFORM: " << OpenSSL_version(OPENSSL_PLATFORM) << std::endl;
    std::cout << "CFLAGS: " << OpenSSL_version(OPENSSL_CFLAGS) << std::endl;
    std::cout << "DIR: " << OpenSSL_version(OPENSSL_DIR) << std::endl;
    std::cout << "ENGINES_DIR: " << OpenSSL_version(OPENSSL_ENGINES_DIR) << std::endl;
#endif

    std::cout << "\nSSLeay: " << std::hex << SSLeay() << std::endl;
    std::cout << "SSLEAY_VERSION: " << SSLeay_version(SSLEAY_VERSION) << std::endl;
    std::cout << "SSLEAY_CFLAGS: " << SSLeay_version(SSLEAY_CFLAGS) << std::endl;
    std::cout << "SSLEAY_BUILT_ON: " << SSLeay_version(SSLEAY_BUILT_ON) << std::endl;
    std::cout << "SSLEAY_PLATFORM: " << SSLeay_version(SSLEAY_PLATFORM) << std::endl;
    std::cout << "SSLEAY_DIR: " << SSLeay_version(SSLEAY_DIR) << std::endl;

    std::cout << "===========================================================================" << std::endl;
}


/// How to create a self-signed PEM file:
/// openssl req -newkey rsa:2048 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem
///

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);
    // printVersionInfo();

    // Experiments::TestCertificate();
    // Experiments::TestCertificate_PEM_1();
    Experiments::TestCertificate_PEM_2();
    // Experiments::TestReadCertificate();
    // Experiments::GetSerialNumber();

    // CertificateGenerator::TestAll();

    // Keys::Generate_RSA_Keys();

    // Errors::InspectErrors();

    // Encryption::TestAll();
    // EncryptionRSA::TestAll();


    return EXIT_SUCCESS;
}

