#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/conversions.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v0/parser_types.hpp>
#include <mechanism_configuration/v0/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    ConfigParseStatus BranchedParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;

      auto required = {
        validation::TYPE, validation::REACTANTS, validation::ALKOXY_PRODUCTS, validation::NITRATE_PRODUCTS, validation::X, validation::Y,
        validation::A0,   validation::n
      };

      status = ValidateSchema(object, required, {});

      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> alkoxy_products;
      std::vector<types::ReactionComponent> nitrate_products;

      std::vector<std::function<void()>> parseSteps = { [&]()
                                                        {
                                                          if (status == ConfigParseStatus::Success)
                                                            status = ParseReactants(object[validation::REACTANTS], reactants);
                                                        },
                                                        [&]()
                                                        {
                                                          if (status == ConfigParseStatus::Success)
                                                            status = ParseProducts(object[validation::ALKOXY_PRODUCTS], alkoxy_products);
                                                        },
                                                        [&]()
                                                        {
                                                          if (status == ConfigParseStatus::Success)
                                                            status = ParseProducts(object[validation::NITRATE_PRODUCTS], nitrate_products);
                                                        } };

      for (const auto& step : parseSteps)
      {
        step();
      }

      if (status == ConfigParseStatus::Success)
      {
        types::Branched parameters;
        parameters.X = object[validation::X].as<double>();
        // Account for the conversion of reactant concentrations to molecules cm-3
        parameters.X *= std::pow(conversions::MolesM3ToMoleculesCm3, reactants.size() - 1);
        parameters.Y = object[validation::Y].as<double>();
        parameters.a0 = object[validation::A0].as<double>();
        parameters.n = object[validation::n].as<int>();

        parameters.reactants = reactants;
        parameters.alkoxy_products = alkoxy_products;
        parameters.nitrate_products = nitrate_products;

        mechanism->reactions.branched.push_back(parameters);
      }

      return status;
    }
  }  // namespace v0
}  // namespace mechanism_configuration
