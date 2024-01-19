#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, ParsesFullConfiguration)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("examples/full_configuration.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);
  EXPECT_EQ(mechanism.name, "Full Configuration");
  EXPECT_EQ(mechanism.species.size(), 11);
  EXPECT_EQ(mechanism.phases.size(), 4);
  EXPECT_EQ(mechanism.reactions.arrhenius.size(), 1);
  EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius.size(), 1);
  EXPECT_EQ(mechanism.reactions.troe.size(), 1);
  EXPECT_EQ(mechanism.reactions.branched.size(), 1);
  EXPECT_EQ(mechanism.reactions.tunneling.size(), 1);
  EXPECT_EQ(mechanism.reactions.surface.size(), 1);
  EXPECT_EQ(mechanism.reactions.photolysis.size(), 1);
  EXPECT_EQ(mechanism.reactions.emission.size(), 1);
  EXPECT_EQ(mechanism.reactions.first_order_loss.size(), 1);
}

TEST(JsonParser, ParserReportsBadFiles)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("examples/_missing_configuration.json"));
  EXPECT_EQ(status, ConfigParseStatus::InvalidFilePath);
}