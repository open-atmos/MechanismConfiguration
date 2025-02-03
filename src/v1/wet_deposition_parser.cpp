#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors WetDepositionParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::WetDeposition wet_deposition;

      auto validate = ValidateSchema(object, validation::wet_deposition.required_keys, validation::wet_deposition.optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
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

        auto it =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const auto& phase) { return phase.name == aerosol_phase; });
        if (it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::keys.aerosol_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::keys.aerosol_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, "Unknown phase: " + aerosol_phase + " at line " + line + " column " + column });
        }

        wet_deposition.aerosol_phase = aerosol_phase;
        wet_deposition.unknown_properties = GetComments(object, validation::wet_deposition.required_keys, validation::wet_deposition.optional_keys);
        reactions.wet_deposition.push_back(wet_deposition);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration
