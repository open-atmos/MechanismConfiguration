#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseValidSurfaceReaction)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/surface/valid.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);

  EXPECT_EQ(mechanism.reactions.surface.size(), 2);

  EXPECT_EQ(mechanism.reactions.surface[0].gas_phase, "gas");
  EXPECT_EQ(mechanism.reactions.surface[0].name, "my surface");
  EXPECT_EQ(mechanism.reactions.surface[0].aerosol_phase, "surface reacting phase");
  EXPECT_EQ(mechanism.reactions.surface[0].reaction_probability, 2.0e-2);
  EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_species.species_name, "A");
  EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_species.coefficient, 1);
  EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products.size(), 2);
  EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products[0].species_name, "B");
  EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products[0].coefficient, 1);
  EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products[1].species_name, "C");
  EXPECT_EQ(mechanism.reactions.surface[0].gas_phase_products[1].coefficient, 1);
  EXPECT_EQ(mechanism.reactions.surface[0].unknown_properties.size(), 1);
  EXPECT_EQ(mechanism.reactions.surface[0].unknown_properties["__comment"], "\"key lime pie is superior to all other pies\"");

  EXPECT_EQ(mechanism.reactions.surface[1].gas_phase, "gas");
  EXPECT_EQ(mechanism.reactions.surface[1].aerosol_phase, "surface reacting phase");
  EXPECT_EQ(mechanism.reactions.surface[1].reaction_probability, 1.0);
  EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_species.species_name, "A");
  EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_species.coefficient, 1);
  EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products.size(), 2);
  EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[0].species_name, "B");
  EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[0].coefficient, 1);
  EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[0].unknown_properties.size(), 1);
  EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[0].unknown_properties["__optional thing"], "\"hello\"");
  EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[1].species_name, "C");
  EXPECT_EQ(mechanism.reactions.surface[1].gas_phase_products[1].coefficient, 1);
}

TEST(JsonParser, SurfaceDetectsUnknownSpecies)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/surface/unknown_species.json"));
  EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
}

TEST(JsonParser, SurfaceDetectsBadReactionComponent)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/surface/bad_reaction_component.json"));
  EXPECT_EQ(status, ConfigParseStatus::RequiredKeyNotFound);
}

TEST(JsonParser, SurfaceDetectsUnknownAerosolPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/surface/missing_aerosol_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
}

TEST(JsonParser, SurfaceDetectsUnknownGasPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/surface/missing_gas_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
}