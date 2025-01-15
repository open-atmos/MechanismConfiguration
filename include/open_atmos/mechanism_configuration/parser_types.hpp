// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yaml-cpp/yaml.h>

#include <open_atmos/mechanism_configuration/parse_status.hpp>
#include <open_atmos/mechanism_configuration/utils.hpp>
#include <open_atmos/types.hpp>
#include <vector>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    class CondensedPhaseArrheniusParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };

    class TroeParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };

    class BranchedParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };

    class TunnelingParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };

    class SurfaceParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };

    class CondensedPhasePhotolysisParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };

    class SimpolPhaseTransferParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };

    class EmissionParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };

    class PhotolysisParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };

    class FirstOrderLossParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };

    class AqueousEquilibriumParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };

    class WetDepositionParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };

    class HenrysLawParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };

    class ArrheniusParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override;
    };
  }  // namespace mechanism_configuration
}  // namespace open_atmos
