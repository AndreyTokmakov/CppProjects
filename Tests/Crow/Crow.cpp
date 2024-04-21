/**============================================================================
Name        : Crow.cpp
Created on  : 19.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Crow.cpp
============================================================================**/

#include "Crow.h"
#include "Logger.h"
#include "Utils.h"

#include "../Helpers/Wrapper.h"

#include <cstring>

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>

namespace Crow::Common
{
    enum class HTTPMethod : char
    {
#ifndef DELETE
        DELETE = 0,
        GET,
        HEAD,
        POST,
        PUT,

        CONNECT,
        OPTIONS,
        TRACE,

        PATCH,
        PURGE,

        COPY,
        LOCK,
        MKCOL,
        MOVE,
        PROPFIND,
        PROPPATCH,
        SEARCH,
        UNLOCK,
        BIND,
        REBIND,
        UNBIND,
        ACL,

        REPORT,
        MKACTIVITY,
        CHECKOUT,
        MERGE,

        MSEARCH,
        NOTIFY,
        SUBSCRIBE,
        UNSUBSCRIBE,

        MKCALENDAR,

        LINK,
        UNLINK,

        SOURCE,
#endif

        Delete = 0,
        Get,
        Head,
        Post,
        Put,

        Connect,
        Options,
        Trace,

        Patch,
        Purge,

        Copy,
        Lock,
        MkCol,
        Move,
        Propfind,
        Proppatch,
        Search,
        Unlock,
        Bind,
        Rebind,
        Unbind,
        Acl,

        Report,
        MkActivity,
        Checkout,
        Merge,

        MSearch,
        Notify,
        Subscribe,
        Unsubscribe,

        MkCalendar,

        Link,
        Unlink,

        Source,


        InternalMethodCount,
        // should not add an item below this line: used for array count
    };
}

namespace Crow::Common::Tests
{
    void Method()
    {
        HTTPMethod method;
        if (HTTPMethod::Delete == method)
        {
            std::cout << "Default = HTTPMethod::Delete" << std::endl;
        }
    }
};


namespace Crow::StringQuery
{
// isxdigit _is_ available in <ctype.h>, but let's avoid another header instead
#define CROW_QS_ISHEX(x)    ((((x)>='0'&&(x)<='9') || ((x)>='A'&&(x)<='F') || ((x)>='a'&&(x)<='f')) ? 1 : 0)
#define CROW_QS_HEX2DEC(x)  (((x)>='0'&&(x)<='9') ? (x)-48 : ((x)>='A'&&(x)<='F') ? (x)-55 : ((x)>='a'&&(x)<='f') ? (x)-87 : 0)
#define CROW_QS_ISQSCHR(x) ((((x)=='=')||((x)=='#')||((x)=='&')||((x)=='\0')) ? 0 : 1)



    inline int qs_strncmp(const char * s, const char * qs, size_t n)
    {
        unsigned char u1, u2, unyb, lnyb;

        while(n-- > 0)
        {
            u1 = static_cast<unsigned char>(*s++);
            u2 = static_cast<unsigned char>(*qs++);

            if ( ! CROW_QS_ISQSCHR(u1) ) {  u1 = '\0';  }
            if ( ! CROW_QS_ISQSCHR(u2) ) {  u2 = '\0';  }

            if ( u1 == '+' ) {  u1 = ' ';  }
            if ( u1 == '%' ) // easier/safer than scanf
            {
                unyb = static_cast<unsigned char>(*s++);
                lnyb = static_cast<unsigned char>(*s++);
                if ( CROW_QS_ISHEX(unyb) && CROW_QS_ISHEX(lnyb) )
                    u1 = (CROW_QS_HEX2DEC(unyb) * 16) + CROW_QS_HEX2DEC(lnyb);
                else
                    u1 = '\0';
            }

            if ( u2 == '+' ) {  u2 = ' ';  }
            if ( u2 == '%' ) // easier/safer than scanf
            {
                unyb = static_cast<unsigned char>(*qs++);
                lnyb = static_cast<unsigned char>(*qs++);
                if ( CROW_QS_ISHEX(unyb) && CROW_QS_ISHEX(lnyb) )
                    u2 = (CROW_QS_HEX2DEC(unyb) * 16) + CROW_QS_HEX2DEC(lnyb);
                else
                    u2 = '\0';
            }

            if ( u1 != u2 )
                return u1 - u2;
            if ( u1 == '\0' )
                return 0;
        }
        if ( CROW_QS_ISQSCHR(*qs) )
            return -1;
        else
            return 0;
    }

    inline int qs_decode(char * qs)
    {
        int i=0, j=0;

        while( CROW_QS_ISQSCHR(qs[j]) )
        {
            if ( qs[j] == '+' ) {  qs[i] = ' ';  }
            else if ( qs[j] == '%' ) // easier/safer than scanf
            {
                if ( ! CROW_QS_ISHEX(qs[j+1]) || ! CROW_QS_ISHEX(qs[j+2]) )
                {
                    qs[i] = '\0';
                    return i;
                }
                qs[i] = (CROW_QS_HEX2DEC(qs[j+1]) * 16) + CROW_QS_HEX2DEC(qs[j+2]);
                j+=2;
            }
            else
            {
                qs[i] = qs[j];
            }
            i++;  j++;
        }
        qs[i] = '\0';

        return i;
    }


    inline size_t qs_parse(char* qs, char* qs_kv[], size_t qs_kv_size, bool parse_url = true)
    {
        size_t i, j;
        char * substr_ptr;

        for(i=0; i<qs_kv_size; i++)  qs_kv[i] = NULL;

        // find the beginning of the k/v substrings or the fragment
        substr_ptr = parse_url ? qs + strcspn(qs, "?#") : qs;
        if (parse_url)
        {
            if (substr_ptr[0] != '\0')
                substr_ptr++;
            else
                return 0; // no query or fragment
        }

        i=0;
        while(i<qs_kv_size)
        {
            qs_kv[i] = substr_ptr;
            j = strcspn(substr_ptr, "&");
            if ( substr_ptr[j] == '\0' ) { i++; break;  } // x &'s -> means x iterations of this loop -> means *x+1* k/v pairs
            substr_ptr += j + 1;
            i++;
        }

        // we only decode the values in place, the keys could have '='s in them
        // which will hose our ability to distinguish keys from values later
        for(j=0; j<i; j++)
        {
            substr_ptr = qs_kv[j] + strcspn(qs_kv[j], "=&#");
            if ( substr_ptr[0] == '&' || substr_ptr[0] == '\0')  // blank value: skip decoding
                substr_ptr[0] = '\0';
            else
                qs_decode(++substr_ptr);
        }

#ifdef _qsSORTING
        // TODO: qsort qs_kv, using qs_strncmp() for the comparison
#endif

        return i;
    }


    inline char * qs_k2v(const char * key, char * const * qs_kv, size_t qs_kv_size, int nth = 0)
    {
        size_t i;
        size_t key_len, skip;

        key_len = strlen(key);

#ifdef _qsSORTING
        // TODO: binary search for key in the sorted qs_kv
#else  // _qsSORTING
        for(i=0; i<qs_kv_size; i++)
        {
            // we rely on the unambiguous '=' to find the value in our k/v pair
            if ( qs_strncmp(key, qs_kv[i], key_len) == 0 )
            {
                skip = strcspn(qs_kv[i], "=");
                if ( qs_kv[i][skip] == '=' )
                    skip++;
                // return (zero-char value) ? ptr to trailing '\0' : ptr to value
                if(nth == 0)
                    return qs_kv[i] + skip;
                else
                    --nth;
            }
        }
#endif  // _qsSORTING

        return nullptr;
    }

    inline std::pair<std::pair<std::string, std::string>, bool>
    qs_dict_name2kv(const char * dict_name, char * const * qs_kv, size_t qs_kv_size, int nth = 0)
    {
        size_t skip_to_eq, skip_to_brace_open, skip_to_brace_close;
        const size_t name_len = strlen(dict_name);

#ifdef _qsSORTING
        // TODO: binary search for key in the sorted qs_kv
#else  // _qsSORTING
        for (size_t i = 0; i < qs_kv_size; i++)
        {
            if ( strncmp(dict_name, qs_kv[i], name_len) == 0 )
            {
                skip_to_eq = strcspn(qs_kv[i], "=");
                if ( qs_kv[i][skip_to_eq] == '=' )
                    skip_to_eq++;
                skip_to_brace_open = strcspn(qs_kv[i], "[");
                if ( qs_kv[i][skip_to_brace_open] == '[' )
                    skip_to_brace_open++;
                skip_to_brace_close = strcspn(qs_kv[i], "]");

                if ( skip_to_brace_open <= skip_to_brace_close &&
                     skip_to_brace_open > 0 &&
                     skip_to_brace_close > 0 &&
                     nth == 0 )
                {
                    std::string key = std::string(qs_kv[i] + skip_to_brace_open, skip_to_brace_close - skip_to_brace_open);
                    std::string value = std::string(qs_kv[i] + skip_to_eq);
                    return std::make_pair<std::pair<std::string, std::string>, bool> (
                            {std::move(key), std::move(value)}, true);
                }
                else
                {
                    --nth;
                }
            }
        }
#endif  // _qsSORTING

        return std::make_pair<std::pair<std::string, std::string>, bool> ({}, false);;
    }

    inline std::unique_ptr<std::pair<std::string, std::string>>
    qs_dict_name2kv_old(const char * dict_name, char * const * qs_kv, size_t qs_kv_size, int nth = 0)
    {
        size_t skip_to_eq, skip_to_brace_open, skip_to_brace_close;
        const size_t name_len = strlen(dict_name);

#ifdef _qsSORTING
        // TODO: binary search for key in the sorted qs_kv
#else  // _qsSORTING
        for(size_t i = 0; i < qs_kv_size; i++)
        {
            if ( strncmp(dict_name, qs_kv[i], name_len) == 0 )
            {
                skip_to_eq = strcspn(qs_kv[i], "=");
                if ( qs_kv[i][skip_to_eq] == '=' )
                    skip_to_eq++;
                skip_to_brace_open = strcspn(qs_kv[i], "[");
                if ( qs_kv[i][skip_to_brace_open] == '[' )
                    skip_to_brace_open++;
                skip_to_brace_close = strcspn(qs_kv[i], "]");

                if ( skip_to_brace_open <= skip_to_brace_close &&
                     skip_to_brace_open > 0 &&
                     skip_to_brace_close > 0 &&
                     nth == 0 )
                {
                    auto key = std::string(qs_kv[i] + skip_to_brace_open, skip_to_brace_close - skip_to_brace_open);
                    auto value = std::string(qs_kv[i] + skip_to_eq);
                    return std::make_unique<std::pair<std::string, std::string>>(std::move(key), std::move(value));
                }
                else
                {
                    --nth;
                }
            }
        }
#endif  // _qsSORTING

        return nullptr;
    }

    class query_string
    {
    public:
        static const int MAX_KEY_VALUE_PAIRS_COUNT = 256;

        // INFO:
        query_string() = default;

        query_string(const query_string& qs):
                url_(qs.url_)
        {
            for (auto p : qs.key_value_pairs_)
            {
                key_value_pairs_.push_back((char*)(p - qs.url_.c_str() + url_.c_str()));
            }
        }

        query_string& operator=(const query_string& qs)
        {
            url_ = qs.url_;
            key_value_pairs_.clear();
            for (auto p : qs.key_value_pairs_)
            {
                key_value_pairs_.push_back((char*)(p - qs.url_.c_str() + url_.c_str()));
            }
            return *this;
        }

        // INFO:
        // FIXME : noexept
        query_string& operator=(query_string&& qs) noexcept
        {
            key_value_pairs_ = std::move(qs.key_value_pairs_);
            char* old_data = (char*)qs.url_.c_str();
            url_ = std::move(qs.url_);
            for (auto& p : key_value_pairs_)
            {
                p += (char*)url_.c_str() - old_data;
            }
            return *this;
        }


        // INFO: explicit
        explicit query_string(std::string params, bool url = true):
                url_(std::move(params))
        {
            if (url_.empty())
                return;

            key_value_pairs_.resize(MAX_KEY_VALUE_PAIRS_COUNT);

            const size_t count = qs_parse(&url_[0], &key_value_pairs_[0], MAX_KEY_VALUE_PAIRS_COUNT, url);
            key_value_pairs_.resize(count);

            // INFO: Free unused memory
            key_value_pairs_.shrink_to_fit();
        }

        void clear()
        {
            key_value_pairs_.clear();
            url_.clear();
        }

        friend std::ostream& operator<<(std::ostream& os, const query_string& qs)
        {
            os << "[ ";
            for (size_t i = 0; i < qs.key_value_pairs_.size(); ++i)
            {
                if (i)
                    os << ", ";
                os << qs.key_value_pairs_[i];
            }
            os << " ]";
            return os;
        }

        /// Get a value from a name, used for `?name=value`.

        ///
        /// Note: this method returns the value of the first occurrence of the key only, to return all occurrences, see \ref get_list().
        char* get(const std::string& name) const
        {
            char* ret = qs_k2v(name.c_str(), key_value_pairs_.data(), key_value_pairs_.size());
            return ret;
        }

        /// Works similar to \ref get() except it removes the item from the query string.
        char* pop(const std::string& name)
        {
            char* ret = get(name);
            if (ret != nullptr)
            {
                for (unsigned int i = 0; i < key_value_pairs_.size(); i++)
                {
                    std::string str_item(key_value_pairs_[i]);
                    if (str_item.substr(0, name.size() + 1) == name + '=')
                    {
                        key_value_pairs_.erase(key_value_pairs_.begin() + i);
                        break;
                    }
                }
            }
            return ret;
        }

        /// Returns a list of values, passed as `?name[]=value1&name[]=value2&...name[]=valuen` with n being the size of the list.

        ///
        /// Note: Square brackets in the above example are controlled by `use_brackets` boolean (true by default).
        /// If set to false, the example becomes `?name=value1,name=value2...name=valuen`
        std::vector<char*> get_list(const std::string& name, bool use_brackets = true) const
        {
            std::vector<char*> ret;
            std::string plus = name + (use_brackets ? "[]" : "");
            char* element = nullptr;

            int count = 0;
            while (true)
            {
                element = qs_k2v(plus.c_str(), key_value_pairs_.data(), key_value_pairs_.size(), count++);
                if (!element)
                    break;
                ret.push_back(element);
            }
            return ret;
        }

        /// Similar to \ref get_list() but it removes the
        std::vector<char*> pop_list(const std::string& name, bool use_brackets = true)
        {
            std::vector<char*> ret = get_list(name, use_brackets);
            if (!ret.empty())
            {
                for (unsigned int i = 0; i < key_value_pairs_.size(); i++)
                {
                    std::string str_item(key_value_pairs_[i]);
                    if ((use_brackets ? (str_item.substr(0, name.size() + 3) == name + "[]=") : (str_item.substr(0, name.size() + 1) == name + '=')))
                    {
                        key_value_pairs_.erase(key_value_pairs_.begin() + i--);
                    }
                }
            }
            return ret;
        }

        /// Works similar to \ref get_list() except the brackets are mandatory must not be empty.

        ///
        /// For example calling `get_dict(yourname)` on `?yourname[sub1]=42&yourname[sub2]=84` would give a map containing `{sub1 : 42, sub2 : 84}`.
        ///
        /// if your query string has both empty brackets and ones with a key inside, use pop_list() to get all the values without a key before running this method.
        [[nodiscard]]
        std::unordered_map<std::string, std::string> get_dict(const std::string& name) const
        {
            std::unordered_map<std::string, std::string> ret;

            int count = 0;
            while (true)
            {
                /*
                auto element = qs_dict_name2kv(name.c_str(), key_value_pairs_.data(), key_value_pairs_.size(), count++);
                if (element.second)
                    ret.emplace(std::move(element.first));
                else
                    break;
                */

                auto element = qs_dict_name2kv_old(name.c_str(), key_value_pairs_.data(), key_value_pairs_.size(), count++);
                if (element)
                    ret.emplace(std::move(element->first), std::move(element->second));
                else
                    break;
            }
            return ret;
        }

        /// Works the same as \ref get_dict() but removes the values from the query string.
        std::unordered_map<std::string, std::string> pop_dict(const std::string& name)
        {
            std::unordered_map<std::string, std::string> ret = get_dict(name);
            if (!ret.empty())
            {
                for (unsigned int i = 0; i < key_value_pairs_.size(); i++)
                {
                    std::string str_item(key_value_pairs_[i]);
                    if (str_item.substr(0, name.size() + 1) == name + '[')
                    {
                        key_value_pairs_.erase(key_value_pairs_.begin() + i--);
                    }
                }
            }
            return ret;
        }

        std::vector<std::string> keys() const noexcept
        {
            std::vector<std::string> keys;
            keys.reserve(key_value_pairs_.size());

            for (const char* const element : key_value_pairs_)
            {
                const char* delimiter = strchr(element, '=');
                if (delimiter)
                    keys.emplace_back(element, delimiter);
                else
                    keys.emplace_back(element);
            }

            return keys;
        }

        std::vector<std::string> keys_old() const
        {
            std::vector<std::string> ret;
            for (auto element : key_value_pairs_)
            {
                std::string str_element(element);
                ret.emplace_back(str_element.substr(0, str_element.find('=')));
            }
            return ret;
        }

    // private:
        std::string url_;
        std::vector<char*> key_value_pairs_;
    };
}


namespace Crow::StringQuery::Tests
{
    // INFO: Tests
    //  check capacity() of key_value_pairs_ | Construction: query_string()

    // TODO:
    //   Refactor:  std::vector<std::string> keys() const
    //              **** PERF ****
    //  qs_dict_name2kv()
    //      return | std::pair<std::pair<std::string, std::string>, bool>
    //      performance Tests
    //  get_dict()
    //


    void CreateTest()
    {
        std::string urlParam { "params?h=1&foo=bar&lol&count[]=1&count[]=4&pew=5.2"};
        query_string query_params(urlParam);


        std::cout << query_params.url_ << std::endl;
        for (const auto entry: query_params.key_value_pairs_)
            std::cout << "\t" << entry << std::endl;
    }


    void KeysTest()
    {
        std::string urlParam { "params?h=1&foo=bar&lol&count[]=1&count[]=4&pew=5.2"};
        query_string query_params(urlParam);


        const std::vector<std::string> keys = query_params.keys();
        for (const auto& entry: keys)
            std::cout << "\t" << entry << std::endl;

        std::cout << std::endl;

        const std::vector<std::string> keysOld = query_params.keys_old();
        for (const auto& entry: keysOld)
            std::cout << "\t" << entry << std::endl;
    }

    void KeysTest_Perf()
    {
        const std::string urlParam { "params?h=1&foo=bar&lol&count[]=1&count[]=4&pew=5.2&h=1&foo=bar&lol&count[]=1&count[]=4"
                                     "&pew=5.2&h=1&foo=bar&lol&count[]=1&count[]=4&pew=5.2&h=1&foo=bar&lol&count[]=1&count[]=4&pew=5.2"
                                     "&pew=5.2&h=1&foo=bar&lol&count[]=1&count[]=4&pew=5.2&h=1&foo=bar&lol&count[]=1&count[]=4&pew=5.2"
                                     "&pew=5.2&h=1&foo=bar&lol&count[]=1&count[]=4&pew=5.2&h=1&foo=bar&lol&count[]=1&count[]=4&pew=5.2"
                                     "&pew=5.2&h=1&foo=bar&lol&count[]=1&count[]=4&pew=5.2&h=1&foo=bar&lol&count[]=1&count[]=4&pew=5.2"};
        query_string query_params(urlParam);


        {
            Utils::ScopedTimer timer {"keys()"};
            for (int i = 0; i < 1'000'000; ++i)
                const std::vector<std::string> keys = query_params.keys();
        }

        {
            Utils::ScopedTimer timer {"keys_old()"};
            for (int i = 0; i < 1'000'000; ++i)
                const std::vector<std::string> keys = query_params.keys_old();
        }
    }

    void GetDict_Test()
    {
        std::string urlParam { "params?h=1&foo=bar&lol&count[one]=1&count[two]=2&pew=5.2"};
        query_string query_params(urlParam);


        const auto params = query_params.get_dict("count");
        for (const auto& [k,v ]: params)
            std::cout << k << " = " << v << std::endl;

    }

    void GetList_Test()
    {
        std::string urlParam { "params?h=1&tag[]=foo&lol&count[one]=1&tag[]=bar&count[two]=2&tag[]=three&pew=5.2"};
        query_string query_params(urlParam);


        const auto params = query_params.get_list("tag");
        for (const auto entry: params)
            std::cout << entry << std::endl;

    }

    void qs_dict_name2kv_Test_perf()
    {
        const std::string name {"count"};
        const std::string urlParam { "params?h=1&foo=bar&lol"
                               "&count[1]=1&count[2]=2&count[3]=3&count[4]=4&count[5]=5&count[6]=6"
                               "&count[7]=7&count[8]=8&count[9]=9&count[10]=10&count[11]=11&count[12]=12"};
        query_string query_params(urlParam);


        {
            Utils::ScopedTimer timer {"qs_dict_name2kv()"};
            for (int i = 0; i < 1'000'000; ++i)
            {
                int count = 0;
                while (true)
                {
                    auto element = qs_dict_name2kv(name.c_str(),
                                                   query_params.key_value_pairs_.data(),
                                                   query_params.key_value_pairs_.size(),
                                                   count++);
                    if (!element.second)
                        break;;
                }
            }
        }


        {
            Utils::ScopedTimer timer {"qs_dict_name2kv_old()"};
            for (int i = 0; i < 1'000'000; ++i)
            {
                int count = 0;
                while (true)
                {
                    auto element = qs_dict_name2kv_old(name.c_str(),
                                                   query_params.key_value_pairs_.data(),
                                                   query_params.key_value_pairs_.size(),
                                                   count++);
                    if (!element)
                        break;;
                }
            }
        }
    }
}

namespace TestData
{
    std::string params(const std::vector<std::pair<std::string, std::string>>& paramList)
    {
        std::string paramsStr {};
        for (const auto& param: paramList)
            paramsStr.append(param.first).append(1, '=').append(param.second).append(1, '&');
        paramsStr.resize(paramsStr.size()-1);
        return paramsStr;
    }

    void GenerateParams()
    {
        std::cout << params({{"one", "1"}, {"two", "2"}}) << std::endl;
    }
}


namespace Memory
{
    using Helpers::Integer;

    struct IntegerHash {
        std::size_t operator()(const Integer& s) const noexcept {
            return std::hash<int>{}(s.getValue());
        }
    };

    std::unique_ptr<std::pair<Integer, Integer>>
    getPairPtr()
    {
        // return std::make_unique<std::pair<Integer, Integer>>(1,1);

        Integer i1 {101}, i2 {202};
        // return std::make_unique<std::pair<Integer, Integer>>(std::move(i1),std::move(i2));
        return std::unique_ptr<std::pair<Integer, Integer>>(new std::pair<Integer, Integer>(std::move(i1),std::move(i2)));
    }

    void TestReturn_UniquePtr()
    {
        std::unordered_map<Integer, Integer, IntegerHash> store;
        std::unique_ptr<std::pair<Integer, Integer>> ptr = getPairPtr();

        std::cout << std::endl;

        // store.emplace(*std::move(ptr));
        store.emplace(std::move(ptr->first), std::move(ptr->second));
    }
}


void Crow::TestAll()
{
    Logger::TestAll();


    // Common::Tests::Method();

    // StringQuery::Tests::CreateTest();
    // StringQuery::Tests::KeysTest();
    // StringQuery::Tests::KeysTest_Perf();

    // StringQuery::Tests::GetList_Test();

    // StringQuery::Tests::GetDict_Test();
    // StringQuery::Tests::qs_dict_name2kv_Test_perf();

    // TestData::GenerateParams();

    // Memory::TestReturn_UniquePtr();

};
