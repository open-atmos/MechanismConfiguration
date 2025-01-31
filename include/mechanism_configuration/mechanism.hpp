#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
#include <mechanism_configuration/parse_status.hpp>
#include <string>
#include <variant>

namespace mechanism_configuration
{
  struct Version {
    unsigned int major;
    unsigned int minor;
    unsigned int patch;

    Version() : major(0), minor(0), patch(0) {}

    Version(unsigned int major, unsigned int minor, unsigned int patch) : major(major), minor(minor), patch(patch) {}

    Version(std::string version) {
      std::string delimiter = ".";
      size_t pos = 0;
      std::string token;
      int i = 0;
      while ((pos = version.find(delimiter)) != std::string::npos) {
        token = version.substr(0, pos);
        switch (i) {
          case 0:
            major = std::stoi(token);
            break;
          case 1:
            minor = std::stoi(token);
            break;
          case 2:
            patch = std::stoi(token);
            break;
        }
        version.erase(0, pos + delimiter.length());
        i++;
      }
    }

    std::string to_string() {
      return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }
  };

  struct Mechanism
  {
    Version version;

    Mechanism(Version version) : version(version) {}
    Mechanism() : version() {}
    virtual ~Mechanism() = default;
  };

  using GlobalMechanism = ::mechanism_configuration::Mechanism;
}  // namespace mechanism_configuration