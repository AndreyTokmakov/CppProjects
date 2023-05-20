//============================================================================
// Name        : CertificateGenerator.cpp
// Created on  : 09.08.2022
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : CertificateGenerator
//============================================================================

#include "CertificateGenerator.h"



#include <cstring>
#include <algorithm>
#include <iostream>

#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>
#include <openssl/engine.h>

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/dh.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>

#include <iostream>
#include <fstream>
#include <openssl/stack.h>

#include <openssl/pkcs12.h>
#include <pthread.h>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#define fatal(msg) fatal_error(__FILE__, __LINE__, msg)


namespace {

    struct CertEntries {
        std::string country_name;
        std::string state_province_name;
        std::string locality_name;
        std::string organization_name;
        std::string organizational_unit_name;
        std::string common_name;
    };

    struct RawCertificate {
        std::string        public_key_pem;
        std::string        private_key_pem;
        std::vector<char>  key_pkcs12;
    };

    struct CaCert {
        std::string public_key_pem;
        std::string private_key_pem;
        std::string pass;
    };


    struct SSLGenerator
    {
        /* define if pem format is enabled */
        bool is_pem;

        /* define if pkcs12 format is enabled */
        bool is_p12;

        /* set output public key PEM file path */
        char *public_key_pem_file;

        /* set output private key PEM file path */
        char *private_key_pem_file;

        /* set output pkcs12 file path */
        char *key_p12_file;


        static void fatal_error(const char *file,
                                int line,
                                const char *msg) {

            fprintf(stderr, "**FATAL** %s:%i %s\n", file, line, msg);
            ERR_print_errors_fp(stderr);
            exit(-1);
        }

        void setOutputPEM(bool enable_pem,
                          char* public_key_file,
                          char* private_key_file)
        {
            is_pem = enable_pem;
            this->public_key_pem_file = public_key_file;
            this->private_key_pem_file = private_key_file;
        }

        void setOutputP12(bool enable_p12,
                          char* key_file)
        {
            is_p12 = enable_p12;
            this->key_p12_file = key_file;
        }

        int create_standalone_keys(CertEntries *entries,
                                   tm *date_start,
                                   tm *date_end,
                                   int serial,
                                   char *passin,
                                   int rsa_key_size,
                                   RawCertificate *certs)
        {
            return createKeys(entries,
                              date_start,
                              date_end,
                              false,
                              0,
                              serial,
                              passin,
                              rsa_key_size,
                              certs);
        }

        static void createCertEntry(X509_NAME *subj,
                                    std::string_view entry_key,
                                    std::string_view entry_val) {

            int nid;                  // ASN numeric identifier
            X509_NAME_ENTRY *ent;

            if ((nid = OBJ_txt2nid(entry_key.data())) == NID_undef){
                fprintf(stderr, "Error finding NID for %s\n", entry_key.data());
                fatal("Error on lookup");
            }

            if (!(ent = X509_NAME_ENTRY_create_by_NID(nullptr,
                                                      nid,
                                                      MBSTRING_ASC,
                                                      reinterpret_cast<const unsigned char *>(entry_val.data()),
                                                      -1)))
                fatal("Error creating Name entry from NID");

            if (X509_NAME_add_entry(subj, ent, -1, 0) != 1)
                fatal("Error adding entry to Name");

            X509_NAME_ENTRY_free(ent);
        }

        static int add_ext(X509 *cert,X509* issuer,
                           int nid,
                           std::string_view *value) {
            X509V3_CTX ctx;
            // This sets the 'context' of the extensions. No configuration database
            X509V3_set_ctx_nodb(&ctx);

            // Issuer and subject certs: both the target since it is self signed no request and no CRL
            X509V3_set_ctx(&ctx, issuer, cert, nullptr, nullptr, 0);
            X509_EXTENSION *ex = X509V3_EXT_conf_nid(nullptr, &ctx, nid, value->data());
            if (!ex)
                return 0;

            X509_add_ext(cert,ex,-1);
            X509_EXTENSION_free(ex);
            return 1;
        }

        int createKeys(CertEntries *entries,
                       tm *dateStart,
                       tm *dateEnd,
                       bool isSignWithCa,
                       CaCert *cert_item,
                       int serial,
                       char *passin,
                       int rsa_key_size,
                       RawCertificate *certs) const
        {
            time_t systime, caltime_start,caltime_end; /* time_t is a long */

            long timediff_start,timediff_end;

            time(&systime); /* get the system time in seconds since EPOCH */
            localtime(&systime); /* and return a pointer to the time structure */

            /* get time in seconds from the time structure */
            caltime_start = mktime(dateStart);
            caltime_end = mktime(dateEnd);

            /* don't care if time difference is negative */
            timediff_start = caltime_start-systime;
            if (timediff_start < 0) {
                timediff_start = timediff_start * (-1);
            } else{
                timediff_start=0;
            }

            timediff_end=caltime_end-systime;

            if (timediff_end < 0)
                timediff_end = 0;

            if (serial==-1){
                std::cout << "Error happened due to sequence number generation" << std::endl;
                return -1;
            }

            // openssl setup
            OpenSSL_add_all_algorithms();
            ERR_load_crypto_strings();

            // seed PRNG
            RAND_load_file("/dev/urandom", 128);

            // Generate the RSA key
            RSA* rsakey = RSA_generate_key(rsa_key_size, RSA_F4, nullptr, nullptr);

            // Create evp obj to hold our rsakey
            EVP_PKEY *pkey = EVP_PKEY_new();
            if (!pkey)
                fatal("Could not create EVP object");

            if (!(EVP_PKEY_set1_RSA(pkey, rsakey)))
                fatal("Could not assign RSA key to EVP object");

            // create request object
            X509 *req = X509_new();
            if (!req)
                fatal("Failed to create X509_REQ object");

            X509_set_version(req, NID_X509);
            X509_set_pubkey(req, pkey);
            X509_NAME *subj = X509_get_subject_name(req);
            ASN1_INTEGER_set(X509_get_serialNumber(req),serial);

            if (!X509_gmtime_adj(X509_get_notBefore(req), (long)timediff_start))
                fatal("Error setting start date");
            if (!X509_gmtime_adj(X509_get_notAfter(req), (long)timediff_end))
                fatal("Error setting end date");
            if (X509_set_subject_name(req, subj) != 1)
                fatal("Error adding subject to request");

            if (!isSignWithCa) {
                createCertEntry(subj,"countryName",entries->country_name);
                createCertEntry(subj,"stateOrProvinceName",entries->state_province_name);
                createCertEntry(subj,"localityName",entries->locality_name);
                createCertEntry(subj,"organizationName",entries->organization_name);
                createCertEntry(subj,"organizationalUnitName",entries->organizational_unit_name);
                createCertEntry(subj,"commonName",entries->common_name);

                X509_set_issuer_name(req,subj);
                EVP_MD *digest = (EVP_MD *)EVP_sha1();

                if (!(X509_sign(req, pkey, digest)))
                    fatal("Error signing request");
            }
            else {
                std::cout << "signing certs with CA cert" << std::endl;
                if (strcmp(cert_item->private_key_pem.data(),"")!=0){
                    if (cert_item->private_key_pem.length()>0){

                        createCertEntry(subj,"countryName",entries->country_name);
                        createCertEntry(subj,"stateOrProvinceName",entries->state_province_name);
                        createCertEntry(subj,"localityName",entries->locality_name);
                        createCertEntry(subj,"organizationName",entries->organization_name);
                        createCertEntry(subj,"organizationalUnitName",entries->organizational_unit_name);
                        createCertEntry(subj,"commonName",entries->common_name);

                        BIO *bioCa = BIO_new(BIO_s_mem());
                        BIO_write(bioCa,(char*)cert_item->public_key_pem.data(),cert_item->public_key_pem.length());
                        X509 *x509Ca = X509_new();
                        PEM_read_bio_X509(bioCa,&x509Ca,0,0);

                        X509_set_issuer_name(req,X509_get_subject_name(x509Ca));

                        BIO *bio = BIO_new(BIO_s_mem());
                        BIO_write(bio,(char*)cert_item->private_key_pem.data(),cert_item->private_key_pem.length());

                        EVP_PKEY* prv_key = nullptr;
                        if (strcmp(cert_item->pass.data(),"")!=0)
                            prv_key = PEM_read_bio_PrivateKey(bio, &prv_key, nullptr, (char*)cert_item->pass.data());
                        else
                            prv_key = PEM_read_bio_PrivateKey(bio, &prv_key, nullptr, nullptr);

                        EVP_MD *digest = (EVP_MD*)EVP_sha1();
                        if (!(X509_sign(req, prv_key, digest)))
                            fatal("Error signing request");

                        BIO_free(bioCa);
                        BIO_free(bio);
                        X509_free(x509Ca);
                        EVP_PKEY_free(prv_key);

                    } else {
                        std::cout << "CA was not found in database" << std::endl;
                        return -1;
                    }
                } else {
                    std::cout << "An error has occured! " << std::endl;
                    return -1;
                }
            }

            //PEM PUBLIC KEY
            if (is_pem) {

                BIO *b64Cert = BIO_new (BIO_s_mem());
                PEM_write_bio_X509(b64Cert, req);
                BUF_MEM *bptrCert;
                BIO_get_mem_ptr(b64Cert, &bptrCert);
                int length = bptrCert->length;
                char*  public_key = new char[length];
                BIO_read(b64Cert,public_key,length);

                public_key[length-1]='\0';

                certs->public_key_pem=public_key;

                delete[] public_key;
                public_key=0;
                BIO_free(b64Cert);
            }

            //PEM PRIVATE KEY AES 256 ENCODED
            if (is_pem) {
                BIO *b64Key = BIO_new (BIO_s_mem());
                if (strcmp(passin,"")==0){
                    PEM_write_bio_PrivateKey(b64Key, pkey,nullptr, nullptr, 0, nullptr, nullptr);
                } else{
                    PEM_write_bio_PrivateKey(b64Key, pkey,EVP_aes_256_cbc(), nullptr, 0, nullptr, passin);
                }

                BUF_MEM *bptrKey;
                BIO_get_mem_ptr(b64Key, &bptrKey);
                int length2 = bptrKey->length;
                char* private_key = new char[length2];
                BIO_read(b64Key,private_key,length2);

                private_key[length2-1]='\0';

                certs->private_key_pem=private_key;

                delete[] private_key;
                private_key = nullptr;
                BIO_free(b64Key);
            }


            //PKCS12
            PKCS12 *p12;

            p12 = PKCS12_create(passin, (char*)entries->common_name.data(), pkey, req, nullptr, 0,0,0,0,0);

            if (isSignWithCa){

                if (strcmp(cert_item->public_key_pem.data(),"")!=0){

                    if (cert_item->public_key_pem.length()>0){

                        BIO *bioCa = BIO_new(BIO_s_mem());

                        BIO_write(bioCa,(char*)cert_item->public_key_pem.data(),cert_item->public_key_pem.length());

                        X509 *x509Ca=X509_new();

                        PEM_read_bio_X509(bioCa,&x509Ca,0,0);

                        STACK_OF(X509) *stackX509=sk_X509_new_null();
                        sk_X509_push(stackX509,x509Ca);

                        PKCS12_free(p12);
                        BIO_free(bioCa);

                        p12 = PKCS12_create(passin, (char*)entries->common_name.data(), pkey, req, stackX509, 0,0,0,0,0);

                        sk_X509_pop_free(stackX509, X509_free);

                        // add ca that signed the cert to p12 certificate
                        //PKCS12_add_cert(p12,x509Ca);
                    }
                }
            }

            // PKCS12
            if (is_p12) {

                BIO * p12Bio = BIO_new(BIO_s_mem());
                i2d_PKCS12_bio(p12Bio,p12);
                BUF_MEM *bptrP12;
                BIO_get_mem_ptr(p12Bio, &bptrP12);
                int length3 = bptrP12->length;
                char* p12Cert = new char[length3];
                BIO_read(p12Bio,p12Cert,length3);

                std::vector<char> p12vector(p12Cert,p12Cert+length3);

                certs->key_pkcs12=p12vector;

                delete[] p12Cert;
                p12Cert=0;

                BIO_free(p12Bio);
            }


            if (is_pem && strcmp(public_key_pem_file,"")!=0) {

                FILE *fp;

                if (!(fp = fopen(public_key_pem_file, "w"))) {

                    fprintf(stderr, "Error opening file %s\n", public_key_pem_file);
                    fatal("Error writing to public key file");
                }
                if (PEM_write_X509(fp, req) != 1)
                    fatal("Error while writing public key");
                fclose(fp);
            }

            if (is_pem && strcmp(private_key_pem_file,"")!=0){

                FILE *fp;

                if (!(fp = fopen(private_key_pem_file, "w"))){

                    fprintf(stderr, "Error opening file %s\n", private_key_pem_file);
                    fatal("Error writing to private key file");
                }

                if (strcmp(passin,"")==0){

                    if (PEM_write_PrivateKey(fp, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1)
                        fatal("Error while writing private key");
                }
                else{
                    if (PEM_write_PrivateKey(fp, pkey, EVP_aes_256_cbc(), nullptr, 0, nullptr, passin) != 1)
                        fatal("Error while writing private key");
                }
                fclose(fp);
            }

            if (is_p12 && strcmp(key_p12_file,"")!=0 ) {

                FILE *fp1;

                if (!(fp1 = fopen(key_p12_file, "wb"))) {

                    fprintf(stderr, "Error opening file %s\n", key_p12_file);
                    fatal("Error writing to p12 file");
                }

                i2d_PKCS12_fp(fp1, p12);
                fclose(fp1);
            }

            PKCS12_free(p12);
            X509_free(req);
            EVP_PKEY_free(pkey);
            RSA_free(rsakey);
            CRYPTO_cleanup_all_ex_data();
            ERR_free_strings();
            ERR_remove_state(0);
            EVP_cleanup();

            return 0;
        }

    };

    int dh_key_size = 1024;

    [[maybe_unused]]
    void *generateDhParamThread(std::string_view filePath)
    {

        int g=2;//dh parameters
        DH *dh = DH_new();
        if (!dh || !DH_generate_parameters_ex(dh, dh_key_size, g, nullptr)){
            std::cout << "DH param has been generated !" << std::endl;
        }
        BIO *dhBio = BIO_new (BIO_s_mem());
        PEM_write_bio_DHparams(dhBio,dh);
        BUF_MEM *bptrDh;
        BIO_get_mem_ptr(dhBio, &bptrDh);
        int length3 = bptrDh->length;
        char dhArray[length3];
        BIO_read(dhBio,dhArray,length3);
        FILE *fp;

        if (!(fp = fopen(filePath.data(), "w"))) {

            std::cout << "Error writing to dh file";

        }

        fprintf(fp,"%s",dhArray);
        fclose(fp);
        DH_free(dh);
        BIO_free(dhBio);

        std::cout << "DH Params generation has finished!"<< std::endl;

        return 0;
    }

    /*
    pthread_t create_dh_key(int key_size,
                            std::string_view file_path) {

        if (!file_path.empty()) {

            dh_key_size=key_size;
            pthread_t dh_thread;
            std::cout << "Generate DH Params" << std::endl;
            int rc = pthread_create(&dh_thread, nullptr, generateDhParamThread, file_path);

            if (rc){
                std::cout << "Error:unable to create thread," << rc << std::endl;
                return -1;
            }
            return dh_thread;
        }
        else{
            std::cout << "Error output file path is required" << std::endl;
        }
        return 0;
    }
    */
}

namespace CertificateGenerator
{
    bool setSerialNumber(X509* cert, uint32_t bytes)
    {
        bool result = false;
        ASN1_STRING* serialNumber = X509_get_serialNumber(cert);
        if (serialNumber != nullptr && bytes != 0) {
            std::vector<unsigned char> serial(bytes);
            RAND_bytes(serial.data(), static_cast<int>(serial.size()));
            if (ASN1_STRING_set(serialNumber, serial.data(), static_cast<int>(serial.size())) == 1) {
                result = true;
            }
        }
        return result;
    }

    bool setVersion(X509* cert, long version) {
        return X509_set_version(cert, version) == 1;
    }

    bool updateSubjectName(X509* cert, const char* key, const char* value)
    {
        bool result = false;
        X509_NAME* subjectName = X509_get_subject_name(cert);
        if (subjectName != nullptr) {
            const int res = X509_NAME_add_entry_by_txt(subjectName, key, MBSTRING_ASC, (unsigned char*)value, -1, -1, 0);
            result = res == 1;
        }
        return result;
    }

    bool setNotAfter(X509* cert, uint32_t y, uint32_t m, uint32_t d, int32_t offset_days) {
        tm base;
        memset(&base, 0, sizeof(base));
        base.tm_year = y - 1900;
        base.tm_mon = m - 1;
        base.tm_mday = d;
        time_t tm = mktime(&base);

        bool result = false;
        ASN1_STRING* notAfter = X509_getm_notAfter(cert);
        if (notAfter != nullptr) {
            X509_time_adj(notAfter, 86400L * offset_days, &tm);
            result = true;
        }
        return result;
    }

    bool setNotBefore(X509* cert, uint32_t y, uint32_t m, uint32_t d, int32_t offset_days)
    {
        tm base;
        memset(&base, 0, sizeof(base));
        base.tm_year = y - 1900;
        base.tm_mon = m - 1;
        base.tm_mday = d;
        time_t tm = mktime(&base);

        bool result = false;
        ASN1_STRING* notBefore = X509_getm_notBefore(cert);
        if (notBefore != nullptr) {
            X509_time_adj(notBefore, 86400L * offset_days, &tm);
            result = true;
        }
        return result;
    }

    bool setIssuer(X509* cert, X509* issuer)
    {
        bool result = false;
        X509_NAME* subjectName = X509_get_subject_name(issuer);
        if (subjectName != nullptr) {
            result = X509_set_issuer_name(cert, subjectName) == 1;
        }
        return result;
    }

    bool addIssuerInfo(X509* cert, const char* key, const char* value)
    {
        bool result = false;
        X509_NAME* issuerName = X509_get_issuer_name(cert);
        if (issuerName != nullptr) {
            result = X509_NAME_add_entry_by_txt(issuerName, key, MBSTRING_ASC, (unsigned char*)value, -1, -1, 0) == 1;
        }
        return result;
    }

    bool addStandardExtension(X509* cert, X509* issuer, int nid, const char* value)
    {
        X509V3_CTX ctx; // create context
        X509V3_set_ctx_nodb(&ctx); // init context
        X509V3_set_ctx(&ctx, issuer, cert, nullptr, nullptr, 0); // set context

        std::unique_ptr<X509_EXTENSION, decltype(&::X509_EXTENSION_free)> ex(X509V3_EXT_conf_nid(nullptr, &ctx, nid, value), ::X509_EXTENSION_free);
        if (ex != nullptr) {
            return X509_add_ext(cert, ex.get(), -1) == 1;
        }
        return false;
    }

    bool addCustomExtension(X509* cert, const char* key, const char* value, bool critical)
    {
        const int nid = OBJ_create(key, value, nullptr);

        std::unique_ptr<ASN1_OCTET_STRING, decltype(&::ASN1_OCTET_STRING_free)> data(ASN1_OCTET_STRING_new(), ::ASN1_OCTET_STRING_free);
        int ret = ASN1_OCTET_STRING_set(data.get(), reinterpret_cast<unsigned const char*>(value), strlen(value));
        if (ret != 1) {
            return false;
        }

        std::unique_ptr<X509_EXTENSION, decltype(&::X509_EXTENSION_free)> ex(X509_EXTENSION_create_by_NID(nullptr, nid, critical, data.get()), ::X509_EXTENSION_free);
        return X509_add_ext(cert, ex.get(), -1) == 1;
    }

    bool setPublicKey(X509* cert, EVP_PKEY* key) {
        return X509_set_pubkey(cert, key) == 1;
    }

    bool signCert(X509* cert, EVP_PKEY* key, const EVP_MD* algo) {
        return X509_sign(cert, key, algo) != 0;
    }

    bool saveCertToPemFile(X509* cert, const std::string& file) {
        bool result = false;
        std::unique_ptr<BIO, decltype(&::BIO_free)> bio(BIO_new(BIO_s_file()), ::BIO_free);
        if (bio != nullptr) {
            if (BIO_write_filename(bio.get(), const_cast<char*>(file.c_str())) > 0) {
                result = PEM_write_bio_X509(bio.get(), cert) == 1;
            }
        }
        return result;
    }

    /*
    std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)> generateKeyPair(int32_t bits) {
        std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)> key(EVP_RSA_gen(bits), ::EVP_PKEY_free);
        return key;
    }*/
}

void CertificateGenerator::TestAll()
{
    /*
    std::unique_ptr<X509, decltype(&::X509_free)> certificate(X509_new(), ::X509_free);
    if (certificate == nullptr) {
        std::cerr << "Failed to create certificate" << std::endl;
        return ;
    }

    const uint32_t serialNum = 20;
    bool res = setSerialNumber(certificate.get(), serialNum);
    if (!res) {
        std::cerr << "Failed to setSerialNumber" << std::endl;
        return;
    }

    const long ver = 0x0; // version 1
    res = setVersion(certificate.get(), ver);
    if (!res) {
        std::cerr << "Failed to setVersion" << std::endl;
        return;
    }

    static constexpr const char* key = "CN";
    static constexpr const char* value = "Common Name";
    res = updateSubjectName(certificate.get(), key, value);
    if (!res) {
        std::cerr << "Failed to updateSubjectName" << std::endl;
        return;
    }

    const uint32_t y = 2022;
    const uint32_t m = 12;
    const uint32_t d = 25;
    const int32_t offset_days = 0;
    res = setNotAfter(certificate.get(), y, m, d, offset_days);
    if (!res) {
        std::cerr << "Failed to setNotAfter" << std::endl;
        return;
    }

    res = setNotBefore(certificate.get(), y, m, d, offset_days);
    if (!res) {
        std::cerr << "Failed to setNotBefore" << std::endl;
        return;
    }

    const int32_t bits = 2048;
    std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)> keyPair = generateKeyPair(bits);
    res = setPublicKey(certificate.get(), keyPair.get());
    if (!res) {
        std::cerr << "Failed to setPublicKey" << std::endl;
        return;
    }

    const int nid = NID_basic_constraints;
    static const char* extensionValue = "critical,CA:TRUE";

    res = addStandardExtension(certificate.get(), nullptr, nid, extensionValue);
    if (!res) {
        std::cerr << "Failed to addStandardExtension" << std::endl;
        return;
    }

    res = addCustomExtension(certificate.get(), "1.2.3", "myvalue", false);
    if (!res) {
        std::cerr << "Failed to addCustomExtension" << std::endl;
        return;
    }

    res = signCert(certificate.get(), keyPair.get(), EVP_sha256());
    if (!res) {
        std::cerr << "Failed to signCert" << std::endl;
        return;
    }

    std::unique_ptr<X509, decltype(&::X509_free)> duplicate(X509_dup(certificate.get()), ::X509_free);
    if (duplicate == nullptr) {
        std::cerr << "Failed to duplicate certificate" << std::endl;
        return;
    }

    res = setIssuer(certificate.get(), duplicate.get());
    if (!res) {
        std::cerr << "Failed to setIssuer" << std::endl;
        return;
    }

    res = addIssuerInfo(certificate.get(), key, value);
    if (!res) {
        std::cerr << "Failed to addIssuerInfo" << std::endl;
        return;
    }

    res = signCert(certificate.get(), keyPair.get(), EVP_sha256());
    if (!res) {
        std::cerr << "Failed to signCert" << std::endl;
        return;
    }

    static const std::string filename = "certificate.pem";
    res = saveCertToPemFile(certificate.get(), filename);
    if (!res) {
        std::cerr << "Failed to saveCertToPemFile" << std::endl;
        return;
    }
    */
};

