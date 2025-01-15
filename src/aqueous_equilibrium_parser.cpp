#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/parser.hpp>
#include <open_atmos/mechanism_configuration/utils.hpp>
#include <open_atmos/mechanism_configuration/parser_types.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    ConfigParseStatus AqueousEquilibriumParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        open_atmos::types::Reactions& reactions)
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
  }  // namespace mechanism_configuration
}  // namespace open_atmos
