#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/json_parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseValidWetDepositionReaction)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/wet_deposition/valid.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);

  EXPECT_EQ(mechanism.reactions.wet_deposition.size(), 2);

  EXPECT_EQ(mechanism.reactions.wet_deposition[0].name, "rxn cloud");
  EXPECT_EQ(mechanism.reactions.wet_deposition[0].aerosol_phase, "cloud");
  EXPECT_EQ(mechanism.reactions.wet_deposition[0].scaling_factor, 12.3);
  EXPECT_EQ(mechanism.reactions.wet_deposition[0].unknown_properties.size(), 1);
  EXPECT_EQ(mechanism.reactions.wet_deposition[0].unknown_properties["__comment"], "\"Tuxedo cats are the best\"");

  EXPECT_EQ(mechanism.reactions.wet_deposition[1].name, "rxn cloud2");
  EXPECT_EQ(mechanism.reactions.wet_deposition[1].aerosol_phase, "cloud");
  EXPECT_EQ(mechanism.reactions.wet_deposition[1].scaling_factor, 1);
}

TEST(JsonParser, WetDepositionDetectsUnknownPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/wet_deposition/missing_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
}