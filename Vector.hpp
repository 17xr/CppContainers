#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cstddef>
#include <memory>
#include <initializer_list>
#include <stdexcept>

template<typename T>
class Vector final {
public:
    using size_type = std::size_t;

private:
    std::allocator<T> m_allocator{};
    std::allocator_traits<decltype(m_allocator)> m_traits{};

    T *m_data{};
    size_type m_capacity{};
    size_type m_size{};

    void range_check_exclusive(const size_type pos) const {
        if (pos >= m_size) {
            throw std::out_of_range(
                "Vector::range_check_exclusive: pos (which is " + std::to_string(pos) + ") >= this->m_size() (which is "
                +
                std::to_string(m_size) + ")");
        }
    }

    void range_check_inclusive(const size_type pos) const {
        if (pos > m_size) {
            throw std::out_of_range(
                "Vector::range_check_inclusive: pos (which is " + std::to_string(pos) + ") > this->m_size() (which is "
                +
                std::to_string(m_size) + ")");
        }
    }

    void resize(size_t new_capacity) {
        T *new_data = m_traits.allocate(m_allocator, new_capacity);
        m_traits.construct(m_allocator, new_data, m_size);

        for (size_type i = 0; i < m_size; ++i) {
            new_data[i] = m_data[i];
        }

        m_traits.deallocate(m_allocator, m_data, m_size);
        m_traits.destroy(m_allocator, m_data);

        m_data = new_data;
        m_capacity = new_capacity;
    }

public:
    Vector(std::initializer_list<T> init = {}) {
        if (!std::empty(init)) {
            for (const T &element: init) {
                this->push_back(element);
            }
        }
    }

    Vector(const Vector &vector) {
        if (!std::empty(vector)) {
            for (const T &element: vector) {
                this->push_back(element);
            }
        }
    }

    ~Vector() {
        this->clear();
    }

    Vector &operator=(const Vector &vector) {
        if (this == &vector) {
            return *this;
        }

        m_capacity = vector.m_capacity;
        m_data = vector.m_data;
        m_size = vector.m_size;

        return *this;
    }

    T &operator[](size_type pos) {
        this->range_check_exclusive(pos);

        return m_data[pos];
    }

    const T &operator[](size_type pos) const {
        this->range_check_exclusive(pos);

        return m_data[pos];
    }

    [[nodiscard]] bool empty() const {
        return m_size == 0;
    }

    [[nodiscard]] size_type size() const {
        return m_size;
    }

    [[nodiscard]] size_type capacity() const {
        return m_capacity;
    }

    void reserve(const size_type new_capacity) {
        if (new_capacity > m_capacity) {
            this->resize(new_capacity);
        }
    }

    void shrink_to_fit() {
        if (m_capacity > m_size) {
            this->resize(m_size);
        }
    }

    void push_back(const T &value) {
        if (m_capacity == 0) {
            this->resize(1);
        }

        if (m_size >= m_capacity) {
            this->resize(m_capacity * 2);
        }

        m_data[m_size++] = value;
    }

    void pop_back() {
        if (m_size > 0) {
            --m_size;
        }
    }

    void clear() {
        m_traits.deallocate(m_allocator, m_data, m_size);
        m_traits.destroy(m_allocator, m_data);

        m_capacity = 0;
        m_data = nullptr;
        m_size = 0;
    }
};

#endif
