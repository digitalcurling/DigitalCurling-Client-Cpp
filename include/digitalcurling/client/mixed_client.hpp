// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: Unlicense

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "digitalcurling/client/client_base.hpp"
#include "digitalcurling/client/i_factory_creator.hpp"
#include "digitalcurling/client/i_thinking_engine.hpp"
#include "digitalcurling/client/protocol_models.hpp"

namespace digitalcurling::client {

/// @brief ミックスカーリング用クライアント
class MixedClient : public ClientBase {
public:
    /// @brief クライアントを作成する
    /// @param host サーバーのホスト名
    /// @param id ゲームID
    /// @param match_info 試合情報
    /// @param engine 思考エンジン
    /// @param factory_creator プレイヤーとシミュレータの変換を行うオブジェクト
    /// @return クライアントのインスタンス
    static std::unique_ptr<MixedClient> Create(
        std::string host,
        std::string id,
        MatchInfo const& match_info,
        std::unique_ptr<IMixedThinkingEngine> engine,
        std::unique_ptr<IFactoryCreator> factory_creator
    );

    GameRuleType GetType() const override final;

protected:
    virtual std::vector<std::uint8_t> GetPlayersIndex() const override;

    virtual void OnGameStart(
        Team const& team,
        std::vector<std::pair<GameState, std::optional<moves::Shot>>> states
    ) override;
    virtual void OnNextEnd(StateUpdateEventData const& event_data) override;

    virtual moves::Move OnMyTurn(StateUpdateEventData const& event_data) override;
    virtual void OnOpponentTurn(StateUpdateEventData const& event_data) override;

    virtual void OnGameOver(StateUpdateEventData const& event_data) override;

private:
    std::unique_ptr<IMixedThinkingEngine> engine_;
    std::vector<std::unique_ptr<players::IPlayerFactory>> players_;
    std::vector<uint8_t> players_index_;

    MixedClient(
        std::string host,
        std::string id,
        MatchInfo const& match_info,
        std::unique_ptr<IMixedThinkingEngine> engine,
        std::unique_ptr<IFactoryCreator> factory_creator,
        std::vector<std::unique_ptr<players::IPlayerFactory>> players,
        std::vector<uint8_t> players_index
    ) : ClientBase(std::move(host), std::move(id), match_info),
        engine_(std::move(engine)),
        players_(std::move(players)),
        players_index_(std::move(players_index))
    {}
};

} // namespace digitalcurling::client
