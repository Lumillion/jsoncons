#ifndef JSONCONS_DETAIL_SPAN_HPP
#define JSONCONS_DETAIL_SPAN_HPP

#include <utility> // std::swap
#include <memory> // std::addressof
#include <type_traits> // std::enable_if, std::true_type, std::false_type
#include <jsoncons/config/compiler_support.hpp>
#include <jsoncons/detail/more_type_traits.hpp>
#include <jsoncons/json_exception.hpp>
#include <iterator>
#include <limits>

namespace jsoncons
{
namespace detail
{
    constexpr std::size_t dynamic_extent = (std::numeric_limits<std::size_t>::max)();

    template< class T, std::size_t Extent = dynamic_extent>
    class span;

    template<class T>
    struct is_span : std::false_type{};

    template< class T>
    struct is_span<span<T>> : std::true_type{};

    template<
        class T,
        std::size_t Extent
    > class span
    {
    public:
        using element_type = T;
        using value_type = typename std::remove_volatile<typename std::remove_const<T>::type>::type;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        typedef pointer iterator;
        typedef const_pointer const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    private:
        pointer data_;
        size_type size_;
    public:
        static constexpr std::size_t extent = Extent;

        constexpr span() noexcept
            : data_(nullptr), size_(0)
        {
        }
        constexpr span(pointer data, size_type size)
            : data_(data), size_(size)
        {
        }

        template <typename C>
        constexpr span(C& c,
                       typename std::enable_if<!is_span<C>::value && !is_std_array<C>::value && is_compatible_element<C,element_type>::value && has_data_and_size<C>::value>::type* = 0)
            : data_(c.data()), size_(c.size())
        {
        }

        template <std::size_t N>
        span(element_type (&arr)[N]) noexcept
            : data_(std::addressof(arr[0])), size_(N)
        {
        }

        template <std::size_t N>
        constexpr span(std::array<value_type, N>& arr,
                       typename std::enable_if<(N == dynamic_extent || N == extent)>::type* = 0) noexcept
            : data_(arr.data()), size_(arr.size())
        {
        }

        template <std::size_t N>
        constexpr span(const std::array<value_type, N>& arr,
                       typename std::enable_if<(N == dynamic_extent || N == extent)>::type* = 0) noexcept
            : data_(arr.data()), size_(arr.size())
        {
        }

        template <typename C>
        constexpr span(const C& c,
                       typename std::enable_if<!is_span<C>::value && !is_std_array<C>::value && is_compatible_element<C,element_type>::value && has_data_and_size<C>::value>::type* = 0)
            : data_(c.data()), size_(c.size())
        {
        }

        template <class U, std::size_t N>
        constexpr span(const span<U, N>& s,
                       typename std::enable_if<(N == dynamic_extent || N == extent) && std::is_convertible<U(*)[], element_type(*)[]>::value>::type* = 0) noexcept
            : data_(s.data()), size_(s.size())
        {
        }

        constexpr span(const span& other) noexcept = default;

        span& operator=( const span& other ) noexcept = default;

        constexpr pointer data() const noexcept
        {
            return data_;
        }

        constexpr size_type size() const noexcept
        {
            return size_;
        }

         constexpr bool empty() const noexcept 
        { 
            return size_ == 0; 
        }

         constexpr reference operator[](size_type index) const
         {
             return data_[index];
         }

        // iterator support 
        const_iterator begin() const noexcept
        {
            return data_;
        }
        const_iterator end() const noexcept
        {
            return data_ + size_;
        }
        const_iterator cbegin() const noexcept 
        { 
            return data_; 
        }
        const_iterator cend() const noexcept 
        { 
            return data_ + size_; 
        }
        const_reverse_iterator rbegin() const noexcept 
        { 
            return const_reverse_iterator(end()); 
        }
        const_reverse_iterator rend() const noexcept 
        { 
            return const_reverse_iterator(begin()); 
        }
        const_reverse_iterator crbegin() const noexcept 
        { 
            return const_reverse_iterator(end()); 
        }
        const_reverse_iterator crend() const noexcept 
        { 
            return const_reverse_iterator(begin()); 
        }

        span<element_type, dynamic_extent>
        first(std::size_t count) const
        {
            JSONCONS_ASSERT(count <= size());

            return span< element_type, dynamic_extent >( data(), count );
        }

        span<element_type, dynamic_extent>
        last(std::size_t count) const
        {
            JSONCONS_ASSERT(count <= size());

            return span<element_type, dynamic_extent>(data() + ( size() - count ), count);
        }

        span<element_type, dynamic_extent>
        subspan(std::size_t offset, std::size_t count = dynamic_extent) const
        {
            JSONCONS_ASSERT(offset <= size() && (count == dynamic_extent || (offset + count <= size())));

            return span<element_type, dynamic_extent>(
                data() + offset, count == dynamic_extent ? size() - offset : count );
        }
    };

} // namespace detail
} // namespace jsoncons

#endif
