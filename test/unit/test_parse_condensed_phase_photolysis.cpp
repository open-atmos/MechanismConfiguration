#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(Parser, CanParseValidCondensedPhasePhotolysisReaction)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/valid") + extension);
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
}

TEST(Parser, CondensedPhasePhotolysisDetectsUnknownSpecies)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/unknown_species") + extension);
    EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
  }
}

TEST(Parser, CondensedPhasePhotolysisDetectsBadReactionComponent)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/bad_reaction_component") + extension);
    EXPECT_EQ(status, ConfigParseStatus::InvalidKey);
  }
}

TEST(Parser, CondensedPhasePhotolysisDetectsUnknownPhase)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/missing_phase") + extension);
    EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
  }
}

TEST(Parser, CondensedPhasePhotolysisDoesNotAcceptMoreThanOneReactant)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/more_than_one_reactant") + extension);
    EXPECT_EQ(status, ConfigParseStatus::TooManyReactionComponents);
  }
}

TEST(Parser, CondensedPhasePhotolysisDetectsWhenRequestedSpeciesAreNotInAerosolPhase)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/species_not_in_aerosol_phase") + extension);
    EXPECT_EQ(status, ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase);
  }
}

TEST(Parser, CondensedPhaseArrheniusDetectsUnknownAerosolPhaseWater)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/condensed_phase_photolysis/missing_aerosol_phase_water") + extension);
    EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
  }
}