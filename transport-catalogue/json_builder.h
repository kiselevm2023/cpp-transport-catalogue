 #pragma once

#include "json.h"

#include <string>
#include <iostream>
#include <vector>
#include <cassert>
#include <utility>

namespace json {

class Builder {
private:
    class DictContext;
    class KeyContext;
    class ArrayContext;

public:
    Builder();

    DictContext StartDict();
    Builder& EndDict();

    ArrayContext StartArray();
    Builder& EndArray();

    KeyContext Key(std::string key);

    Builder& Value(Node value);

    const Node& Build();

private:
    Node root_;
    std::vector<Node*> nodes_stack_;

    const Node* GetNode() const;
    void SetNode(Node node, bool one_shot);

    class ContextManager {
    public:
        ContextManager(Builder& builder) : builder_(builder) {}

        DictContext StartDict() {
            builder_.StartDict();
            return builder_;
        }
        Builder& EndDict() {
            builder_.EndDict();
            return builder_;
        }

        ArrayContext StartArray() {
            builder_.StartArray();
            return builder_;
        }
        Builder& EndArray() {
            builder_.EndArray();
            return builder_;
        }

        KeyContext Key(std::string key) {
            builder_.Key(std::move(key));
            return builder_;
        }
        Builder& Value(Node value) {
            builder_.Value(std::move(value));
            return builder_;
        }
    private:
        Builder& builder_;
    };

    class KeyContext : public ContextManager {
    public:
        KeyContext(Builder& builder) : ContextManager(builder) {}

        Builder& EndDict() = delete;
        Builder& EndArray() = delete;
        KeyContext Key(std::string key) = delete;

        DictContext Value(Node value) {
            return ContextManager::Value(std::move(value));
        }
    };

    class DictContext : public ContextManager {
    public:
        DictContext(Builder& builder) : ContextManager(builder) {}

        ArrayContext StartArray() = delete;
        Builder& EndArray() = delete;
        DictContext StartDict() = delete;
        Builder& Value(Node value) = delete;
    };

    class ArrayContext : public ContextManager {
    public:
        ArrayContext(Builder& builder) : ContextManager(builder) {}

        Builder& EndDict() = delete;
        KeyContext Key(std::string key) = delete;

        ArrayContext Value(Node value) {
            return ContextManager::Value(std::move(value));
        }
    };
};

} // namespace json