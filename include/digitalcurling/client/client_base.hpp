// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: Unlicense

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <httplib.h>
#include <digitalcurling/digitalcurling.hpp>
#include "digitalcurling/client/protocol_models.hpp"

namespace digitalcurling::client {

struct ClientConnectSetting {
    struct Callback {
        /// @brief 接続成功時のコールバック関数
        std::function<void()> on_connected = nullptr;
        /// @brief 最新の試合状況の更新イベント受信時のコールバック関数
        std::function<void(StateUpdateEventData const&)> on_latest_state_update = nullptr;
        /// @brief 試合状況の更新イベント受信時のコールバック関数
        std::function<void(StateUpdateEventData const&)> on_state_update = nullptr;
        /// @brief イベント処理中にエラーが発生した際のコールバック関数
        std::function<bool(std::runtime_error const&)> on_event_process_error = nullptr;
    };

    /// @brief 接続の最大リトライ回数
    int max_retry_count = 5;
    /// @brief リトライする際の待機時間
    std::chrono::milliseconds retry_wait_time = std::chrono::seconds(5);
    /// @brief コールバック関数
    Callback callback;
};

class ClientBase {
public:
    /// @brief コンストラクタ
    /// @param host サーバーのホスト名
    /// @param id クライアントID
    /// @param match_info 試合情報
    ClientBase(std::string host, std::string id, MatchInfo const& match_info);
    virtual ~ClientBase() = default;

    /// @brief クライアントの名前を返す
    /// @return クライアントの名前
    virtual std::string GetName() const;

    /// @brief クライアントのバージョンを返す
    /// @return クライアントのバージョン
    virtual Version GetVersion() const;

    /// @brief クライアントの対応しているプロトコルバージョンを返す
    /// @return プロトコルバージョン
    Version GetProtocolVersion() const;

    /// @brief クライアントが対応しているルールの種類を返す
    /// @return 対応しているルール
    virtual GameRuleType GetType() const = 0;

    /// @brief 試合に参加する
    /// @param team 参加するチーム
    /// @param auth_id 認証ID
    /// @param auth_pw 認証パスワード
    /// @return 参加したチーム
    Team JoinGame(Team const& team, std::string const& auth_id, std::string const& auth_pw);

    /// @brief サーバーに接続する
    /// @param setting 接続設定
    void Connect(ClientConnectSetting const& setting = {});

protected:
    /// @brief ホスト
    std::string host_;
    /// @brief ゲームID
    std::string game_id_;
    /// @brief サーバー通信クライアント
    httplib::Client http_client_;
    /// @brief 自分のチーム
    Team team_;

    /// @brief プレイヤーの投球順を返す
    /// @return プレイヤーの投球順のリスト
    virtual std::vector<std::uint8_t> GetPlayersIndex() const = 0;

    /// @brief ゲーム開始の通知
    /// @param team 自チーム
    /// @param states これまでの試合状況とショットの履歴
    virtual void OnGameStart(
        Team const& team,
        std::vector<std::pair<GameState, std::optional<moves::Shot>>> states
    ) = 0;

    /// @brief 次のエンドの通知
    /// @param game_state 現在の試合状況
    virtual void OnNextEnd(StateUpdateEventData const& event_data) = 0;

    /// @brief 自チームのターンのアクション
    /// @param[in] game_state 現在の試合状況
    /// @return 行動
    virtual moves::Move OnMyTurn(StateUpdateEventData const& event_data) = 0;

    /// @brief 相手チームのターンのアクション
    /// @param[in] game_state 現在の試合状況
    virtual void OnOpponentTurn(StateUpdateEventData const& event_data) = 0;

    /// @brief 試合終了時の通知
    /// @param[in] game_state 現在の試合状況
    virtual void OnGameOver(StateUpdateEventData const& event_data) = 0;

private:
    httplib::Headers sse_headers_;

    digitalcurling::GameRuleType rule_type_;
    std::uint8_t max_end_;
    nlohmann::json players_;
    Team current_hammer_;

    bool is_first_update_ = true;
    std::vector<std::pair<digitalcurling::GameState, std::optional<moves::Shot>>> states_;

    /// @brief SSEの `latest_state_update` イベントを処理する
    /// @param[in] message メッセージ
    void OnReceiveLatestStateUpdateEvent(StateUpdateEventData const& event_data);

    /// @brief SSEの `state_update` イベントを処理する
    /// @param[in] message メッセージ
    void OnReceiveStateUpdateEvent(StateUpdateEventData const& event_data);

    StateUpdateEventData ParseStateUpdateEventData(httplib::sse::SSEMessage const& message);
};

} // namespace digitalcurling::client
