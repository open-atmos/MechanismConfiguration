#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/parser.hpp>
#include <open_atmos/mechanism_configuration/utils.hpp>
#include <open_atmos/mechanism_configuration/parser_types.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    ConfigParseStatus WetDepositionParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        open_atmos::types::Reactions& reactions)
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
  }  // namespace mechanism_configuration
}  // namespace open_atmos
