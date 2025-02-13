#pragma once

#include <mechanism_configuration/v1/types.hpp>
#include <string>

namespace mechanism_configuration
{
  namespace v1
  {
    class Writer
    {
     public:
      static bool Write(const types::Mechanism& mechanism, const std::string& filepath);
    };
  }  // namespace v1
}  // namespace mechanism_configuration
