#include <mechanism_configuration/conversions.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v0/parser_types.hpp>
#include <mechanism_configuration/v0/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    ConfigParseStatus ArrheniusParser(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;

      auto required = { validation::TYPE, validation::REACTANTS, validation::PRODUCTS };
      auto optional = { validation::A, validation::B, validation::C, validation::D, validation::E, validation::Ea, validation::MUSICA_NAME };

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
        types::Arrhenius parameters;
        if (object[validation::A])
        {
          parameters.A = object[validation::A].as<double>();
        }
        parameters.A *= std::pow(conversions::MolesM3ToMoleculesCm3, reactants.size() - 1);
        if (object[validation::B])
        {
          parameters.B = object[validation::B].as<double>();
        }
        if (object[validation::C])
        {
          parameters.C = object[validation::C].as<double>();
        }
        if (object[validation::D])
        {
          parameters.D = object[validation::D].as<double>();
        }
        if (object[validation::E])
        {
          parameters.E = object[validation::E].as<double>();
        }
        if (object[validation::Ea])
        {
          if (parameters.C != 0)
          {
            status = ConfigParseStatus::MutuallyExclusiveOption;
          }
          else
          {
            // Calculate 'C' using 'Ea'
            parameters.C = -1 * object[validation::Ea].as<double>() / constants::boltzmann;
          }
        }

        parameters.reactants = reactants;
        parameters.products = products;

        mechanism->reactions.arrhenius.push_back(parameters);
      }

      return status;
    }
  }  // namespace v0
}  // namespace mechanism_configuration
