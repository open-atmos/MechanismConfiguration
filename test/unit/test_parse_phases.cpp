#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(JsonParser, CanParseValidPhases)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/phases/valid_phases.json"));

  EXPECT_EQ(status, ConfigParseStatus::Success);
  EXPECT_EQ(mechanism.species.size(), 3);
  EXPECT_EQ(mechanism.phases.size(), 2);

  EXPECT_EQ(mechanism.phases[0].name, "gas");
  EXPECT_EQ(mechanism.phases[0].species.size(), 2);
  EXPECT_EQ(mechanism.phases[0].species[0], "A");
  EXPECT_EQ(mechanism.phases[0].species[1], "B");
  EXPECT_EQ(mechanism.phases[0].unknown_properties.size(), 1);
  EXPECT_EQ(mechanism.phases[0].unknown_properties["__other"], "\"key\"");

  EXPECT_EQ(mechanism.phases[1].name, "aerosols");
  EXPECT_EQ(mechanism.phases[1].species.size(), 1);
  EXPECT_EQ(mechanism.phases[1].species[0], "C");
  EXPECT_EQ(mechanism.phases[1].unknown_properties.size(), 2);
  EXPECT_EQ(mechanism.phases[1].unknown_properties["__other1"], "\"key1\"");
  EXPECT_EQ(mechanism.phases[1].unknown_properties["__other2"], "\"key2\"");
}

TEST(JsonParser, DetectsDuplicatePhases)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/phases/duplicate_phases.json"));

  EXPECT_EQ(status, ConfigParseStatus::DuplicatePhasesDetected);
}

TEST(JsonParser, DetectsMissingRequiredKeys)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/phases/missing_required_key.json"));

  EXPECT_EQ(status, ConfigParseStatus::RequiredKeyNotFound);
}

TEST(JsonParser, DetectsInvalidKeys)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/phases/invalid_key.json"));

  EXPECT_EQ(status, ConfigParseStatus::InvalidKey);
}

TEST(JsonParser, DetectsPhaseRequestingUnknownSpecies)
{
  JsonParser parser;
  auto [status, mechanism] = parser.Parse(std::string("unit_configs/phases/unknown_species.json"));

  EXPECT_EQ(status, ConfigParseStatus::PhaseRequiresUnknownSpecies);
}