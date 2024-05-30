#pragma once

#include <charconv>
#include <cmath>
#include <lexer.hpp>

namespace calc {

template <class F>
struct Node {
    ~Node() {
        switch (type) {
            case Type::Number:
            case Type::Error:
                return;
            case Type::Positive:
            case Type::Negative: {
                delete unary.operand;
            }
            default: {
                delete binary.left;
                delete binary.right;
            }
        }
    }

    enum class Type {
        Error,
        Number,
        Positive,
        Negative,
        Add,
        Sub,
        Mul,
        Div,
        Pow,
    } type;

    struct Unary {
        Node* operand;
    };

    struct Binary {
        Node *left, *right;
    };

    union {
        F value;
        Unary unary;
        Binary binary;
    };
};

template <class F, class It>
constexpr auto parse_number(const Token<It>& token) -> Node<F>* {
    using Node = Node<F>;

    F value{};
    std::from_chars(token.lexeme_start, token.lexeme_end, value);

    return new Node{
        .type = Node::Type::Number,
        .value = value,
    };
}

template <class F, class It>
constexpr auto parse_prefix(Node<F>* left, It begin, It end) -> Node<F>* {
    const auto token = next_token(begin, end);

    if (token.type == Token<It>::Type::Error) {
        return new Node<F>{
            .type = Node<F>::Type::Error,
        };
    }

    switch (token.type) {
        case Token<It>::Type::Number:
            return parse_number<F>(token);
    }
}

// template <class F, class It>
// constexpr auto parse_infix(Node<F>* left, It begin, It end) {
//     const auto token = next_token(begin, end);
//
//     switch (token.type) {
//         case Token<It>::Type::Plus:
//             return new Node<F>{.type=Node<F>::Type::Add,
//             .left=left,.right=parse()}
//     }
//
// }

template <class F, class It>
constexpr auto parse(Node<F>* root, It begin, It end) -> Node<F>* {
    Node<F>* left = parse_infix(root);
}

template <class F>
constexpr auto evaluate(Node<F>* node) -> F {
    using Type = Node<F>::Type;

    switch (node->type) {
        case Type::Number:
            return node->value;
        case Type::Positive:
            return evaluate(node->unary.operand);
        case Type::Negative:
            return -evaluate(node->unary.operand);
        case Type::Add:
            return evaluate(node->binary.left) + evaluate(node->binary.right);
        case Type::Sub:
            return evaluate(node->binary.left) - evaluate(node->binary.right);
        case Type::Div:
            return evaluate(node->binary.left) / evaluate(node->binary.right);
        case Type::Mul:
            return evaluate(node->binary.left) * evaluate(node->binary.right);
        case Type::Pow:
            return std::pow((node->binary.left), evaluate(node->binary.right));
    }
}

}  // namespace calc
