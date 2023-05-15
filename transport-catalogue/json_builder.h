#include "json.h"

namespace json {

    class Builder {
        class BaseContext;

        class KeyContext;

        class ValueContext;

        class DictionaryContext;

        class ArrayContext;

    public:
        Builder();

        KeyContext Key(std::string key);

        Builder &Value(Node value);

        DictionaryContext StartDict();

        Builder &EndDict();

        ArrayContext StartArray();

        Builder &EndArray();

        Node Build();

    private:
        Node root_;
        std::vector<Node *> nodes_stack_;

        template<typename T>
        void InputResult(T elem) {
            if (nodes_stack_.back()->IsArray()) {
                const_cast<Array &>(nodes_stack_.back()->AsArray()).push_back(elem);
                nodes_stack_.emplace_back(&const_cast<Array &>(nodes_stack_.back()->AsArray()).back());
            } else {
                *nodes_stack_.back() = elem;
            }
        }
    };

    class Builder::BaseContext {
    public:
        BaseContext(Builder &builder) : builder_(builder) {};

        KeyContext Key(std::string key);

        Builder &Value(Node value);

        DictionaryContext StartDict();

        Builder &EndDict();

        ArrayContext StartArray();

        Builder &EndArray();

    private:
        Builder &builder_;
    };

    class Builder::KeyContext : public BaseContext {
    public:
        KeyContext(Builder &builder) : BaseContext(builder) {};

        KeyContext Key(std::string key) = delete;

        Builder &EndDict() = delete;

        Builder &EndArray() = delete;

        ValueContext Value(Node value);
    };

    class Builder::ValueContext : public BaseContext {
    public:
        ValueContext(Builder &builder) : BaseContext(builder) {};

        Builder &Value(Node value) = delete;

        DictionaryContext StartDict() = delete;

        ArrayContext StartArray() = delete;

        Builder &EndArray() = delete;
    };

    class Builder::DictionaryContext : public BaseContext {
    public:
        DictionaryContext(Builder &builder) : BaseContext(builder) {};

        DictionaryContext StartDict() = delete;

        ArrayContext StartArray() = delete;

        Builder &Value(Node value) = delete;

        Builder &EndArray() = delete;
    };

    class Builder::ArrayContext : public BaseContext {
    public:
        ArrayContext(Builder &builder) : BaseContext(builder) {};

        KeyContext Key(std::string key) = delete;

        Builder &EndDict() = delete;

        ArrayContext Value(Node value) { return BaseContext::Value(std::move(value)); }
    };
}