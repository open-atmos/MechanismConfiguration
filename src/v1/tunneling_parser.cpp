#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors TunnelingParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::Tunneling tunneling;

      auto required_keys = { validation::keys.products, validation::keys.reactants, validation::keys.type, validation::keys.gas_phase };
      auto optional_keys = { validation::keys.name, validation::keys.A, validation::keys.B, validation::keys.C };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        auto products = ParseReactantsOrProducts(validation::keys.products, object);
        errors.insert(errors.end(), products.first.begin(), products.first.end());
        auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object);
        errors.insert(errors.end(), reactants.first.begin(), reactants.first.end());

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
        for (const auto& spec : products.second)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : reactants.second)
        {
          requested_species.push_back(spec.species_name);
        }

        if (RequiresUnknownSpecies(requested_species, existing_species))
        {
          std::string line = std::to_string(object.Mark().line + 1);
          std::string column = std::to_string(object.Mark().column + 1);
          errors.push_back({ ConfigParseStatus::ReactionRequiresUnknownSpecies, line + ":" + column + ": Reaction requires unknown species" });
        }

        std::string gas_phase = object[validation::keys.gas_phase].as<std::string>();
        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::keys.gas_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::keys.gas_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + gas_phase });
        }

        tunneling.gas_phase = gas_phase;
        tunneling.products = products.second;
        tunneling.reactants = reactants.second;
        tunneling.unknown_properties = GetComments(object, required_keys, optional_keys);
        reactions.tunneling.push_back(tunneling);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration
