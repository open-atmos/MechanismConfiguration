// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/conversions.hpp>
#include <mechanism_configuration/v0/parser.hpp>

namespace mechanism_configuration
{
  namespace v0
  {
    // required keys
    const std::string NAME = "name";
    const std::string TYPE = "type";

    const std::string VALUE = "value";

    const std::string REACTIONS = "reactions";

    const std::string TRACER_TYPE = "tracer type";
    const std::string ABS_TOLERANCE = "absolute tolerance";
    const std::string DIFFUSION_COEFF = "diffusion coefficient [m2 s-1]";
    const std::string MOL_WEIGHT = "molecular weight [kg mol-1]";
    const std::string THIRD_BODY = "THIRD_BODY";

    const std::string REACTANTS = "reactants";
    const std::string PRODUCTS = "products";
    const std::string MUSICA_NAME = "MUSICA name";
    const std::string SCALING_FACTOR = "scaling factor";
    const std::string GAS_PHASE_REACTANT = "gas-phase reactant";
    const std::string GAS_PHASE_PRODUCTS = "gas-phase products";

    const std::string QTY = "qty";
    const std::string YIELD = "yield";

    const std::string SPECIES = "species";

    const std::string ALKOXY_PRODUCTS = "alkoxy products";
    const std::string NITRATE_PRODUCTS = "nitrate products";
    const std::string X = "X";
    const std::string Y = "Y";
    const std::string A0 = "a0";
    const std::string N = "n";

    const std::string PROBABILITY = "reaction probability";

    bool ValidateSchema(const YAML::Node& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys)
    {
      YAML::Emitter out;
      out << object;  // Serialize the object to a string

      // standard keys are:
      // those in required keys
      // those in optional keys
      // starting with __
      // anything else is reported as an error so that typos are caught, specifically for optional keys
      if (object && object.size() > 0 && object.begin()->second.IsNull())
      {
        return false;
      }

      std::vector<std::string> sorted_object_keys;
      for (auto it = object.begin(); it != object.end(); ++it)
      {
        std::string key = it->first.as<std::string>();
        sorted_object_keys.push_back(key);
      }

      auto sorted_required_keys = required_keys;
      auto sorted_optional_keys = optional_keys;
      std::sort(sorted_object_keys.begin(), sorted_object_keys.end());
      std::sort(sorted_required_keys.begin(), sorted_required_keys.end());
      std::sort(sorted_optional_keys.begin(), sorted_optional_keys.end());

      // get the difference between the object keys and those required
      // what's left should be the optional keys and valid comments
      std::vector<std::string> difference;
      std::set_difference(
          sorted_object_keys.begin(),
          sorted_object_keys.end(),
          sorted_required_keys.begin(),
          sorted_required_keys.end(),
          std::back_inserter(difference));

      // check that the number of keys remaining is exactly equal to the expected number of required keys
      if (difference.size() != (sorted_object_keys.size() - required_keys.size()))
      {
        std::vector<std::string> missing_keys;
        std::set_difference(
            sorted_required_keys.begin(),
            sorted_required_keys.end(),
            sorted_object_keys.begin(),
            sorted_object_keys.end(),
            std::back_inserter(missing_keys));
        std::string msg;
        for (auto& key : missing_keys)
        {
          msg += "Missing required key '" + key + "' in object: " + out.c_str() + "\n";
        }

        std::cerr << msg << std::endl;
        return false;
      }

      std::vector<std::string> remaining;
      std::set_difference(
          difference.begin(), difference.end(), sorted_optional_keys.begin(), sorted_optional_keys.end(), std::back_inserter(remaining));

      // now, anything left must be standard comment starting with __
      for (auto& key : remaining)
      {
        if (!key.rfind("__", 0))
        {
          std::string msg = "Non-standard key '" + key + "' found in object" + out.c_str();
          std::cerr << msg << std::endl;
          return false;
        }
      }
      return true;
    }

    bool ParseRelativeTolerance(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (!ValidateSchema(object, { VALUE, TYPE }, {}))
      {
        std::cerr << "Invalid schema for relative tolerance" << std::endl;
        return false;
      }
      mechanism->relative_tolerance = object[VALUE].as<double>();
      return true;
    }

    bool ParseMechanismArray(std::unique_ptr<types::Mechanism>& mechanism, const std::vector<YAML::Node>& objects);

    bool ParseMechanism(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (!ValidateSchema(object, { NAME, REACTIONS, TYPE }, {}))
      {
        std::cerr << "Invalid schema for mechanism" << std::endl;
        return false;
      }
      std::vector<YAML::Node> objects;
      for (const auto& element : object[REACTIONS])
      {
        objects.push_back(element);
      }

      if (!ParseMechanismArray(mechanism, objects))
      {
        std::cerr << "Failed to parse mechanism array" << std::endl;
        return false;
      }

      return true;
    }

    bool ParseReactants(const YAML::Node& object, std::vector<types::ReactionComponent>& reactants)
    {
      for (auto it = object.begin(); it != object.end(); ++it)
      {
        auto key = it->first.as<std::string>();
        auto value = it->second;

        double qty = 1;
        if (!ValidateSchema(value, {}, { QTY }))
        {
          std::cerr << "Invalid schema for reactants" << std::endl;
          return false;
        }
        if (value[QTY])
          qty = value[QTY].as<std::size_t>();
        types::ReactionComponent reactant = { .species_name = key, .coefficient = qty };
        reactants.push_back(reactant);
      }

      return true;
    }

    bool ParseProducts(const YAML::Node& object, std::vector<types::ReactionComponent>& products)
    {
      constexpr double DEFAULT_YIELD = 1.0;
      for (auto it = object.begin(); it != object.end(); ++it)
      {
        auto key = it->first.as<std::string>();
        auto value = it->second;

        if (!ValidateSchema(value, {}, { YIELD }))
        {
          std::cerr << "Invalid schema for products" << std::endl;
          return false;
        }
        types::ReactionComponent product = { .species_name = key, .coefficient = 1 };
        if (value[YIELD])
        {
          double yield = value[YIELD].as<double>();
          product.coefficient = yield;
        }
        products.push_back(product);
      }
      return true;
    }

    bool ParsePhotolysis(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (!ValidateSchema(object, { TYPE, REACTANTS, PRODUCTS, MUSICA_NAME }, { SCALING_FACTOR }))
      {
        std::cerr << "Invalid schema for photolysis" << std::endl;
        return false;
      }

      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;
      if (!ParseReactants(object[REACTANTS], reactants))
      {
        std::cerr << "Failed to parse reactants" << std::endl;
        return false;
      }
      if (!ParseProducts(object[PRODUCTS], products))
      {
        std::cerr << "Failed to parse products" << std::endl;
        return false;
      }

      double scaling_factor = object[SCALING_FACTOR] ? object[SCALING_FACTOR].as<double>() : 1.0;

      std::string name = "PHOTO." + object[MUSICA_NAME].as<std::string>();
      types::UserDefined user_defined = { .name = name, .scaling_factor = scaling_factor, .reactants = reactants, .products = products };
      mechanism->reactions.user_defined.push_back(user_defined);

      return true;
    }

    bool ParseEmission(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (!ValidateSchema(object, { TYPE, SPECIES, MUSICA_NAME }, { SCALING_FACTOR, PRODUCTS }))
      {
        std::cerr << "Invalid schema for emission" << std::endl;
        return false;
      }

      std::string species = object[SPECIES].as<std::string>();
      YAML::Node products_object{};
      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;
      products.push_back({ .species_name = species, .coefficient = 1.0 });
      double scaling_factor = object[SCALING_FACTOR] ? object[SCALING_FACTOR].as<double>() : 1.0;

      std::string name = "EMIS." + object[MUSICA_NAME].as<std::string>();
      types::UserDefined user_defined = { .name = name, .scaling_factor = scaling_factor, .reactants = reactants, .products = products };
      mechanism->reactions.user_defined.push_back(user_defined);

      return true;
    }

    bool ParseFirstOrderLoss(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (!ValidateSchema(object, { TYPE, SPECIES, MUSICA_NAME }, { SCALING_FACTOR }))
      {
        std::cerr << "Invalid schema for first order loss" << std::endl;
        return false;
      }

      std::string species = object[SPECIES].as<std::string>();
      YAML::Node products_object{};
      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;
      products.push_back({ .species_name = species, .coefficient = 1.0 });
      double scaling_factor = object[SCALING_FACTOR] ? object[SCALING_FACTOR].as<double>() : 1.0;

      std::string name = "LOSS." + object[MUSICA_NAME].as<std::string>();
      types::UserDefined user_defined = { .name = name, .scaling_factor = scaling_factor, .reactants = reactants, .products = products };
      mechanism->reactions.user_defined.push_back(user_defined);

      return true;
    }

    bool ParseArrhenius(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (!ValidateSchema(object, { TYPE, REACTANTS, PRODUCTS }, { "A", "B", "C", "D", "E", "Ea", MUSICA_NAME }))
      {
        std::cerr << "Invalid schema for Arrhenius" << std::endl;
        return false;
      }

      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;
      if (!ParseReactants(object[REACTANTS], reactants))
      {
        std::cerr << "Failed to parse reactants" << std::endl;
        return false;
      }
      if (!ParseProducts(object[PRODUCTS], products))
      {
        std::cerr << "Failed to parse products" << std::endl;
        return false;
      }

      types::Arrhenius parameters;
      if (object["A"])
      {
        parameters.A = object["A"].as<double>();
      }
      parameters.A *= std::pow(conversions::MolesM3ToMoleculesCm3, reactants.size() - 1);
      if (object["B"])
      {
        parameters.B = object["B"].as<double>();
      }
      if (object["C"])
      {
        parameters.C = object["C"].as<double>();
      }
      if (object["D"])
      {
        parameters.D = object["D"].as<double>();
      }
      if (object["E"])
      {
        parameters.E = object["E"].as<double>();
      }
      if (object["Ea"])
      {
        if (parameters.C != 0)
        {
          std::cerr << "Ea is specified when C is also specified for an Arrhenius reaction. Pick one." << std::endl;
          return false;
        }
        // Calculate 'C' using 'Ea'
        parameters.C = -1 * object["Ea"].as<double>() / constants::boltzmann;
      }

      parameters.reactants = reactants;
      parameters.products = products;

      mechanism->reactions.arrhenius.push_back(parameters);

      return true;
    }

    bool ParseTroe(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (!ValidateSchema(object, { TYPE, REACTANTS, PRODUCTS }, { "k0_A", "k0_B", "k0_C", "kinf_A", "kinf_B", "kinf_C", "Fc", "N" }))
      {
        std::cerr << "Invalid schema for Troe" << std::endl;
        return false;
      }

      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;
      if (!ParseReactants(object[REACTANTS], reactants))
      {
        std::cerr << "Failed to parse reactants" << std::endl;
        return false;
      }
      if (!ParseProducts(object[PRODUCTS], products))
      {
        std::cerr << "Failed to parse products" << std::endl;
        return false;
      }

      types::Troe parameters;
      if (object["k0_A"])
      {
        parameters.k0_A = object["k0_A"].as<double>();
      }
      // Account for the conversion of reactant concentrations (including M) to molecules cm-3
      parameters.k0_A *= std::pow(conversions::MolesM3ToMoleculesCm3, reactants.size());
      if (object["k0_B"])
      {
        parameters.k0_B = object["k0_B"].as<double>();
      }
      if (object["k0_C"])
      {
        parameters.k0_C = object["k0_C"].as<double>();
      }
      if (object["kinf_A"])
      {
        parameters.kinf_A = object["kinf_A"].as<double>();
      }
      // Account for terms in denominator and exponent that include [M] but not other reactants
      parameters.kinf_A *= std::pow(conversions::MolesM3ToMoleculesCm3, reactants.size() - 1);
      if (object["kinf_B"])
      {
        parameters.kinf_B = object["kinf_B"].as<double>();
      }
      if (object["kinf_C"])
      {
        parameters.kinf_C = object["kinf_C"].as<double>();
      }
      if (object["Fc"])
      {
        parameters.Fc = object["Fc"].as<double>();
      }
      if (object["N"])
      {
        parameters.N = object["N"].as<double>();
      }

      parameters.reactants = reactants;
      parameters.products = products;
      mechanism->reactions.troe.push_back(parameters);

      return true;
    }

    bool ParseTernaryChemicalActivation(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (!ValidateSchema(object, { TYPE, REACTANTS, PRODUCTS }, { "k0_A", "k0_B", "k0_C", "kinf_A", "kinf_B", "kinf_C", "Fc", "N" }))
      {
        std::cerr << "Invalid schema for Ternary Chemical Activation" << std::endl;
        return false;
      }

      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;
      if (!ParseReactants(object[REACTANTS], reactants))
      {
        std::cerr << "Failed to parse reactants" << std::endl;
        return false;
      }
      if (!ParseProducts(object[PRODUCTS], products))
      {
        std::cerr << "Failed to parse products" << std::endl;
        return false;
      }

      types::TernaryChemicalActivation parameters;
      if (object["k0_A"])
      {
        parameters.k0_A = object["k0_A"].as<double>();
      }
      // Account for the conversion of reactant concentrations (including M) to molecules cm-3
      parameters.k0_A *= std::pow(conversions::MolesM3ToMoleculesCm3, reactants.size() - 1);
      if (object["k0_B"])
      {
        parameters.k0_B = object["k0_B"].as<double>();
      }
      if (object["k0_C"])
      {
        parameters.k0_C = object["k0_C"].as<double>();
      }
      if (object["kinf_A"])
      {
        parameters.kinf_A = object["kinf_A"].as<double>();
      }
      // Account for terms in denominator and exponent that include [M] but not other reactants
      parameters.kinf_A *= std::pow(conversions::MolesM3ToMoleculesCm3, reactants.size() - 2);
      if (object["kinf_B"])
      {
        parameters.kinf_B = object["kinf_B"].as<double>();
      }
      if (object["kinf_C"])
      {
        parameters.kinf_C = object["kinf_C"].as<double>();
      }
      if (object["Fc"])
      {
        parameters.Fc = object["Fc"].as<double>();
      }
      if (object["N"])
      {
        parameters.N = object["N"].as<double>();
      }

      parameters.reactants = reactants;
      parameters.products = products;
      mechanism->reactions.ternary_chemical_activation.push_back(parameters);

      return true;
    }

    bool ParseBranched(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (!ValidateSchema(object, { TYPE, REACTANTS, ALKOXY_PRODUCTS, NITRATE_PRODUCTS, X, Y, A0, N }, {}))
      {
        std::cerr << "Invalid schema for branched" << std::endl;
        return false;
      }

      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> alkoxy_products;
      std::vector<types::ReactionComponent> nitrate_products;

      if (!ParseReactants(object[REACTANTS], reactants))
      {
        std::cerr << "Failed to parse reactants" << std::endl;
        return false;
      }
      if (!ParseProducts(object[ALKOXY_PRODUCTS], alkoxy_products))
      {
        std::cerr << "Failed to parse alkoxy products" << std::endl;
        return false;
      }
      if (!ParseProducts(object[NITRATE_PRODUCTS], nitrate_products))
      {
        std::cerr << "Failed to parse nitrate products" << std::endl;
        return false;
      }

      types::Branched parameters;
      parameters.X = object[X].as<double>();
      // Account for the conversion of reactant concentrations to molecules cm-3
      parameters.X *= std::pow(conversions::MolesM3ToMoleculesCm3, reactants.size() - 1);
      parameters.Y = object[Y].as<double>();
      parameters.a0 = object[A0].as<double>();
      parameters.n = object[N].as<int>();

      parameters.reactants = reactants;
      parameters.alkoxy_products = alkoxy_products;
      parameters.nitrate_products = nitrate_products;

      mechanism->reactions.branched.push_back(parameters);
      return true;
    }

    bool ParseTunneling(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (!ValidateSchema(object, { TYPE, REACTANTS, PRODUCTS }, { "A", "B", "C" }))
      {
        // print out a serizlied version of the object
        YAML::Emitter out;
        out << object;
        std::cerr << out.c_str() << std::endl;
        std::cerr << "Invalid schema for tunneling" << std::endl;
        return false;
      }

      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;
      if (!ParseReactants(object[REACTANTS], reactants))
      {
        std::cerr << "Failed to parse reactants" << std::endl;
        return false;
      }
      if (!ParseProducts(object[PRODUCTS], products))
      {
        std::cerr << "Failed to parse products" << std::endl;
        return false;
      }

      types::Tunneling parameters;
      if (object["A"])
      {
        parameters.A = object["A"].as<double>();
      }
      // Account for the conversion of reactant concentrations to molecules cm-3
      parameters.A *= std::pow(conversions::MolesM3ToMoleculesCm3, reactants.size() - 1);
      if (object["B"])
      {
        parameters.B = object["B"].as<double>();
      }
      if (object["C"])
      {
        parameters.C = object["C"].as<double>();
      }

      parameters.reactants = reactants;
      parameters.products = products;
      mechanism->reactions.tunneling.push_back(parameters);

      return true;
    }

    bool ParseSurface(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (!ValidateSchema(object, { TYPE, GAS_PHASE_PRODUCTS, GAS_PHASE_REACTANT, MUSICA_NAME }, { PROBABILITY }))
      {
        std::cerr << "Invalid schema for surface" << std::endl;
        return false;
      }

      std::string species_name = object[GAS_PHASE_REACTANT].as<std::string>();

      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;

      reactants.push_back({ .species_name = species_name, .coefficient = 1.0 });

      if (!ParseProducts(object[GAS_PHASE_PRODUCTS], products))
      {
        std::cerr << "Failed to parse products" << std::endl;
        return false;
      }

      types::Surface parameters;

      parameters.gas_phase_species = reactants[0];
      parameters.gas_phase_products = products;

      if (object[PROBABILITY])
      {
        parameters.reaction_probability = object[PROBABILITY].as<double>();
      }

      std::string name = "SURF." + object[MUSICA_NAME].as<std::string>();
      parameters.name = name;

      mechanism->reactions.surface.push_back(parameters);

      return true;
    }

    bool ParseUserDefined(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (!ValidateSchema(object, { TYPE, REACTANTS, PRODUCTS, MUSICA_NAME }, { SCALING_FACTOR })) {
        std::cerr << "Invalid schema for user defined" << std::endl;
        return false;
      }

      std::vector<types::ReactionComponent> reactants;
      std::vector<types::ReactionComponent> products;
      if (!ParseReactants(object[REACTANTS], reactants))
      {
        std::cerr << "Failed to parse reactants" << std::endl;
        return false;
      }
      if (!ParseProducts(object[PRODUCTS], products))
      {
        std::cerr << "Failed to parse products" << std::endl;
        return false;
      }

      double scaling_factor = object[SCALING_FACTOR] ? object[SCALING_FACTOR].as<double>() : 1.0;

      std::string name = "USER." + object[MUSICA_NAME].as<std::string>();

      types::UserDefined user_defined = { .name = name, .scaling_factor = scaling_factor, .reactants = reactants, .products = products };
      mechanism->reactions.user_defined.push_back(user_defined);

      return true;
    }

    bool ParseMechanismArray(std::unique_ptr<types::Mechanism>& mechanism, const std::vector<YAML::Node>& objects)
    {
      for (const auto& object : objects)
      {
        std::string type = object[TYPE].as<std::string>();

        bool success = true;

        if (type == "MECHANISM")
        {
          success = ParseMechanism(mechanism, object);
        }
        else if (type == "PHOTOLYSIS")
        {
          success = ParsePhotolysis(mechanism, object);
        }
        else if (type == "EMISSION")
        {
          success = ParseEmission(mechanism, object);
        }
        else if (type == "FIRST_ORDER_LOSS")
        {
          success = ParseFirstOrderLoss(mechanism, object);
        }
        else if (type == "ARRHENIUS")
        {
          success = ParseArrhenius(mechanism, object);
        }
        else if (type == "TROE")
        {
          success = ParseTroe(mechanism, object);
        }
        else if (type == "TERNARY_CHEMICAL_ACTIVATION")
        {
          success = ParseTernaryChemicalActivation(mechanism, object);
        }
        else if (type == "BRANCHED" || type == "WENNBERG_NO_RO2")
        {
          success = ParseBranched(mechanism, object);
        }
        else if (type == "TUNNELING" || type == "WENNBERG_TUNNELING")
        {
          success = ParseTunneling(mechanism, object);
        }
        else if (type == "SURFACE")
        {
          success = ParseSurface(mechanism, object);
        }
        else if (type == "USER_DEFINED")
        {
          success = ParseUserDefined(mechanism, object);
        }
        //   else
        //   {
        //     throw std::system_error{ make_error_code(MicmConfigErrc::UnknownKey), type };
        //   }

        if (!success)
        {
          std::cerr << "Failed to parse mechanism array" << std::endl;
          return false;
        }
      }

      return true;
    }

    bool ParseChemicalSpecies(std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      if (!ValidateSchema(object, { NAME, TYPE }, { TRACER_TYPE, ABS_TOLERANCE, DIFFUSION_COEFF, MOL_WEIGHT }))
      {
        return false;
      }

      std::string name = object[NAME].as<std::string>();
      types::Species species;
      species.name = name;

      // Load remaining keys as properties
      for (auto it = object.begin(); it != object.end(); ++it)
      {
        auto key = it->first.as<std::string>();
        auto value = it->second;

        if (key.empty())
        {
          std::cerr << "Empty key found in object: " << object << std::endl;
          return false;
        }

        if (key != NAME && key != TYPE)
        {
          std::string stringValue = value.as<std::string>();
          species.unknown_properties[key] = stringValue;

          if (key == TRACER_TYPE && stringValue == THIRD_BODY)
          {
            species.third_body = true;
          }
        }
      }
      mechanism->species.push_back(species);

      return true;
    }

    bool ParseSpeciesArray(std::unique_ptr<types::Mechanism>& mechanism, const std::vector<YAML::Node>& objects)
    {
      for (const auto& object : objects)
      {
        std::string type = object[TYPE].as<std::string>();

        if (type == "CHEM_SPEC")
        {
          if (!ParseChemicalSpecies(mechanism, object))
          {
            return false;
          }
        }
        else if (type == "RELATIVE_TOLERANCE")
        {
          if (!ParseRelativeTolerance(mechanism, object))
          {
            return false;
          }
        }
      }
      return true;
    }

    std::optional<std::unique_ptr<GlobalMechanism>> Parser::TryParse(const std::filesystem::path& config_path)
    {
      std::unique_ptr<types::Mechanism> mechanism = std::make_unique<types::Mechanism>();
      // Look for CAMP config path
      if (!std::filesystem::exists(config_path))
      {
        std::cerr << "File does not exist: " << config_path << std::endl;
        return std::nullopt;
      }

      std::filesystem::path config_dir;
      std::filesystem::path config_file;

      if (std::filesystem::is_directory(config_path))
      {
        // If config path is a directory, use default config file name
        config_dir = config_path;
        if (std::filesystem::exists(config_dir / DEFAULT_CONFIG_FILE_YAML))
        {
          config_file = config_dir / DEFAULT_CONFIG_FILE_YAML;
        }
        else
        {
          config_file = config_dir / DEFAULT_CONFIG_FILE_JSON;
        }
      }
      else
      {
        // Extract configuration dir from configuration file path
        config_dir = config_path.parent_path();
        config_file = config_path;
      }

      // Load the CAMP file list YAML
      YAML::Node camp_data = YAML::LoadFile(config_file.string());
      if (!camp_data[CAMP_FILES])
      {
        std::cerr << "CAMP files not found in: " << config_file << std::endl;
        return std::nullopt;
      }

      // Build a list of individual CAMP config files
      std::vector<std::filesystem::path> camp_files;
      for (const auto& element : camp_data[CAMP_FILES])
      {
        std::filesystem::path camp_file = config_dir / element.as<std::string>();
        if (!std::filesystem::exists(camp_file))
        {
          std::cerr << "CAMP file not found: " << camp_file << std::endl;
          return std::nullopt;
        }
        camp_files.push_back(camp_file);
      }

      // No config files found
      if (camp_files.size() < 1)
      {
        std::cerr << "No CAMP files found in: " << config_file << std::endl;
        return std::nullopt;
      }

      std::vector<YAML::Node> species_objects;
      std::vector<YAML::Node> mechanism_objects;

      // Iterate CAMP file list and form CAMP data object arrays
      for (const auto& camp_file : camp_files)
      {
        YAML::Node config_subset = YAML::LoadFile(camp_file.string());

        if (!config_subset[CAMP_DATA])
        {
          YAML::Emitter out;
          out << config_subset;
          std::cerr << "CAMP data not found in: " << camp_file << std::endl;
          return std::nullopt;
        }
        // Iterate YAML objects from CAMP data entry
        for (const auto& object : config_subset[CAMP_DATA])
        {
          if (object)
          {
            // Require object to have a type entry
            if (!object[TYPE])
            {
              YAML::Emitter out;
              out << object;
              std::string msg = "object: " + std::string(out.c_str()) + "; type: " + TYPE;

              std::cerr << "Object type not found in: " << camp_file << std::endl;
              return std::nullopt;
            }

            // Sort into object arrays by type
            std::string type = object[TYPE].as<std::string>();
            // CHEM_SPEC and RELATIVE_TOLERANCE parsed first by ParseSpeciesArray
            if ((type == "CHEM_SPEC") || (type == "RELATIVE_TOLERANCE"))
            {
              species_objects.push_back(object);
            }
            // All other objects will be parsed by ParseMechanismArray
            else
            {
              mechanism_objects.push_back(object);
            }
          }
        }
      }

      if (!ParseSpeciesArray(mechanism, species_objects))
      {
        std::cerr << "Failed to parse species array" << std::endl;
        return std::nullopt;
      }

      // all species in version 0 are in the gas phase
      types::Phase gas_phase;
      gas_phase.name = "GAS";
      for (auto& species : mechanism->species)
      {
        gas_phase.species.push_back(species.name);
      }

      if (!ParseMechanismArray(mechanism, mechanism_objects))
      {
        std::cerr << "Failed to parse mechanism array" << std::endl;
        return std::nullopt;
      }

      return std::unique_ptr<GlobalMechanism>(std::move(mechanism));
    }

  }  // namespace v0
}  // namespace mechanism_configuration