// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: Unlicense

#pragma once

#include <optional>
#include <string>
#include <nlohmann/json.hpp>
#include <digitalcurling/game_rule.hpp>
#include <digitalcurling/game_setting.hpp>
#include <digitalcurling/game_state.hpp>
#include <digitalcurling/moves/shot.hpp>

namespace digitalcurling::client {

struct MatchInfo {
    std::string name;
    std::optional<std::string> winner;
    GameRule rule;
    GameSetting setting;
    nlohmann::json simulator;
    nlohmann::json players;
};
inline void from_json(nlohmann::json const& j, MatchInfo & v) {
    GameRule rule;
    GameSetting setting;

    rule.is_wheelchair = false;
    auto rule_type = j.at("game_mode").get<std::string>();
    if (rule_type == "standard") {
        rule.type = GameRuleType::kStandard;
    } else if (rule_type == "mix_doubles") {
        rule.type = GameRuleType::kMixedDoubles;
    } else {
        throw std::invalid_argument("Unsupported game_mode: " + rule_type);
    }
    auto applied_rule = j.at("applied_rule").get<int>();
    if (applied_rule == 0) {
        rule.free_guard_zone = rules::FreeGuardZoneRule(true);
    } else if (applied_rule == 1) {
        rule.no_tick_shot = rules::NoTickShotRule(true);
    } else if (applied_rule == 2) {
        rule.free_guard_zone = rules::FreeGuardZoneRule(true, 3);
    } else {
        throw std::invalid_argument("Unsupported applied_rule: " + std::to_string(applied_rule));
    }

    setting.max_end = j.at("standard_end_count").get<std::uint8_t>();
    auto time_limit = j.at("time_limit").get<std::uint32_t>() * 1000;
    setting.thinking_time = {
        std::chrono::milliseconds(time_limit),
        std::chrono::milliseconds(time_limit)
    };
    auto extra_time_limit = j.at("extra_end_time_limit").get<std::uint32_t>() * 1000;
    setting.extra_end_thinking_time = {
        std::chrono::milliseconds(extra_time_limit),
        std::chrono::milliseconds(extra_time_limit)
    };

    v.name = j.at("match_name").get<std::string>();
    v.winner = j.at("winner_team_id").get<std::optional<std::string>>();
    v.rule = rule;
    v.setting = setting;

    v.simulator = {
        { "type", j.at("simulator").at("simulator_name").get<std::string>() },
        { "seconds_per_frame", 0.001 }
    };

    if (rule.type == GameRuleType::kMixedDoubles) {
        v.players = {
            {
                { "type", "normal_dist" },
                { "max_speed", 4.0 },
                { "stddev_speed", 0.08 },
                { "stddev_angle", 0.006 },
                { "gender", "male" }
            },
            {
                { "type", "normal_dist" },
                { "max_speed", 3.0 },
                { "stddev_speed", 0.025 },
                { "stddev_angle", 0.0025 },
                { "gender", "female" }
            }
        };
    } else {
        nlohmann::json player_factory_json = {
            { "type", "normal_dist" },
            { "max_speed", 4.0 },
            { "stddev_speed", 0.0076 },
            { "stddev_angle", 0.0018 },
            { "gender", "male" }
        };
        v.players = nlohmann::json(4, player_factory_json);
    }
}

struct StateUpdateEventData {
    int total_shot_number;
    Team next_shot_team;
    GameState game_state;
    std::optional<moves::Shot> last_shot;
};

} // namespace digitalcurling::client
