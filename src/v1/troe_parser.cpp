#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    ConfigParseStatus TroeParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::Troe troe;

      status = ValidateSchema(object, validation::troe.required_keys, validation::troe.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        auto products = ParseReactantsOrProducts(validation::keys.products, object, status);
        auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

        if (object[validation::keys.k0_A])
        {
          troe.k0_A = object[validation::keys.k0_A].as<double>();
        }
        if (object[validation::keys.k0_B])
        {
          troe.k0_B = object[validation::keys.k0_B].as<double>();
        }
        if (object[validation::keys.k0_C])
        {
          troe.k0_C = object[validation::keys.k0_C].as<double>();
        }
        if (object[validation::keys.kinf_A])
        {
          troe.kinf_A = object[validation::keys.kinf_A].as<double>();
        }
        if (object[validation::keys.kinf_B])
        {
          troe.kinf_B = object[validation::keys.kinf_B].as<double>();
        }
        if (object[validation::keys.kinf_C])
        {
          troe.kinf_C = object[validation::keys.kinf_C].as<double>();
        }
        if (object[validation::keys.Fc])
        {
          troe.Fc = object[validation::keys.Fc].as<double>();
        }
        if (object[validation::keys.N])
        {
          troe.N = object[validation::keys.N].as<double>();
        }

        if (object[validation::keys.name])
        {
          troe.name = object[validation::keys.name].as<std::string>();
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

        troe.gas_phase = gas_phase;
        troe.products = products;
        troe.reactants = reactants;
        troe.unknown_properties = GetComments(object, validation::troe.required_keys, validation::troe.optional_keys);
        reactions.troe.push_back(troe);
      }

      return status;
    }
  }  // namespace v1
}  // namespace mechanism_configuration
