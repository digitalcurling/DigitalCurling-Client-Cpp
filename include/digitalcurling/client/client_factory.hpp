// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: Unlicense

#pragma once

#include <memory>
#include <string>
#include "digitalcurling/client/client_base.hpp"
#include "digitalcurling/client/i_factory_creator.hpp"
#include "digitalcurling/client/i_thinking_engine.hpp"

namespace digitalcurling::client {

class ClientFactory {
public:
    /// @brief 思考エンジンを生成する
    /// @return 思考エンジンのインスタンス
    static std::unique_ptr<ClientBase> CreateClient(
        std::string const& host,
        std::string const& id,
        std::unique_ptr<IThinkingEngine> engine,
        std::unique_ptr<IFactoryCreator> factory_creator
    );
};

} // namespace digitalcurling::client
