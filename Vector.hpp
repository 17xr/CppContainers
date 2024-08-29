#ifndef VECTOR_HPP
#define VECTOR_HPP
#include <vector>

// TODO: overloading comparison operators

template<typename T>
class Vector final {
public:
    using size_type = size_t;

private:
    std::allocator<T> m_allocator{};
    std::allocator_traits<std::allocator<T> > m_traits{};

    T *m_data{};
    size_type m_capacity{};
    size_type m_size{};

    void range_check_exclusive(const size_type pos, const std::string &placeholder) const {
        if (pos >= m_size) {
            throw std::out_of_range(
                "Vector::range_check_exclusive: " + placeholder + " (which is " + std::to_string(pos) +
                ") >= this->m_size (which is " + std::to_string(m_size) + ")");
        }
    }

    void range_check_inclusive(const size_type pos, const std::string &placeholder) const {
        if (pos > m_size) {
            throw std::out_of_range(
                "Vector::range_check_inclusive: " + placeholder + " (which is " + std::to_string(pos) +
                ") > this->m_size (which is " + std::to_string(m_size) + ")");
        }
    }

    void range_check_order(const size_type first, const size_type last) const {
        if (first > last) {
            throw std::out_of_range(
                "Vector::range_check_order: first (which is " + std::to_string(first) +
                ") > last (which is " + std::to_string(last) + ")");
        }
    }

    void resize(size_type new_capacity) {
        T *new_data = m_traits.allocate(m_allocator, new_capacity);
        m_traits.construct(m_allocator, new_data, m_size);

        for (size_type i = 0; i < m_size; i++) {
            new_data[i] = m_data[i];
        }

        m_traits.destroy(m_allocator, m_data);
        m_traits.deallocate(m_allocator, m_data, m_size);

        m_data = new_data;
        m_capacity = new_capacity;
    }

public:
    Vector(std::initializer_list<T> init = {}) noexcept {
        if (!std::empty(init)) {
            for (const T &element: init) {
                this->push_back(element);
            }
        }
    }

    Vector(const Vector &vector) noexcept {
        if (!std::empty(vector)) {
            for (size_type i = 0; i < vector.m_size; ++i) {
                this->push_back(vector.m_data[i]);
            }
        }
    }

    ~Vector() noexcept {
        this->clear();
    }

    Vector &operator=(const Vector &vector) noexcept {
        if (this == &vector) {
            return *this;
        }

        this->m_data = vector.m_data;
        this->m_capacity = vector.m_capacity;
        this->m_size = vector.m_size;

        return *this;
    }

    T &operator[](size_type pos) {
        this->range_check_exclusive(pos, "pos");

        return m_data[pos];
    }

    const T &operator[](size_type pos) const {
        this->range_check_exclusive(pos, "pos");

        return m_data[pos];
    }

    const T &front() const {
        this->range_check_exclusive(0, "pos");

        return m_data[0];
    }

    const T &back() const {
        this->range_check_exclusive(0, "pos");

        return m_data[m_size - 1];
    }

    T *data() noexcept {
        return m_data;
    }

    const T *data() const noexcept {
        return m_data;
    }

    friend std::ostream &operator<<(std::ostream &out, const Vector &vector) noexcept {
        if (vector.m_size == 0) {
            return out;
        }

        for (size_type i = 0; i < vector.m_size; i++) {
            out << vector.m_data[i] << (i < vector.m_size - 1 ? ", " : ".");
        }

        return out;
    }

    [[nodiscard]] bool empty() const noexcept {
        return m_size == 0;
    }

    [[nodiscard]] size_type size() const noexcept {
        return m_size;
    }

    void reserve(const size_type new_capacity) {
        if (new_capacity > m_capacity) {
            this->resize(new_capacity);
        }
    }

    [[nodiscard]] size_type capacity() const noexcept {
        return m_capacity;
    }

    void shrink_to_fit() {
        if (m_size < m_capacity) {
            this->resize(m_size);
        }
    }

    void clear() {
        m_traits.destroy(m_allocator, m_data);
        m_traits.deallocate(m_allocator, m_data, m_size);

        m_capacity = 0;
        m_size = 0;
    }

    size_type insert(const size_type pos, const size_type count, const T &element) {
        this->range_check_inclusive(pos, "pos");

        size_type new_capacity = m_capacity;

        while (new_capacity < m_size + count) {
            new_capacity *= 2;
        }

        this->resize(new_capacity);

        if (pos < m_size) {
            for (size_type i = m_size + count - 1; i > pos + count - 1; --i) {
                m_data[i] = m_data[i - count];
            }

            for (size_type i = pos; i < pos + count; ++i) {
                m_data[i] = element;
            }
        } else {
            for (size_type i = m_size; i < m_size + count; ++i) {
                m_data[i] = element;
            }
        }

        m_size += count;

        return pos;
    }

    size_type insert(const size_type pos, const std::initializer_list<T> &list) {
        this->range_check_inclusive(pos, "pos");

        size_type new_capacity = m_capacity;

        while (new_capacity < m_size + list.size()) {
            new_capacity *= 2;
        }

        this->resize(new_capacity);

        if (pos < m_size) {
            for (size_type i = m_size + list.size() - 1; i > pos + list.size() - 1; --i) {
                m_data[i] = m_data[i - list.size()];
            }

            size_type i = pos;

            for (const T &element: list) {
                m_data[i++] = element;
            }
        } else {
            size_type i = m_size;

            for (const T &element: list) {
                m_data[i++] = element;
            }
        }

        m_size += list.size();

        return pos;
    }

    size_type insert(const size_type pos, const T &element) {
        return this->insert(pos, 1, element);
    }

    size_type erase(const size_type first, const size_type last) {
        this->range_check_exclusive(first, "first");
        this->range_check_exclusive(last, "first");
        this->range_check_order(first, last);

        if (last < m_size) {
            for (size_type i = first; i < m_size - last + 1; ++i) {
                m_data[i] = m_data[i + last - first + 1];
            }

            m_size -= last - first + 1;
        } else {
            m_size = first;
        }

        return first;
    }

    size_type erase(const size_type pos) {
        return this->erase(pos, pos);
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

    void pop_back() noexcept {
        if (m_size > 0) {
            --m_size;
        }
    }

    void swap(Vector &vector) noexcept {
        std::swap(this->m_data, vector.m_data);
        std::swap(this->m_size, vector.m_size);
        std::swap(this->m_capacity, vector.m_capacity);
    }
};

#endif
