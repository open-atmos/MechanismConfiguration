#include <gtest/gtest.h>

#include <mechanism_configuration/v0/parser.hpp>

using namespace mechanism_configuration;

TEST(ArrheniusConfig, DetectsInvalidConfig)
{
  v0::Parser parser;
  std::vector<std::string> extensions = { ".json", ".yaml" };
  for (auto& extension : extensions)
  {
    std::string file = "./v0_unit_configs/arrhenius/missing_reactants/config" + extension;
    auto parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    for(auto& error : parsed.errors)
    {
      std::cout << error.second <<  " " << configParseStatusToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/arrhenius/missing_products/config" + extension;
    parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::RequiredKeyNotFound);
    for(auto& error : parsed.errors)
    {
      std::cout << error.second <<  " " << configParseStatusToString(error.first) << std::endl;
    }

    file = "./v0_unit_configs/arrhenius/mutually_exclusive/config" + extension;
    parsed = parser.Parse(file);
    EXPECT_FALSE(parsed);
    EXPECT_EQ(parsed.errors.size(), 1);
    EXPECT_EQ(parsed.errors[0].first, ConfigParseStatus::MutuallyExclusiveOption);
    for(auto& error : parsed.errors)
    {
      std::cout << error.second <<  " " << configParseStatusToString(error.first) << std::endl;
    }
  }
}

// TEST(ArrheniusConfig, ParseConfig)
// {
//   micm::SolverConfig solver_config;

//   EXPECT_NO_THROW(solver_config.ReadAndParse("./v0_unit_configs/arrhenius/valid"));

//   micm::SolverParameters solver_params = solver_config.GetSolverParams();

//   auto& process_vector = solver_params.processes_;

//   // Convert Arrhenius parameters from expecting molecules cm-3 to moles m-3
//   const double conv = 1.0e-6 * 6.02214076e23;

//   // first reaction
//   {
//     EXPECT_EQ(process_vector[0].reactants_.size(), 3);
//     EXPECT_EQ(process_vector[0].reactants_[0].name_, "foo");
//     EXPECT_EQ(process_vector[0].reactants_[1].name_, "quz");
//     EXPECT_EQ(process_vector[0].reactants_[2].name_, "quz");
//     EXPECT_EQ(process_vector[0].products_.size(), 2);
//     EXPECT_EQ(process_vector[0].products_[0].first.name_, "bar");
//     EXPECT_EQ(process_vector[0].products_[0].second, 1.0);
//     EXPECT_EQ(process_vector[0].products_[1].first.name_, "baz");
//     EXPECT_EQ(process_vector[0].products_[1].second, 3.2);
//     micm::ArrheniusRateConstant* ternary_rate_constant = dynamic_cast<micm::ArrheniusRateConstant*>(process_vector[0].rate_constant_.get());
//     auto& params = ternary_rate_constant->parameters_;
//     EXPECT_EQ(params.A_, 1.0 * conv * conv);
//     EXPECT_EQ(params.B_, 0.0);
//     EXPECT_EQ(params.C_, 0.0);
//     EXPECT_EQ(params.D_, 300);
//     EXPECT_EQ(params.E_, 0.0);
//   }

//   // second reaction
//   {
//     EXPECT_EQ(process_vector[1].reactants_.size(), 2);
//     EXPECT_EQ(process_vector[1].reactants_[0].name_, "bar");
//     EXPECT_EQ(process_vector[1].reactants_[1].name_, "baz");
//     EXPECT_EQ(process_vector[1].products_.size(), 2);
//     EXPECT_EQ(process_vector[1].products_[0].first.name_, "bar");
//     EXPECT_EQ(process_vector[1].products_[0].second, 0.5);
//     EXPECT_EQ(process_vector[1].products_[1].first.name_, "foo");
//     EXPECT_EQ(process_vector[1].products_[1].second, 1.0);
//     micm::ArrheniusRateConstant* ternary_rate_constant = dynamic_cast<micm::ArrheniusRateConstant*>(process_vector[1].rate_constant_.get());
//     auto& params = ternary_rate_constant->parameters_;
//     EXPECT_EQ(params.A_, 32.1 * conv);
//     EXPECT_EQ(params.B_, -2.3);
//     EXPECT_EQ(params.C_, 102.3);
//     EXPECT_EQ(params.D_, 63.4);
//     EXPECT_EQ(params.E_, -1.3);
//   }

//   // third reaction
//   {
//     EXPECT_EQ(process_vector[2].reactants_.size(), 2);
//     EXPECT_EQ(process_vector[2].reactants_[0].name_, "bar");
//     EXPECT_EQ(process_vector[2].reactants_[1].name_, "baz");
//     EXPECT_EQ(process_vector[2].products_.size(), 2);
//     EXPECT_EQ(process_vector[2].products_[0].first.name_, "bar");
//     EXPECT_EQ(process_vector[2].products_[0].second, 0.5);
//     EXPECT_EQ(process_vector[2].products_[1].first.name_, "foo");
//     EXPECT_EQ(process_vector[2].products_[1].second, 1.0);
//     micm::ArrheniusRateConstant* ternary_rate_constant = dynamic_cast<micm::ArrheniusRateConstant*>(process_vector[2].rate_constant_.get());
//     auto& params = ternary_rate_constant->parameters_;
//     EXPECT_EQ(params.A_, 32.1 * conv);
//     EXPECT_EQ(params.B_, -2.3);
//     EXPECT_EQ(params.C_, -1 * 2e23 / micm::constants::BOLTZMANN_CONSTANT);
//     EXPECT_EQ(params.D_, 63.4);
//     EXPECT_EQ(params.E_, -1.3);
//   }
// }

// TEST(ArrheniusConfig, DetectsNonstandardKeys)
// {
//   micm::SolverConfig solver_config;

//   try
//   {
//     solver_config.ReadAndParse("./v0_unit_configs/arrhenius/contains_nonstandard_key");
//   }
//   catch (const std::system_error& e)
//   {
//     EXPECT_EQ(e.code().value(), static_cast<int>(MicmConfigErrc::ContainsNonStandardKey));
//   }
// }

// TEST(ArrheniusConfig, DetectsNonstandardProductCoefficient)
// {
//   micm::SolverConfig solver_config;

//   try
//   {
//     solver_config.ReadAndParse("./v0_unit_configs/arrhenius/nonstandard_product_coef");
//   }
//   catch (const std::system_error& e)
//   {
//     EXPECT_EQ(e.code().value(), static_cast<int>(MicmConfigErrc::ContainsNonStandardKey));
//   }
// }

// TEST(ArrheniusConfig, DetectsNonstandardReactantCoefficient)
// {
//   micm::SolverConfig solver_config;

//   try
//   {
//     solver_config.ReadAndParse("./v0_unit_configs/arrhenius/nonstandard_reactant_coef");
//   }
//   catch (const std::system_error& e)
//   {
//     EXPECT_EQ(e.code().value(), static_cast<int>(MicmConfigErrc::ContainsNonStandardKey));
//   }
// }
