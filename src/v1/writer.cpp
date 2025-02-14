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
      YAML::Emitter out;
      out << YAML::BeginMap;
      out << YAML::Key << "version" << YAML::Value << mechanism.version.to_string();
      out << YAML::Key << "name" << YAML::Value << mechanism.name;

      auto write_property = [&](const std::string& key, const std::string& value)
      {
        try
        {
          size_t pos;
          double numeric_value = std::stod(value, &pos);
          if (pos == value.size())
          {
            if (numeric_value == static_cast<int>(numeric_value))
            {
              out << YAML::Key << key << YAML::Value << static_cast<int>(numeric_value);
            }
            else
            {
              out << YAML::Key << key << YAML::Value << numeric_value;
            }
          }
          else
          {
            out << YAML::Key << key << YAML::Value << value;
          }
        }
        catch (const std::invalid_argument&)
        {
          out << YAML::Key << key << YAML::Value << value;
        }
      };

      out << YAML::Key << "species" << YAML::Value << YAML::BeginSeq;
      for (const auto& species : mechanism.species)
      {
        out << YAML::BeginMap;
        out << YAML::Key << "name" << YAML::Value << species.name;
        for (const auto& prop : species.optional_numerical_properties)
        {
          write_property(prop.first, std::to_string(prop.second));
        }
        for (const auto& prop : species.unknown_properties)
        {
          write_property(prop.first, prop.second);
        }
        out << YAML::EndMap;
      }
      out << YAML::EndSeq;

      out << YAML::Key << "phases" << YAML::Value << YAML::BeginSeq;
      for (const auto& phase : mechanism.phases)
      {
        out << YAML::BeginMap;
        out << YAML::Key << "name" << YAML::Value << phase.name;
        out << YAML::Key << "species" << YAML::Value << phase.species;
        for (const auto& prop : phase.unknown_properties)
        {
          write_property(prop.first, prop.second);
        }
        out << YAML::EndMap;
      }
      out << YAML::EndSeq;

      out << YAML::Key << "reactions" << YAML::Value << YAML::BeginSeq;

      auto serialize_reaction_component = [&](const types::ReactionComponent& component)
      {
        out << YAML::BeginMap;
        out << YAML::Key << "species_name" << YAML::Value << component.species_name;
        out << YAML::Key << "coefficient" << YAML::Value << component.coefficient;
        for (const auto& prop : component.unknown_properties)
        {
          write_property(prop.first, prop.second);
        }
        out << YAML::EndMap;
      };

      auto serialize_reaction = [&](const auto& reaction, const std::string& type)
      {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << type;
        out << YAML::Key << "name" << YAML::Value << reaction.name;
        if constexpr (has_gas_phase<decltype(reaction)>())
        {
          out << YAML::Key << "gas phase" << YAML::Value << reaction.gas_phase;
        }
        if constexpr (has_reactants<decltype(reaction)>())
        {
          out << YAML::Key << "reactants" << YAML::Value << YAML::BeginSeq;
          for (const auto& reactant : reaction.reactants)
          {
            serialize_reaction_component(reactant);
          }
          out << YAML::EndSeq;
        }
        if constexpr (has_products<decltype(reaction)>())
        {
          out << YAML::Key << "products" << YAML::Value << YAML::BeginSeq;
          for (const auto& product : reaction.products)
          {
            serialize_reaction_component(product);
          }
          out << YAML::EndSeq;
        }
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(prop.first, prop.second);
        }
        out << YAML::EndMap;
      };

      for (const auto& reaction : mechanism.reactions.arrhenius)
      {
        serialize_reaction(reaction, "ARRHENIUS");
        out << YAML::Key << "A" << YAML::Value << reaction.A;
        out << YAML::Key << "B" << YAML::Value << reaction.B;
        out << YAML::Key << "C" << YAML::Value << reaction.C;
        out << YAML::Key << "D" << YAML::Value << reaction.D;
        out << YAML::Key << "E" << YAML::Value << reaction.E;
      }

      for (const auto& reaction : mechanism.reactions.condensed_phase_arrhenius)
      {
        serialize_reaction(reaction, "CONDENSED_PHASE_ARRHENIUS");
        out << YAML::Key << "A" << YAML::Value << reaction.A;
        out << YAML::Key << "B" << YAML::Value << reaction.B;
        out << YAML::Key << "C" << YAML::Value << reaction.C;
        out << YAML::Key << "D" << YAML::Value << reaction.D;
        out << YAML::Key << "E" << YAML::Value << reaction.E;
        out << YAML::Key << "aerosol_phase" << YAML::Value << reaction.aerosol_phase;
        out << YAML::Key << "aerosol_phase_water" << YAML::Value << reaction.aerosol_phase_water;
      }

      for (const auto& reaction : mechanism.reactions.troe)
      {
        serialize_reaction(reaction, "TROE");
        out << YAML::Key << "k0_A" << YAML::Value << reaction.k0_A;
        out << YAML::Key << "k0_B" << YAML::Value << reaction.k0_B;
        out << YAML::Key << "k0_C" << YAML::Value << reaction.k0_C;
        out << YAML::Key << "kinf_A" << YAML::Value << reaction.kinf_A;
        out << YAML::Key << "kinf_B" << YAML::Value << reaction.kinf_B;
        out << YAML::Key << "kinf_C" << YAML::Value << reaction.kinf_C;
        out << YAML::Key << "Fc" << YAML::Value << reaction.Fc;
        out << YAML::Key << "N" << YAML::Value << reaction.N;
      }

      static_assert(!has_products<types::Branched>(), "has_products should be false for Branched");

      for (const auto& reaction : mechanism.reactions.branched)
      {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "BRANCHED";
        out << YAML::Key << "name" << YAML::Value << reaction.name;
        out << YAML::Key << "gas phase" << YAML::Value << reaction.gas_phase;
        out << YAML::Key << "reactants" << YAML::Value << YAML::BeginSeq;
        for (const auto& reactant : reaction.reactants)
        {
          serialize_reaction_component(reactant);
        }
        out << YAML::EndSeq;
        out << YAML::Key << "X" << YAML::Value << reaction.X;
        out << YAML::Key << "Y" << YAML::Value << reaction.Y;
        out << YAML::Key << "a0" << YAML::Value << reaction.a0;
        out << YAML::Key << "n" << YAML::Value << reaction.n;
        out << YAML::Key << "nitrate_products" << YAML::Value << YAML::BeginSeq;
        for (const auto& product : reaction.nitrate_products)
        {
          serialize_reaction_component(product);
        }
        out << YAML::EndSeq;
        out << YAML::Key << "alkoxy_products" << YAML::Value << YAML::BeginSeq;
        for (const auto& product : reaction.alkoxy_products)
        {
          serialize_reaction_component(product);
        }
        out << YAML::EndSeq;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(prop.first, prop.second);
        }
        out << YAML::EndMap;
      }

      for (const auto& reaction : mechanism.reactions.surface)
      {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "SURFACE";
        out << YAML::Key << "name" << YAML::Value << reaction.name;
        out << YAML::Key << "reaction_probability" << YAML::Value << reaction.reaction_probability;
        out << YAML::Key << "gas_phase_species" << YAML::Value << YAML::BeginMap;
        serialize_reaction_component(reaction.gas_phase_species);
        out << YAML::EndMap;
        out << YAML::Key << "gas_phase_products" << YAML::Value << YAML::BeginSeq;
        for (const auto& product : reaction.gas_phase_products)
        {
          serialize_reaction_component(product);
        }
        out << YAML::EndSeq;
        out << YAML::Key << "aerosol_phase" << YAML::Value << reaction.aerosol_phase;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(prop.first, prop.second);
        }
        out << YAML::EndMap;
      }

      for (const auto& reaction : mechanism.reactions.wet_deposition)
      {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "WET_DEPOSITION";
        out << YAML::Key << "name" << YAML::Value << reaction.name;
        out << YAML::Key << "scaling_factor" << YAML::Value << reaction.scaling_factor;
        out << YAML::Key << "aerosol_phase" << YAML::Value << reaction.aerosol_phase;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(prop.first, prop.second);
        }
        out << YAML::EndMap;
      }

      for (const auto& reaction : mechanism.reactions.henrys_law)
      {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "HENRYS_LAW";
        out << YAML::Key << "name" << YAML::Value << reaction.name;
        out << YAML::Key << "gas phase" << YAML::Value << reaction.gas_phase;
        out << YAML::Key << "gas_phase_species" << YAML::Value << reaction.gas_phase_species;
        out << YAML::Key << "aerosol_phase" << YAML::Value << reaction.aerosol_phase;
        out << YAML::Key << "aerosol_phase_species" << YAML::Value << reaction.aerosol_phase_species;
        out << YAML::Key << "aerosol_phase_water" << YAML::Value << reaction.aerosol_phase_water;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(prop.first, prop.second);
        }
        out << YAML::EndMap;
      }

      for (const auto& reaction : mechanism.reactions.tunneling)
      {
        serialize_reaction(reaction, "TUNNELING");
        out << YAML::Key << "A" << YAML::Value << reaction.A;
        out << YAML::Key << "B" << YAML::Value << reaction.B;
        out << YAML::Key << "C" << YAML::Value << reaction.C;
      }

      for (const auto& reaction : mechanism.reactions.photolysis)
      {
        serialize_reaction(reaction, "PHOTOLYSIS");
        out << YAML::Key << "scaling_factor" << YAML::Value << reaction.scaling_factor;
      }

      for (const auto& reaction : mechanism.reactions.condensed_phase_photolysis)
      {
        serialize_reaction(reaction, "CONDENSED_PHASE_PHOTOLYSIS");
        out << YAML::Key << "scaling_factor" << YAML::Value << reaction.scaling_factor_;
        out << YAML::Key << "aerosol_phase" << YAML::Value << reaction.aerosol_phase;
        out << YAML::Key << "aerosol_phase_water" << YAML::Value << reaction.aerosol_phase_water;
      }

      for (const auto& reaction : mechanism.reactions.emission)
      {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "EMISSION";
        out << YAML::Key << "name" << YAML::Value << reaction.name;
        out << YAML::Key << "scaling_factor" << YAML::Value << reaction.scaling_factor;
        out << YAML::Key << "products" << YAML::Value << YAML::BeginSeq;
        for (const auto& product : reaction.products)
        {
          serialize_reaction_component(product);
        }
        out << YAML::EndSeq;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(prop.first, prop.second);
        }
        out << YAML::EndMap;
      }

      for (const auto& reaction : mechanism.reactions.first_order_loss)
      {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "FIRST_ORDER_LOSS";
        out << YAML::Key << "name" << YAML::Value << reaction.name;
        out << YAML::Key << "scaling_factor" << YAML::Value << reaction.scaling_factor;
        out << YAML::Key << "reactants" << YAML::Value << YAML::BeginSeq;
        for (const auto& reactant : reaction.reactants)
        {
          serialize_reaction_component(reactant);
        }
        out << YAML::EndSeq;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(prop.first, prop.second);
        }
        out << YAML::EndMap;
      }

      for (const auto& reaction : mechanism.reactions.aqueous_equilibrium)
      {
        serialize_reaction(reaction, "AQUEOUS_EQUILIBRIUM");
        out << YAML::Key << "A" << YAML::Value << reaction.A;
        out << YAML::Key << "C" << YAML::Value << reaction.C;
        out << YAML::Key << "k_reverse" << YAML::Value << reaction.k_reverse;
        out << YAML::Key << "aerosol_phase" << YAML::Value << reaction.aerosol_phase;
        out << YAML::Key << "aerosol_phase_water" << YAML::Value << reaction.aerosol_phase_water;
      }

      for (const auto& reaction : mechanism.reactions.simpol_phase_transfer)
      {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "SIMPOL_PHASE_TRANSFER";
        out << YAML::Key << "name" << YAML::Value << reaction.name;
        out << YAML::Key << "gas phase" << YAML::Value << reaction.gas_phase;
        out << YAML::Key << "gas_phase_species" << YAML::Value << YAML::BeginMap;
        serialize_reaction_component(reaction.gas_phase_species);
        out << YAML::EndMap;
        out << YAML::Key << "aerosol_phase" << YAML::Value << reaction.aerosol_phase;
        out << YAML::Key << "aerosol_phase_species" << YAML::Value << YAML::BeginMap;
        serialize_reaction_component(reaction.aerosol_phase_species);
        out << YAML::EndMap;
        out << YAML::Key << "B" << YAML::Value << YAML::Flow << YAML::BeginSeq;
        for (const auto& b : reaction.B)
        {
          out << b;
        }
        out << YAML::EndSeq;
        for (const auto& prop : reaction.unknown_properties)
        {
          write_property(prop.first, prop.second);
        }
        out << YAML::EndMap;
      }

      out << YAML::EndSeq;
      out << YAML::EndMap;

      std::ofstream file(filepath);
      if (!file.is_open())
      {
        return false;
      }
      file << out.c_str();
      file.close();
      return true;
    }
  }  // namespace v1
}  // namespace mechanism_configuration
