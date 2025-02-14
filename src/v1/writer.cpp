#include <yaml-cpp/yaml.h>

#include <fstream>
#include <mechanism_configuration/v1/writer.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    template<typename T>
    constexpr bool has_gas_phase()
    {
      return std::is_same_v<T, types::Arrhenius> || std::is_same_v<T, types::Troe> || std::is_same_v<T, types::Branched> ||
             std::is_same_v<T, types::Tunneling> || std::is_same_v<T, types::Surface> || std::is_same_v<T, types::Photolysis> ||
             std::is_same_v<T, types::Emission> || std::is_same_v<T, types::FirstOrderLoss> || std::is_same_v<T, types::HenrysLaw> ||
             std::is_same_v<T, types::SimpolPhaseTransfer>;
    }

    template<typename T>
    constexpr bool has_reactants()
    {
      return !std::is_same_v<T, types::Emission> && !std::is_same_v<T, types::WetDeposition> && !std::is_same_v<T, types::Surface>;
    }

    template<typename T>
    constexpr bool has_products()
    {
      return !std::is_same_v<T, types::FirstOrderLoss> && !std::is_same_v<T, types::WetDeposition> && !std::is_same_v<T, types::Surface> &&
             !std::is_same_v<T, types::Branched>;
    }

    bool Writer::Write(const types::Mechanism& mechanism, const std::string& filepath)
    {
      YAML::Node root;
      root["version"] = mechanism.version.to_string();
      root["name"] = mechanism.name;

      auto write_property = [](YAML::Node& node, const std::string& key, const std::string& value)
      {
        try
        {
          size_t pos;
          double numeric_value = std::stod(value, &pos);
          if (pos == value.size())
          {
            if (numeric_value == static_cast<int>(numeric_value))
            {
              node[key] = static_cast<int>(numeric_value);
            }
            else
            {
              node[key] = numeric_value;
            }
          }
          else
          {
            node[key] = value;
          }
        }
        catch (const std::invalid_argument&)
        {
          node[key] = value;
        }
      };

      YAML::Node species_node = YAML::Node(YAML::NodeType::Sequence);
      for (const auto& species : mechanism.species)
      {
        YAML::Node species_entry;
        species_entry["name"] = species.name;
        for (const auto& prop : species.optional_numerical_properties)
        {
          write_property(species_entry, prop.first, std::to_string(prop.second));
        }
        for (const auto& prop : species.unknown_properties)
        {
          write_property(species_entry, prop.first, prop.second);
        }
        species_node.push_back(species_entry);
      }
      root["species"] = species_node;

      YAML::Node phases_node = YAML::Node(YAML::NodeType::Sequence);
      for (const auto& phase : mechanism.phases)
      {
        YAML::Node phase_entry;
        phase_entry["name"] = phase.name;
        phase_entry["species"] = phase.species;
        for (const auto& prop : phase.unknown_properties)
        {
          write_property(phase_entry, prop.first, prop.second);
        }
        phases_node.push_back(phase_entry);
      }
      root["phases"] = phases_node;

      YAML::Node reactions_node = YAML::Node(YAML::NodeType::Sequence);

      auto serialize_reaction_component = [&](YAML::Node& node, const types::ReactionComponent& component)
      {
        YAML::Node component_node;
        component_node["species_name"] = component.species_name;
        component_node["coefficient"] = component.coefficient;
        for (const auto& prop : component.unknown_properties)
        {
          write_property(component_node, prop.first, prop.second);
        }
        node.push_back(component_node);
      };

      auto serialize_reaction = [&](YAML::Node& node, const auto& reaction, const std::string& type)
      {
        YAML::Node reaction_node;
        reaction_node["type"] = type;
        reaction_node["name"] = reaction.name;
        if constexpr (has_gas_phase<decltype(reaction)>())
        {
          reaction_node["gas phase"] = reaction.gas_phase;
        }
        if constexpr (has_reactants<decltype(reaction)>())
        {
          YAML::Node reactants_node = YAML::Node(YAML::NodeType::Sequence);
          for (const auto& reactant : reaction.reactants)
          {
            serialize_reaction_component(reactants_node, reactant);
          }
          reaction_node["reactants"] = reactants_node;
        }
        if constexpr (has_products<decltype(reaction)>())
        {
          YAML::Node products_node = YAML::Node(YAML::NodeType::Sequence);
          for (const auto& product : reaction.products)
          {
            serialize_reaction_component(products_node, product);
          }
          reaction_node["products"] = products_node;
        }
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(reaction_node, prop.first, prop.second);
        }
        node.push_back(reaction_node);
      };

      // Serialize each reaction type
      for (const auto& reaction : mechanism.reactions.arrhenius)
      {
        YAML::Node reaction_node;
        serialize_reaction(reaction_node, reaction, "ARRHENIUS");
        reaction_node["A"] = reaction.A;
        reaction_node["B"] = reaction.B;
        reaction_node["C"] = reaction.C;
        reaction_node["D"] = reaction.D;
        reaction_node["E"] = reaction.E;
        reactions_node.push_back(reaction_node);
      }

      for (const auto& reaction : mechanism.reactions.condensed_phase_arrhenius)
      {
        YAML::Node reaction_node;
        serialize_reaction(reaction_node, reaction, "CONDENSED_PHASE_ARRHENIUS");
        reaction_node["A"] = reaction.A;
        reaction_node["B"] = reaction.B;
        reaction_node["C"] = reaction.C;
        reaction_node["D"] = reaction.D;
        reaction_node["E"] = reaction.E;
        reaction_node["aerosol_phase"] = reaction.aerosol_phase;
        reaction_node["aerosol_phase_water"] = reaction.aerosol_phase_water;
        reactions_node.push_back(reaction_node);
      }

      for (const auto& reaction : mechanism.reactions.troe)
      {
        YAML::Node reaction_node;
        serialize_reaction(reaction_node, reaction, "TROE");
        reaction_node["k0_A"] = reaction.k0_A;
        reaction_node["k0_B"] = reaction.k0_B;
        reaction_node["k0_C"] = reaction.k0_C;
        reaction_node["kinf_A"] = reaction.kinf_A;
        reaction_node["kinf_B"] = reaction.kinf_B;
        reaction_node["kinf_C"] = reaction.kinf_C;
        reaction_node["Fc"] = reaction.Fc;
        reaction_node["N"] = reaction.N;
        reactions_node.push_back(reaction_node);
      }

      static_assert(!has_products<types::Branched>(), "has_products should be false for Branched");

      for (const auto& reaction : mechanism.reactions.branched)
      {
        YAML::Node reaction_node;
        reaction_node["type"] = "BRANCHED";
        reaction_node["name"] = reaction.name;
        reaction_node["gas phase"] = reaction.gas_phase;
        YAML::Node reactants_node = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& reactant : reaction.reactants)
        {
          serialize_reaction_component(reactants_node, reactant);
        }
        reaction_node["reactants"] = reactants_node;
        reaction_node["X"] = reaction.X;
        reaction_node["Y"] = reaction.Y;
        reaction_node["a0"] = reaction.a0;
        reaction_node["n"] = reaction.n;
        YAML::Node nitrate_products_node = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& product : reaction.nitrate_products)
        {
          serialize_reaction_component(nitrate_products_node, product);
        }
        reaction_node["nitrate_products"] = nitrate_products_node;
        YAML::Node alkoxy_products_node = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& product : reaction.alkoxy_products)
        {
          serialize_reaction_component(alkoxy_products_node, product);
        }
        reaction_node["alkoxy_products"] = alkoxy_products_node;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(reaction_node, prop.first, prop.second);
        }
        reactions_node.push_back(reaction_node);
      }

      for (const auto& reaction : mechanism.reactions.surface)
      {
        YAML::Node reaction_node;
        reaction_node["type"] = "SURFACE";
        reaction_node["name"] = reaction.name;
        reaction_node["reaction_probability"] = reaction.reaction_probability;
        YAML::Node gas_phase_species_node;
        serialize_reaction_component(gas_phase_species_node, reaction.gas_phase_species);
        reaction_node["gas_phase_species"] = gas_phase_species_node;
        YAML::Node gas_phase_products_node = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& product : reaction.gas_phase_products)
        {
          serialize_reaction_component(gas_phase_products_node, product);
        }
        reaction_node["gas_phase_products"] = gas_phase_products_node;
        reaction_node["aerosol_phase"] = reaction.aerosol_phase;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(reaction_node, prop.first, prop.second);
        }
        reactions_node.push_back(reaction_node);
      }

      for (const auto& reaction : mechanism.reactions.wet_deposition)
      {
        YAML::Node reaction_node;
        reaction_node["type"] = "WET_DEPOSITION";
        reaction_node["name"] = reaction.name;
        reaction_node["scaling_factor"] = reaction.scaling_factor;
        reaction_node["aerosol_phase"] = reaction.aerosol_phase;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(reaction_node, prop.first, prop.second);
        }
        reactions_node.push_back(reaction_node);
      }

      for (const auto& reaction : mechanism.reactions.henrys_law)
      {
        YAML::Node reaction_node;
        reaction_node["type"] = "HENRYS_LAW";
        reaction_node["name"] = reaction.name;
        reaction_node["gas phase"] = reaction.gas_phase;
        reaction_node["gas_phase_species"] = reaction.gas_phase_species;
        reaction_node["aerosol_phase"] = reaction.aerosol_phase;
        reaction_node["aerosol_phase_species"] = reaction.aerosol_phase_species;
        reaction_node["aerosol_phase_water"] = reaction.aerosol_phase_water;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(reaction_node, prop.first, prop.second);
        }
        reactions_node.push_back(reaction_node);
      }

      for (const auto& reaction : mechanism.reactions.tunneling)
      {
        YAML::Node reaction_node;
        serialize_reaction(reaction_node, reaction, "TUNNELING");
        reaction_node["A"] = reaction.A;
        reaction_node["B"] = reaction.B;
        reaction_node["C"] = reaction.C;
        reactions_node.push_back(reaction_node);
      }

      for (const auto& reaction : mechanism.reactions.photolysis)
      {
        YAML::Node reaction_node;
        serialize_reaction(reaction_node, reaction, "PHOTOLYSIS");
        reaction_node["scaling_factor"] = reaction.scaling_factor;
        reactions_node.push_back(reaction_node);
      }

      for (const auto& reaction : mechanism.reactions.condensed_phase_photolysis)
      {
        YAML::Node reaction_node;
        serialize_reaction(reaction_node, reaction, "CONDENSED_PHASE_PHOTOLYSIS");
        reaction_node["scaling_factor"] = reaction.scaling_factor_;
        reaction_node["aerosol_phase"] = reaction.aerosol_phase;
        reaction_node["aerosol_phase_water"] = reaction.aerosol_phase_water;
        reactions_node.push_back(reaction_node);
      }

      for (const auto& reaction : mechanism.reactions.emission)
      {
        YAML::Node reaction_node;
        reaction_node["type"] = "EMISSION";
        reaction_node["name"] = reaction.name;
        reaction_node["scaling_factor"] = reaction.scaling_factor;
        YAML::Node products_node = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& product : reaction.products)
        {
          serialize_reaction_component(products_node, product);
        }
        reaction_node["products"] = products_node;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(reaction_node, prop.first, prop.second);
        }
        reactions_node.push_back(reaction_node);
      }

      for (const auto& reaction : mechanism.reactions.first_order_loss)
      {
        YAML::Node reaction_node;
        reaction_node["type"] = "FIRST_ORDER_LOSS";
        reaction_node["name"] = reaction.name;
        reaction_node["scaling_factor"] = reaction.scaling_factor;
        YAML::Node reactants_node = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& reactant : reaction.reactants)
        {
          serialize_reaction_component(reactants_node, reactant);
        }
        reaction_node["reactants"] = reactants_node;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(reaction_node, prop.first, prop.second);
        }
        reactions_node.push_back(reaction_node);
      }

      for (const auto& reaction : mechanism.reactions.aqueous_equilibrium)
      {
        YAML::Node reaction_node;
        serialize_reaction(reaction_node, reaction, "AQUEOUS_EQUILIBRIUM");
        reaction_node["A"] = reaction.A;
        reaction_node["C"] = reaction.C;
        reaction_node["k_reverse"] = reaction.k_reverse;
        reaction_node["aerosol_phase"] = reaction.aerosol_phase;
        reaction_node["aerosol_phase_water"] = reaction.aerosol_phase_water;
        reactions_node.push_back(reaction_node);
      }

      for (const auto& reaction : mechanism.reactions.simpol_phase_transfer)
      {
        YAML::Node reaction_node;
        reaction_node["type"] = "SIMPOL_PHASE_TRANSFER";
        reaction_node["name"] = reaction.name;
        reaction_node["gas phase"] = reaction.gas_phase;
        YAML::Node gas_phase_species_node;
        serialize_reaction_component(gas_phase_species_node, reaction.gas_phase_species);
        reaction_node["gas_phase_species"] = gas_phase_species_node;
        reaction_node["aerosol_phase"] = reaction.aerosol_phase;
        YAML::Node aerosol_phase_species_node;
        serialize_reaction_component(aerosol_phase_species_node, reaction.aerosol_phase_species);
        reaction_node["aerosol_phase_species"] = aerosol_phase_species_node;
        YAML::Node B_node = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& b : reaction.B)
        {
          B_node.push_back(b);
        }
        reaction_node["B"] = B_node;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(reaction_node, prop.first, prop.second);
        }
        reactions_node.push_back(reaction_node);
      }

      root["reactions"] = reactions_node;

      std::ofstream file(filepath);
      if (!file.is_open())
      {
        return false;
      }
      file << root;
      file.close();
      return true;
    }
  }  // namespace v1
}  // namespace mechanism_configuration
