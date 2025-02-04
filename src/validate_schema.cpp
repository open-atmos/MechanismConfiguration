#include <iostream>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
    Errors
    ValidateSchema(const YAML::Node& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys)
    {
      Errors errors;
      std::string line = std::to_string(object.Mark().line + 1);
      std::string column = std::to_string(object.Mark().column + 1);
      if (!object || object.IsNull())
      {
        errors.push_back({ConfigParseStatus::RequiredKeyNotFound, line + ":" + column + ": error: Object is null"});
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

      // get the difference between the object keys and those required
      // what's left should be the optional keys and valid comments
      std::vector<std::string> difference;
      std::set_difference(
          sorted_object_keys.begin(),
          sorted_object_keys.end(),
          sorted_required_keys.begin(),
          sorted_required_keys.end(),
          std::back_inserter(difference));

      // check that the number of keys remaining is exactly equal to the expected number of required keys
      if (difference.size() != (sorted_object_keys.size() - required_keys.size()))
      {
        std::vector<std::string> missing_keys;
        std::set_difference(
            sorted_required_keys.begin(),
            sorted_required_keys.end(),
            sorted_object_keys.begin(),
            sorted_object_keys.end(),
            std::back_inserter(missing_keys));
        for (auto& key : missing_keys) {
          errors.push_back({ConfigParseStatus::RequiredKeyNotFound, line + ":" + column + ": error: Missing required key '" + key + "'"});
        }
      }

      std::vector<std::string> remaining;
      std::set_difference(
          difference.begin(), difference.end(), sorted_optional_keys.begin(), sorted_optional_keys.end(), std::back_inserter(remaining));
        
      // now, anything left must be standard comment starting with __
      for (auto& key : remaining)
      {
        if (key.find("__") == std::string::npos)
        {
          std::string line = std::to_string(object[key].Mark().line + 1);
          std::string column = std::to_string(object[key].Mark().column + 1);
          errors.push_back({ConfigParseStatus::InvalidKey, line + ":" + column + ": error: Non-standard key '" + key + "' found"});
        }
      }

      return errors;
    }
}  // namespace mechanism_configuration
