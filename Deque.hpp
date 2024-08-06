#ifndef DEQUE_HPP
#define DEQUE_HPP

#include <deque>
#include <initializer_list>
#include <memory>
#include <stdexcept>

template<typename T>
class Deque final {
public:
    using size_type = std::size_t;

private:
    struct Node {
        T value{};
        std::shared_ptr<Node> prev{};
        std::shared_ptr<Node> next{};

        Node()
            : value{}, next{} {
        }

        explicit Node(T input_value)
            : value{input_value}, prev{nullptr}, next{nullptr} {
        }

        explicit Node(T input_value, std::shared_ptr<Node> input_prev, std::shared_ptr<Node> input_next)
            : value{input_value}, prev{input_prev},
              next{input_next} {
        }
    };

    void range_check_exclusive(const size_type pos) const {
        if (pos >= m_size) {
            throw std::out_of_range(
                "Deque::range_check_exclusive: pos (which is " + std::to_string(pos) + ") >= this->m_size() (which is "
                +
                std::to_string(m_size) + ")");
        }
    }

    void range_check_inclusive(const size_type pos) const {
        if (pos > m_size) {
            throw std::out_of_range(
                "Deque::range_check_inclusive: pos (which is " + std::to_string(pos) + ") > this->m_size() (which is " +
                std::to_string(m_size) + ")");
        }
    }

    std::shared_ptr<Node> m_head{};
    std::shared_ptr<Node> m_tail{};
    size_type m_size{};

public:
    Deque(std::initializer_list<T> lst = {}) noexcept
        : m_head{}, m_tail{}, m_size{} {
        if (!std::empty(lst)) {
            for (const T &elem: lst) {
                this->push_back(elem);
            }
        }
    }

    Deque(const Deque<T> &deque) noexcept {
        for (std::shared_ptr<Node> current = deque.m_head; current != nullptr; current = current->next) {
            this->push_back(current->value);
        }
    }

    ~Deque() noexcept {
        clear();
    }

    Deque<T> &operator=(const Deque<T> &deque) noexcept {
        if (this == &deque) {
            return *this;
        }

        this->m_head = deque.m_head;
        this->m_tail = deque.m_tail;
        this->m_size = deque.m_size;

        return *this;
    }

    friend std::ostream &operator<<(std::ostream &out, const Deque<T> &deque) {
        if (deque.m_size == 0) {
            return out;
        }

        for (std::shared_ptr<Node> current = deque.m_head; current != nullptr; current = current->next) {
            out << current->value << (current->next ? ", " : ".");
        }

        return out;
    }

    T &operator[](const size_type pos) {
        this->range_check_exclusive(pos);

        if (pos <= m_size - pos) {
            std::shared_ptr<Node> current{m_head};
            for (size_type i = 0; i < pos; ++i) {
                current = current->next;
            }

            return current->value;
        } else {
            std::shared_ptr<Node> current{m_tail};
            for (size_type i = 0; i < m_size - pos - 1; ++i) {
                current = current->prev;
            }

            return current->value;
        }
    }

    const T &operator[](const size_type pos) const {
        this->range_check_exclusive(pos);

        if (pos <= m_size - pos) {
            std::shared_ptr<Node> current{m_head};
            for (size_type i = 0; i < pos; ++i) {
                current = current->next;
            }

            return current->value;
        } else {
            std::shared_ptr<Node> current{m_tail};
            for (size_type i = 0; i < m_size - pos - 1; ++i) {
                current = current->prev;
            }

            return current->value;
        }
    }

    [[nodiscard]] bool empty() const {
        return m_size == 0;
    }

    [[nodiscard]] size_type size() const {
        return m_size;
    }

    const T &back() const {
        return m_tail->value;
    }

    const T &front() const {
        return m_head->value;
    }

    void clear() noexcept {
        m_head = nullptr;
        m_tail = nullptr;
        m_size = 0;
    }

    void push_back(const T &value) noexcept {
        std::shared_ptr<Node> temp{std::make_shared<Node>(value, m_tail, nullptr)};

        if (m_size == 0) {
            m_head = temp;
        } else {
            m_tail->next = temp;
        }

        m_tail = temp;
        m_size += 1;
    }

    void pop_back() noexcept {
        std::shared_ptr<Node> temp{m_tail->prev};

        if (m_size == 1) {
            m_head = nullptr;
        } else {
            temp->next = nullptr;
        }

        m_tail = temp;
        m_size -= 1;
    }

    void push_front(const T &value) noexcept {
        std::shared_ptr<Node> temp{std::make_shared<Node>(value, nullptr, m_head)};

        if (m_size == 0) {
            m_tail = temp;
        } else {
            m_head->prev = temp;
        }

        m_head = temp;
        m_size += 1;
    }

    void pop_front() noexcept {
        std::shared_ptr<Node> temp{m_head->next};

        if (m_size == 1) {
            m_tail = nullptr;
        } else {
            temp->prev = nullptr;
        }

        m_head = temp;
        m_size -= 1;
    }

    size_type erase(const size_type pos) {
        this->range_check_exclusive(pos);

        if (pos <= m_size - pos) {
            std::shared_ptr<Node> dummy{std::make_shared<Node>(0, nullptr, m_head)};
            std::shared_ptr<Node> current{dummy};

            for (size_type i = 0; i < pos; ++i) {
                current = current->next;
            }

            current->next = current->next->next;
            current->next->prev = current;
            dummy->next->prev = nullptr;
            m_head = dummy->next;
        } else {
            std::shared_ptr<Node> dummy{std::make_shared<Node>(0, m_tail, nullptr)};
            std::shared_ptr<Node> current{dummy};

            for (size_type i = pos; i < m_size - 1; ++i) {
                current = current->prev;
            }

            current->prev = current->prev->prev;
            current->prev->next = current;
            dummy->prev->next = nullptr;
            m_tail = dummy->prev;
        }

        return pos;
    }

    size_type erase(const size_type first, const size_type last) {
        this->range_check_exclusive(first);

        for (size_type i = first; i <= last; ++i) {
            erase(first);
        }

        return first;
    }

    size_type insert(const size_type pos, const T &value) {
        this->range_check_inclusive(pos);

        if (pos <= m_size - pos) {
            std::shared_ptr<Node> dummy{std::make_shared<Node>(0, nullptr, m_head)};
            std::shared_ptr<Node> current{dummy};

            for (size_type i = 0; i < pos; ++i) {
                current = current->next;
            }

            std::shared_ptr<Node> temp{std::make_shared<Node>(value, current, current->next)};
            temp->prev->next = temp;
            temp->next->prev = temp;
            dummy->next->prev = nullptr;
            m_head = dummy->next;
        } else {
            std::shared_ptr<Node> dummy{std::make_shared<Node>(0, m_tail, nullptr)};
            std::shared_ptr<Node> current{dummy};

            for (size_type i = pos; i < m_size; ++i) {
                current = current->prev;
            }

            std::shared_ptr<Node> temp{std::make_shared<Node>(value, current->prev, current)};
            temp->prev->next = temp;
            temp->next->prev = temp;
            dummy->prev->next = nullptr;
            m_tail = dummy->prev;
        }

        return pos;
    }

    size_type insert(const size_type pos, const size_type count, const T &value) {
        this->range_check_inclusive(pos);

        for (size_type i = 0; i < count; ++i) {
            this->insert(pos, value);
        }

        return pos;
    }

    size_type insert(const size_type pos, std::initializer_list<T> arr) {
        this->range_check_inclusive(pos);

        for (const T &elem: arr) {
            this->insert(pos, elem);
        }

        return pos;
    }
};

#endif
