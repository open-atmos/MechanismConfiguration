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
      std::cout << "Attempting to parse with v1 parser" << std::endl;
      auto v1_result = v1_parser.Parse(config_path);

      if (v1_result)
      {
      std::cout << "v1 parser succeeded" << std::endl;
      result.mechanism = std::move(v1_result.mechanism);
      return result;
      }
      else
      {
      std::cout << "v1 parser failed" << std::endl;
      }

      v0::Parser v0_parser;
      std::cout << "Attempting to parse with v0 parser" << std::endl;
      auto v0_result = v0_parser.Parse(config_path);

      if (v0_result)
      {
      std::cout << "v0 parser succeeded" << std::endl;
      result.mechanism = std::move(v0_result.mechanism);
      return result;
      }
      else
      {
      std::cout << "v0 parser failed" << std::endl;
      }

      std::cout << "Both parsers failed" << std::endl;
      result.errors = v1_result.errors;
      result.errors.insert(result.errors.end(), v0_result.errors.begin(), v0_result.errors.end());
      return result;
    }
  };
}  // namespace mechanism_configuration
