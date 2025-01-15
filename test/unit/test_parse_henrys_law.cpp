#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(Parser, CanParseValidHenrysLawReaction)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/henrys_law/valid") + extension);
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
}

TEST(Parser, HenrysLawDetectsUnknownSpecies)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/henrys_law/unknown_species") + extension);
    EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
  }
}

TEST(Parser, HenrysLawDetectsUnknownPhase)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/henrys_law/missing_phase") + extension);
    EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
  }
}

TEST(Parser, HenrysLawDetectsUnknownAerosolPhaseWater)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/henrys_law/missing_aerosol_phase_water") + extension);
    EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
  }
}

TEST(Parser, HenrysLawDetectsWhenRequestedSpeciesAreNotInAerosolPhase)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/henrys_law/species_not_in_aerosol_phase") + extension);
    EXPECT_EQ(status, ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase);
  }
}