#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, ParsesFullConfiguration)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("examples/full_configuration.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);
  EXPECT_EQ(mechanism.name, "Full Configuration");
  EXPECT_EQ(mechanism.species.size(), 10);
  EXPECT_EQ(mechanism.reactions.arrhenius.size(), 1);
}

TEST(JsonParser, ParserReportsBadFiles)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("examples/_missing_configuration.json"));
  EXPECT_EQ(status, ConfigParseStatus::InvalidFilePath);
}