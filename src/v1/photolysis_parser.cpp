#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors PhotolysisParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::Photolysis photolysis;

      auto validate = ValidateSchema(object, validation::photolysis.required_keys, validation::photolysis.optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        auto products = ParseReactantsOrProducts(validation::keys.products, object);
        errors.insert(errors.end(), products.first.begin(), products.first.end());
        auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object);
        errors.insert(errors.end(), reactants.first.begin(), reactants.first.end());

        if (object[validation::keys.scaling_factor])
        {
          photolysis.scaling_factor = object[validation::keys.scaling_factor].as<double>();
        }

        if (object[validation::keys.name])
        {
          photolysis.name = object[validation::keys.name].as<std::string>();
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
          errors.push_back({ ConfigParseStatus::ReactionRequiresUnknownSpecies, "Reaction requires unknown species in object at line " + line + " column " + column });
        }

        std::string gas_phase = object[validation::keys.gas_phase].as<std::string>();
        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::keys.gas_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::keys.gas_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, "Unknown phase: " + gas_phase + " at line " + line + " column " + column });
        }

        if (reactants.second.size() > 1)
        {
          std::string line = std::to_string(object[validation::keys.reactants].Mark().line + 1);
          std::string column = std::to_string(object[validation::keys.reactants].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::TooManyReactionComponents, "Too many reactants in object at line " + line + " column " + column });
        }

        photolysis.gas_phase = gas_phase;
        photolysis.products = products.second;
        photolysis.reactants = reactants.second;
        photolysis.unknown_properties = GetComments(object, validation::photolysis.required_keys, validation::photolysis.optional_keys);
        reactions.photolysis.push_back(photolysis);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration
