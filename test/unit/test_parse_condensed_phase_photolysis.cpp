#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseValidCondensedPhasePhotolysisReaction)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/valid.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);

  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis.size(), 2);

  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].aerosol_phase, "aqueous aerosol");
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].aerosol_phase_water, "H2O_aq");
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].name, "my condensed phase photolysis");
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].scaling_factor_, 12.3);
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].reactants.size(), 1);
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].reactants[0].species_name, "B");
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].reactants[0].coefficient, 1);
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].products.size(), 1);
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].products[0].species_name, "C");
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].products[0].coefficient, 1);
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].unknown_properties.size(), 1);
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[0].unknown_properties["__comment"], "\"hi\"");

  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].aerosol_phase, "aqueous aerosol");
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].aerosol_phase_water, "H2O_aq");
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].scaling_factor_, 1);
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].reactants.size(), 1);
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].reactants[0].species_name, "B");
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].reactants[0].coefficient, 1.2);
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].products.size(), 1);
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].products[0].species_name, "C");
  EXPECT_EQ(mechanism.reactions.condensed_phase_photolysis[1].products[0].coefficient, 0.2);
}

TEST(JsonParser, CondensedPhasePhotolysisDetectsUnknownSpecies)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/unknown_species.json"));
  EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
}

TEST(JsonParser, CondensedPhasePhotolysisDetectsBadReactionComponent)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/bad_reaction_component.json"));
  EXPECT_EQ(status, ConfigParseStatus::InvalidKey);
}

TEST(JsonParser, CondensedPhasePhotolysisDetectsUnknownPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/missing_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
}

TEST(JsonParser, CondensedPhasePhotolysisDoesNotAcceptMoreThanOneReactant)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/more_than_one_reactant.json"));
  EXPECT_EQ(status, ConfigParseStatus::TooManyReactionComponents);
}

TEST(JsonParser, CondensedPhasePhotolysisDetectsWhenRequestedSpeciesAreNotInAerosolPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/species_not_in_aerosol_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase);
}

TEST(JsonParser, CondensedPhaseArrheniusDetectsUnknownAerosolPhaseWater)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/missing_aerosol_phase_water.json"));
  EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
}