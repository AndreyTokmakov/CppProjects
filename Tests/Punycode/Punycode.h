#pragma once

#include <string>

namespace Punycode
{
    [[nodiscard]]
    std::string punycodeDecode(const std::string_view input);

    [[nodiscard]]
    std::string punycodeEncode(const std::string_view input);

    void Tests();
}
