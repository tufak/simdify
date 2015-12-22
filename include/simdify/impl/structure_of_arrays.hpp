#ifndef SIMDIFY_STRUCTURE_OF_ARRAYS
#define SIMDIFY_STRUCTURE_OF_ARRAYS

#include "containers_common.hpp"

namespace simd {

    template <typename Simd_t, typename Ids, typename Sequence>
    struct structure_of_arrays_impl;

    template <typename Simd_t, typename... Ids, std::size_t... I>
    struct structure_of_arrays_impl<Simd_t, detail::group<Ids...>, sequence<I...>> {
        using self_t = structure_of_arrays_impl;
        using simd_t = Simd_t;
        using f_t = typename simd_t::f_t;
        using mm_t = typename simd_t::mm_t;

        enum : std::size_t { N = detail::group<Ids...>::size, W = simd_t::W };

        static_assert(N == sizeof...(I), "structure_of_arrays_impl: sequence size mismatch");
        static_assert(std::is_trivial<f_t>::value, "structure_of_arrays_impl: f_t not trivial");

        using value_type = named_array<f_t, Ids...>;
        using value_type_vector = named_array<simd_t, Ids...>;
        using reference = named_array<simd::reference<simd::storage<f_t>>, Ids...>;
        using const_reference = named_array<simd::const_reference<simd::storage<f_t>>, Ids...>;
        using reference_vector = named_array<simd::reference<simd::storage<Simd_t>>, Ids...>;
        using const_reference_vector = named_array<simd::const_reference<simd::storage<Simd_t>>, Ids...>;

        SIMDIFY_CONTAINERS_COMMON_CONSTRUCTION(structure_of_arrays_impl);

        void reserve(std::size_t count) {
            if (count <= m_cap) return;

            std::size_t new_cap = m_cap;
            if (new_cap == 0) {
                new_cap = div_ceil_shift<W>(count);
            }
            else do { new_cap *= 2; } while (new_cap < count);

            decltype(m_data) new_data(aligned_malloc<f_t, alignof(mm_t)>(N * new_cap), aligned_deleter{});

            if (!new_data) throw std::bad_alloc{};

            if (m_sz != 0) {
                detail::no_op(std::memcpy(new_data.get() + I*new_cap, m_data.get() + I*m_cap, sizeof(f_t)*m_sz)...);
            }

            std::swap(m_data, new_data);
            m_cap = new_cap;
        }

        void fill(const value_type& val, std::size_t from = 0) {
            f_t* first = m_data.get() + from;
            f_t* last = m_data.get() + m_sz;
            for (int n = 0; n < N; ++n, first += m_cap, last += m_cap) {
                f_t f = val[n];
                for (f_t* curr = first; curr != last; ++curr) {
                    *curr = f;
                }
            }
        }

        SIMIDFY_CONTAINERS_COMMON_SIZE_MANAGEMENT;

        reference operator[](std::size_t i) {
            reference res;
            auto base = m_data.get() + i;
            detail::no_op(simd::get<I>(res).reset(base + I*m_cap)...);
            return res;
        }

        const_reference operator[](std::size_t i) const {
            const_reference res;
            auto base = m_data.get() + i;
            detail::no_op(simd::get<I>(res).reset(base + I*m_cap)...);
            return res;
        }

        SIMDIFY_CONTAINERS_COMMON_ACCESS("structure_of_arrays");

        void push_back(const value_type& val) {
            reserve(m_sz + 1);
            f_t* base = m_data.get() + m_sz;
            detail::no_op(*(base + I*m_cap) = simd::get<I>(val)...);
            ++m_sz;
        }

        SIMDIFY_CONTAINERS_COMMON_POP_BACK("structure_of_arrays");

        template <typename Ref>
        struct reference_iterator : std::iterator<std::random_access_iterator_tag, Ref> {
            reference_iterator(const self_t& self, std::size_t idx) {
                auto base = self.m_data.get() + idx;
                detail::no_op(simd::get<I>(m_ref).reset(base + I*self.m_cap)...);
            }

            reference_iterator& operator=(const reference_iterator& rhs) {
                detail::no_op(simd::get<I>(m_ref).reset(simd::get<I>(rhs.m_ref).ptr())...);
                return *this;
            }

            reference_iterator& operator++() { detail::no_op(++simd::get<I>(m_ref).ptr()...); return *this; }
            reference_iterator& operator--() { detail::no_op(--simd::get<I>(m_ref).ptr()...); return *this; }
            reference_iterator& operator+=(std::ptrdiff_t add) { detail::no_op(simd::get<I>(m_ref).ptr() += add...); return *this; }
            std::ptrdiff_t operator-(const reference_iterator& rhs) { return m_ref.get().ptr() - rhs.m_ref.get().ptr(); }
            bool operator<(const reference_iterator& rhs) const { return m_ref.get().ptr() < rhs.m_ref.get().ptr(); }
            bool operator<=(const reference_iterator& rhs) const { return m_ref.get().ptr() <= rhs.m_ref.get().ptr(); }
            bool operator==(const reference_iterator& rhs) const { return m_ref.get().ptr() == rhs.m_ref.get().ptr(); }
            Ref& operator*() { return m_ref; }
            Ref* operator->() { return &m_ref; }

            SIMDIFY_CONTAINERS_COMMON_ITERATOR_FACILITIES(reference_iterator);

        private:
            Ref m_ref;
        };

        using iterator = reference_iterator<reference>;
        using const_iterator = reference_iterator<const_reference>;
        using iterator_vector = reference_iterator<reference_vector>;
        using const_iterator_vector = reference_iterator<const_reference_vector>;

        SIMDIFY_CONTAINERS_COMMON_ITERATION;

    private:
        SIMDIFY_CONTAINERS_COMMON_DATA;
    };

    template <typename Simd_t, typename... Ids>
    using structure_of_arrays = structure_of_arrays_impl<Simd_t, detail::group<Ids...>, make_sequence_t<0, detail::group<Ids...>::size>>;
}

#endif // SIMDIFY_STRUCTURE_OF_ARRAYS