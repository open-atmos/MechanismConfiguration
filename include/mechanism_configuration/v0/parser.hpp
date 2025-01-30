// Copyright (C) 2023-2024 National Center for Atmospheric Research
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/v0/types.hpp>
#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/parser_base.hpp>
#include <filesystem>
#include <iostream>

namespace mechanism_configuration
{
  namespace v0
  {
    using V0Mechanism = ::mechanism_configuration::v0::types::Mechanism;

    class Parser : public ::mechanism_configuration::ParserBase<V0Mechanism>
    {
      const std::string DEFAULT_CONFIG_FILE_JSON = "config.json";
      const std::string DEFAULT_CONFIG_FILE_YAML = "config.yaml";
      const std::string CAMP_FILES = "camp-files";
      const std::string CAMP_DATA = "camp-data";
      const std::string TYPE = "type";

      ConfigParseStatus GetCampFiles(const std::filesystem::path& config_path, std::vector<std::filesystem::path>& camp_files);

     public:
      std::optional<std::unique_ptr<GlobalMechanism>> TryParse(const std::filesystem::path& config_path) override;

      std::optional<std::unique_ptr<GlobalMechanism>> TryParse(const std::string& config_path) override {
        return TryParse(std::filesystem::path(config_path));
      }

      std::optional<std::unique_ptr<GlobalMechanism>> TryParse(const YAML::Node& source) override {
        std::cerr << "Version 0 can only parse from a filepath" << std::endl;
        // version 0 must be given a file
        return std::nullopt;
      }
    };
  }  // namespace v0
}  // namespace mechanism_configuration
