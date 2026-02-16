// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: Unlicense

#pragma once

#ifndef DIGITALCURLING_CLIENT_USE_LOADER
    #error "PluginFactoryCreator needs digitalcurling_plugin_loader."
#endif

#include <memory>
#include <digitalcurling/plugins/plugin_manager.hpp>
#include "digitalcurling/client/i_factory_creator.hpp"

namespace digitalcurling::plugins {

class PluginFactoryCreator : public client::IFactoryCreator {
public:
    virtual inline std::unique_ptr<players::IPlayerFactory> CreatePlayerFactory(nlohmann::json const& json) override {
        auto& manager = digitalcurling::plugins::PluginManager::GetInstance();
        if (!manager.IsPluginLoaded(digitalcurling::plugins::PluginType::player, json["type"]))
            throw std::runtime_error("PluginFactoryCreator: player plugin not loaded: " + json["type"].get<std::string>());
        return manager.CreatePlayerFactory(json);
    }
    virtual inline std::unique_ptr<simulators::ISimulatorFactory> CreateSimulatorFactory(nlohmann::json const& json) override {
        auto& manager = digitalcurling::plugins::PluginManager::GetInstance();
        if (!manager.IsPluginLoaded(digitalcurling::plugins::PluginType::simulator, json["type"]))
            throw std::runtime_error("PluginFactoryCreator: simulator plugin not loaded: " + json["type"].get<std::string>());
        return manager.CreateSimulatorFactory(json);
    }
};

} // namespace digitalcurling::plugins
