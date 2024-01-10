// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <open_atmos/mechanism_configuration/parser.hpp>
#include <nlohmann/json.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    // explicit template instanatiation
    template class ConfigurationReader<JsonReaderPolicy>;

    ConfigParseStatus JsonReaderPolicy::Parse(const std::filesystem::path &config_path) {
      return ConfigParseStatus::Success;
    }
  }
}