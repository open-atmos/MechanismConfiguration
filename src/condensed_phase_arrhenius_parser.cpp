#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/utils.hpp>
#include <open_atmos/mechanism_configuration/parser.hpp>
#include <open_atmos/mechanism_configuration/parser_types.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    ConfigParseStatus CondensedPhaseArrheniusParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        open_atmos::types::Reactions& reactions)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::CondensedPhaseArrhenius condensed_phase_arrhenius;

      status = ValidateSchema(object, validation::condensed_phase_arrhenius.required_keys, validation::condensed_phase_arrhenius.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        auto products = ParseReactantsOrProducts(validation::keys.products, object, status);
        auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

        if (object[validation::keys.A])
        {
          condensed_phase_arrhenius.A = object[validation::keys.A].as<double>();
        }
        if (object[validation::keys.B])
        {
          condensed_phase_arrhenius.B = object[validation::keys.B].as<double>();
        }
        if (object[validation::keys.C])
        {
          condensed_phase_arrhenius.C = object[validation::keys.C].as<double>();
        }
        if (object[validation::keys.D])
        {
          condensed_phase_arrhenius.D = object[validation::keys.D].as<double>();
        }
        if (object[validation::keys.E])
        {
          condensed_phase_arrhenius.E = object[validation::keys.E].as<double>();
        }
        if (object[validation::keys.Ea])
        {
          if (condensed_phase_arrhenius.C != 0)
          {
            std::cerr << "Ea is specified when C is also specified for an CondensedPhasecondensed_phase_arrhenius reaction. Pick one." << std::endl;
            status = ConfigParseStatus::MutuallyExclusiveOption;
          }
          condensed_phase_arrhenius.C = -1 * object[validation::keys.Ea].as<double>() / constants::boltzmann;
        }

        if (object[validation::keys.name])
        {
          condensed_phase_arrhenius.name = object[validation::keys.name].as<std::string>();
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

        condensed_phase_arrhenius.aerosol_phase = aerosol_phase;
        condensed_phase_arrhenius.aerosol_phase_water = aerosol_phase_water;
        condensed_phase_arrhenius.products = products;
        condensed_phase_arrhenius.reactants = reactants;
        condensed_phase_arrhenius.unknown_properties =
            GetComments(object, validation::condensed_phase_arrhenius.required_keys, validation::condensed_phase_arrhenius.optional_keys);
        reactions.condensed_phase_arrhenius.push_back(condensed_phase_arrhenius);
      }

      return status;
    }
  }  // namespace mechanism_configuration
}  // namespace open_atmos
