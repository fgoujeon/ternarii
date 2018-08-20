#ifndef ARRAY2D_HPP
#define ARRAY2D_HPP

#include <array>

namespace libgame
{

template<typename T, size_t ColumnCount, size_t RowCount>
struct array2d
{
    using impl_t = std::array<T, ColumnCount * RowCount>;

    using value_type = typename impl_t::value_type;
    using iterator = typename impl_t::iterator;
    using const_iterator = typename impl_t::const_iterator;

    static const size_t column_count = ColumnCount;
    static const size_t row_count = RowCount;

    iterator begin()
    {
        return impl_.begin();
    }

    const_iterator begin() const
    {
        return impl_.begin();
    }

    const_iterator cbegin() const
    {
        return impl_.cbegin();
    }

    iterator end()
    {
        return impl_.end();
    }

    const_iterator end() const
    {
        return impl_.end();
    }

    const_iterator cend() const
    {
        return impl_.cend();
    }

    size_t size() const
    {
        return impl_.size();
    }

    T& at(const size_t index)
    {
        return impl_.at(index);
    }

    const T& at(const size_t index) const
    {
        return impl_.at(index);
    }

    T& at(const size_t x, const size_t y)
    {
        return impl_.at(x + y * ColumnCount);
    }

    const T& at(const size_t x, const size_t y) const
    {
        return impl_.at(x + y * ColumnCount);
    }

    void fill(const T& value)
    {
        impl_.fill(value);
    }

    impl_t impl_;
};

} //namespace libgame

#endif
