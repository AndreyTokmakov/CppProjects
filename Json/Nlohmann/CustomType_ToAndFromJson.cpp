/**============================================================================
Name        : CustomType_ToAndFromJson.cpp
Created on  : 24.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Nlohmann.h
============================================================================**/

#include "CustomType_ToAndFromJson.h"


#include <iostream>
#include <string_view>
#include <print>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cmath>

#include <nlohmann/json.hpp>


namespace
{
    constexpr std::filesystem::path getTestDataDir() noexcept
    {
        return std::filesystem::current_path() / "../../Json/data/";
    }
}


namespace CustomType_ToAndFromJson::model
{
    struct Profile
    {
        std::optional<int32_t> id { std::nullopt };
        std::optional<int32_t> seq { std::nullopt };
        std::string uuid;
        std::string name;
        std::string description;
        double ratio { NAN };
    };

    std::ostream& operator<<(std::ostream& stream, const Profile& profile)
    {
        stream << "Profile {"
               << "\n\t id: " << profile.id.value_or(-1)
               << "\n\t seq: " << profile.seq.value_or(-1)
               << "\n\t uuid: " << profile.uuid
               << "\n\t name: " << profile.name
               << "\n\t ratio: " << profile.ratio
               << "\n\t description: " << profile.description
               << "\n}";
        return stream;
    }
}

namespace CustomType_ToAndFromJson::details
{
    void profile_from_json(const nlohmann::json& json, model::Profile& profile);
    void json_from_profile(nlohmann::json& json, const model::Profile& profile);
}

namespace nlohmann
{
    template <typename T>
    struct adl_serializer<std::optional<T>>
    {
        static void from_json(const json& json, std::optional<T>& opt)
        {
            if (json.is_null()) {
                opt = std::nullopt;
            } else {
                opt = json.get<T>();
            }
        }
    };

    template <>
    struct adl_serializer<CustomType_ToAndFromJson::model::Profile>
    {
        static void from_json(const json& json, CustomType_ToAndFromJson::model::Profile& profile) {
            CustomType_ToAndFromJson::details::profile_from_json(json , profile);
        }

        static void to_json(json& json, const CustomType_ToAndFromJson::model::Profile& profile) {
            CustomType_ToAndFromJson::details::json_from_profile(json, profile);
        }
    };
}

namespace CustomType_ToAndFromJson::details
{
    void profile_from_json(const nlohmann::json& json, model::Profile& profile)
    {
        if (auto key = json.find("id"); key != json.end()) {
            profile.id = key->get<decltype(profile.id)>();
        }
        json.at("seq").get_to(profile.seq);
        json.at("uuid").get_to(profile.uuid);
        json.at("name").get_to(profile.name);
        json.at("description").get_to(profile.description);
        json.at("ratio").get_to(profile.ratio);
    }

    void json_from_profile(nlohmann::json& json, const model::Profile& profile)
    {
        json["id"] = profile.id.value_or(0);
        json["seq"] = profile.seq.value_or(0);
        json["uuid"] = profile.uuid;
        json["name"] = profile.name;
        json["description"] = profile.description;
        json["ratio"] = profile.ratio;
    }
}



namespace CustomType_ToAndFromJson::Tests
{
    std::vector<model::Profile> parseProfiles(const std::filesystem::path& filePath)
    {
        std::vector<model::Profile> profiles;
        const nlohmann::basic_json data = nlohmann::json::parse(std::ifstream (filePath));
        for (const auto& jsonProfile: data.at("profiles"))
        {
            std::cout << jsonProfile << std::endl;
            jsonProfile.get_to(profiles.emplace_back());
        }
        return profiles;
    }

    void parseFromFile(const std::filesystem::path& dir)
    {
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(dir)){
            if (std::filesystem::is_regular_file(entry.status()))
            {
                const std::vector<model::Profile> profiles = parseProfiles(entry.path());
                for (const model::Profile& profile: profiles) {
                    std::cout << profile << std::endl;
                }

                std::cout << std::string(160, '-') << std::endl;

                for (const model::Profile& profile: profiles) {
                    nlohmann::json j = profile;
                    std::cout << j << std::endl;
                }
            }
        }
    }

    void parseString()
    {
        constexpr std::string_view input = R"( {
            "uuid": "7e3f47ec-c682-4a02-9336-82c4aa643849",
            "name": "Profile-2",
            "description": "Some Test Profile-2 description"
        })";

        const nlohmann::json data = nlohmann::json::parse(input);

        model::Profile profile;
        data.get_to(profile);

        profile.id = 123;

        nlohmann::json j = profile;
        std::cout << j << std::endl;
    }
};

void CustomType_ToAndFromJson::TestAll()
{
    Tests::parseFromFile(getTestDataDir() / "input");
    // Tests::parseString();

}
