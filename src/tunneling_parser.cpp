#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/parser.hpp>
#include <open_atmos/mechanism_configuration/parser_types.hpp>
#include <open_atmos/mechanism_configuration/utils.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    ConfigParseStatus TunnelingParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        open_atmos::types::Reactions& reactions)
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
  }  // namespace mechanism_configuration
}  // namespace open_atmos
