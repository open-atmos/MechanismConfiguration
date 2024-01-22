#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseValidAqueousEquilibriumReaction)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/aqueous_equilibrium/valid.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);

  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium.size(), 2);

  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].name, "my aqueous eq");
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].aerosol_phase, "aerosol");
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].aerosol_phase_water, "H2O_aq");
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].A, 1.14e-2);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].C, 2300.0);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].k_reverse, 0.32);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].reactants.size(), 1);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].reactants[0].species_name, "A");
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].reactants[0].coefficient, 2);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products.size(), 2);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products[0].species_name, "B");
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products[0].coefficient, 1);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products[1].species_name, "C");
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].products[1].coefficient, 1);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].ion_pair[0].species_name, "A");
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].ion_pair[1].species_name, "B");
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].unknown_properties.size(), 1);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[0].unknown_properties["__comment"], "\"GIF is pronounced with a hard g\"");

  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].aerosol_phase, "aerosol");
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].aerosol_phase_water, "H2O_aq");
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].A, 1);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].C, 0);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].k_reverse, 0.32);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].reactants.size(), 1);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].reactants[0].species_name, "A");
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].reactants[0].coefficient, 2);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products.size(), 2);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products[0].species_name, "B");
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products[0].coefficient, 1);
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products[1].species_name, "C");
  EXPECT_EQ(mechanism.reactions.aqueous_equilibrium[1].products[1].coefficient, 1);
}

// TEST(JsonParser, AqueousEquilibriumDetectsUnknownSpecies)
// {
//   JsonParser parser;
//   auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/aqueous_equilibrium/unknown_species.json"));
//   EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
// }

// TEST(JsonParser, AqueousEquilibriumDetectsMutuallyExclusiveOptions)
// {
//   JsonParser parser;
//   auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/aqueous_equilibrium/mutually_exclusive.json"));
//   EXPECT_EQ(status, ConfigParseStatus::MutuallyExclusiveOption);
// }

// TEST(JsonParser, AqueousEquilibriumDetectsBadReactionComponent)
// {
//   JsonParser parser;
//   auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/aqueous_equilibrium/bad_reaction_component.json"));
//   EXPECT_EQ(status, ConfigParseStatus::RequiredKeyNotFound);
// }

// TEST(JsonParser, AqueousEquilibriumDetectsUnknownPhase)
// {
//   JsonParser parser;
//   auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/aqueous_equilibrium/missing_phase.json"));
//   EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
// }