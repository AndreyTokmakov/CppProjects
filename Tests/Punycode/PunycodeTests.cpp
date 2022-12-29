
#include <iostream>
#include <vector>

#include <codecvt>
#include <locale>
#include "Punycode.h"

char asciitolower(char in) {
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}

std::u32string to_utf32(std::string& str) {
    return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(str);
}

std::string to_utf8(std::u32string& str32) {
    return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.to_bytes(str32);
}

template<typename... Args>
void insertToString(std::string & str, size_t& start, Args&&... args) {
    ((str[start++] = args), ...);
}


void Punycode::Tests()
{

    std::string str { "0000000000"};
    size_t pos = 0;

    insertToString(str, pos, '1', '2', '3', '4', '5');
    std::cout << str << std::endl;
    std::cout << pos << std::endl;



    /*
    std::setlocale(LC_ALL, "");
    std::wstring data = L"п Р и в Е т";
    std::wcout << data << std::endl;

    // C std::towlower
    for(auto c: data)
    {
        std::wcout << static_cast<wchar_t>(std::towlower(c));
    }
    std::wcout << std::endl;

    // C++ std::tolower(charT, std::locale)
    std::locale loc("");
    for(auto c: data)
    {
        // This is recommended
        std::wcout << std::tolower(c, loc);
    }
    std::wcout << std::endl;
    */



    /*
    const std::string encodedString = Punycode::punycodeEncode(str);
    std::cout << encodedString << std::endl;
    const std::string& decodedString = Punycode::punycodeDecode(encodedString);
    std::cout << decodedString << std::endl;
*/

    /*
    std::vector<std::pair<std::string, std::string>> values{
            {"https://привет.label.мир.com/", "https://xn--b1agh1afp.label.xn--h1ahn.com/"},
            {"點看", "xn--c1yn36f"},
            {"abæcdöef", "xn--abcdef-qua4k"},
            {"abcdef", "abcdef"},
            {"αβγ", "xn--mxacd"},
            {"ยจฆฟคฏข", "xn--22cdfh1b8fsa"},
            {"правда", "xn--80aafi6cg"},
            {"Bahnhof München-Ost", "xn--Bahnhof Mnchen-Ost-u6b"},
            {"-> $1.00 <-", "-> $1.00 <-"},
            {"ドメイン名例", "xn--eckwd4c7cu47r2wf"},
            {"MajiでKoiする5秒前", "xn--MajiKoi5-783gue6qz075azm5e"},
            {"「bücher」", "xn--bcher-kva8445foa"},
            {"https://google.com/", "https://google.com/"},
    };

    for (const auto & [nameExpected, encodedExpected]: values) {
        const std::string encodedString = Punycode::punycodeEncode(nameExpected);
        if (encodedString != encodedExpected) {
            std::cout << "ERROR! Expected: " << encodedExpected << ", Actual: " << encodedString << std::endl;
            continue;
        }
        // else std::cout << "OK:" << encodedExpected << std::endl;

        const std::string decodedString = Punycode::punycodeDecode(encodedString);
        if (decodedString != nameExpected) {
            std::cout << "ERROR! Expected: " << nameExpected << ", Actual: " << decodedString << std::endl;
            continue;
        }
        //else std::cout << "OK:" << nameExpected << std::endl;
    }
    */


    /*
    std::string input { "aBc"};
    std::string_view view {input.data(), 3};
    std::cout<< view << std::endl;
    */






}
