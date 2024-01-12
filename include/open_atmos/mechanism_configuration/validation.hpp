// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <vector>

namespace open_atmos
{
  namespace validation
  {
    struct Keys
    {
      // Shared, but also Mechanism
      const std::string version = "version";
      const std::string name = "name";

      // Configuration
      const std::string species = "species";
      const std::string phases = "phases";
      const std::string reactions = "reactions";

      // Species
      const std::string tracer_type = "tracer type";
      const std::string absolute_tolerance = "absolute tolerance";
      const std::string diffusion_coefficient = "diffusion coefficient [m2 s-1]";
      const std::string molecular_weight = "molecular weight [kg mol-1]";
      const std::string henrys_law_constant_298 = "HLC(298K) [mol m-3 Pa-1]";
      const std::string henrys_law_constant_exponential_factor = "HLC exponential factor [K]";
      const std::string phase = "phase";
      const std::string n_star = "N star";
      const std::string density = "density [kg m-3]";
    } keys;

    // Initialize static const members of Keys

    struct Configuration
    {
      const std::vector<std::string> required_keys{ keys.version, keys.species, keys.phases, keys.reactions };
      const std::vector<std::string> optional_keys{ keys.name };
    } configuration;

    struct Species
    {
      const std::vector<std::string> required_keys{ keys.name, keys.phase };
      const std::vector<std::string> optional_keys{ keys.tracer_type,
                                                    keys.absolute_tolerance,
                                                    keys.diffusion_coefficient,
                                                    keys.molecular_weight,
                                                    keys.henrys_law_constant_298,
                                                    keys.henrys_law_constant_exponential_factor,
                                                    keys.n_star,
                                                    keys.density };
    } species;

    struct Phase
    {
      const std::vector<std::string> required_keys {};
      const std::vector<std::string> optional_keys {};
    } phase;

    struct Mechanism
    {
      const std::vector<std::string> required_keys {};
      const std::vector<std::string> optional_keys {};
    } mechanism;

  }  // namespace validation
}  // namespace open_atmos