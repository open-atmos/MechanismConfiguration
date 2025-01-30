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
    ConfigParseStatus TroeParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;

      auto required = { validation::TYPE, validation::REACTANTS, validation::PRODUCTS };
      auto optional = { validation::K0_A,   validation::K0_B,   validation::K0_C, validation::KINF_A,
                        validation::KINF_B, validation::KINF_C, validation::FC,   validation::N };

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
        types::Troe parameters;
        if (object[validation::K0_A])
        {
          parameters.k0_A = object[validation::K0_A].as<double>();
        }
        // Account for the conversion of reactant concentrations (including M) to molecules cm-3
        parameters.k0_A *= std::pow(conversions::MolesM3ToMoleculesCm3, reactants.size());
        if (object[validation::K0_B])
        {
          parameters.k0_B = object[validation::K0_B].as<double>();
        }
        if (object[validation::K0_C])
        {
          parameters.k0_C = object[validation::K0_C].as<double>();
        }
        if (object[validation::KINF_A])
        {
          parameters.kinf_A = object[validation::KINF_A].as<double>();
        }
        // Account for terms in denominator and exponent that include [M] but not other reactants
        parameters.kinf_A *= std::pow(conversions::MolesM3ToMoleculesCm3, reactants.size() - 1);
        if (object[validation::KINF_B])
        {
          parameters.kinf_B = object[validation::KINF_B].as<double>();
        }
        if (object[validation::KINF_C])
        {
          parameters.kinf_C = object[validation::KINF_C].as<double>();
        }
        if (object[validation::FC])
        {
          parameters.Fc = object[validation::FC].as<double>();
        }
        if (object[validation::N])
        {
          parameters.N = object[validation::N].as<double>();
        }

        parameters.reactants = reactants;
        parameters.products = products;
        mechanism->reactions.troe.push_back(parameters);
      }

      return status;
    }
  }  // namespace v0
}  // namespace mechanism_configuration
