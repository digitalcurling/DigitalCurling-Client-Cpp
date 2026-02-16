// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: Unlicense

#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <digitalcurling/players/i_player_factory.hpp>
#include <digitalcurling/simulators/i_simulator_factory.hpp>

namespace digitalcurling::client {

class IFactoryCreator {
public:
    virtual std::unique_ptr<players::IPlayerFactory> CreatePlayerFactory(nlohmann::json const& json) = 0;
    virtual std::unique_ptr<simulators::ISimulatorFactory> CreateSimulatorFactory(nlohmann::json const& json) = 0;
};

} // namespace digitalcurling::client
