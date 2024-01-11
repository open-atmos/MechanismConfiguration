#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseSpecies)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/species.json"));

  EXPECT_EQ(status, ConfigParseStatus::Success);
  EXPECT_EQ(mechanism.species.size(), 3);
}