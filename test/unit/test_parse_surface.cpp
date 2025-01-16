#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(Parser, CanParseValidSurfaceReaction)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/surface/valid") + extension);
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
}

TEST(Parser, SurfaceDetectsUnknownSpecies)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/surface/unknown_species") + extension);
    EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
  }
}

TEST(Parser, SurfaceDetectsBadReactionComponent)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/surface/bad_reaction_component") + extension);
    EXPECT_EQ(status, ConfigParseStatus::RequiredKeyNotFound);
  }
}

TEST(Parser, SurfaceDetectsUnknownAerosolPhase)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/surface/missing_aerosol_phase") + extension);
    EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
  }
}

TEST(Parser, SurfaceDetectsUnknownGasPhase)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/surface/missing_gas_phase") + extension);
    EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
  }
}