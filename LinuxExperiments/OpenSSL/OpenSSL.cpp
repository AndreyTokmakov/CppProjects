//============================================================================
// Name        : OpenSSL.h
// Created on  : 12.07.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : OpenSSL
//============================================================================

#include "OpenSSL.h"

#include <iostream>
#include <vector>
#include <string_view>
#include <fstream>

#include <unistd.h>
#include <malloc.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>


#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/sha.h>

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/buffer.h>
#include <openssl/err.h>
#include <openssl/x509.h>

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/crypto.h>

#include <memory>


namespace {
    using ptrBigNumber = std::unique_ptr<BIGNUM, decltype(&::BN_free)>;
    using ptrRSA = std::unique_ptr<RSA, decltype(&::RSA_free)>;
    using ptrBIO = std::unique_ptr<BIO, decltype(&::BIO_free)>;

    using ptrPKEY = std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;

    using ptrCert509 = std::unique_ptr<X509, decltype(&::X509_free)>;

    using ptrAsnInteger = std::unique_ptr<ASN1_INTEGER, decltype(&::ASN1_INTEGER_free)>;
}

namespace OpenSSL {
    constexpr std::string_view certPath{
            "/home/andtokm/DiskS/Chromium/chromium/src/net/third_party/nist-pkits/certs/GoodCACert.crt"};

    constexpr std::string_view certPath1{
            "/home/andtokm/DiskS/Chromium/chromium/src/net/third_party/nist-pkits/certs/GoodCACert.crt"};

    constexpr std::string_view validPem{
            "/home/andtokm/DiskS/Chromium/chromium/src/net/data/verify_name_match_unittest/names/valid-minimal.pem"};

#if 0
    void test()
    {
        constexpr std::string_view filePath { R"(/home/andtokm/tmp/TEST_FILES/TestFile.txt)" };

        std::unique_ptr<FILE, decltype(&fclose)> file (fopen(filePath.data(), "r"), fclose);
        if (!file) {
            std::cout << "Failed to open file. Error = " << errno << std::endl;
            return;
        }

        X509 *cert = PEM_read_X509(file.get(), nullptr, nullptr, nullptr);
        if (!cert) {
            fprintf(stderr, "unable to parse certificate in: %s\n", filePath.data());
        }
    }
#endif

    [[nodiscard]]
    std::vector<char> readCertificate(std::string_view path) noexcept {
        std::vector<char> data{};
        if (std::fstream file(path.data(), std::ios::in | std::ios::binary); file.is_open() && file.good()) {
            file.seekg(0, std::ios_base::end);
            const auto bytesLength{file.tellg()};
            file.seekg(0, std::ios_base::beg);

            data.resize(bytesLength);
            file.read(data.data(), bytesLength);
        }
        return data;
    }

    void TestCertificate() {
        const std::vector<char> certData = readCertificate(certPath);
        const auto *data = reinterpret_cast<const unsigned char *>(certData.data());

        ptrCert509 cert{d2i_X509(nullptr, &data, certData.size()), X509_free};
        if (!cert) {
            std::cout << "Unable to parse certificate in memory" << std::endl;
            return;
        }

        int version = ((int) X509_get_version(cert.get())) + 1;
        std::cout << "version = " << version << std::endl;
    }

    void GetCertificateVersion() {
        const std::vector<char> certData = readCertificate(certPath);
        const auto *data = reinterpret_cast<const unsigned char *>(certData.data());

        ptrCert509 cert{d2i_X509(nullptr, &data, certData.size()), X509_free};
        if (cert) {
            int version = ((int) X509_get_version(cert.get())) + 1;
            std::cout << "version = " << version << std::endl;
        } else {
            std::cout << "Unable to parse certificate in memory" << std::endl;
        }
    }

    void GetCertificateValidity() {
        const std::vector<char> certData = readCertificate(certPath);
        const auto *data = reinterpret_cast<const unsigned char *>(certData.data());

        ptrCert509 cert{d2i_X509(nullptr, &data, certData.size()), X509_free};
        if (cert) {
            ASN1_TIME *not_before = X509_get_notBefore(cert.get());
            ASN1_TIME *not_after = X509_get_notAfter(cert.get());
        } else {
            std::cout << "Unable to parse certificate in memory" << std::endl;
        }
    }

    void GetSerialNumber(const std::vector<std::string_view> &params) {
        const std::string_view path = !params.empty() ? params.front() : certPath1;

        const std::vector<char> certData = readCertificate(certPath1);
        const auto *data = reinterpret_cast<const unsigned char *>(certData.data());


        ptrCert509 certificateX509{d2i_X509(nullptr, &data, certData.size()), X509_free};
        ASN1_INTEGER *serialNumber = X509_get_serialNumber(certificateX509.get());

        ptrBigNumber bigNum{ASN1_INTEGER_to_BN(serialNumber, nullptr), BN_free};
        if (!bigNum)
            return;

        char *hex = BN_bn2hex(bigNum.get());
        if (!hex)
            return;

        std::cout << hex << std::endl;

        // ASN1_INTEGER_free(serialNumber);

        /*
        ptrCert509 cert { d2i_X509(nullptr, &data, certData.size()), X509_free };
        ptrAsnInteger serialNumber {X509_get_serialNumber(cert.get()), ASN1_INTEGER_free };

        ptrBigNumber bigNum {ASN1_INTEGER_to_BN(serialNumber.get(), nullptr), BN_free };
        if (!bigNum)
            return;

        char* hex = BN_bn2hex(bigNum.get());
        if (!hex)
            return;

        std::cout << hex << std::endl;

        OPENSSL_free(hex);
         */
    }

    void GetSerialNumber2(const std::vector<std::string_view> &params)
    {
        const std::string_view path = !params.empty() ? params.front() : certPath1;
        const std::vector<char> certData = readCertificate(certPath1);
        const auto *data = reinterpret_cast<const unsigned char *>(certData.data());

        X509* cert = d2i_X509(nullptr, &data, certData.size());

        const int version = static_cast<int>(X509_get_version(cert)) + 1;
        std::cout << "version = " << version << std::endl;

        const ASN1_INTEGER *serial = X509_get_serialNumber(cert);
        ptrBigNumber serialNumber {BN_new(), BN_free};
        ASN1_INTEGER_to_BN(serial, serialNumber.get());

        ptrBigNumber bigNum {ASN1_INTEGER_to_BN(serial, nullptr), BN_free};
        char* hex = BN_bn2dec(bigNum.get());

        std::cout << hex << std::endl;
    }


    void GetPublicKey(const std::vector<std::string_view>& params)
    {
        [[maybe_unused]]
        const std::string_view path = !params.empty() ? params.front() : certPath1;
        const std::vector<char> certData = readCertificate(certPath1);
        const auto *data = reinterpret_cast<const unsigned char*>(certData.data());

        ptrCert509 certificateX509 { d2i_X509(nullptr, &data, certData.size()), X509_free};
        ptrPKEY pubKey {X509_get_pubkey(certificateX509.get()), EVP_PKEY_free};
        if (!pubKey) {
            std::cout << "X509_get_pubkey failed" << std::endl;
            return;
        }

        const int pubLen = i2d_PublicKey(pubKey.get(), nullptr);
        std::cout << "pubLen = " << pubLen << std::endl;

        ASN1_BIT_STRING * pubkey = X509_get0_pubkey_bitstr(certificateX509.get());
        const int nLen = pubkey->length;
        std::cout << "nLen = " << nLen << std::endl;

        std::string key(reinterpret_cast<const char*>(pubkey->data), nLen);
        std::cout << key<< std::endl;
    }


    // https://www.openssl.org/docs/man1.0.2/man3/bn.html
    void generate_kay_1() {
        ptrBigNumber bigNum {BN_new(), BN_free };
        [[maybe_unused]]
        auto ret = BN_set_word(bigNum.get(), RSA_F4);

        constexpr int bits = 2048;
        ptrRSA rsa {RSA_new(),::RSA_free};
        RSA_generate_key_ex(rsa.get(), bits, bigNum.get(), nullptr);  //here we generate the RSA keys

        /** clean up **/
        // free(pri_key);free(pub_key);
        // BIO_free_all(bp_public);
        // BIO_free_all(bp_private);
        // BIO_free(pbkeybio);
        // BIO_free(prkeybio);
    }


    RSA * createRSAWithFilename(std::string_view filePath, bool isPublic)
    {
        std::unique_ptr<FILE, decltype(&fclose)> file (fopen(filePath.data(), "rb"), fclose);
        if (file) {
            std::cout << "Unable to open file " << file << std::endl;
            return nullptr;
        }

        RSA *rsa = RSA_new(); // TODO: Handle mem
        if (isPublic)
            return PEM_read_RSA_PUBKEY(file.get(), &rsa,nullptr, nullptr);
        else
            return PEM_read_RSAPrivateKey(file.get(), &rsa,nullptr, nullptr);
    }

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

    // FIXME: Unimplamented
    void serialNumber2String(const ASN1_INTEGER* serial) {
        const ptrBigNumber bigNum {ASN1_INTEGER_to_BN(serial, nullptr), BN_free};
        if (!bigNum) {
            return ;
        }

        // const std::unique_ptr<char[]> hex { BN_bn2hex(bigNum.get())};
        // const std::unique_ptr<char, decltype(&OPENSSL_free)> hex(BN_bn2dec(bigNum.get()), OPENSSL_free);

        constexpr auto deleter = [](char* ptr) { ::OPENSSL_free(ptr); };
        const std::unique_ptr<char, decltype(deleter)> hex(BN_bn2dec(bigNum.get()), deleter);
        if (!hex)
            return;
    }
};

void OpenSSL::TestAll([[maybe_unused]] const std::vector<std::string_view>& params) {
    // TestCertificate();

    // GetCertificateVersion();
    // GetCertificateValidity();

    // GetSerialNumber(params);
    // GetSerialNumber2(params);

    // GetPublicKey(params);

    Generate_RSA_Keys();
};
