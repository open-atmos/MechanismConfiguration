#include <gtest/gtest.h>

#include <open_atmos/mechanism_configuration/yaml_parser.hpp>

using namespace open_atmos::mechanism_configuration;

TEST(YamlParser, ParsesFullConfiguration)
{
  YamlParser parser;
  EXPECT_ANY_THROW(parser.Parse(std::string("examples/full_configuration.yaml")));
}

TEST(YamlParser, ParserReportsBadFiles)
{
  YamlParser parser;
  EXPECT_ANY_THROW(parser.Parse(std::string("examples/_missing_configuration.yaml")));
}