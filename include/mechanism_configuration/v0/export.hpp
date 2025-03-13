// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/v0/types.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    class Export
    {
     public:
      void ExportMechanism(const types::Mechanism& mechanism, const std::string& filename);
    };
  }  // namespace v0
}  // namespace mechanism_configuration
