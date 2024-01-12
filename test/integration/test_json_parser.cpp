#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, Returns)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("examples/full_configuration.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);
}