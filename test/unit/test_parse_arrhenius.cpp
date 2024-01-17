#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseValidArrheniusReaction)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/arrhenius/valid.json"));

  EXPECT_EQ(mechanism.reactions.arrhenius.size(), 2);

  EXPECT_EQ(mechanism.reactions.arrhenius[0].name, "my arrhenius");
  EXPECT_EQ(mechanism.reactions.arrhenius[0].gas_phase, "gas");
  EXPECT_EQ(mechanism.reactions.arrhenius[0].A, 32.1);
  EXPECT_EQ(mechanism.reactions.arrhenius[0].B, -2.3);
  EXPECT_EQ(mechanism.reactions.arrhenius[0].C, 102.3);
  EXPECT_EQ(mechanism.reactions.arrhenius[0].D, 63.4);
  EXPECT_EQ(mechanism.reactions.arrhenius[0].E, -1.3);
  EXPECT_EQ(mechanism.reactions.arrhenius[0].reactants.size(), 1);
  EXPECT_EQ(mechanism.reactions.arrhenius[0].reactants[0].species_name, "A");
  EXPECT_EQ(mechanism.reactions.arrhenius[0].reactants[0].coefficient, 1);
  EXPECT_EQ(mechanism.reactions.arrhenius[0].products.size(), 2);
  EXPECT_EQ(mechanism.reactions.arrhenius[0].products[0].species_name, "B");
  EXPECT_EQ(mechanism.reactions.arrhenius[0].products[0].coefficient, 1.2);
  EXPECT_EQ(mechanism.reactions.arrhenius[0].products[1].species_name, "C");
  EXPECT_EQ(mechanism.reactions.arrhenius[0].products[1].coefficient, 0.3);
  EXPECT_EQ(mechanism.reactions.arrhenius[0].unknown_properties.size(), 1);
  EXPECT_EQ(mechanism.reactions.arrhenius[0].unknown_properties["__solver_param"], "0.1");

  EXPECT_EQ(mechanism.reactions.arrhenius[1].name, "my arrhenius2");
  EXPECT_EQ(mechanism.reactions.arrhenius[1].gas_phase, "gas");
  EXPECT_EQ(mechanism.reactions.arrhenius[1].A, 3.1);
  EXPECT_EQ(mechanism.reactions.arrhenius[1].B, -0.3);
  EXPECT_EQ(mechanism.reactions.arrhenius[1].C, 12.3);
  EXPECT_EQ(mechanism.reactions.arrhenius[1].D, 6.4);
  EXPECT_EQ(mechanism.reactions.arrhenius[1].E, -0.3);
  EXPECT_EQ(mechanism.reactions.arrhenius[1].reactants.size(), 2);
  EXPECT_EQ(mechanism.reactions.arrhenius[1].reactants[0].species_name, "A");
  EXPECT_EQ(mechanism.reactions.arrhenius[1].reactants[0].coefficient, 2);
  EXPECT_EQ(mechanism.reactions.arrhenius[1].reactants[1].species_name, "B");
  EXPECT_EQ(mechanism.reactions.arrhenius[1].reactants[1].coefficient, 0.1);

  EXPECT_EQ(mechanism.reactions.arrhenius[1].products.size(), 1);
  EXPECT_EQ(mechanism.reactions.arrhenius[1].products[0].species_name, "C");
  EXPECT_EQ(mechanism.reactions.arrhenius[1].products[0].coefficient, 0.5);
  EXPECT_EQ(mechanism.reactions.arrhenius[1].products[0].unknown_properties.size(), 1);
  EXPECT_EQ(mechanism.reactions.arrhenius[1].products[0].unknown_properties["__optional thing"], "\"hello\"");
}