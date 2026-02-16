#include "digitalcurling/client/mixed_doubles_client.hpp"

namespace digitalcurling::client {

using StoneOpts = IMixedDoublesThinkingEngine::PositionedStoneOptions;

std::unique_ptr<MixedDoublesClient> MixedDoublesClient::Create(
    std::string host,
    std::string id,
    MatchInfo const& match_info,
    std::unique_ptr<IMixedDoublesThinkingEngine> engine,
    std::unique_ptr<IFactoryCreator> factory_creator
) {
    std::vector<std::unique_ptr<players::IPlayerFactory>> players(2);
    for (int i = 0; i < 2; i++) {
        players[i] = factory_creator->CreatePlayerFactory(match_info.players[i]);
    }

    auto idxs = engine->OnInit(
        match_info.rule,
        match_info.setting,
        factory_creator->CreateSimulatorFactory(match_info.simulator),
        std::move(players)
    );
    if (idxs.size() != 2)
        throw std::runtime_error("MixedDoublesClient: Number of players after OnInit is not 2");

    return std::unique_ptr<MixedDoublesClient>(
        new MixedDoublesClient(std::move(host), std::move(id), match_info, std::move(engine), std::move(factory_creator), std::move(players), std::move(idxs))
    );
}

GameRuleType MixedDoublesClient::GetType() const {
    return GameRuleType::kMixedDoubles;
}

std::vector<std::uint8_t> MixedDoublesClient::GetPlayersIndex() const {
    return players_index_;
}

void MixedDoublesClient::OnGameStart(
    Team const& team,
    std::vector<std::pair<GameState, std::optional<moves::Shot>>> states
) {
    engine_->OnGameStart(team, std::move(states));
}

void MixedDoublesClient::OnNextEnd(StateUpdateEventData const& event_data) {
    if (event_data.next_shot_team == Team::kInvalid) {
        if (event_data.game_state.hammer != team_ || event_data.last_shot.has_value()) return;

        std::string team_path = "/matches/" + game_id_ + "/end-setup?request=";
        switch (engine_->OnDecidePositionedStone(event_data.game_state)) {
            case StoneOpts::kCenterGuard:
                team_path += "center_guard";
                break;
            case StoneOpts::kCenterHouse:
                team_path += "center_house";
                break;
            case StoneOpts::kPowerPlayLeft:
                team_path += "pp_left";
                break;
            case StoneOpts::kPowerPlayRight:
                team_path += "pp_right";
                break;
            default:
                throw std::runtime_error("Invalid PositionedStoneOptions");
        }

        auto result = http_client_.Post(team_path);
        if (!result) {
            throw std::runtime_error("Failed to setup end stones: " + httplib::to_string(result.error()));
        } else if (result->status != 200) {
            std::string err = "Failed to setup end stones: return status code " + std::to_string(result->status);
            if (!result->body.empty()) err += " " + result->body;
            throw std::runtime_error(err);
        }
    } else {
        engine_->OnNextEnd(event_data.game_state);
    }
}
moves::Move MixedDoublesClient::OnMyTurn(StateUpdateEventData const& event_data) {
    int index = event_data.game_state.shot == 0 || event_data.game_state.shot == 4 ? 0 : 1;
    return engine_->OnMyTurn(
        players_[players_index_[index]], event_data.game_state, event_data.last_shot
    );
}
void MixedDoublesClient::OnOpponentTurn(StateUpdateEventData const& event_data) {
    engine_->OnOpponentTurn(event_data.game_state, event_data.last_shot);
}

void MixedDoublesClient::OnGameOver(StateUpdateEventData const& event_data) {
    engine_->OnGameOver(event_data.game_state);
}

} // namespace digitalcurling::client
