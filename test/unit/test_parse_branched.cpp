#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseValidBranchedReaction)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/branched/valid.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);

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

TEST(JsonParser, BranchedDetectsUnknownSpecies)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/branched/unknown_species.json"));
  EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
}

TEST(JsonParser, BranchedDetectsBadReactionComponent)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/branched/bad_reaction_component.json"));
  EXPECT_EQ(status, ConfigParseStatus::RequiredKeyNotFound);
}

TEST(JsonParser, BranchedDetectsUnknownPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/branched/missing_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
}