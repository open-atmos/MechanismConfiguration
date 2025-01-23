#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/v1/validation.hpp>

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

    ConfigParseStatus
    ValidateSchema(const YAML::Node& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys)
    {
      if (!object || object.IsNull())
      {
        return ConfigParseStatus::RequiredKeyNotFound;
      }

      std::vector<std::string> sorted_object_keys;
      for (const auto& key : object)
      {
        sorted_object_keys.push_back(key.first.as<std::string>());
      }

      auto sorted_required_keys = required_keys;
      auto sorted_optional_keys = optional_keys;
      std::sort(sorted_object_keys.begin(), sorted_object_keys.end());
      std::sort(sorted_required_keys.begin(), sorted_required_keys.end());
      std::sort(sorted_optional_keys.begin(), sorted_optional_keys.end());

      std::vector<std::string> difference;
      std::set_difference(
          sorted_object_keys.begin(),
          sorted_object_keys.end(),
          sorted_required_keys.begin(),
          sorted_required_keys.end(),
          std::back_inserter(difference));

      if (difference.size() != (sorted_object_keys.size() - required_keys.size()))
      {
        std::vector<std::string> missing_keys;
        std::set_difference(
            sorted_required_keys.begin(),
            sorted_required_keys.end(),
            sorted_object_keys.begin(),
            sorted_object_keys.end(),
            std::back_inserter(missing_keys));
        for (auto& key : missing_keys)
          std::cerr << "Missing required key '" << key << "' in object: " << object << std::endl;

        return ConfigParseStatus::RequiredKeyNotFound;
      }

      std::vector<std::string> remaining;
      std::set_difference(
          difference.begin(), difference.end(), sorted_optional_keys.begin(), sorted_optional_keys.end(), std::back_inserter(remaining));

      for (auto& key : remaining)
      {
        if (key.find("__") == std::string::npos)
        {
          std::cerr << "Non-standard key '" << key << "' found in object" << object << std::endl;

          return ConfigParseStatus::InvalidKey;
        }
      }
      return ConfigParseStatus::Success;
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

  }  // namespace v1
}  // namespace mechanism_configuration