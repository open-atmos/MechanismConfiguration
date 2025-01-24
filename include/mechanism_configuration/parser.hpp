#pragma once

#include <mechanism_configuration/parser_base.hpp>
#include <mechanism_configuration/load_node.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <memory>
#include <vector>

namespace mechanism_configuration
{
  class UniversalParser
  {
   public:
    UniversalParser() {
      RegisterParser(std::make_unique<ParserWrapperImpl<v0::types::Mechanism>>(
          std::make_unique<v0::Parser>()));
      RegisterParser(std::make_unique<ParserWrapperImpl<v1::types::Mechanism>>(
          std::make_unique<v1::Parser>()));
    }

    /// @brief Attempts to parse the input using the registered parsers
    /// @param source A YAML node, file path, or string representing a file path
    /// @return A unique pointer to the parsed mechanism, or nullptr if no parser succeeded
    template<typename T>
    std::optional<std::unique_ptr<GlobalMechanism>> Parse(const T& source)
    {
      YAML::Node node = LoadNode(source);

      for (const auto& parser : parsers_)
      {
        auto result = parser->TryParse(node);
        if (result)
        {
          return result;
        }
      }
      return nullptr;
    }

   private:
    // Type-erased wrapper for Parsers of different mechanism types
    class ParserWrapper
    {
     public:
      virtual ~ParserWrapper() = default;
      virtual std::optional<std::unique_ptr<GlobalMechanism>> TryParse(const YAML::Node& node) = 0;
    };

    template<typename MechanismType>
    class ParserWrapperImpl : public ParserWrapper
    {
     public:
      explicit ParserWrapperImpl(std::unique_ptr<::mechanism_configuration::ParserBase<MechanismType>> parser)
          : parser_(std::move(parser)) {}

      std::optional<std::unique_ptr<GlobalMechanism>> TryParse(const YAML::Node& node) override
      {
        return parser_->TryParse(node);
      }

     private:
      std::unique_ptr<::mechanism_configuration::ParserBase<MechanismType>> parser_;
    };

    /// @brief Registers a new parser
    /// @param parser A unique pointer to a parser
    void RegisterParser(std::unique_ptr<ParserWrapper> parser)
    {
      parsers_.emplace_back(std::move(parser));
    }

    std::vector<std::unique_ptr<ParserWrapper>> parsers_;
  };
}  // namespace mechanism_configuration
