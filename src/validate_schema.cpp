#include <iostream>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
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
}  // namespace mechanism_configuration