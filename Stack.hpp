#ifndef STACK_HPP
#define STACK_HPP

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <memory>

template<typename T>
class Stack final {
private:
    struct Node {
        T value{};
        std::shared_ptr<Node> next{};

        Node() : value{}, next{} {
        }

        explicit Node(T input_value)
            : value{input_value}, next{nullptr} {
        }

        explicit Node(T input_value, std::shared_ptr<Node> input_next) : value{input_value}, next{input_next} {
        }
    };

    std::shared_ptr<Node> m_head{};
    size_t m_size{};

public:
    Stack(std::initializer_list<T> arr = {}) noexcept
        : m_head{}, m_size{} {
        if (!std::empty(arr)) {
            for (const T &elem: arr) {
                this->push(elem);
            }
        }
    }

    Stack(const Stack<T> &stack) noexcept {
        std::shared_ptr<Node> dummy{std::make_shared<Node>()};
        std::shared_ptr<Node> temp{dummy};

        for (std::shared_ptr<Node> current = stack.m_head; current != nullptr; current = current->next) {
            temp->next = std::make_shared<Node>(current->value);
            temp = temp->next;
        }

        m_head = dummy->next;
        m_size = stack.m_size;
    }

    ~Stack() noexcept {
        clear();
    }

    Stack<T> &operator=(const Stack<T> &stack) noexcept {
        if (this == &stack) {
            return *this;
        }

        this->m_head = stack.m_head;
        this->m_size = stack.m_size;

        return *this;
    }

    const T &top() const noexcept {
        return m_head->value;
    }

    [[nodiscard]] bool empty() const noexcept {
        return m_head == nullptr;
    }

    [[nodiscard]] size_t size() const noexcept {
        return m_size;
    }

    void push(const T &value) noexcept {
        std::shared_ptr<Node> current{std::make_shared<Node>(value, m_head)};

        m_head = current;
        m_size += 1;
    }

    void pop() noexcept {
        if (!m_head) {
            return;
        }

        std::shared_ptr<Node> current{m_head->next};

        m_head = current;
        m_size -= 1;
    }

    void clear() noexcept {
        m_head = nullptr;
        m_size = 0;
    }

    void print() const noexcept {
        if (!m_head) {
            return;
        }

        for (std::shared_ptr<Node> current = m_head; current != nullptr; current = current->next) {
            std::cout << current->value << (current->next ? ", " : ".\n");
        }
    }
};

#endif
