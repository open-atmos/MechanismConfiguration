// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <open_atmos/mechanism_configuration/parse_status.hpp>
#include <open_atmos/types.hpp>
#include <open_atmos/mechanism_configuration/validation.hpp>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    class IReactionParser
    {
     public:
      virtual ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          types::Reactions& reactions) = 0;
      virtual ~IReactionParser() = default;
    };

    std::unordered_map<std::string, std::string>
    GetComments(const YAML::Node& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys);

    ConfigParseStatus
    ValidateSchema(const YAML::Node& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys);
    
    std::pair<ConfigParseStatus, std::vector<types::Species>> ParseSpecies(const YAML::Node& objects);

    std::pair<ConfigParseStatus, std::vector<types::Phase>> ParsePhases(const YAML::Node& objects, const std::vector<types::Species> existing_species);

    std::pair<ConfigParseStatus, types::ReactionComponent> ParseReactionComponent(const YAML::Node& object);

    std::vector<types::ReactionComponent> ParseReactantsOrProducts(const std::string& key, const YAML::Node& object, ConfigParseStatus& status);

    template<typename T>
    bool ContainsUniqueObjectsByName(const std::vector<T>& collection)
    {
      for (size_t i = 0; i < collection.size(); ++i)
      {
        for (size_t j = i + 1; j < collection.size(); ++j)
        {
          if (collection[i].name == collection[j].name)
          {
            return false;
          }
        }
      }
      return true;
    }

    template<typename SpeciesType>
    bool RequiresUnknownSpecies(const std::vector<std::string>& requested_species, const std::vector<SpeciesType>& existing_species)
    {
      for (const auto& spec : requested_species)
      {
        auto it = std::find_if(
            existing_species.begin(),
            existing_species.end(),
            [&spec](const auto& existing)
            {
              if constexpr (std::is_same<SpeciesType, std::string>::value)
              {
                return existing == spec;
              }
              else
              {
                return existing.name == spec;
              }
            });

        if (it == existing_species.end())
        {
          return true;
        }
      }
      return false;
    }

  }  // namespace mechanism_configuration
}  // namespace open_atmos
