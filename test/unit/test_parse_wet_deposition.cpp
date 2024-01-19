#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseValidWetDepositionReaction)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/wet_deposition/valid.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);

  EXPECT_EQ(mechanism.reactions.wet_deposition.size(), 1);

  EXPECT_EQ(mechanism.reactions.wet_deposition[0].name, "my arrhenius");
  EXPECT_EQ(mechanism.reactions.wet_deposition[0].gas_phase, "gas");
  EXPECT_EQ(mechanism.reactions.wet_deposition[0].unknown_properties.size(), 1);
  EXPECT_EQ(mechanism.reactions.wet_deposition[0].unknown_properties["__solver_param"], "0.1");
}

TEST(JsonParser, WetDepositionDetectsUnknownSpecies)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/wet_deposition/unknown_species.json"));
  EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
}

TEST(JsonParser, WetDepositionDetectsMutuallyExclusiveOptions)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/wet_deposition/mutually_exclusive.json"));
  EXPECT_EQ(status, ConfigParseStatus::MutuallyExclusiveOption);
}

TEST(JsonParser, WetDepositionDetectsBadReactionComponent)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/wet_deposition/bad_reaction_component.json"));
  EXPECT_EQ(status, ConfigParseStatus::RequiredKeyNotFound);
}

TEST(JsonParser, WetDepositionDetectsUnknownPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/wet_deposition/missing_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
}