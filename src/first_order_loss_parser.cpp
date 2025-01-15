#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/parser.hpp>
#include <open_atmos/mechanism_configuration/parser_types.hpp>
#include <open_atmos/mechanism_configuration/utils.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    ConfigParseStatus FirstOrderLossParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        open_atmos::types::Reactions& reactions)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::FirstOrderLoss first_order_loss;

      status = ValidateSchema(object, validation::first_order_loss.required_keys, validation::first_order_loss.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

        if (object[validation::keys.scaling_factor])
        {
          first_order_loss.scaling_factor = object[validation::keys.scaling_factor].as<double>();
        }

        if (object[validation::keys.name])
        {
          first_order_loss.name = object[validation::keys.name].as<std::string>();
        }

        std::vector<std::string> requested_species;
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

        if (status == ConfigParseStatus::Success && reactants.size() > 1)
        {
          status = ConfigParseStatus::TooManyReactionComponents;
        }

        first_order_loss.gas_phase = gas_phase;
        first_order_loss.reactants = reactants;
        first_order_loss.unknown_properties =
            GetComments(object, validation::first_order_loss.required_keys, validation::first_order_loss.optional_keys);
        reactions.first_order_loss.push_back(first_order_loss);
      }

      return status;
    }
  }  // namespace mechanism_configuration
}  // namespace open_atmos
