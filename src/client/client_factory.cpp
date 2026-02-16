
#include <optional>
#include <string>
#include <httplib.h>
#include "digitalcurling/client/client_base.hpp"
#include "digitalcurling/client/standard_client.hpp"
#include "digitalcurling/client/mixed_client.hpp"
#include "digitalcurling/client/mixed_doubles_client.hpp"
#include "digitalcurling/client/i_thinking_engine.hpp"
#include "digitalcurling/client/protocol_models.hpp"
#include "digitalcurling/client/client_factory.hpp"

namespace digitalcurling::client {

std::unique_ptr<ClientBase> ClientFactory::CreateClient(
    std::string const& host,
    std::string const& id,
    std::unique_ptr<IThinkingEngine> engine,
    std::unique_ptr<IFactoryCreator> factory_creator
) {
    // Resolve URL
    std::vector<std::string> target_urls;

    size_t pos = host.find("://");
    if (pos != std::string::npos) {
        auto schema = host.substr(0, pos);
        if (schema == "https") {
#ifdef DIGITALCURLING_CLIENT_NO_SSL
            throw std::runtime_error("Failed to create client: the client does not support HTTPS.");
#else
            target_urls.push_back(host);
#endif
        } else if (schema == "http") {
            target_urls.push_back(host);
        } else {
            throw std::runtime_error("Failed to create client: invalid URL schema.");
        }
    } else {
#ifndef DIGITALCURLING_CLIENT_NO_SSL
        target_urls.push_back("https://" + host);
#endif
        target_urls.push_back("http://" + host);
    }

    httplib::Result result;
    const std::string match_path = "/matches/" + id;

    std::string valid_host;
    for (const auto& url : target_urls) {
        try {
            auto http_client = httplib::Client(url);
            http_client.set_follow_location(true);
            http_client.set_connection_timeout(5);

            result = http_client.Get(match_path);
            if (result) {
                valid_host = url;
                break;
            }
        } catch (...) {
            continue;
        }
    }

    if (valid_host.empty()) {
        throw std::runtime_error("Failed to create HTTP client for host: " + host);
    } else if (!result) {
        throw std::runtime_error("Failed to connect to host: " + httplib::to_string(result.error()));
    } else if (result->status != 200) {
        std::string err = "Failed to get match information: return status code " + std::to_string(result->status);
        if (!result->body.empty()) err += " " + result->body;
        throw std::runtime_error(err);
    }

    MatchInfo match_info;
    try {
        auto body_json = nlohmann::json::parse(result->body);
        match_info = body_json.get<MatchInfo>();
    } catch (nlohmann::json::parse_error& e) {
        throw std::runtime_error("Failed to get match information: " + std::string(e.what()));
    }

    std::string expected_rule;
    std::unique_ptr<ClientBase> client;
    if (match_info.rule.type == GameRuleType::kStandard) {
#ifdef DIGITALCURLING_CLIENT_BUILD_STANDARD_CLIENT
        expected_rule = "standard";
        if (dynamic_cast<IStandardThinkingEngine*>(engine.get())) {
            auto e = std::unique_ptr<IStandardThinkingEngine>(dynamic_cast<IStandardThinkingEngine*>(engine.release()));
            return StandardClient::Create(
                std::move(valid_host), id, std::move(match_info), std::move(e), std::move(factory_creator)
            );
        }
#else
        throw std::runtime_error("Failed to create client: support for standard client is disabled.");
#endif
    } else if (match_info.rule.type == GameRuleType::kMixed) {
#ifdef DIGITALCURLING_CLIENT_BUILD_MIXED_CLIENT
        expected_rule = "mixed";
        if (dynamic_cast<IMixedThinkingEngine*>(engine.get())) {
            auto e = std::unique_ptr<IMixedThinkingEngine>(dynamic_cast<IMixedThinkingEngine*>(engine.release()));
            return MixedClient::Create(
                std::move(valid_host), id, std::move(match_info), std::move(e), std::move(factory_creator)
            );
        }
#else
        throw std::runtime_error("Failed to create client: support for mixed client is disabled.");
#endif
    } else if (match_info.rule.type == GameRuleType::kMixedDoubles) {
#ifdef DIGITALCURLING_CLIENT_BUILD_MIXED_DOUBLES_CLIENT
        expected_rule = "mixed doubles";
        if (dynamic_cast<IMixedDoublesThinkingEngine*>(engine.get())) {
            auto e = std::unique_ptr<IMixedDoublesThinkingEngine>(dynamic_cast<IMixedDoublesThinkingEngine*>(engine.release()));
            return MixedDoublesClient::Create(
                std::move(valid_host), id, std::move(match_info), std::move(e), std::move(factory_creator)
            );
        }
#else
        throw std::runtime_error("Failed to create client: support for mixed doubles client is disabled.");
#endif
    } else {
        throw std::runtime_error("Unsupported game rule type.");
    }
    throw std::runtime_error("The thinking engine is not compatible with " + expected_rule + " rule.");
}

} // namespace digitalcurling::client
