#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(Parser, ParsesFullConfiguration)
{
  Parser parser;
  std::vector<std::string> extensions = { ".yaml", ".json" };
  for (auto& extension : extensions)
  {
    std::string path = "examples/full_configuration" + extension;
    auto [status, mechanism] = parser.Parse(path);
    EXPECT_EQ(status, ConfigParseStatus::Success);
    EXPECT_EQ(mechanism.name, "Full Configuration");
    EXPECT_EQ(mechanism.species.size(), 11);
    EXPECT_EQ(mechanism.phases.size(), 4);
    EXPECT_EQ(mechanism.reactions.aqueous_equilibrium.size(), 1);
    EXPECT_EQ(mechanism.reactions.arrhenius.size(), 2);
    EXPECT_EQ(mechanism.reactions.branched.size(), 1);
    EXPECT_EQ(mechanism.reactions.condensed_phase_arrhenius.size(), 2);
    EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis.size(), 1);
    EXPECT_EQ(mechanism.reactions.emission.size(), 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss.size(), 1);
    EXPECT_EQ(mechanism.reactions.henrys_law.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis.size(), 1);
    EXPECT_EQ(mechanism.reactions.photolysis.size(), 1);
    EXPECT_EQ(mechanism.reactions.simpol_phase_transfer.size(), 1);
    EXPECT_EQ(mechanism.reactions.surface.size(), 1);
    EXPECT_EQ(mechanism.reactions.surface.size(), 1);
    EXPECT_EQ(mechanism.reactions.troe.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling.size(), 1);
  }
}

TEST(Parser, ParserReportsBadFiles)
{
  Parser parser;
  std::vector<std::string> extensions = { ".yaml", ".json" };
  for (auto& extension : extensions)
  {
    std::string path = "examples/_missing_configuration" + extension;
    auto [status, mechanism] = parser.Parse(path);
    EXPECT_EQ(status, ConfigParseStatus::InvalidFilePath);
  }
}