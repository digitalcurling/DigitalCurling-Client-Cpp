// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: Unlicense

#pragma once

#ifndef DIGITALCURLING_CLIENT_USE_LOADER
    #error "JsonConvert needs digitalcurling_plugin_loader."
#endif

#include <string>
#include <sstream>
#include <nlohmann/json.hpp>
#include "digitalcurling/plugins/plugin_loader.hpp"

using nlohmann::json;

namespace digitalcurling::plugins {

template <typename TPlugin, typename T>
class JsonConvert {

static_assert(std::is_base_of_v<JsonConverter<T>, TPlugin>);
static_assert(std::is_base_of_v<IPlugin, TPlugin>);

private:
    static void Convert(
        std::vector<std::shared_ptr<TPlugin>> (digitalcurling::plugins::PluginLoader::*plugin_func)(),
        std::function<bool(JsonConverter<T> *)> func,
        std::string const& target_name
    ){
        std::vector<std::shared_ptr<TPlugin>> plugins = (plugins::PluginLoader::GetInstance()->*plugin_func)();
        bool found = false;
        for (std::shared_ptr<TPlugin> plugin : plugins) {
            auto converter = dynamic_cast<JsonConverter<T>*>(plugin.get());
            if (converter && (found = func(converter))) break;
        }

        if (!found) {
            std::ostringstream stream;
            stream << "adl_serializer<" << typeid(T).name() << ">: no plugin found for " << target_name << ".";
            throw std::runtime_error(stream.str());
        }
    };

public:
    inline static void ToJson(
        std::vector<std::shared_ptr<TPlugin>> (digitalcurling::plugins::PluginLoader::*plugin_func)(),
        nlohmann::json & j,
        T const& v
    ) {
        auto func = [&j, &v] (JsonConverter<T> *converter) {
            return converter->ToJson(j, v);
        };
        Convert(plugin_func, func, "class:" + std::string(typeid(v).name()));
    };

    inline static void FromJson(
        std::vector<std::shared_ptr<TPlugin>> (digitalcurling::plugins::PluginLoader::*plugin_func)(),
        nlohmann::json const& j,
        std::unique_ptr<T> & v
    ) {

        auto func = [&j, &v] (JsonConverter<T> *converter) {
            return converter->FromJson(j, v);
        };
        Convert(plugin_func, func, "json:" + j.at("type").get<std::string>());
    };
};

} // namespace digitalcurling::plugins

/// @cond Doxygen_Suppress
namespace nlohmann {

template <>
struct adl_serializer<digitalcurling::simulators::ISimulatorFactory> {
    static void to_json(nlohmann::json & j, digitalcurling::simulators::ISimulatorFactory const& v) {
        digitalcurling::plugins::JsonConvert<digitalcurling::plugins::SimulatorPluginBase, digitalcurling::simulators::ISimulatorFactory>::ToJson(
            &digitalcurling::plugins::PluginLoader::GetSimulatorPlugins, j, v
        );
    }
};
template <>
struct adl_serializer<std::unique_ptr<digitalcurling::simulators::ISimulatorFactory>> {
    static void to_json(nlohmann::json & j, std::unique_ptr<digitalcurling::simulators::ISimulatorFactory> const& v) {
        digitalcurling::plugins::JsonConvert<digitalcurling::plugins::SimulatorPluginBase, digitalcurling::simulators::ISimulatorFactory>::ToJson(
            &digitalcurling::plugins::PluginLoader::GetSimulatorPlugins, j, *v
        );
    }
    static void from_json(nlohmann::json const& j, std::unique_ptr<digitalcurling::simulators::ISimulatorFactory> & v) {
        digitalcurling::plugins::JsonConvert<digitalcurling::plugins::SimulatorPluginBase, digitalcurling::simulators::ISimulatorFactory>::FromJson(
            &digitalcurling::plugins::PluginLoader::GetSimulatorPlugins, j, v
        );
    }
};

template <>
struct adl_serializer<digitalcurling::simulators::ISimulatorStorage> {
    static void to_json(nlohmann::json & j, digitalcurling::simulators::ISimulatorStorage const& v) {
        digitalcurling::plugins::JsonConvert<digitalcurling::plugins::SimulatorPluginBase, digitalcurling::simulators::ISimulatorStorage>::ToJson(
            &digitalcurling::plugins::PluginLoader::GetSimulatorPlugins, j, v
        );
    }
};
template <>
struct adl_serializer<std::unique_ptr<digitalcurling::simulators::ISimulatorStorage>> {
    static void to_json(nlohmann::json & j, std::unique_ptr<digitalcurling::simulators::ISimulatorStorage> const& v) {
        digitalcurling::plugins::JsonConvert<digitalcurling::plugins::SimulatorPluginBase, digitalcurling::simulators::ISimulatorStorage>::ToJson(
            &digitalcurling::plugins::PluginLoader::GetSimulatorPlugins, j, *v
        );
    }
    static void from_json(nlohmann::json const& j, std::unique_ptr<digitalcurling::simulators::ISimulatorStorage> & v) {
        digitalcurling::plugins::JsonConvert<digitalcurling::plugins::SimulatorPluginBase, digitalcurling::simulators::ISimulatorStorage>::FromJson(
            &digitalcurling::plugins::PluginLoader::GetSimulatorPlugins, j, v
        );
    }
};

template <>
struct adl_serializer<digitalcurling::players::IPlayerFactory> {
    static void to_json(nlohmann::json & j, digitalcurling::players::IPlayerFactory const& v) {
        digitalcurling::plugins::JsonConvert<digitalcurling::plugins::PlayerPluginBase, digitalcurling::players::IPlayerFactory>::ToJson(
            &digitalcurling::plugins::PluginLoader::GetPlayerPlugins, j, v
        );
    }
};
template <>
struct adl_serializer<std::unique_ptr<digitalcurling::players::IPlayerFactory>> {
    static void to_json(nlohmann::json & j, std::unique_ptr<digitalcurling::players::IPlayerFactory> const& v) {
        digitalcurling::plugins::JsonConvert<digitalcurling::plugins::PlayerPluginBase, digitalcurling::players::IPlayerFactory>::ToJson(
            &digitalcurling::plugins::PluginLoader::GetPlayerPlugins, j, *v
        );
    }
    static void from_json(nlohmann::json const& j, std::unique_ptr<digitalcurling::players::IPlayerFactory> & v) {
        digitalcurling::plugins::JsonConvert<digitalcurling::plugins::PlayerPluginBase, digitalcurling::players::IPlayerFactory>::FromJson(
            &digitalcurling::plugins::PluginLoader::GetPlayerPlugins, j, v
        );
    }
};

template <>
struct adl_serializer<digitalcurling::players::IPlayerStorage> {
    static void to_json(nlohmann::json & j, digitalcurling::players::IPlayerStorage const& v) {
        digitalcurling::plugins::JsonConvert<digitalcurling::plugins::PlayerPluginBase, digitalcurling::players::IPlayerStorage>::ToJson(
            &digitalcurling::plugins::PluginLoader::GetPlayerPlugins, j, v
        );
    }
};
template <>
struct adl_serializer<std::unique_ptr<digitalcurling::players::IPlayerStorage>> {
    static void to_json(nlohmann::json & j, std::unique_ptr<digitalcurling::players::IPlayerStorage> const& v) {
        digitalcurling::plugins::JsonConvert<digitalcurling::plugins::PlayerPluginBase, digitalcurling::players::IPlayerStorage>::ToJson(
            &digitalcurling::plugins::PluginLoader::GetPlayerPlugins, j, *v
        );
    }
    static void from_json(nlohmann::json const& j, std::unique_ptr<digitalcurling::players::IPlayerStorage> & v) {
        digitalcurling::plugins::JsonConvert<digitalcurling::plugins::PlayerPluginBase, digitalcurling::players::IPlayerStorage>::FromJson(
            &digitalcurling::plugins::PluginLoader::GetPlayerPlugins, j, v
        );
    }
};

} // namespace nlohmann
/// @endcond
