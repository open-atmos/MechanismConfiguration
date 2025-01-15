// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/validation.hpp>
#include <open_atmos/mechanism_configuration/version.hpp>
#include <open_atmos/mechanism_configuration/parser.hpp>

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
    GetComments(const YAML::Node& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys)
    {
      std::unordered_map<std::string, std::string> unknown_properties;
      const std::string comment_start = "__";
      for (const auto& key : object)
      {
        std::string key_str = key.first.as<std::string>();
        if (key_str.compare(0, comment_start.size(), comment_start) == 0)
        {
          YAML::Emitter emitter;
          emitter << YAML::DoubleQuoted << YAML::Flow  // json style output
                  << key.second;
          unknown_properties[key_str] = emitter.c_str();
        }
      }
      return unknown_properties;
    }

    ConfigParseStatus
    ValidateSchema(const YAML::Node& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys)
    {
      if (!object || object.IsNull())
      {
        return ConfigParseStatus::Success;
      }

      std::vector<std::string> sorted_object_keys;
      for (const auto& key : object)
      {
        sorted_object_keys.push_back(key.first.as<std::string>());
      }

      auto sorted_required_keys = required_keys;
      auto sorted_optional_keys = optional_keys;
      std::sort(sorted_object_keys.begin(), sorted_object_keys.end());
      std::sort(sorted_required_keys.begin(), sorted_required_keys.end());
      std::sort(sorted_optional_keys.begin(), sorted_optional_keys.end());

      std::vector<std::string> difference;
      std::set_difference(
          sorted_object_keys.begin(),
          sorted_object_keys.end(),
          sorted_required_keys.begin(),
          sorted_required_keys.end(),
          std::back_inserter(difference));

      if (difference.size() != (sorted_object_keys.size() - required_keys.size()))
      {
        std::vector<std::string> missing_keys;
        std::set_difference(
            sorted_required_keys.begin(),
            sorted_required_keys.end(),
            sorted_object_keys.begin(),
            sorted_object_keys.end(),
            std::back_inserter(missing_keys));
        for (auto& key : missing_keys)
          std::cerr << "Missing required key '" << key << "' in object: " << object << std::endl;

        return ConfigParseStatus::RequiredKeyNotFound;
      }

      std::vector<std::string> remaining;
      std::set_difference(
          difference.begin(), difference.end(), sorted_optional_keys.begin(), sorted_optional_keys.end(), std::back_inserter(remaining));

      for (auto& key : remaining)
      {
        if (key.find("__") == std::string::npos)
        {
          std::cerr << "Non-standard key '" << key << "' found in object" << object << std::endl;

          return ConfigParseStatus::InvalidKey;
        }
      }
      return ConfigParseStatus::Success;
    }

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

    std::pair<ConfigParseStatus, std::vector<types::Species>> ParseSpecies(const YAML::Node& objects)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      std::vector<types::Species> all_species;

      for (const auto& object : objects)
      {
        types::Species species;
        status = ValidateSchema(object, validation::species.required_keys, validation::species.optional_keys);
        if (status != ConfigParseStatus::Success)
        {
          break;
        }

        std::string name = object[validation::keys.name].as<std::string>();

        std::map<std::string, double> numerical_properties{};
        for (const auto& key : validation::species.optional_keys)
        {
          if (object[key])
          {
            double val = object[key].as<double>();
            numerical_properties[key] = val;
          }
        }

        species.name = name;
        species.optional_numerical_properties = numerical_properties;
        species.unknown_properties = GetComments(object, validation::species.required_keys, validation::species.optional_keys);

        all_species.push_back(species);
      }

      if (!ContainsUniqueObjectsByName<types::Species>(all_species))
        status = ConfigParseStatus::DuplicateSpeciesDetected;

      return { status, all_species };
    }

    std::pair<ConfigParseStatus, std::vector<types::Phase>> ParsePhases(const YAML::Node& objects, const std::vector<types::Species> existing_species)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      std::vector<types::Phase> all_phases;

      for (const auto& object : objects)
      {
        types::Phase phase;
        status = ValidateSchema(object, validation::phase.required_keys, validation::phase.optional_keys);
        if (status != ConfigParseStatus::Success)
        {
          break;
        }

        std::string name = object[validation::keys.name].as<std::string>();

        std::vector<std::string> species{};
        for (const auto& spec : object[validation::keys.species])
        {
          species.push_back(spec.as<std::string>());
        }

        phase.name = name;
        phase.species = species;
        phase.unknown_properties = GetComments(object, validation::phase.required_keys, validation::phase.optional_keys);

        if (RequiresUnknownSpecies(species, existing_species))
        {
          status = ConfigParseStatus::PhaseRequiresUnknownSpecies;
          break;
        }

        all_phases.push_back(phase);
      }

      if (status == ConfigParseStatus::Success && !ContainsUniqueObjectsByName<types::Phase>(all_phases))
        status = ConfigParseStatus::DuplicatePhasesDetected;

      return { status, all_phases };
    }

    std::pair<ConfigParseStatus, types::ReactionComponent> ParseReactionComponent(const YAML::Node& object)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::ReactionComponent component;

      status = ValidateSchema(object, validation::reaction_component.required_keys, validation::reaction_component.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::string species_name = object[validation::keys.species_name].as<std::string>();
        double coefficient = 1;
        if (object[validation::keys.coefficient])
        {
          coefficient = object[validation::keys.coefficient].as<double>();
        }

        component.species_name = species_name;
        component.coefficient = coefficient;
        component.unknown_properties =
            GetComments(object, validation::reaction_component.required_keys, validation::reaction_component.optional_keys);
      }

      return { status, component };
    }

    std::vector<types::ReactionComponent> ParseReactantsOrProducts(const std::string& key, const YAML::Node& object, ConfigParseStatus& status)
    {
      std::vector<types::ReactionComponent> result{};
      for (const auto& product : object[key])
      {
        auto component_parse = ParseReactionComponent(product);
        status = component_parse.first;
        if (status != ConfigParseStatus::Success)
        {
          break;
        }
        result.push_back(component_parse.second);
      }
      return result;
    }

    class CondensedPhaseArrheniusParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::CondensedPhaseArrhenius condensed_phase_arrhenius;

        status = ValidateSchema(object, validation::condensed_phase_arrhenius.required_keys, validation::condensed_phase_arrhenius.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          auto products = ParseReactantsOrProducts(validation::keys.products, object, status);
          auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

          if (object[validation::keys.A])
          {
            condensed_phase_arrhenius.A = object[validation::keys.A].as<double>();
          }
          if (object[validation::keys.B])
          {
            condensed_phase_arrhenius.B = object[validation::keys.B].as<double>();
          }
          if (object[validation::keys.C])
          {
            condensed_phase_arrhenius.C = object[validation::keys.C].as<double>();
          }
          if (object[validation::keys.D])
          {
            condensed_phase_arrhenius.D = object[validation::keys.D].as<double>();
          }
          if (object[validation::keys.E])
          {
            condensed_phase_arrhenius.E = object[validation::keys.E].as<double>();
          }
          if (object[validation::keys.Ea])
          {
            if (condensed_phase_arrhenius.C != 0)
            {
              std::cerr << "Ea is specified when C is also specified for an CondensedPhasecondensed_phase_arrhenius reaction. Pick one." << std::endl;
              status = ConfigParseStatus::MutuallyExclusiveOption;
            }
            condensed_phase_arrhenius.C = -1 * object[validation::keys.Ea].as<double>() / constants::boltzmann;
          }

          if (object[validation::keys.name])
          {
            condensed_phase_arrhenius.name = object[validation::keys.name].as<std::string>();
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

          condensed_phase_arrhenius.aerosol_phase = aerosol_phase;
          condensed_phase_arrhenius.aerosol_phase_water = aerosol_phase_water;
          condensed_phase_arrhenius.products = products;
          condensed_phase_arrhenius.reactants = reactants;
          condensed_phase_arrhenius.unknown_properties =
              GetComments(object, validation::condensed_phase_arrhenius.required_keys, validation::condensed_phase_arrhenius.optional_keys);
          reactions.condensed_phase_arrhenius.push_back(condensed_phase_arrhenius);
        }

        return status;
      }
    };

    class TroeParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::Troe troe;

        status = ValidateSchema(object, validation::troe.required_keys, validation::troe.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          auto products = ParseReactantsOrProducts(validation::keys.products, object, status);
          auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

          if (object[validation::keys.k0_A])
          {
            troe.k0_A = object[validation::keys.k0_A].as<double>();
          }
          if (object[validation::keys.k0_B])
          {
            troe.k0_B = object[validation::keys.k0_B].as<double>();
          }
          if (object[validation::keys.k0_C])
          {
            troe.k0_C = object[validation::keys.k0_C].as<double>();
          }
          if (object[validation::keys.kinf_A])
          {
            troe.kinf_A = object[validation::keys.kinf_A].as<double>();
          }
          if (object[validation::keys.kinf_B])
          {
            troe.kinf_B = object[validation::keys.kinf_B].as<double>();
          }
          if (object[validation::keys.kinf_C])
          {
            troe.kinf_C = object[validation::keys.kinf_C].as<double>();
          }
          if (object[validation::keys.Fc])
          {
            troe.Fc = object[validation::keys.Fc].as<double>();
          }
          if (object[validation::keys.N])
          {
            troe.N = object[validation::keys.N].as<double>();
          }

          if (object[validation::keys.name])
          {
            troe.name = object[validation::keys.name].as<std::string>();
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

          troe.gas_phase = gas_phase;
          troe.products = products;
          troe.reactants = reactants;
          troe.unknown_properties = GetComments(object, validation::troe.required_keys, validation::troe.optional_keys);
          reactions.troe.push_back(troe);
        }

        return status;
      }
    };

    class BranchedParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::Branched branched;

        status = ValidateSchema(object, validation::branched.required_keys, validation::branched.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          auto alkoxy_products = ParseReactantsOrProducts(validation::keys.alkoxy_products, object, status);
          auto nitrate_products = ParseReactantsOrProducts(validation::keys.nitrate_products, object, status);
          auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

          branched.X = object[validation::keys.X].as<double>();
          branched.Y = object[validation::keys.Y].as<double>();
          branched.a0 = object[validation::keys.a0].as<double>();
          branched.n = object[validation::keys.n].as<double>();

          if (object[validation::keys.name])
          {
            branched.name = object[validation::keys.name].as<std::string>();
          }

          std::vector<std::string> requested_species;
          for (const auto& spec : nitrate_products)
          {
            requested_species.push_back(spec.species_name);
          }
          for (const auto& spec : alkoxy_products)
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

          branched.gas_phase = gas_phase;
          branched.nitrate_products = nitrate_products;
          branched.alkoxy_products = alkoxy_products;
          branched.reactants = reactants;
          branched.unknown_properties = GetComments(object, validation::branched.required_keys, validation::branched.optional_keys);
          reactions.branched.push_back(branched);
        }

        return status;
      }
    };

    class TunnelingParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::Tunneling tunneling;

        status = ValidateSchema(object, validation::tunneling.required_keys, validation::tunneling.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          auto products = ParseReactantsOrProducts(validation::keys.products, object, status);
          auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

          if (object[validation::keys.A])
          {
            tunneling.A = object[validation::keys.A].as<double>();
          }
          if (object[validation::keys.B])
          {
            tunneling.B = object[validation::keys.B].as<double>();
          }
          if (object[validation::keys.C])
          {
            tunneling.C = object[validation::keys.C].as<double>();
          }

          if (object[validation::keys.name])
          {
            tunneling.name = object[validation::keys.name].as<std::string>();
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

          tunneling.gas_phase = gas_phase;
          tunneling.products = products;
          tunneling.reactants = reactants;
          tunneling.unknown_properties = GetComments(object, validation::tunneling.required_keys, validation::tunneling.optional_keys);
          reactions.tunneling.push_back(tunneling);
        }

        return status;
      }
    };

    class SurfaceParser : public IReactionParser
    {
     public:
      ConfigParseStatus parse(
          const YAML::Node& object,
          const std::vector<types::Species>& existing_species,
          const std::vector<types::Phase>& existing_phases,
          open_atmos::types::Reactions& reactions) override
      {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::Surface surface;

        status = ValidateSchema(object, validation::surface.required_keys, validation::surface.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          std::string gas_phase_species = object[validation::keys.gas_phase_species].as<std::string>();

          auto products = ParseReactantsOrProducts(validation::keys.gas_phase_products, object, status);

          if (object[validation::keys.reaction_probability])
          {
            surface.reaction_probability = object[validation::keys.reaction_probability].as<double>();
          }

          if (object[validation::keys.name])
          {
            surface.name = object[validation::keys.name].as<std::string>();
          }

          std::vector<std::string> requested_species;
          for (const auto& spec : products)
          {
            requested_species.push_back(spec.species_name);
          }
          requested_species.push_back(gas_phase_species);

          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
          {
            status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
          }

          std::string aerosol_phase = object[validation::keys.aerosol_phase].as<std::string>();
          auto it = std::find_if(
              existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const auto& phase) { return phase.name == aerosol_phase; });
          if (status == ConfigParseStatus::Success && it == existing_phases.end())
          {
            status = ConfigParseStatus::UnknownPhase;
          }

          surface.gas_phase = object[validation::keys.gas_phase].as<std::string>();
          surface.aerosol_phase = aerosol_phase;
          surface.gas_phase_products = products;
          types::ReactionComponent component;
          component.species_name = gas_phase_species;
          surface.gas_phase_species = component;
          surface.unknown_properties = GetComments(object, validation::surface.required_keys, validation::surface.optional_keys);
          reactions.surface.push_back(surface);
        }

        return status;
      }
    };

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