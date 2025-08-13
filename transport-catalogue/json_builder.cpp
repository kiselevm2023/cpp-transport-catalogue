 #include "json_builder.h"

#include <stdexcept>

namespace json {

Builder::Builder() : root_(), nodes_stack_{&root_} {}

Builder::DictContext Builder::StartDict() {
    SetNode(Dict{}, false);
    return *this;
}
Builder& Builder::EndDict() {
    if (!GetNode()->IsMap()) {
        throw(std::logic_error("EndDict() outside a dict"));
    }
    nodes_stack_.pop_back();
    return *this;
}

Builder::ArrayContext Builder::StartArray() {
    SetNode(Array{}, false);
    return *this;
}
Builder& Builder::EndArray() {
    if (!GetNode()->IsArray()) {
        throw(std::logic_error("EndArray() outside a array"));
    }
    nodes_stack_.pop_back();
    return *this;
}

Builder::KeyContext Builder::Key(std::string key) {
    if (!GetNode()->IsMap()) {
        throw(std::logic_error("Key() outside a dict"));
    }
    Dict& cur_node = const_cast<Dict&>(GetNode()->AsMap());
    nodes_stack_.push_back(&cur_node[std::move(key)]);
    return *this;
}

Builder& Builder::Value(Node value) {
    SetNode(std::move(value), true);
    return *this;
}

const Node& Builder::Build() {
    if (!nodes_stack_.empty()) {
        throw(std::logic_error("JSON is empty"));
    }
    return std::move(root_);
}

const Node* Builder::GetNode() const {
    if (nodes_stack_.empty()) {
        throw(std::logic_error("Empty stack"));
    }
    return nodes_stack_.back();
}

void Builder::SetNode(Node node, bool one_shot) {
    if (GetNode()->IsArray()) {
        Array& cur_node = const_cast<Array&>(GetNode()->AsArray());
        cur_node.emplace_back(node);
        if (!one_shot) {
            nodes_stack_.push_back(&cur_node.back());
        }
    } else if (GetNode()->IsNull()) {
        Node* cur_value = const_cast<Node*>(GetNode());
        *cur_value = std::move(node);
        if (one_shot) {
            nodes_stack_.pop_back();
        }
    } else {
        throw std::logic_error("Bad request");
    }
}

} // namespace json