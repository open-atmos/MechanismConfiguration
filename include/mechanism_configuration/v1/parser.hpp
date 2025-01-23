// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/parser_base.hpp>
#include <mechanism_configuration/v1/types.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    using V1Mechanism = ::mechanism_configuration::v1::types::Mechanism;

    class Parser : public ::mechanism_configuration::ParserBase<V1Mechanism>
    {
     public:
      std::optional<std::unique_ptr<GlobalMechanism>> TryParse(const YAML::Node& source);
    };
  }  // namespace v1
}  // namespace mechanism_configuration
