// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: Unlicense

#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <digitalcurling/digitalcurling.hpp>

namespace digitalcurling::client {

class IThinkingEngine {
public:
    IThinkingEngine() = default;
    virtual ~IThinkingEngine() = default;

    /// @brief 思考エンジンの名前を返す
    /// @return 思考エンジンの名前
    virtual std::string GetName() const = 0;

    /// @brief 思考エンジンの初期化処理
    /// @param[in] game_rule 試合ルール
    /// @param[in] game_setting 試合設定
    /// @param[in] simulator シミュレーターのファクトリー
    /// @param[in] players プレイヤーのファクトリーのリスト
    /// @returns 投球順のリスト
    virtual std::vector<std::uint8_t> OnInit(
        GameRule const& game_rule,
        GameSetting const& game_setting,
        std::unique_ptr<simulators::ISimulatorFactory> simulator,
        std::vector<std::unique_ptr<players::IPlayerFactory>> const& players
    ) = 0;

    /// @brief ゲーム開始の通知
    /// @param team 自チーム
    /// @param states これまでの試合状況とショットの履歴
    virtual void OnGameStart(
        Team const& team,
        std::vector<std::pair<GameState, std::optional<moves::Shot>>> states
    ) = 0;

    /// @brief 次のエンドの通知
    /// @param game_state 現在の試合状況
    virtual void OnNextEnd(GameState const& game_state) = 0;

    /// @brief 自チームのターンのアクション
    /// @param[in] game_state 現在の試合状況
    /// @return 行動
    virtual moves::Move OnMyTurn(
        std::unique_ptr<players::IPlayerFactory> const& player_factory,
        GameState const& game_state,
        std::optional<moves::Shot> const& last_shot
    ) = 0;

    /// @brief 相手チームのターンのアクション
    /// @param[in] game_state 現在の試合状況
    virtual void OnOpponentTurn(
        GameState const& game_state,
        std::optional<moves::Shot> const& last_shot
    ) = 0;

    /// @brief 試合終了の通知
    /// @param[in] game_state 現在の試合状況
    virtual void OnGameOver(GameState const& game_state) = 0;
};

class IStandardThinkingEngine : public virtual IThinkingEngine { };

class IMixedThinkingEngine : public virtual IThinkingEngine { };

class IMixedDoublesThinkingEngine : public virtual IThinkingEngine {
public:
    enum class PositionedStoneOptions {
        kCenterGuard = 0,
        kCenterHouse = 1,
        kPowerPlayLeft = 2,
        kPowerPlayRight = 3
    };

    /// @brief 次のエンドの開始ストーン位置を決定する
    /// @param game_state 現在の試合状況
    /// @return ストーン位置の選択肢
    virtual PositionedStoneOptions OnDecidePositionedStone(GameState const& game_state) = 0;
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    IMixedDoublesThinkingEngine::PositionedStoneOptions,
    {
        { IMixedDoublesThinkingEngine::PositionedStoneOptions::kCenterGuard, "center_guard" },
        { IMixedDoublesThinkingEngine::PositionedStoneOptions::kCenterHouse, "center_house" },
        { IMixedDoublesThinkingEngine::PositionedStoneOptions::kPowerPlayLeft, "pp_left" },
        { IMixedDoublesThinkingEngine::PositionedStoneOptions::kPowerPlayRight, "pp_right" }
    }
);

} // namespace digitalcurling::client
