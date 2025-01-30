
#include <mechanism_configuration/v0/parser_types.hpp>
#include <mechanism_configuration/v0/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    ConfigParseStatus ParseChemicalSpecies(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      auto required = { validation::NAME, validation::TYPE };
      auto optional = { validation::TRACER_TYPE, validation::ABS_TOLERANCE, validation::DIFFUSION_COEFF, validation::MOL_WEIGHT };

      status = ValidateSchema(object, required, optional);

      if (status == ConfigParseStatus::Success)
      {
        std::string name = object[validation::NAME].as<std::string>();
        types::Species species;
        species.name = name;

        // Load remaining keys as properties
        for (auto it = object.begin(); it != object.end(); ++it)
        {
          auto key = it->first.as<std::string>();
          auto value = it->second;

          if (key != validation::NAME && key != validation::TYPE)
          {
            std::string stringValue = value.as<std::string>();
            species.unknown_properties[key] = stringValue;

            if (key == validation::TRACER_TYPE && stringValue == validation::THIRD_BODY)
            {
              species.third_body = true;
            }
          }
        }
        mechanism->species.push_back(species);
      }

      return status;
    }

    ConfigParseStatus ParseRelativeTolerance(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      auto required = { validation::VALUE, validation::TYPE };

      status = ValidateSchema(object, required, {});

      if (status == ConfigParseStatus::Success)
      {
        mechanism->relative_tolerance = object[validation::VALUE].as<double>();
      }

      return status;
    }

    ConfigParseStatus ParseReactants(const YAML::Node& object, std::vector<types::ReactionComponent>& reactants)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      for (auto it = object.begin(); it != object.end(); ++it)
      {
        auto key = it->first.as<std::string>();
        auto value = it->second;

        status = ValidateSchema(value, {}, { validation::QTY });
        if (status == ConfigParseStatus::Success)
        {
          double qty = 1;
          if (value[validation::QTY])
            qty = value[validation::QTY].as<std::size_t>();
          types::ReactionComponent reactant = { .species_name = key, .coefficient = qty };
          reactants.push_back(reactant);
        }
      }

      return status;
    }

    ConfigParseStatus ParseProducts(const YAML::Node& object, std::vector<types::ReactionComponent>& products)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      for (auto it = object.begin(); it != object.end(); ++it)
      {
        auto key = it->first.as<std::string>();
        auto value = it->second;

        status = ValidateSchema(value, {}, { validation::YIELD });
        if (status == ConfigParseStatus::Success)
        {
          types::ReactionComponent product = { .species_name = key, .coefficient = 1 };
          if (value[validation::YIELD])
          {
            double yield = value[validation::YIELD].as<double>();
            product.coefficient = yield;
          }
          products.push_back(product);
        }
      }
      return status;
    }
  }  // namespace v0
}  // namespace mechanism_configuration