#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v0/parser_types.hpp>
#include <mechanism_configuration/v0/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    ConfigParseStatus UserDefinedParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;

      auto required = { validation::TYPE, validation::REACTANTS, validation::PRODUCTS, validation::MUSICA_NAME };
      auto optional = { validation::SCALING_FACTOR };

      status = ValidateSchema(object, required, optional);
      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;

      std::vector<std::function<void()>> parseSteps = { [&]()
                                                        {
                                                          if (status == ConfigParseStatus::Success)
                                                            status = ParseReactants(object[validation::REACTANTS], reactants);
                                                        },
                                                        [&]()
                                                        {
                                                          if (status == ConfigParseStatus::Success)
                                                            status = ParseProducts(object[validation::PRODUCTS], products);
                                                        } };

      for (const auto& step : parseSteps)
      {
        step();
      }

      if (status == ConfigParseStatus::Success)
      {
        double scaling_factor = object[validation::SCALING_FACTOR] ? object[validation::SCALING_FACTOR].as<double>() : 1.0;

        std::string name = "USER." + object[validation::MUSICA_NAME].as<std::string>();

        types::UserDefined user_defined = { .name = name, .scaling_factor = scaling_factor, .reactants = reactants, .products = products };
        mechanism->reactions.user_defined.push_back(user_defined);
      }

      return status;
    }
  }  // namespace v0
}  // namespace mechanism_configuration
