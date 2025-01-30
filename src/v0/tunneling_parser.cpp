#include <mechanism_configuration/conversions.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v0/parser_types.hpp>
#include <mechanism_configuration/v0/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    ConfigParseStatus TunnelingParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;

      auto required = { validation::TYPE, validation::REACTANTS, validation::PRODUCTS };
      auto optional = { validation::A, validation::B, validation::C };

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
        types::Tunneling parameters;
        if (object[validation::A])
        {
          parameters.A = object[validation::A].as<double>();
        }
        // Account for the conversion of reactant concentrations to molecules cm-3
        parameters.A *= std::pow(conversions::MolesM3ToMoleculesCm3, reactants.size() - 1);
        if (object[validation::B])
        {
          parameters.B = object[validation::B].as<double>();
        }
        if (object[validation::C])
        {
          parameters.C = object[validation::C].as<double>();
        }

        parameters.reactants = reactants;
        parameters.products = products;
        mechanism->reactions.tunneling.push_back(parameters);
      }

      return status;
    }
  }  // namespace v0
}  // namespace mechanism_configuration
