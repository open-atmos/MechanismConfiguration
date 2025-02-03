#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors ArrheniusParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::Arrhenius arrhenius;

      auto validate = ValidateSchema(object, validation::arrhenius.required_keys, validation::arrhenius.optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        auto products = ParseReactantsOrProducts(validation::keys.products, object);
        errors.insert(errors.end(), products.first.begin(), products.first.end());
        auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object);
        errors.insert(errors.end(), reactants.first.begin(), reactants.first.end());

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
            std::string line = std::to_string(object[validation::keys.Ea].Mark().line + 1);
            std::string column = std::to_string(object[validation::keys.Ea].Mark().column + 1);
            errors.push_back({ ConfigParseStatus::MutuallyExclusiveOption, "Mutually exclusive option: Ea and C at line " + line + " column " + column });
          }
          arrhenius.C = -1 * object[validation::keys.Ea].as<double>() / constants::boltzmann;
        }

        if (object[validation::keys.name])
        {
          arrhenius.name = object[validation::keys.name].as<std::string>();
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

        arrhenius.gas_phase = gas_phase;
        arrhenius.products = products.second;
        arrhenius.reactants = reactants.second;
        arrhenius.unknown_properties = GetComments(object, validation::arrhenius.required_keys, validation::arrhenius.optional_keys);
        reactions.arrhenius.push_back(arrhenius);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration
