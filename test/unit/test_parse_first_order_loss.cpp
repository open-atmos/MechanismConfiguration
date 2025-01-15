#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(Parser, CanParseValidFirstOrderLossReaction)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/first_order_loss/valid") + extension);
    EXPECT_EQ(status, ConfigParseStatus::Success);

    EXPECT_EQ(mechanism.reactions.first_order_loss.size(), 2);

    EXPECT_EQ(mechanism.reactions.first_order_loss[0].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].name, "my first order loss");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].scaling_factor, 12.3);
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].reactants[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].reactants[0].coefficient, 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].unknown_properties.size(), 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss[0].unknown_properties["__comment"], "\"Strawberries are the superior fruit\"");

    EXPECT_EQ(mechanism.reactions.first_order_loss[1].gas_phase, "gas");
    EXPECT_EQ(mechanism.reactions.first_order_loss[1].scaling_factor, 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss[1].reactants.size(), 1);
    EXPECT_EQ(mechanism.reactions.first_order_loss[1].reactants[0].species_name, "C");
    EXPECT_EQ(mechanism.reactions.first_order_loss[1].reactants[0].coefficient, 1);
  }
}

TEST(Parser, FirstOrderLossDetectsUnknownSpecies)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/first_order_loss/unknown_species") + extension);
    EXPECT_EQ(status, ConfigParseStatus::ReactionRequiresUnknownSpecies);
  }
}

TEST(Parser, FirstOrderLossDetectsBadReactionComponent)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/first_order_loss/bad_reaction_component") + extension);
    EXPECT_EQ(status, ConfigParseStatus::RequiredKeyNotFound);
  }
}

TEST(Parser, FirstOrderLossDetectsUnknownPhase)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/first_order_loss/missing_phase") + extension);
    EXPECT_EQ(status, ConfigParseStatus::UnknownPhase);
  }
}

TEST(Parser, FirstOrderLossDetectsMoreThanOneSpecies)
{
  Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    auto [status, mechanism] = parser.Parse(std::string("unit_configs/reactions/first_order_loss/too_many_reactants") + extension);
    EXPECT_EQ(status, ConfigParseStatus::TooManyReactionComponents);
  }
}