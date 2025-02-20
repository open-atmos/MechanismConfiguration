#pragma once

#include <filesystem>
#include <mechanism_configuration/parser_result.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <memory>
#include <vector>

namespace mechanism_configuration
{
  class UniversalParser
  {
   public:
    ParserResult<GlobalMechanism> Parse(const std::filesystem::path& config_path)
    {
      ParserResult<GlobalMechanism> result;
      std::cout << "Starting Parse function" << std::endl;

      if (!std::filesystem::exists(config_path))
      {
      std::cout << "File not found: " << config_path << std::endl;
      result.errors.push_back({ ConfigParseStatus::FileNotFound, "File not found" });
      return result;
      }

      std::cout << "File exists: " << config_path << std::endl;

      v1::Parser v1_parser;
        return result;
      }

      v0::Parser v0_parser;
      auto v0_result = v0_parser.Parse(config_path);

      if (v0_result)
      {
        result.mechanism = std::move(v0_result.mechanism);
        return result;
      }

      result.errors = v1_result.errors;
      result.errors.insert(result.errors.end(), v0_result.errors.begin(), v0_result.errors.end());
      return result;
    }
  };
}  // namespace mechanism_configuration
