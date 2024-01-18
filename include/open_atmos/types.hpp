// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <unordered_map>

namespace open_atmos
{
  namespace types
  {
    struct Species
    {
      std::string name;

      std::map<std::string, double> optional_numerical_properties;

      std::unordered_map<std::string, std::string> unknown_properties;
    };

    struct Phase
    {
      std::string name;
      std::vector<std::string> species;
      std::unordered_map<std::string, std::string> unknown_properties;
    };

    struct ReactionComponent
    {
      std::string species_name;
      double coefficient;
      std::unordered_map<std::string, std::string> unknown_properties;
    };

    struct Arrhenius
    {
      /// @brief Pre-exponential factor [(mol m−3)^(−(𝑛−1)) s−1]
      double A{ 1 };
      /// @brief Unitless exponential factor
      double B{ 0 };
      /// @brief Activation threshold, expected to be the negative activation energy divided by the boltzman constant
      ///        [-E_a / k_b), K]
      double C{ 0 };
      /// @brief A factor that determines temperature dependence [K]
      double D{ 300 };
      /// @brief A factor that determines pressure dependence [Pa-1]
      double E{ 0 };

      /// @brief A list of reactants
      std::vector<ReactionComponent> reactants;
      /// @brief A list of products
      std::vector<ReactionComponent> products;
      /// @brief An identifier, optional, uniqueness not enforced
      std::string name;
      /// @brief An identifier indicating which gas phase this reaction takes place in
      std::string gas_phase;

      std::unordered_map<std::string, std::string> unknown_properties;
    };

    struct Reactions
    {
      std::vector<types::Arrhenius> arrhenius;
    };

    struct Mechanism
    {
      /// @brief An identifier, optional
      std::string name;
      std::vector<types::Species> species;
      std::vector<types::Phase> phases;
      Reactions reactions;
    };

  }  // namespace types
}  // namespace open_atmos