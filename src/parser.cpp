// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/validation.hpp>
#include <open_atmos/mechanism_configuration/version.hpp>
#include <open_atmos/mechanism_configuration/parser.hpp>
#include <open_atmos/mechanism_configuration/parser_types.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    class PhotolysisParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::Photolysis photolysis;

        status = ValidateSchema(object, validation::photolysis.required_keys, validation::photolysis.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          auto products = ParseReactantsOrProducts(validation::keys.products, object, status);
          auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

          if (object[validation::keys.scaling_factor])
          {
            photolysis.scaling_factor = object[validation::keys.scaling_factor].as<double>();
          }

          if (object[validation::keys.name])
          {
            photolysis.name = object[validation::keys.name].as<std::string>();
          }

          std::vector<std::string> requested_species;
          for (const auto& spec : products)
          {
            requested_species.push_back(spec.species_name);
          }
          for (const auto& spec : reactants)
          {
            requested_species.push_back(spec.species_name);
          }

          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
          {
            status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
          }

          std::string gas_phase = object[validation::keys.gas_phase].as<std::string>();
          auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
          if (status == ConfigParseStatus::Success && it == existing_phases.end())
          {
            status = ConfigParseStatus::UnknownPhase;
          }

          if (status == ConfigParseStatus::Success && reactants.size() > 1)
          {
            status = ConfigParseStatus::TooManyReactionComponents;
          }

          photolysis.gas_phase = gas_phase;
          photolysis.products = products;
          photolysis.reactants = reactants;
          photolysis.unknown_properties = GetComments(object, validation::photolysis.required_keys, validation::photolysis.optional_keys);
          reactions.photolysis.push_back(photolysis);
        }

        return status;
      }
    };

    class CondensedPhasePhotolysisParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::CondensedPhasePhotolysis condensed_phase_photolysis;

        status = ValidateSchema(object, validation::condensed_phase_photolysis.required_keys, validation::photolysis.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          auto products = ParseReactantsOrProducts(validation::keys.products, object, status);
          auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

          if (object[validation::keys.scaling_factor])
          {
            condensed_phase_photolysis.scaling_factor_ = object[validation::keys.scaling_factor].as<double>();
          }

          if (object[validation::keys.name])
          {
            condensed_phase_photolysis.name = object[validation::keys.name].as<std::string>();
          }

          std::string aerosol_phase = object[validation::keys.aerosol_phase].as<std::string>();
          std::string aerosol_phase_water = object[validation::keys.aerosol_phase_water].as<std::string>();

          std::vector<std::string> requested_species;
          for (const auto& spec : products)
          {
            requested_species.push_back(spec.species_name);
          }
          for (const auto& spec : reactants)
          {
            requested_species.push_back(spec.species_name);
          }
          requested_species.push_back(aerosol_phase_water);

          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
          {
            status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
          }

          if (status == ConfigParseStatus::Success && reactants.size() > 1)
          {
            status = ConfigParseStatus::TooManyReactionComponents;
          }

          auto phase_it = std::find_if(
              existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const types::Phase& phase) { return phase.name == aerosol_phase; });

          if (phase_it != existing_phases.end())
          {
            std::vector<std::string> aerosol_phase_species = { (*phase_it).species.begin(), (*phase_it).species.end() };
            if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, aerosol_phase_species))
            {
              status = ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase;
            }
          }
          else
          {
            status = ConfigParseStatus::UnknownPhase;
          }

          condensed_phase_photolysis.aerosol_phase = aerosol_phase;
          condensed_phase_photolysis.aerosol_phase_water = aerosol_phase_water;
          condensed_phase_photolysis.products = products;
          condensed_phase_photolysis.reactants = reactants;
          condensed_phase_photolysis.unknown_properties =
              GetComments(object, validation::condensed_phase_photolysis.required_keys, validation::photolysis.optional_keys);
          reactions.condensed_phase_photolysis.push_back(condensed_phase_photolysis);
        }

        return status;
      }
    };

    class EmissionParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::Emission emission;

        status = ValidateSchema(object, validation::emission.required_keys, validation::emission.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          auto products = ParseReactantsOrProducts(validation::keys.products, object, status);

          if (object[validation::keys.scaling_factor])
          {
            emission.scaling_factor = object[validation::keys.scaling_factor].as<double>();
          }

          if (object[validation::keys.name])
          {
            emission.name = object[validation::keys.name].as<std::string>();
          }

          std::vector<std::string> requested_species;
          for (const auto& spec : products)
          {
            requested_species.push_back(spec.species_name);
          }

          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
          {
            status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
          }

          std::string gas_phase = object[validation::keys.gas_phase].as<std::string>();
          auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
          if (status == ConfigParseStatus::Success && it == existing_phases.end())
          {
            status = ConfigParseStatus::UnknownPhase;
          }

          emission.gas_phase = gas_phase;
          emission.products = products;
          emission.unknown_properties = GetComments(object, validation::emission.required_keys, validation::emission.optional_keys);
          reactions.emission.push_back(emission);
        }

        return status;
      }
    };

    class FirstOrderLossParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::FirstOrderLoss first_order_loss;

        status = ValidateSchema(object, validation::first_order_loss.required_keys, validation::first_order_loss.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

          if (object[validation::keys.scaling_factor])
          {
            first_order_loss.scaling_factor = object[validation::keys.scaling_factor].as<double>();
          }

          if (object[validation::keys.name])
          {
            first_order_loss.name = object[validation::keys.name].as<std::string>();
          }

          std::vector<std::string> requested_species;
          for (const auto& spec : reactants)
          {
            requested_species.push_back(spec.species_name);
          }

          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
          {
            status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
          }

          std::string gas_phase = object[validation::keys.gas_phase].as<std::string>();
          auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
          if (status == ConfigParseStatus::Success && it == existing_phases.end())
          {
            status = ConfigParseStatus::UnknownPhase;
          }

          if (status == ConfigParseStatus::Success && reactants.size() > 1)
          {
            status = ConfigParseStatus::TooManyReactionComponents;
          }

          first_order_loss.gas_phase = gas_phase;
          first_order_loss.reactants = reactants;
          first_order_loss.unknown_properties =
              GetComments(object, validation::first_order_loss.required_keys, validation::first_order_loss.optional_keys);
          reactions.first_order_loss.push_back(first_order_loss);
        }

        return status;
      }
    };

    class SimpolPhaseTransferParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::SimpolPhaseTransfer simpol_phase_transfer;

        status = ValidateSchema(object, validation::simpol_phase_transfer.required_keys, validation::simpol_phase_transfer.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          std::string gas_phase_species = object[validation::keys.gas_phase_species].as<std::string>();
          std::string aerosol_phase_species = object[validation::keys.aerosol_phase_species].as<std::string>();

          if (object[validation::keys.name])
          {
            simpol_phase_transfer.name = object[validation::keys.name].as<std::string>();
          }

          std::vector<std::string> requested_species{ gas_phase_species, aerosol_phase_species };
          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
          {
            status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
          }

          std::string aerosol_phase = object[validation::keys.aerosol_phase].as<std::string>();
          auto aerosol_it = std::find_if(
              existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const auto& phase) { return phase.name == aerosol_phase; });
          if (status == ConfigParseStatus::Success && aerosol_it == existing_phases.end())
          {
            status = ConfigParseStatus::UnknownPhase;
          }
          else
          {
            auto phase = *aerosol_it;
            auto spec_it = std::find_if(
                phase.species.begin(),
                phase.species.end(),
                [&aerosol_phase_species](const std::string& species) { return species == aerosol_phase_species; });
            if (spec_it == phase.species.end())
            {
              status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
            }
          }

          std::string gas_phase = object[validation::keys.gas_phase].as<std::string>();
          auto gas_it =
              std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
          if (status == ConfigParseStatus::Success && gas_it == existing_phases.end())
          {
            status = ConfigParseStatus::UnknownPhase;
          }
          else
          {
            auto phase = *gas_it;
            auto spec_it = std::find_if(
                phase.species.begin(),
                phase.species.end(),
                [&gas_phase_species](const std::string& species) { return species == gas_phase_species; });
            if (spec_it == phase.species.end())
            {
              status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
            }
          }

          if (object[validation::keys.B] && object[validation::keys.B].IsSequence() && object[validation::keys.B].size() == 4)
          {
            for (size_t i = 0; i < 4; ++i)
            {
              simpol_phase_transfer.B[i] = object[validation::keys.B][i].as<double>();
            }
          }

          simpol_phase_transfer.gas_phase = gas_phase;
          types::ReactionComponent gas_component;
          gas_component.species_name = gas_phase_species;
          simpol_phase_transfer.gas_phase_species = gas_component;
          simpol_phase_transfer.aerosol_phase = aerosol_phase;
          types::ReactionComponent aerosol_component;
          aerosol_component.species_name = aerosol_phase_species;
          simpol_phase_transfer.aerosol_phase_species = aerosol_component;
          simpol_phase_transfer.unknown_properties =
              GetComments(object, validation::simpol_phase_transfer.required_keys, validation::simpol_phase_transfer.optional_keys);
          reactions.simpol_phase_transfer.push_back(simpol_phase_transfer);
        }

        return status;
      }
    };

    class AqueousEquilibriumParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::AqueousEquilibrium aqueous_equilibrium;

        status = ValidateSchema(object, validation::aqueous_equilibrium.required_keys, validation::aqueous_equilibrium.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          auto products = ParseReactantsOrProducts(validation::keys.products, object, status);
          auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

          if (object[validation::keys.A])
          {
            aqueous_equilibrium.A = object[validation::keys.A].as<double>();
          }
          if (object[validation::keys.C])
          {
            aqueous_equilibrium.C = object[validation::keys.C].as<double>();
          }

          aqueous_equilibrium.k_reverse = object[validation::keys.k_reverse].as<double>();

          if (object[validation::keys.name])
          {
            aqueous_equilibrium.name = object[validation::keys.name].as<std::string>();
          }

          std::string aerosol_phase = object[validation::keys.aerosol_phase].as<std::string>();
          std::string aerosol_phase_water = object[validation::keys.aerosol_phase_water].as<std::string>();

          std::vector<std::string> requested_species;
          for (const auto& spec : products)
          {
            requested_species.push_back(spec.species_name);
          }
          for (const auto& spec : reactants)
          {
            requested_species.push_back(spec.species_name);
          }
          requested_species.push_back(aerosol_phase_water);

          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
          {
            status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
          }

          auto phase_it = std::find_if(
              existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const types::Phase& phase) { return phase.name == aerosol_phase; });

          if (phase_it != existing_phases.end())
          {
            std::vector<std::string> aerosol_phase_species = { (*phase_it).species.begin(), (*phase_it).species.end() };
            if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, aerosol_phase_species))
            {
              status = ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase;
            }
          }
          else
          {
            status = ConfigParseStatus::UnknownPhase;
          }

          aqueous_equilibrium.aerosol_phase = aerosol_phase;
          aqueous_equilibrium.aerosol_phase_water = aerosol_phase_water;
          aqueous_equilibrium.products = products;
          aqueous_equilibrium.reactants = reactants;
          aqueous_equilibrium.unknown_properties =
              GetComments(object, validation::aqueous_equilibrium.required_keys, validation::aqueous_equilibrium.optional_keys);
          reactions.aqueous_equilibrium.push_back(aqueous_equilibrium);
        }

        return status;
      }
    };

    class WetDepositionParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::WetDeposition wet_deposition;

        status = ValidateSchema(object, validation::wet_deposition.required_keys, validation::wet_deposition.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          if (object[validation::keys.scaling_factor])
          {
            wet_deposition.scaling_factor = object[validation::keys.scaling_factor].as<double>();
          }

          if (object[validation::keys.name])
          {
            wet_deposition.name = object[validation::keys.name].as<std::string>();
          }

          std::string aerosol_phase = object[validation::keys.aerosol_phase].as<std::string>();

          auto it = std::find_if(
              existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const auto& phase) { return phase.name == aerosol_phase; });
          if (status == ConfigParseStatus::Success && it == existing_phases.end())
          {
            status = ConfigParseStatus::UnknownPhase;
          }

          wet_deposition.aerosol_phase = aerosol_phase;
          wet_deposition.unknown_properties = GetComments(object, validation::wet_deposition.required_keys, validation::wet_deposition.optional_keys);
          reactions.wet_deposition.push_back(wet_deposition);
        }

        return status;
      }
    };

    class HenrysLawParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::HenrysLaw henrys_law;

        status = ValidateSchema(object, validation::henrys_law.required_keys, validation::henrys_law.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          std::string gas_phase = object[validation::keys.gas_phase].as<std::string>();
          std::string gas_phase_species = object[validation::keys.gas_phase_species].as<std::string>();
          std::string aerosol_phase = object[validation::keys.aerosol_phase].as<std::string>();
          std::string aerosol_phase_species = object[validation::keys.aerosol_phase_species].as<std::string>();
          std::string aerosol_phase_water = object[validation::keys.aerosol_phase_water].as<std::string>();

          if (object[validation::keys.name])
          {
            henrys_law.name = object[validation::keys.name].as<std::string>();
          }

          std::vector<std::string> requested_species;
          requested_species.push_back(gas_phase_species);
          requested_species.push_back(aerosol_phase_species);
          requested_species.push_back(aerosol_phase_water);

          std::vector<std::string> requested_aerosol_species;
          requested_aerosol_species.push_back(aerosol_phase_species);
          requested_aerosol_species.push_back(aerosol_phase_water);

          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
          {
            status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
          }

          auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
          if (status == ConfigParseStatus::Success && it == existing_phases.end())
          {
            status = ConfigParseStatus::UnknownPhase;
          }

          auto phase_it = std::find_if(
              existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const types::Phase& phase) { return phase.name == aerosol_phase; });

          if (phase_it != existing_phases.end())
          {
            std::vector<std::string> aerosol_phase_species = { (*phase_it).species.begin(), (*phase_it).species.end() };
            if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_aerosol_species, aerosol_phase_species))
            {
              status = ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase;
            }
          }
          else
          {
            status = ConfigParseStatus::UnknownPhase;
          }

          henrys_law.gas_phase = gas_phase;
          henrys_law.gas_phase_species = gas_phase_species;
          henrys_law.aerosol_phase = aerosol_phase;
          henrys_law.aerosol_phase_species = aerosol_phase_species;
          henrys_law.aerosol_phase_water = aerosol_phase_water;
          henrys_law.unknown_properties = GetComments(object, validation::henrys_law.required_keys, validation::henrys_law.optional_keys);
          reactions.henrys_law.push_back(henrys_law);
        }

        return status;
      }
    };

    class ArrheniusParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::Arrhenius arrhenius;

        status = ValidateSchema(object, validation::arrhenius.required_keys, validation::arrhenius.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          auto products = ParseReactantsOrProducts(validation::keys.products, object, status);
          auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

          if (object[validation::keys.A])
          {
            arrhenius.A = object[validation::keys.A].as<double>();
          }
          if (object[validation::keys.B])
          {
            arrhenius.B = object[validation::keys.B].as<double>();
          }
          if (object[validation::keys.C])
          {
            arrhenius.C = object[validation::keys.C].as<double>();
          }
          if (object[validation::keys.D])
          {
            arrhenius.D = object[validation::keys.D].as<double>();
          }
          if (object[validation::keys.E])
          {
            arrhenius.E = object[validation::keys.E].as<double>();
          }
          if (object[validation::keys.Ea])
          {
            if (arrhenius.C != 0)
            {
              std::cerr << "Ea is specified when C is also specified for an Arrhenius reaction. Pick one." << std::endl;
              status = ConfigParseStatus::MutuallyExclusiveOption;
            }
            arrhenius.C = -1 * object[validation::keys.Ea].as<double>() / constants::boltzmann;
          }

          if (object[validation::keys.name])
          {
            arrhenius.name = object[validation::keys.name].as<std::string>();
          }

          std::vector<std::string> requested_species;
          for (const auto& spec : products)
          {
            requested_species.push_back(spec.species_name);
          }
          for (const auto& spec : reactants)
          {
            requested_species.push_back(spec.species_name);
          }

          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
          {
            status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
          }

          std::string gas_phase = object[validation::keys.gas_phase].as<std::string>();
          auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
          if (status == ConfigParseStatus::Success && it == existing_phases.end())
          {
            status = ConfigParseStatus::UnknownPhase;
          }

          arrhenius.gas_phase = gas_phase;
          arrhenius.products = products;
          arrhenius.reactants = reactants;
          arrhenius.unknown_properties = GetComments(object, validation::arrhenius.required_keys, validation::arrhenius.optional_keys);
          reactions.arrhenius.push_back(arrhenius);
        }

        return status;
      }
    };

    std::pair<ConfigParseStatus, types::Reactions>
    ParseReactions(const YAML::Node& objects, const std::vector<types::Species>& existing_species, const std::vector<types::Phase>& existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::Reactions reactions;

      std::map<std::string, std::unique_ptr<IReactionParser>> parsers;
      parsers[validation::keys.Arrhenius_key] = std::make_unique<ArrheniusParser>();
      parsers[validation::keys.HenrysLaw_key] = std::make_unique<HenrysLawParser>();
      parsers[validation::keys.WetDeposition_key] = std::make_unique<WetDepositionParser>();
      parsers[validation::keys.AqueousPhaseEquilibrium_key] = std::make_unique<AqueousEquilibriumParser>();
      parsers[validation::keys.SimpolPhaseTransfer_key] = std::make_unique<SimpolPhaseTransferParser>();
      parsers[validation::keys.FirstOrderLoss_key] = std::make_unique<FirstOrderLossParser>();
      parsers[validation::keys.Emission_key] = std::make_unique<EmissionParser>();
      parsers[validation::keys.CondensedPhasePhotolysis_key] = std::make_unique<CondensedPhasePhotolysisParser>();
      parsers[validation::keys.Photolysis_key] = std::make_unique<PhotolysisParser>();
      parsers[validation::keys.Surface_key] = std::make_unique<SurfaceParser>();
      parsers[validation::keys.Tunneling_key] = std::make_unique<TunnelingParser>();
      parsers[validation::keys.Branched_key] = std::make_unique<BranchedParser>();
      parsers[validation::keys.Troe_key] = std::make_unique<TroeParser>();
      parsers[validation::keys.CondensedPhaseArrhenius_key] = std::make_unique<CondensedPhaseArrheniusParser>();

      for (const auto& object : objects)
      {
        std::string type = object[validation::keys.type].as<std::string>();
        auto it = parsers.find(type);
        if (it != parsers.end())
        {
          auto parse_status = it->second->parse(object, existing_species, existing_phases, reactions);
          status = parse_status;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
        }
        else
        {
          const std::string& msg = "Unknown type: " + type;
          throw std::runtime_error(msg);
        }
      }

      return { status, reactions };
    }

    /// @brief Parse a mechanism
    /// @param file_path a location on the hard drive containing a mechanism
    /// @return A pair containing the parsing status and a mechanism
    std::pair<ConfigParseStatus, types::Mechanism> Parser::Parse(const std::string& file_path)
    {
      return Parser::Parse(std::filesystem::path(file_path));
    }

    /// @brief Parse a mechanism
    /// @param file_path a location on the hard drive containing a mechanism
    /// @return A pair containing the parsing status and a mechanism
    std::pair<ConfigParseStatus, types::Mechanism> Parser::Parse(const std::filesystem::path& file_path)
    {
      ConfigParseStatus status;

      if (!std::filesystem::exists(file_path) || std::filesystem::is_directory(file_path))
      {
        status = ConfigParseStatus::InvalidFilePath;
        std::string msg = configParseStatusToString(status);
        std::cerr << msg << std::endl;
        return { status, types::Mechanism() };
      }

      YAML::Node config = YAML::LoadFile(file_path.string());

      return Parser::Parse(config);
    }

    /// @brief Parse a mechanism
    /// @param node A yaml object representing a mechanism
    /// @return A pair containing the parsing status and a mechanism
    std::pair<ConfigParseStatus, types::Mechanism> Parser::Parse(const YAML::Node& object)
    {
      ConfigParseStatus status;
      types::Mechanism mechanism;

      status = ValidateSchema(object, validation::mechanism.required_keys, validation::mechanism.optional_keys);

      if (status != ConfigParseStatus::Success)
      {
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Invalid top level configuration." << std::endl;
        return { status, mechanism };
      }

      std::string version = object[validation::keys.version].as<std::string>();

      if (version != getVersionString())
      {
        status = ConfigParseStatus::InvalidVersion;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] This parser supports version " << getVersionString() << " and you requested version " << version
                  << ". Please download the appropriate version of the parser or switch to the supported format's version." << std::endl;
      }

      std::string name = object[validation::keys.name].as<std::string>();
      mechanism.name = name;

      auto species_parsing = ParseSpecies(object[validation::keys.species]);

      if (species_parsing.first != ConfigParseStatus::Success)
      {
        status = species_parsing.first;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Failed to parse the species." << std::endl;
      }

      auto phases_parsing = ParsePhases(object[validation::keys.phases], species_parsing.second);

      if (phases_parsing.first != ConfigParseStatus::Success)
      {
        status = phases_parsing.first;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Failed to parse the phases." << std::endl;
      }

      auto reactions_parsing = ParseReactions(object[validation::keys.reactions], species_parsing.second, phases_parsing.second);

      if (reactions_parsing.first != ConfigParseStatus::Success)
      {
        status = reactions_parsing.first;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Failed to parse the reactions." << std::endl;
      }

      mechanism.species = species_parsing.second;
      mechanism.phases = phases_parsing.second;
      mechanism.reactions = reactions_parsing.second;

      return { status, mechanism };
    }
  }  // namespace mechanism_configuration
}  // namespace open_atmos