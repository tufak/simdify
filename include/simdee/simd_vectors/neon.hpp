// This file is a part of Simdee, see homepage at http://github.com/hrabalik/simdee
// This file is distributed under the MIT license.

#ifndef SIMDEE_SIMD_TYPES_NEON_HPP
#define SIMDEE_SIMD_TYPES_NEON_HPP

#include "common.hpp"

#if !SIMDEE_NEON
#error "NEON intrinsics are required to use the NEON SIMD type. Please check your build options."
#endif

#include <arm_neon.h>

namespace sd {
    namespace impl {
        SIMDEE_INL uint32x4_t neon_load(const bool32_t* ptr) {
            return vld1q_u32(reinterpret_cast<const uint32_t*>(ptr));
        }
        SIMDEE_INL float32x4_t neon_load(const float* ptr) { return vld1q_f32(ptr); }
        SIMDEE_INL uint32x4_t neon_load(const uint32_t* ptr) { return vld1q_u32(ptr); }
        SIMDEE_INL int32x4_t neon_load(const int32_t* ptr) { return vld1q_s32(ptr); }
        SIMDEE_INL void neon_store(const uint32x4_t& vec, bool32_t* ptr) {
            vst1q_u32(reinterpret_cast<uint32_t*>(ptr), vec);
        }
        SIMDEE_INL void neon_store(const float32x4_t& vec, float* ptr) { vst1q_f32(ptr, vec); }
        SIMDEE_INL void neon_store(const uint32x4_t& vec, uint32_t* ptr) { vst1q_u32(ptr, vec); }
        SIMDEE_INL void neon_store(const int32x4_t& vec, int32_t* ptr) { vst1q_s32(ptr, vec); }
    }

    struct neonb;
    struct neonf;
    struct neonu;
    struct neons;

    template <>
    struct is_simd_vector<neonb> : std::integral_constant<bool, true> {};
    template <>
    struct is_simd_vector<neonf> : std::integral_constant<bool, true> {};
    template <>
    struct is_simd_vector<neonu> : std::integral_constant<bool, true> {};
    template <>
    struct is_simd_vector<neons> : std::integral_constant<bool, true> {};

    template <typename Simd_t, typename Scalar_t>
    struct neon_traits {
        using simd_t = Simd_t;
        using scalar_t = Scalar_t;
        using vec_b = neonb;
        using vec_f = neonf;
        using vec_u = neonu;
        using vec_s = neons;
        using mask_t = impl::mask<0xfU>;
        using storage_t = impl::storage<simd_t, scalar_t, alignof(int32x4_t)>;
    };

    template <>
    struct simd_vector_traits<neonb> : neon_traits<neonb, bool32_t> {
        using vector_t = uint32x4_t;
    };

    template <>
    struct simd_vector_traits<neonf> : neon_traits<neonf, float> {
        using vector_t = float32x4_t;
    };

    template <>
    struct simd_vector_traits<neonu> : neon_traits<neonu, uint32_t> {
        using vector_t = uint32x4_t;
    };

    template <>
    struct simd_vector_traits<neons> : neon_traits<neons, int32_t> {
        using vector_t = int32x4_t;
    };

    template <typename Crtp>
    struct neon_base : simd_base<Crtp> {
    protected:
        using simd_base<Crtp>::mm;

    public:
        using vector_t = typename simd_base<Crtp>::vector_t;
        using scalar_t = typename simd_base<Crtp>::scalar_t;
        using storage_t = typename simd_base<Crtp>::storage_t;
        using simd_base<Crtp>::width;
        using simd_base<Crtp>::self;

        SIMDEE_TRIVIAL_TYPE(neon_base);
        SIMDEE_BASE_CTOR(neon_base, vector_t, mm = r);
        SIMDEE_BASE_CTOR_TPL(neon_base, expr::aligned<T>, aligned_load(r.ptr));
        SIMDEE_BASE_CTOR_TPL(neon_base, expr::unaligned<T>, unaligned_load(r.ptr));
        SIMDEE_BASE_CTOR_TPL(neon_base, expr::interleaved<T>, interleaved_load(r.ptr, r.step));
        SIMDEE_BASE_CTOR(neon_base, storage_t, aligned_load(r.data()));

        SIMDEE_INL void aligned_load(const scalar_t* r) { mm = impl::neon_load(r); }
        SIMDEE_INL void aligned_store(scalar_t* r) const { impl::neon_store(mm, r); }
        SIMDEE_INL void unaligned_load(const scalar_t* r) { mm = impl::neon_load(r); }
        SIMDEE_INL void unaligned_store(scalar_t* r) const { impl::neon_store(mm, r); }

        void interleaved_load(const scalar_t* r, int step) {
            storage_t temp;
            for (size_t i = 0; i < width; i++, r += step) { temp[i] = *r; }
            mm = impl::neon_load(temp.data());
        }

        void interleaved_store(scalar_t* r, int step) const {
            storage_t temp;
            impl::neon_store(mm, temp.data());
            for (size_t i = 0; i < width; i++, r += step) { *r = temp[i]; }
        }
    };

// clang-format off
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SIMDEE_NEON_COMMON( CLASS, SUFFIX, SCALAR_TYPE )                                                 \
SIMDEE_CTOR( CLASS , scalar_t, mm = vmovq_n_ ## SUFFIX ( SCALAR_TYPE (r)));                              \
SIMDEE_CTOR_TPL( CLASS, expr::init<T>, mm = vmovq_n_ ## SUFFIX (r.template to< SCALAR_TYPE >()));        \
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    // clang-format on

    struct neonb final : neon_base<neonb> {
        using neon_base::neon_base;
        SIMDEE_TRIVIAL_TYPE(neonb);
        SIMDEE_NEON_COMMON(neonb, u32, uint32_t);
    };

    struct neonf final : neon_base<neonf> {
        using neon_base::neon_base;
        SIMDEE_TRIVIAL_TYPE(neonf);
        SIMDEE_NEON_COMMON(neonf, f32, float);
        SIMDEE_INL explicit neonf(const neons&);
    };

    struct neonu final : neon_base<neonu> {
        using neon_base::neon_base;
        SIMDEE_TRIVIAL_TYPE(neonu);
        SIMDEE_NEON_COMMON(neonu, u32, uint32_t);
        SIMDEE_INL explicit neonu(const neonb&);
        SIMDEE_INL explicit neonu(const neons&);
    };

    struct neons final : neon_base<neons> {
        using neon_base::neon_base;
        SIMDEE_TRIVIAL_TYPE(neons);
        SIMDEE_NEON_COMMON(neons, s32, int32_t);
        SIMDEE_INL explicit neons(const neonf&);
        SIMDEE_INL explicit neons(const neonu&);
    };

    SIMDEE_INL neonf::neonf(const neons& r) { mm = vcvtq_f32_s32(r.data()); }
    SIMDEE_INL neons::neons(const neonf& r) { mm = vcvtq_s32_f32(r.data()); }
    SIMDEE_INL neonu::neonu(const neonb& r) { mm = r.data(); }
    SIMDEE_INL neonu::neonu(const neons& r) { mm = vreinterpretq_u32_s32(r.data()); }
    SIMDEE_INL neons::neons(const neonu& r) { mm = vreinterpretq_s32_u32(r.data()); }
}

#endif // SIMDEE_SIMD_TYPES_NEON_HPP
