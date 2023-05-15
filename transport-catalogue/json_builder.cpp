#include <utility>

#include "json_builder.h"

namespace json {

    Builder::Builder() {
        nodes_stack_.emplace_back(&root_);
    }

    Builder::KeyContext Builder::Key(std::string key) {
        if (!(!nodes_stack_.empty() && nodes_stack_.back()->IsDict())) {
            throw std::logic_error("Key outside the dictionary");
        }
        nodes_stack_.emplace_back(&const_cast<Dict &>(nodes_stack_.back()->AsDict())[key]);
        return *this;
    }

    Builder &Builder::Value(Node value) {
        if (nodes_stack_.empty() || (!nodes_stack_.back()->IsNull() && !nodes_stack_.back()->IsArray())) {
            throw std::logic_error("Value error");
        }
        if (nodes_stack_.back()->IsArray()) {
            const_cast<Array &>(nodes_stack_.back()->AsArray()).emplace_back(value);
        } else {
            *nodes_stack_.back() = value;
            nodes_stack_.pop_back();
        }
        return *this;
    }

    Builder::DictionaryContext Builder::StartDict() {
        if (nodes_stack_.empty() || (!nodes_stack_.back()->IsNull() && !nodes_stack_.back()->IsArray())) {
            throw std::logic_error("StartDict error");
        }
        InputResult(Dict());
        return *this;
    }

    Builder::ArrayContext Builder::StartArray() {
        if (nodes_stack_.empty() || (!nodes_stack_.back()->IsNull() && !nodes_stack_.back()->IsArray())) {
            throw std::logic_error("StartArray error");
        }
        InputResult(Array());
        return *this;
    }

    Builder &Builder::EndDict() {
        if (!(!nodes_stack_.empty() && nodes_stack_.back()->IsDict())) {
            throw std::logic_error("EndDict outside the dictionary");
        }
        nodes_stack_.pop_back();
        return *this;
    }

    Builder &Builder::EndArray() {
        if (!(!nodes_stack_.empty() && nodes_stack_.back()->IsArray())) {
            throw std::logic_error("EndArray outside the array");
        }
        nodes_stack_.pop_back();
        return *this;
    }

    Node Builder::Build() {
        if (!nodes_stack_.empty()) {
            throw std::logic_error("Object haven't build");
        }
        return root_;
    }

    Builder::KeyContext Builder::BaseContext::Key(std::string key) {
        return builder_.Key(std::move(key));
    }

    Builder &Builder::BaseContext::Value(Node value) {
        return builder_.Value(std::move(value));
    }

    Builder::DictionaryContext Builder::BaseContext::StartDict() {
        return builder_.StartDict();
    }

    Builder::ArrayContext Builder::BaseContext::StartArray() {
        return builder_.StartArray();
    }

    Builder &Builder::BaseContext::EndDict() {
        return builder_.EndDict();
    }

    Builder &Builder::BaseContext::EndArray() {
        return builder_.EndArray();
    }

    Builder::ValueContext Builder::KeyContext::Value(Node value) {
        return BaseContext::Value(std::move(value));
    }

}