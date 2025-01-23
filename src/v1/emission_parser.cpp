#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    ConfigParseStatus EmissionParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
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
  }  // namespace v1
}  // namespace mechanism_configuration
