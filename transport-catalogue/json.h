#pragma once

#include <map>
#include <string>
#include <vector>
#include <variant>
#include <iostream>

namespace transport_catalogue::json {

    class Node;

    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using Number = std::variant<int, double>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node : private std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict> {
    public:
        using variant::variant;

        using Value = variant;

        const Array &AsArray() const;

        const Dict &AsDict() const;

        int AsInt() const;

        double AsDouble() const;

        bool AsBool() const;

        const std::string &AsString() const;


        bool IsNull() const;

        bool IsInt() const;

        bool IsDouble() const;

        bool IsRealDouble() const;

        bool IsBool() const;

        bool IsString() const;

        bool IsArray() const;

        bool IsDict() const;

        Value& GetValue();

        const Value &GetValue() const;

    };

    bool operator==(const Node& lhs, const Node& rhs);
    bool operator!=(const Node& lhs, const Node& rhs);

    class Document {
    public:
        Document() = default;
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root_;
    };

    bool operator==(const Document& lhs, const Document& rhs);
    bool operator!=(const Document& lhs, const Document& rhs);

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);
}