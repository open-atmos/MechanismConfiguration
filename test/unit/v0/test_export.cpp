#include <gtest/gtest.h>
#include <filesystem>
#include <cstdio>
#include <mechanism_configuration/v0/types.hpp>
#include <mechanism_configuration/v0/export.hpp>
#include <mechanism_configuration/v0/parser.hpp>

using namespace mechanism_configuration;

class ExportFixture : public ::testing::Test {
protected:
    std::string file_yaml;
    std::string file_json;

    // Set up the test fixture
    void SetUp() override {
        std::string base_file = std::filesystem::temp_directory_path() / "mechanism";
        file_yaml = base_file + ".yaml";
        file_json = base_file + ".json";
    }

    // Clean up the test fixture
    void TearDown() override {
        // Remove the files if they exist
        if (std::filesystem::exists(file_yaml)) {
            std::filesystem::remove(file_yaml);
        }
        if (std::filesystem::exists(file_json)) {
            std::filesystem::remove(file_json);
        }
    }
};

TEST_F(ExportFixture, CanExportSpecies) {
    v0::types::Mechanism mechanism;
    v0::Export exporter;

    mechanism.species.push_back(v0::types::Species());
    mechanism.species[0].name = "H";
    mechanism.species.push_back(v0::types::Species());
    mechanism.species[1].name = "O";
    mechanism.species[1].molecular_weight = 16.0;

    EXPECT_NO_THROW(exporter.ExportMechanism(mechanism, file_yaml));
    EXPECT_NO_THROW(exporter.ExportMechanism(mechanism, file_json));

    // check that the files exist
    EXPECT_TRUE(std::filesystem::exists(file_yaml));
    EXPECT_TRUE(std::filesystem::exists(file_json));

    v0::Parser parser;
    auto parsed_yaml = parser.Parse(file_yaml);
    EXPECT_TRUE(parsed_yaml);
    v0::types::Mechanism yaml_mechanism = *parsed_yaml;
    EXPECT_EQ(yaml_mechanism.species.size(), 2);
    EXPECT_EQ(yaml_mechanism.species[0].name, "H");
    EXPECT_EQ(yaml_mechanism.species[1].name, "O");
    EXPECT_EQ(yaml_mechanism.species[1].molecular_weight, 16.0);

    auto parsed_json = parser.Parse(file_json);
    EXPECT_TRUE(parsed_json);
    v0::types::Mechanism json_mechanism = *parsed_json;
    EXPECT_EQ(json_mechanism.species.size(), 2);
    EXPECT_EQ(json_mechanism.species[0].name, "H");
    EXPECT_EQ(json_mechanism.species[1].name, "O");
    EXPECT_EQ(json_mechanism.species[1].molecular_weight, 16.0);
}
