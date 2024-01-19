#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseValidFirstOrderLossReaction)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/first_order_loss/valid.json"));
  EXPECT_EQ(status, ConfigParseStatus::Success);

  EXPECT_EQ(mechanism.reactions.first_order_loss.size(), 2);

  EXPECT_EQ(mechanism.reactions.first_order_loss[0].gas_phase, "gas");
  EXPECT_EQ(mechanism.reactions.first_order_loss[0].name, "my first order loss");
  EXPECT_EQ(mechanism.reactions.first_order_loss[0].scaling_factor_, 12.3);
  EXPECT_EQ(mechanism.reactions.first_order_loss[0].products.size(), 1);
  EXPECT_EQ(mechanism.reactions.first_order_loss[0].products[0].species_name, "C");
  EXPECT_EQ(mechanism.reactions.first_order_loss[0].products[0].coefficient, 1);
  EXPECT_EQ(mechanism.reactions.first_order_loss[0].unknown_properties.size(), 1);
  EXPECT_EQ(mechanism.reactions.first_order_loss[0].unknown_properties["__comment"], "\"Strawberries are the superior fruit\"");

  EXPECT_EQ(mechanism.reactions.first_order_loss[1].gas_phase, "gas");
  EXPECT_EQ(mechanism.reactions.first_order_loss[1].scaling_factor_, 1);
  EXPECT_EQ(mechanism.reactions.first_order_loss[1].products.size(), 1);
  EXPECT_EQ(mechanism.reactions.first_order_loss[1].products[0].species_name, "C");
  EXPECT_EQ(mechanism.reactions.first_order_loss[1].products[0].coefficient, 1);
}

TEST(JsonParser, FirstOrderLossDetectsUnknownSpecies)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/first_order_loss/unknown_species.json"));
  EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
}

TEST(JsonParser, FirstOrderLossDetectsBadReactionComponent)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/first_order_loss/bad_reaction_component.json"));
  EXPECT_EQ(status, ConfigParseStatus::RequiredKeyNotFound);
}

TEST(JsonParser, FirstOrderLossDetectsUnknownPhase)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/first_order_loss/missing_phase.json"));
  EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
}