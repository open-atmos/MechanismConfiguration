#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(Parser, CanParseValidTunnelingReaction)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/tunneling/valid") + extension);
    EXPECT_EQ(status, ConfigParseStatus::Success);

    EXPECT_EQ(mechanism.reactions.tunneling.size(), 2);

    EXPECT_EQ(mechanism.reactions.tunneling[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.tunneling[0].A, 123.45);
    EXPECT_EQ(mechanism.reactions.tunneling[0].B, 1200.0);
    EXPECT_EQ(mechanism.reactions.tunneling[0].C, 1.0e8);
    EXPECT_EQ(mechanism.reactions.tunneling[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling[0].reactants[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.tunneling[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.tunneling[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling[0].products[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.tunneling[0].products[0].coefficient, 1);

    EXPECT_EQ(mechanism.reactions.tunneling[1].name, "my tunneling");
    EXPECT_EQ(mechanism.reactions.tunneling[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.tunneling[1].A, 1.0);
    EXPECT_EQ(mechanism.reactions.tunneling[1].B, 0);
    EXPECT_EQ(mechanism.reactions.tunneling[1].C, 0);
    EXPECT_EQ(mechanism.reactions.tunneling[1].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling[1].reactants[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.tunneling[1].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products.size(), 2);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[0].species_name, "A");
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[0].coefficient, 0.2);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[0].unknown_properties["__optional thing"], "\"hello\"");
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[1].species_name, "B");
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[1].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.tunneling[1].products[1].unknown_properties.size(), 0);
  }
}

TEST(Parser, TunnelingDetectsUnknownSpecies)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/tunneling/unknown_species") + extension);
    EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
  }
}

TEST(Parser, TunnelingDetectsBadReactionComponent)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/tunneling/bad_reaction_component") + extension);
    EXPECT_EQ(status, ConfigParseStatus::RequiredKeyNotFound);
  }
}

TEST(Parser, TunnelingDetectsUnknownPhase)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/tunneling/missing_phase") + extension);
    EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
  }
}