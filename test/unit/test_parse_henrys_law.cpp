#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseValidHenrysLawReaction)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/henrys_law/valid.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);

  EXPECT_EQ(mechanism.reactions.henrys_law.size(), 2);

  EXPECT_EQ(mechanism.reactions.henrys_law[0].name, "my henry's law");
  EXPECT_EQ(mechanism.reactions.henrys_law[0].gas_phase, "gas");
  EXPECT_EQ(mechanism.reactions.henrys_law[0].gas_phase_species, "A");
  EXPECT_EQ(mechanism.reactions.henrys_law[0].aerosol_phase, "aqueous aerosol");
  EXPECT_EQ(mechanism.reactions.henrys_law[0].aerosol_phase_species, "B");
  EXPECT_EQ(mechanism.reactions.henrys_law[0].aerosol_phase_water, "H2O_aq");
  EXPECT_EQ(mechanism.reactions.henrys_law[0].unknown_properties.size(), 1);
  EXPECT_EQ(mechanism.reactions.henrys_law[0].unknown_properties["__comment"], "\"hi\"");

  EXPECT_EQ(mechanism.reactions.henrys_law[1].name, "");
  EXPECT_EQ(mechanism.reactions.henrys_law[1].gas_phase, "gas");
  EXPECT_EQ(mechanism.reactions.henrys_law[1].gas_phase_species, "A");
  EXPECT_EQ(mechanism.reactions.henrys_law[1].aerosol_phase, "aqueous aerosol");
  EXPECT_EQ(mechanism.reactions.henrys_law[1].aerosol_phase_species, "B");
  EXPECT_EQ(mechanism.reactions.henrys_law[1].aerosol_phase_water, "H2O_aq");
  EXPECT_EQ(mechanism.reactions.henrys_law[1].unknown_properties.size(), 0);
}

TEST(JsonParser, HenrysLawDetectsUnknownSpecies)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/henrys_law/unknown_species.json"));
  EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
}

TEST(JsonParser, HenrysLawDetectsUnknownPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/henrys_law/missing_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
}

TEST(JsonParser, HenrysLawDetectsUnknownAerosolPhaseWater)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_arrhenius/missing_aerosol_phase_water.json"));
  EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
}

TEST(JsonParser, HenrysLawDetectsWhenRequestedSpeciesAreNotInAerosolPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_arrhenius/species_not_in_aerosol_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase);
}