#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/parser.hpp>
#include <open_atmos/mechanism_configuration/parser_types.hpp>
#include <open_atmos/mechanism_configuration/utils.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    ConfigParseStatus ArrheniusParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        open_atmos::types::Reactions& reactions)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::Arrhenius arrhenius;

      status = ValidateSchema(object, validation::arrhenius.required_keys, validation::arrhenius.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        auto products = ParseReactantsOrProducts(validation::keys.products, object, status);
        auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

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
            std::cerr << "Ea is specified when C is also specified for an Arrhenius reaction. Pick one." << std::endl;
            status = ConfigParseStatus::MutuallyExclusiveOption;
          }
          arrhenius.C = -1 * object[validation::keys.Ea].as<double>() / constants::boltzmann;
        }

        if (object[validation::keys.name])
        {
          arrhenius.name = object[validation::keys.name].as<std::string>();
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

        arrhenius.gas_phase = gas_phase;
        arrhenius.products = products;
        arrhenius.reactants = reactants;
        arrhenius.unknown_properties = GetComments(object, validation::arrhenius.required_keys, validation::arrhenius.optional_keys);
        reactions.arrhenius.push_back(arrhenius);
      }

      return status;
    }
  }  // namespace mechanism_configuration
}  // namespace open_atmos
