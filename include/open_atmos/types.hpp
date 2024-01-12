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
    };

    struct Mechanism
    {
      std::string name; // optional
      std::vector<types::Species> species;
      std::unordered_map<std::string, types::Phase> phases;
    };

  }  // namespace types
}  // namespace open_atmos