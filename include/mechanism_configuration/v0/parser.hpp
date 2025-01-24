// Copyright (C) 2023-2024 National Center for Atmospheric Research
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/parser.hpp>
#include <mechanism_configuration/v0/types.hpp>
#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/parser_base.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    using V0Mechanism = ::mechanism_configuration::v0::types::Mechanism;

    class Parser : public ::mechanism_configuration::ParserBase<V0Mechanism>
    {
     public:
      std::optional<std::unique_ptr<GlobalMechanism>> TryParse(const YAML::Node& source){
        return std::nullopt;
      };
    };
  }  // namespace v0
}  // namespace mechanism_configuration
