/**============================================================================
Name        : Interpreter_DSL.cpp
Created on  :
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : DSL Interpreter
===========================================================================**/

#include "Interpreter.hpp"

#include <cctype>
#include <cstdint>
#include <expected>
#include <iostream>
#include <memory>
#include <string_view>
#include <variant>
#include <vector>

namespace
{

    constexpr uint64_t Mhz = 1'000'000;
    constexpr uint64_t Ghz = 1'000'000'000;

    constexpr double Db = 1.0;
    constexpr double NumberBase = 10.0;
    constexpr double InitialNumberValue = 0.0;
    constexpr double InitialDivisor = 1.0;

    enum class Error: uint8_t {
        Success,
        UnexpectedToken,
        UnexpectedEnd,
        InvalidNumber,
        InvalidUnit,
        InvalidIdentifier,
        InvalidOperator
    };

    enum class Field: uint8_t {
        Frequency,
        Attenuation
    };

    enum class Operator: uint8_t {
        Equal,
        NotEqual,
        Less,
        LessOrEqual,
        Greater,
        GreaterOrEqual
    };

    enum class LogicalOperator: uint8_t {
        And,
        Or
    };

    using NodeId = std::size_t;

    struct Comparison
    {
        Field field;
        Operator operator_;
        double value;
    };

    struct LogicalExpression
    {
        LogicalOperator operator_;
        NodeId left;
        NodeId right;
    };

    using Node = std::variant<Comparison, LogicalExpression>;

    class Expression
    {
    public:

        [[nodiscard]] NodeId addComparison(const Comparison comparison)
        {
            nodes_.emplace_back(comparison);
            return nodes_.size() - 1;
        }

        [[nodiscard]] NodeId addLogicalExpression(const LogicalExpression expression)
        {
            nodes_.emplace_back(expression);
            return nodes_.size() - 1;
        }

        [[nodiscard]] const Node& get(const NodeId id) const noexcept
        {
            return nodes_[id];
        }

    private:

        std::vector<Node> nodes_;
    };

    struct DeviceState
    {
        uint64_t frequency { 0 };
        double attenuation { 0.0 };
    };

    class Parser
    {
    public:

        explicit Parser(const std::string_view input) noexcept : input_ { input } {
        }

        [[nodiscard]] std::expected<Expression, Error> parse()
        {
            Expression expression;

            skipSpaces();

            const auto root = parseExpression(expression);

            if (!root)
            {
                return std::unexpected(root.error());
            }

            skipSpaces();

            if (position_ != input_.size())
            {
                return std::unexpected(Error::UnexpectedToken);
            }

            root_ = *root;

            return expression;
        }

        [[nodiscard]] NodeId root() const noexcept
        {
            return root_;
        }

    private:

        [[nodiscard]] std::expected<NodeId, Error> parseExpression(Expression& expression)
        {
            auto left = parseComparison(expression);

            if (!left)
            {
                return left;
            }

            while (true)
            {
                skipSpaces();

                const auto logicalOperator = parseLogicalOperator();

                if (!logicalOperator)
                {
                    return left;
                }

                auto right = parseComparison(expression);

                if (!right)
                {
                    return right;
                }

                left = expression.addLogicalExpression(
                    LogicalExpression{
                        .operator_ = *logicalOperator,
                        .left = *left,
                        .right = *right
                    });
            }
        }

        [[nodiscard]] std::expected<NodeId, Error> parseComparison(Expression& expression)
        {
            skipSpaces();

            const auto field = parseField();

            if (!field)
            {
                return std::unexpected(field.error());
            }

            skipSpaces();

            const auto operator_ = parseOperator();

            if (!operator_)
            {
                return std::unexpected(operator_.error());
            }

            skipSpaces();

            const auto value = parseValue(*field);

            if (!value)
            {
                return std::unexpected(value.error());
            }

            return expression.addComparison(
                Comparison{
                    .field = *field,
                    .operator_ = *operator_,
                    .value = *value
                });
        }

        [[nodiscard]] std::expected<Field, Error> parseField() noexcept
        {
            if (match("frequency"))
            {
                return Field::Frequency;
            }

            if (match("attenuation"))
            {
                return Field::Attenuation;
            }

            return std::unexpected(Error::InvalidIdentifier);
        }

        [[nodiscard]] std::expected<Operator, Error> parseOperator() noexcept
        {
            if (match(">="))
            {
                return Operator::GreaterOrEqual;
            }

            if (match("<="))
            {
                return Operator::LessOrEqual;
            }

            if (match("!="))
            {
                return Operator::NotEqual;
            }

            if (match("=="))
            {
                return Operator::Equal;
            }

            if (match(">"))
            {
                return Operator::Greater;
            }

            if (match("<"))
            {
                return Operator::Less;
            }

            return std::unexpected(Error::InvalidOperator);
        }

        [[nodiscard]] std::expected<LogicalOperator, Error> parseLogicalOperator() noexcept
        {
            if (match("AND"))
            {
                return LogicalOperator::And;
            }

            if (match("OR"))
            {
                return LogicalOperator::Or;
            }

            return std::unexpected(Error::UnexpectedToken);
        }

        [[nodiscard]] std::expected<double, Error> parseValue(const Field field)
        {
            skipSpaces();

            const auto begin = position_;

            while (position_ < input_.size() &&
                   (std::isdigit(static_cast<unsigned char>(input_[position_])) != 0 ||
                    input_[position_] == '.'))
            {
                ++position_;
            }

            if (begin == position_)
            {
                return std::unexpected(Error::InvalidNumber);
            }

            const auto number = parseNumber(begin, position_);

            if (!number)
            {
                return std::unexpected(Error::InvalidNumber);
            }

            const auto unitBegin = position_;

            while (position_ < input_.size() &&
                   std::isalpha(static_cast<unsigned char>(input_[position_])) != 0)
            {
                ++position_;
            }

            const auto unit = input_.substr(unitBegin, position_ - unitBegin);

            if (field == Field::Frequency)
            {
                if (unit == "MHz")
                {
                    return *number * static_cast<double>(Mhz);
                }

                if (unit == "GHz")
                {
                    return *number * static_cast<double>(Ghz);
                }

                return std::unexpected(Error::InvalidUnit);
            }

            if (field == Field::Attenuation && unit == "dB")
            {
                return *number * Db;
            }

            return std::unexpected(Error::InvalidUnit);
        }

        [[nodiscard]] std::expected<double, Error> parseNumber(
            const std::size_t begin,
            const std::size_t end) const noexcept
        {
            double value = InitialNumberValue;
            double divisor = InitialDivisor;
            bool fractional = false;

            for (std::size_t index = begin; index < end; ++index)
            {
                const char character = input_[index];

                if (character == '.')
                {
                    fractional = true;
                    continue;
                }

                const auto digit = static_cast<double>(character - '0');

                if (!fractional)
                {
                    value = value * NumberBase + digit;
                }
                else
                {
                    divisor *= NumberBase;
                    value += digit / divisor;
                }
            }

            return value;
        }

        [[nodiscard]] bool match(const std::string_view token) noexcept
        {
            skipSpaces();

            if (input_.substr(position_, token.size()) != token)
            {
                return false;
            }

            position_ += token.size();

            return true;
        }

        void skipSpaces() noexcept
        {
            while (position_ < input_.size() &&
                   std::isspace(static_cast<unsigned char>(input_[position_])) != 0)
            {
                ++position_;
            }
        }

        std::string_view input_;
        std::size_t position_ { 0 };
        NodeId root_ { 0 };
    };

    class Interpreter
    {
    public:

        [[nodiscard]]
        constexpr bool evaluate(const Expression& expression,
            const NodeId root,
            const DeviceState& state) const noexcept
        {
            return evaluateNode(expression, root, state);
        }

    private:

        [[nodiscard]]
        constexpr bool evaluateNode(const Expression& expression,
                                    const NodeId nodeId,
                                    const DeviceState& state) const noexcept
        {
            return std::visit([this, &expression, &state](const auto& node){
                return evaluateNode(expression, node, state);
            }, expression.get(nodeId));
        }

        [[nodiscard]]
        static constexpr bool evaluateNode(const Expression&,
                                           const Comparison& comparison,
                                           const DeviceState& state) noexcept
        {
            const double actualValue = getValue(comparison.field, state);
            switch (comparison.operator_)
            {
                case Operator::Equal:
                    return actualValue == comparison.value;
                case Operator::NotEqual:
                    return actualValue != comparison.value;
                case Operator::Less:
                    return actualValue < comparison.value;
                case Operator::LessOrEqual:
                    return actualValue <= comparison.value;
                case Operator::Greater:
                    return actualValue > comparison.value;
                case Operator::GreaterOrEqual:
                    return actualValue >= comparison.value;
            }

            return false;
        }

        [[nodiscard]]
        constexpr bool evaluateNode(const Expression& expression,
                                    const LogicalExpression& logicalExpression,
                                    const DeviceState& state) const noexcept
        {
            const bool left = evaluateNode(expression, logicalExpression.left, state);
            if (logicalExpression.operator_ == LogicalOperator::And){
                return left && evaluateNode(expression, logicalExpression.right, state);
            }

            return left || evaluateNode(expression, logicalExpression.right, state);
        }

        [[nodiscard]]
        static constexpr double getValue(const Field field,
                                         const DeviceState& state) noexcept
        {
            switch (field)
            {
                case Field::Frequency:
                    return static_cast<double>(state.frequency);
                case Field::Attenuation:
                    return state.attenuation;
            }

            return 0.0;
        }
    };

    [[nodiscard]]
    constexpr std::string_view errorToString(const Error error) noexcept
    {
        switch (error)
        {
            case Error::Success:
                return "Success";
            case Error::UnexpectedToken:
                return "UnexpectedToken";
            case Error::UnexpectedEnd:
                return "UnexpectedEnd";
            case Error::InvalidNumber:
                return "InvalidNumber";
            case Error::InvalidUnit:
                return "InvalidUnit";
            case Error::InvalidIdentifier:
                return "InvalidIdentifier";
            case Error::InvalidOperator:
                return "InvalidOperator";
        }
        return "Unknown";
    }
}


void interpreter::interpreter_dsl::TestAll()
{
    constexpr std::string_view Dsl = "frequency > 900MHz AND attenuation < 10dB";

    Parser parser { Dsl };
    auto expression = parser.parse();
    if (!expression){
        std::cout << "Parse failed: " << errorToString(expression.error()) << '\n';
        return;
    }

    constexpr DeviceState state {
        .frequency = 915'000'000,
        .attenuation = 5.0
    };

    const Interpreter interpreter;
    const bool result = interpreter.evaluate(*expression, parser.root(), state);

    std::cout << "DSL: " << Dsl << '\n';
    std::cout << "Result: " << std::boolalpha << result << '\n';
}