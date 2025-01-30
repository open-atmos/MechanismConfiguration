// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/v0/types.hpp>
#include <mechanism_configuration/parse_status.hpp>
#include <vector>

namespace mechanism_configuration
{
  namespace v0
  {
    // species and mechanism
    ConfigParseStatus ParseChemicalSpecies(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    ConfigParseStatus ParseProducts(const YAML::Node& object, std::vector<types::ReactionComponent>& products);
    ConfigParseStatus ParseReactants(const YAML::Node& object, std::vector<types::ReactionComponent>& reactants);
    ConfigParseStatus ParseRelativeTolerance(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);

    // reactions
    ConfigParseStatus ArrheniusParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    ConfigParseStatus BranchedParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    ConfigParseStatus EmissionParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    ConfigParseStatus FirstOrderLossParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    ConfigParseStatus PhotolysisParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    ConfigParseStatus SurfaceParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    ConfigParseStatus TernaryChemicalActivationParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    ConfigParseStatus TroeParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    ConfigParseStatus TunnelingParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);
    ConfigParseStatus UserDefinedParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object);

  }  // namespace v0
}  // namespace mechanism_configuration
