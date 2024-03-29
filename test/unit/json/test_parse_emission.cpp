#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/json_parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseValidEmissionReaction)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/emission/valid.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);

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

TEST(JsonParser, EmissionDetectsUnknownSpecies)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/emission/unknown_species.json"));
  EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
}

TEST(JsonParser, EmissionDetectsBadReactionComponent)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/emission/bad_reaction_component.json"));
  EXPECT_EQ(status, ConfigParseStatus::RequiredKeyNotFound);
}

TEST(JsonParser, EmissionDetectsUnknownPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/emission/missing_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
}