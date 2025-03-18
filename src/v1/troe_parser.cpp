#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors TroeParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::Troe troe;

      auto required_keys = { validation::keys.products, validation::keys.reactants, validation::keys.type, validation::keys.gas_phase };
      auto optional_keys = { validation::keys.name,   validation::keys.k0_A,   validation::keys.k0_B, validation::keys.k0_C, validation::keys.kinf_A,
                             validation::keys.kinf_B, validation::keys.kinf_C, validation::keys.Fc,   validation::keys.N };

      auto validate = ValidateSchema(object, required_keys, optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        auto products = ParseReactantsOrProducts(validation::keys.products, object);
        errors.insert(errors.end(), products.first.begin(), products.first.end());
        auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object);
        errors.insert(errors.end(), reactants.first.begin(), reactants.first.end());

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

        troe.gas_phase = gas_phase;
        troe.products = products.second;
        troe.reactants = reactants.second;
        troe.unknown_properties = GetComments(object);
        reactions.troe.push_back(troe);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration
