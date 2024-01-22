#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseValidPhotolysisReaction)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/photolysis/valid.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);

  EXPECT_EQ(mechanism.reactions.photolysis.size(), 2);

  EXPECT_EQ(mechanism.reactions.photolysis[0].gas_phase, "gas");
  EXPECT_EQ(mechanism.reactions.photolysis[0].name, "my photolysis");
  EXPECT_EQ(mechanism.reactions.photolysis[0].scaling_factor, 12.3);
  EXPECT_EQ(mechanism.reactions.photolysis[0].reactants.size(), 1);
  EXPECT_EQ(mechanism.reactions.photolysis[0].reactants[0].species_name, "B");
  EXPECT_EQ(mechanism.reactions.photolysis[0].reactants[0].coefficient, 1);
  EXPECT_EQ(mechanism.reactions.photolysis[0].products.size(), 1);
  EXPECT_EQ(mechanism.reactions.photolysis[0].products[0].species_name, "C");
  EXPECT_EQ(mechanism.reactions.photolysis[0].products[0].coefficient, 1);
  EXPECT_EQ(mechanism.reactions.photolysis[0].unknown_properties.size(), 1);
  EXPECT_EQ(mechanism.reactions.photolysis[0].unknown_properties["__comment"], "\"hi\"");

  EXPECT_EQ(mechanism.reactions.photolysis[1].gas_phase, "gas");
  EXPECT_EQ(mechanism.reactions.photolysis[1].scaling_factor, 1);
  EXPECT_EQ(mechanism.reactions.photolysis[1].reactants.size(), 1);
  EXPECT_EQ(mechanism.reactions.photolysis[1].reactants[0].species_name, "B");
  EXPECT_EQ(mechanism.reactions.photolysis[1].reactants[0].coefficient, 1.2);
  EXPECT_EQ(mechanism.reactions.photolysis[1].products.size(), 1);
  EXPECT_EQ(mechanism.reactions.photolysis[1].products[0].species_name, "C");
  EXPECT_EQ(mechanism.reactions.photolysis[1].products[0].coefficient, 0.2);
}

TEST(JsonParser, PhotolysisDetectsUnknownSpecies)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/photolysis/unknown_species.json"));
  EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
}

TEST(JsonParser, PhotolysisDetectsBadReactionComponent)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/photolysis/bad_reaction_component.json"));
  EXPECT_EQ(status, ConfigParseStatus::InvalidKey);
}

TEST(JsonParser, PhotolysisDetectsUnknownPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/photolysis/missing_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
}

TEST(JsonParser, PhotolysisDoesNotAcceptMoreThanOneReactant)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/photolysis/more_than_one_reactant.json"));
  EXPECT_EQ(status, ConfigParseStatus::TooManyReactionComponents);
}