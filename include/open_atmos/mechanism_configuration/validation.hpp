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
      const std::string absolute_tolerance = "absolute tolerance";
      const std::string diffusion_coefficient = "diffusion coefficient [m2 s-1]";
      const std::string molecular_weight = "molecular weight [kg mol-1]";
      const std::string henrys_law_constant_298 = "HLC(298K) [mol m-3 Pa-1]";
      const std::string henrys_law_constant_exponential_factor = "HLC exponential factor [K]";
      const std::string phase = "phase";
      const std::string n_star = "N star";
      const std::string density = "density [kg m-3]";

      // Reactions
      const std::string reactants = "reactants";
      const std::string products = "products";
      const std::string type = "type";
      const std::string gas_phase = "gas phase";

      // Reactant and product
      const std::string species_name = "species name";
      const std::string coefficient = "coefficient";

      // Arrhenius
      const std::string Arrhenius_key = "ARRHENIUS";
      const std::string A = "A";
      const std::string B = "B";
      const std::string C = "C";
      const std::string D = "D";
      const std::string E = "E";
      const std::string Ea = "Ea";

      // Condensed Phase Arrhenius
      const std::string CondensedPhaseArrhenius_key = "CONDENSED_PHASE_ARRHENIUS";
      const std::string aerosol_phase_water = "aerosol-phase water";
      // also these
      // aerosol phase
      // A
      // B
      // C
      // D
      // E
      // Ea

      // Troe
      const std::string Troe_key = "TROE";
      const std::string k0_A = "k0_A";
      const std::string k0_B = "k0_B";
      const std::string k0_C = "k0_C";
      const std::string kinf_A = "kinf_A";
      const std::string kinf_B = "kinf_B";
      const std::string kinf_C = "kinf_C";
      const std::string Fc = "Fc";
      const std::string N = "N";

      // Branched
      const std::string Branched_key = "BRANCHED_NO_RO2";
      const std::string X = "X";
      const std::string Y = "Y";
      const std::string a0 = "a0";
      const std::string n = "n";
      const std::string nitrate_products = "nitrate products";
      const std::string alkoxy_products = "alkoxy products";

      // Tunneling
      const std::string Tunneling_key = "TUNNELING";
      // also these, but they are defined above
      // A
      // B
      // C

      // Surface
      const std::string Surface_key = "SURFACE";
      const std::string gas_phase_reactant = "gas-phase reactant";
      const std::string reaction_probability = "reaction probability";
      const std::string gas_phase_products = "gas-phase products";
      const std::string aerosol_phase = "aerosol phase";

      // Photolysis
      const std::string Photolysis_key = "PHOTOLYSIS";
      const std::string scaling_factor = "scaling factor";

      // Condensed Phae Photolysis
      const std::string CondensedPhasePhotolysis_key = "CONDENSED_PHASE_PHOTOLYSIS";
      // also
      // scaling factor
      // aerosol phase
      // aerosol-phase water

      // Emissions
      const std::string Emission_key = "EMISSION";
      // also scaling factor

      // First Order Loss
      const std::string FirstOrderLoss_key = "FIRST_ORDER_LOSS";
      // also scaling factor

    } keys;

    struct Configuration
    {
      const std::vector<std::string> required_keys{ keys.version, keys.species, keys.phases, keys.reactions };
      const std::vector<std::string> optional_keys{ keys.name };
    } configuration;

    struct Species
    {
      const std::vector<std::string> required_keys{ keys.name };
      const std::vector<std::string> optional_keys{ keys.absolute_tolerance,
                                                    keys.diffusion_coefficient,
                                                    keys.molecular_weight,
                                                    keys.henrys_law_constant_298,
                                                    keys.henrys_law_constant_exponential_factor,
                                                    keys.n_star,
                                                    keys.density };
    } species;

    struct Phase
    {
      const std::vector<std::string> required_keys{ keys.name, keys.species };
      const std::vector<std::string> optional_keys{};
    } phase;

    struct ReactionComponent
    {
      const std::vector<std::string> required_keys{ keys.species_name };
      const std::vector<std::string> optional_keys{ keys.coefficient };
    } reaction_component;

    struct Arrhenius
    {
      const std::vector<std::string> required_keys{ keys.products, keys.reactants, keys.type, keys.gas_phase };
      const std::vector<std::string> optional_keys{ keys.A, keys.B, keys.C, keys.D, keys.E, keys.Ea, keys.name };
    } arrhenius;

    struct CondensedPhaseArrhenius
    {
      const std::vector<std::string> required_keys{ keys.products, keys.reactants, keys.type, keys.aerosol_phase, keys.aerosol_phase_water };
      const std::vector<std::string> optional_keys{ keys.A, keys.B, keys.C, keys.D, keys.E, keys.Ea, keys.name };
    } condensed_phase_arrhenius;

    struct Troe
    {
      const std::vector<std::string> required_keys{ keys.products, keys.reactants, keys.type, keys.gas_phase };
      const std::vector<std::string> optional_keys{
        keys.name, keys.k0_A, keys.k0_B, keys.k0_C, keys.kinf_A, keys.kinf_B, keys.kinf_C, keys.Fc, keys.N
      };
    } troe;

    struct Branched
    {
      const std::vector<std::string> required_keys{ keys.nitrate_products, keys.alkoxy_products, keys.reactants, keys.type, keys.gas_phase };
      const std::vector<std::string> optional_keys{ keys.name, keys.X, keys.Y, keys.a0, keys.n };
    } branched;

    struct Tunneling
    {
      const std::vector<std::string> required_keys{ keys.products, keys.reactants, keys.type, keys.gas_phase };
      const std::vector<std::string> optional_keys{ keys.name, keys.A, keys.B, keys.C };
    } tunneling;

    struct Surface
    {
      const std::vector<std::string> required_keys{ keys.gas_phase_products, keys.gas_phase_reactant, keys.type, keys.gas_phase, keys.aerosol_phase };
      const std::vector<std::string> optional_keys{ keys.name, keys.reaction_probability };
    } surface;

    struct Photolysis
    {
      const std::vector<std::string> required_keys{ keys.reactants, keys.products, keys.type, keys.gas_phase };
      const std::vector<std::string> optional_keys{ keys.name, keys.scaling_factor };
    } photolysis;

    struct CondensedPhasePhotolysis
    {
      const std::vector<std::string> required_keys{ keys.reactants, keys.products, keys.type, keys.aerosol_phase, keys.aerosol_phase_water };
      const std::vector<std::string> optional_keys{ keys.name, keys.scaling_factor };
    } condensed_phase_photolysis;

    struct Emission
    {
      const std::vector<std::string> required_keys{ keys.products, keys.type, keys.gas_phase };
      const std::vector<std::string> optional_keys{ keys.name, keys.scaling_factor };
    } emission;

    struct FirstOrderLoss
    {
      const std::vector<std::string> required_keys{ keys.reactants, keys.type, keys.gas_phase };
      const std::vector<std::string> optional_keys{ keys.name, keys.scaling_factor };
    } first_order_loss;

    struct Mechanism
    {
      const std::vector<std::string> required_keys{};
      const std::vector<std::string> optional_keys{};
    } mechanism;

  }  // namespace validation
}  // namespace open_atmos