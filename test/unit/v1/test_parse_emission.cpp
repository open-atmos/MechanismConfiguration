#include <gtest/gtest.h>

#include <mechanism_configuration/v1/parser.hpp>

using namespace mechanism_configuration;

TEST(ParserBase, CanParseValidEmissionReaction)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/emission/valid") + extension);
    EXPECT_TRUE(parsed);
    v1::types::Mechanism mechanism = *parsed;

    EXPECT_EQ(mechanism.reactions.emission.size(), 2);

    EXPECT_EQ(mechanism.reactions.emission[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.emission[0].name, "my emission");
    EXPECT_EQ(mechanism.reactions.emission[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.emission[0].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.emission[0].products[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.emission[0].products[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.emission[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.emission[0].unknown_properties["__comment"], "\"Dr. Pepper outranks any other soda\"");

    EXPECT_EQ(mechanism.reactions.emission[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.emission[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.emission[1].products.size(), 1);
    EXPECT_EQ(mechanism.reactions.emission[1].products[0].species_name, "B");
    EXPECT_EQ(mechanism.reactions.emission[1].products[0].coefficient, 1);
  }
}

TEST(ParserBase, EmissionDetectsUnknownSpecies)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/emission/unknown_species") + extension);
    EXPECT_FALSE(parsed);
  }
}

TEST(ParserBase, EmissionDetectsBadReactionComponent)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/emission/bad_reaction_component") + extension);
    EXPECT_FALSE(parsed);
  }
}

TEST(ParserBase, EmissionDetectsUnknownPhase)
{
  v1::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto parsed = parser.Parse(std::string("v1_unit_configs/reactions/emission/missing_phase") + extension);
    EXPECT_FALSE(parsed);
  }
}