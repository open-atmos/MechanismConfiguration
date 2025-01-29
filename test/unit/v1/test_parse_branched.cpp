#include <gtest/gtest.h>

#include <mechanism_configuration/v1/parser.hpp>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidBranchedReaction)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/branched/valid") + extension);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.branched.size(), 1);

    EXPECT_EQ(mechanism.reactions.branched[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.branched[0].name, "my branched");
    EXPECT_EQ(mechanism.reactions.branched[0].X, 1.2e-4);
    EXPECT_EQ(mechanism.reactions.branched[0].Y, 167);
    EXPECT_EQ(mechanism.reactions.branched[0].a0, 0.15);
    EXPECT_EQ(mechanism.reactions.branched[0].n, 9);
    EXPECT_EQ(mechanism.reactions.branched[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched[0].reactants[0].species_name, "A");
    EXPECT_EQ(mechanism.reactions.branched[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products[0].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched[0].nitrate_products[0].unknown_properties["__thing"], "\"hi\"");
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products.size(), 2);
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products[0].coefficient, 0.2);
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products[1].species_name, "A");
    EXPECT_EQ(mechanism.reactions.branched[0].alkoxy_products[1].coefficient, 1.2);
    EXPECT_EQ(mechanism.reactions.branched[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.branched[0].unknown_properties["__comment"], "\"thing\"");
  }
}

TEST(ParserBase, BranchedDetectsUnknownSpecies)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/branched/unknown_species") + extension);
    EXPECT_FALSE(parsed);
  }
}

TEST(ParserBase, BranchedDetectsBadReactionComponent)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/branched/bad_reaction_component") + extension);
    EXPECT_FALSE(parsed);
  }
}

TEST(ParserBase, BranchedDetectsUnknownPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/branched/missing_phase") + extension);
    EXPECT_FALSE(parsed);
  }
}