#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/json_parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseValidSimpolPhaseTransferReaction)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/simpol_phase_transfer/valid.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);

  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer.size(), 2);

  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].name, "my simpol");
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].gas_phase, "gas");
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].gas_phase_species.species_name, "A");
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].aerosol_phase, "aerosol");
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].aerosol_phase_species.species_name, "B");
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].B[0], -1.97e3);
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].B[1], 2.91e0);
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].B[2], 1.96e-3);
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].B[3], -4.96e-1);
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].unknown_properties.size(), 1);
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[0].unknown_properties["__comment"], "\"cereal is also soup\"");

  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].name, "");
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].gas_phase, "gas");
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].gas_phase_species.species_name, "A");
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].aerosol_phase, "aerosol");
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].aerosol_phase_species.species_name, "B");
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].B[0], -1.97e3);
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].B[1], 2.91e0);
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].B[2], 1.96e-3);
  EXPECT_EQ(mechanism.reactions.simpol_phase_transfer[1].B[3], -4.96e-1);
}

TEST(JsonParser, SimpolPhaseTransferDetectsUnknownSpecies)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/simpol_phase_transfer/unknown_species.json"));
  EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
}

TEST(JsonParser, SimpolPhaseTransferDetectsUnknownAerosolPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/simpol_phase_transfer/missing_aerosol_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
}

TEST(JsonParser, SimpolPhaseTransferDetectsUnknownGasPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/simpol_phase_transfer/missing_gas_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
}

TEST(JsonParser, SimpolPhaseTransferDetectsUnknownGasPhaseSpeciesNotInGasPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/simpol_phase_transfer/missing_gas_phase_species_in_gas_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
}

TEST(JsonParser, SimpolPhaseTransferDetectsUnknownAerosolPhaseSpeciesNotInAerosolPhase)
{
  JsonParser parser;
  auto [status, mechanism] =
      parser.Parse(std::string("unit_configs/reactions/simpol_phase_transfer/missing_aerosol_phase_species_in_aerosol_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
}