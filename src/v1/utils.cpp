#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/v1/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    std::unordered_map<std::string, std::string>
    GetComments(const YAML::Node& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys)
    {
      std::unordered_map<std::string, std::string> unknown_properties;
      const std::string comment_start = "__";
      for (const auto& key : object)
      {
        std::string key_str = key.first.as<std::string>();
        if (key_str.compare(0, comment_start.size(), comment_start) == 0)
        {
          YAML::Emitter emitter;
          emitter << YAML::DoubleQuoted << YAML::Flow  // json style output
                  << key.second;
          unknown_properties[key_str] = emitter.c_str();
        }
      }
      return unknown_properties;
    }

    std::pair<ConfigParseStatus, std::vector<types::Species>> ParseSpecies(const YAML::Node& objects)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      std::vector<types::Species> all_species;

      for (const auto& object : objects)
      {
        types::Species species;
        status = ValidateSchema(object, validation::species.required_keys, validation::species.optional_keys);
        if (status != ConfigParseStatus::Success)
        {
          break;
        }

        std::string name = object[validation::keys.name].as<std::string>();

        std::map<std::string, double> numerical_properties{};
        for (const auto& key : validation::species.optional_keys)
        {
          if (object[key])
          {
            double val = object[key].as<double>();
            numerical_properties[key] = val;
          }
        }

        species.name = name;
        species.optional_numerical_properties = numerical_properties;
        species.unknown_properties = GetComments(object, validation::species.required_keys, validation::species.optional_keys);

        all_species.push_back(species);
      }

      if (!ContainsUniqueObjectsByName<types::Species>(all_species))
        status = ConfigParseStatus::DuplicateSpeciesDetected;

      return { status, all_species };
    }

    std::pair<ConfigParseStatus, std::vector<types::Phase>> ParsePhases(const YAML::Node& objects, const std::vector<types::Species> existing_species)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      std::vector<types::Phase> all_phases;

      for (const auto& object : objects)
      {
        types::Phase phase;
        status = ValidateSchema(object, validation::phase.required_keys, validation::phase.optional_keys);
        if (status != ConfigParseStatus::Success)
        {
          break;
        }

        std::string name = object[validation::keys.name].as<std::string>();

        std::vector<std::string> species{};
        for (const auto& spec : object[validation::keys.species])
        {
          species.push_back(spec.as<std::string>());
        }

        phase.name = name;
        phase.species = species;
        phase.unknown_properties = GetComments(object, validation::phase.required_keys, validation::phase.optional_keys);

        if (RequiresUnknownSpecies(species, existing_species))
        {
          status = ConfigParseStatus::PhaseRequiresUnknownSpecies;
          break;
        }

        all_phases.push_back(phase);
      }

      if (status == ConfigParseStatus::Success && !ContainsUniqueObjectsByName<types::Phase>(all_phases))
        status = ConfigParseStatus::DuplicatePhasesDetected;

      return { status, all_phases };
    }

    std::pair<ConfigParseStatus, types::ReactionComponent> ParseReactionComponent(const YAML::Node& object)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::ReactionComponent component;

      status = ValidateSchema(object, validation::reaction_component.required_keys, validation::reaction_component.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::string species_name = object[validation::keys.species_name].as<std::string>();
        double coefficient = 1;
        if (object[validation::keys.coefficient])
        {
          coefficient = object[validation::keys.coefficient].as<double>();
        }

        component.species_name = species_name;
        component.coefficient = coefficient;
        component.unknown_properties =
            GetComments(object, validation::reaction_component.required_keys, validation::reaction_component.optional_keys);
      }

      return { status, component };
    }

    std::vector<types::ReactionComponent> ParseReactantsOrProducts(const std::string& key, const YAML::Node& object, ConfigParseStatus& status)
    {
      std::vector<types::ReactionComponent> result{};
      for (const auto& product : object[key])
      {
        auto component_parse = ParseReactionComponent(product);
        status = component_parse.first;
        if (status != ConfigParseStatus::Success)
        {
          break;
        }
        result.push_back(component_parse.second);
      }
      return result;
    }

    std::pair<ConfigParseStatus, types::Reactions>
    ParseReactions(const YAML::Node& objects, const std::vector<types::Species>& existing_species, const std::vector<types::Phase>& existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::Reactions reactions;

      std::map<std::string, std::unique_ptr<IReactionParser>> parsers;
      parsers[validation::keys.Arrhenius_key] = std::make_unique<ArrheniusParser>();
      parsers[validation::keys.HenrysLaw_key] = std::make_unique<HenrysLawParser>();
      parsers[validation::keys.WetDeposition_key] = std::make_unique<WetDepositionParser>();
      parsers[validation::keys.AqueousPhaseEquilibrium_key] = std::make_unique<AqueousEquilibriumParser>();
      parsers[validation::keys.SimpolPhaseTransfer_key] = std::make_unique<SimpolPhaseTransferParser>();
      parsers[validation::keys.FirstOrderLoss_key] = std::make_unique<FirstOrderLossParser>();
      parsers[validation::keys.Emission_key] = std::make_unique<EmissionParser>();
      parsers[validation::keys.CondensedPhasePhotolysis_key] = std::make_unique<CondensedPhasePhotolysisParser>();
      parsers[validation::keys.Photolysis_key] = std::make_unique<PhotolysisParser>();
      parsers[validation::keys.Surface_key] = std::make_unique<SurfaceParser>();
      parsers[validation::keys.Tunneling_key] = std::make_unique<TunnelingParser>();
      parsers[validation::keys.Branched_key] = std::make_unique<BranchedParser>();
      parsers[validation::keys.Troe_key] = std::make_unique<TroeParser>();
      parsers[validation::keys.CondensedPhaseArrhenius_key] = std::make_unique<CondensedPhaseArrheniusParser>();

      for (const auto& object : objects)
      {
        std::string type = object[validation::keys.type].as<std::string>();
        auto it = parsers.find(type);
        if (it != parsers.end())
        {
          auto parse_status = it->second->parse(object, existing_species, existing_phases, reactions);
          status = parse_status;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
        }
        else
        {
          const std::string& msg = "Unknown type: " + type;
          throw std::runtime_error(msg);
        }
      }

      return { status, reactions };
    }
  }  // namespace v1
}  // namespace mechanism_configuration