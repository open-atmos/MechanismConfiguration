#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/utils.hpp>
#include <open_atmos/mechanism_configuration/parser.hpp>
#include <open_atmos/mechanism_configuration/parser_types.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    ConfigParseStatus BranchedParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        open_atmos::types::Reactions& reactions)
    {
        ConfigParseStatus status = ConfigParseStatus::Success;
        types::Branched branched;

        status = ValidateSchema(object, validation::branched.required_keys, validation::branched.optional_keys);
        if (status == ConfigParseStatus::Success)
        {
          auto alkoxy_products = ParseReactantsOrProducts(validation::keys.alkoxy_products, object, status);
          auto nitrate_products = ParseReactantsOrProducts(validation::keys.nitrate_products, object, status);
          auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

          branched.X = object[validation::keys.X].as<double>();
          branched.Y = object[validation::keys.Y].as<double>();
          branched.a0 = object[validation::keys.a0].as<double>();
          branched.n = object[validation::keys.n].as<double>();

          if (object[validation::keys.name])
          {
            branched.name = object[validation::keys.name].as<std::string>();
          }

          std::vector<std::string> requested_species;
          for (const auto& spec : nitrate_products)
          {
            requested_species.push_back(spec.species_name);
          }
          for (const auto& spec : alkoxy_products)
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

          branched.gas_phase = gas_phase;
          branched.nitrate_products = nitrate_products;
          branched.alkoxy_products = alkoxy_products;
          branched.reactants = reactants;
          branched.unknown_properties = GetComments(object, validation::branched.required_keys, validation::branched.optional_keys);
          reactions.branched.push_back(branched);
        }

        return status;

    }
  }  // namespace mechanism_configuration
}  // namespace open_atmos
