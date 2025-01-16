#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/parser.hpp>
#include <open_atmos/mechanism_configuration/parser_types.hpp>
#include <open_atmos/mechanism_configuration/utils.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    ConfigParseStatus SurfaceParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        open_atmos::types::Reactions& reactions)
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
        auto it =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const auto& phase) { return phase.name == aerosol_phase; });
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
  }  // namespace mechanism_configuration
}  // namespace open_atmos
