#define SIMDIFY_NEED_AVX
#include <simdify/simd_types.hpp>
#include <simdify/storage.hpp>

#include "catch.hpp"
#include <array>
#include <type_traits>
#include <algorithm>
#include <numeric>
#include <functional>

using F = simd::avxf;
using U = simd::avxu;
using S = simd::avxs;

alignas(F) const F::array_e bufAF = { {
    -0.27787193f, +0.70154146f, -2.05181630f, +2.22944568f,
    -0.82358653f, -1.57705702f, +0.50797465f, -0.59003456f,
} };
alignas(F) const F::array_e bufBF = { {
    -0.23645458f, +2.02369089f, -2.25835397f, +2.22944568f,
    +0.33756370f, -0.87587426f, -1.66416447f, -0.59003456f,
} };
alignas(U) const U::array_e bufAU = { {
    1753029375U, 1117080442U, 3817141237U, 3761735248U,
    244284091U, 1874242234U, 3400252368U, 2648404707U,
} };
alignas(U) const U::array_e bufBU = { {
    1679702461U, 2102346647U, 480083363U, 779803484U,
    1213208312U, 2121108407U, 2718691794U, 3724041010U,
} };
alignas(S) const S::array_e bufAS = { {
    -1712190449, -48692967, -214510247, 440646957,
    471289320, 614985478, -2035465541, 248883859,
} };
alignas(S) const S::array_e bufBS = { {
    724135231, 56848532, 64122653, 1641874147,
    -899302812, -2112882416, 77287484, 1066617619,
} };
alignas(F) const F::array_e bufZF = { { 0, 0, 0, 0, 0, 0, 0, 0 } };
alignas(U) const U::array_e bufZU = { { 0, 0, 0, 0, 0, 0, 0, 0 } };
alignas(S) const S::array_e bufZS = { { 0, 0, 0, 0, 0, 0, 0, 0 } };

TEST_CASE("AVX basic guarantees", "[simd_t][x86][avx]") {
    REQUIRE(F::W == 8);
    REQUIRE(U::W == 8);
    REQUIRE(S::W == 8);
    REQUIRE((std::is_same<F::mm_t, __m256>::value));
    REQUIRE((std::is_same<U::mm_t, __m256>::value));
    REQUIRE((std::is_same<S::mm_t, __m256>::value));
    REQUIRE((std::is_same<F::e_t, float>::value));
    REQUIRE((std::is_same<U::e_t, uint32_t>::value));
    REQUIRE((std::is_same<S::e_t, int32_t>::value));
    REQUIRE((std::is_same<F::array_e, std::array<F::e_t, F::W>>::value));
    REQUIRE((std::is_same<U::array_e, std::array<U::e_t, U::W>>::value));
    REQUIRE((std::is_same<S::array_e, std::array<S::e_t, S::W>>::value));
    REQUIRE(sizeof(F) == sizeof(F::mm_t));
    REQUIRE(sizeof(U) == sizeof(U::mm_t));
    REQUIRE(sizeof(S) == sizeof(S::mm_t));
    REQUIRE(alignof(F) == alignof(F::mm_t));
    REQUIRE(alignof(U) == alignof(U::mm_t));
    REQUIRE(alignof(S) == alignof(S::mm_t));
    REQUIRE(sizeof(F::e_t) * F::W == sizeof(F));
    REQUIRE(sizeof(U::e_t) * U::W == sizeof(U));
    REQUIRE(sizeof(S::e_t) * S::W == sizeof(S));
    REQUIRE((std::is_trivially_copyable<F>::value));
    REQUIRE((std::is_trivially_copyable<U>::value));
    REQUIRE((std::is_trivially_copyable<S>::value));
}

TEST_CASE("AVX explicit construction", "[simd_t][x86][avx]") {
    alignas(F)F::array_e rf = bufZF;
    alignas(U)U::array_e ru = bufZU;
    alignas(S)S::array_e rs = bufZS;
    auto tor = [&rf, &ru, &rs](const F& tf, const U& tu, const S& ts) {
        tf.aligned_store(rf.data());
        tu.aligned_store(ru.data());
        ts.aligned_store(rs.data());
    };

    SECTION("from e_t") {
        F tf(1.2345678f);
        U tu(123456789U);
        S ts(-123456789);
        tor(tf, tu, ts);
        for (auto val : rf) REQUIRE(val == 1.2345678f);
        for (auto val : ru) REQUIRE(val == 123456789U);
        for (auto val : rs) REQUIRE(val == -123456789);
    }
    SECTION("from mm_t") {
        F tf(_mm256_load_ps(bufAF.data()));
        U tu(_mm256_load_ps((float*)bufAU.data()));
        S ts(_mm256_load_ps((float*)bufAS.data()));
        tor(tf, tu, ts);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from aligned pointer") {
        F tf(simd::aligned(bufAF.data()));
        U tu(simd::aligned(bufAU.data()));
        S ts(simd::aligned(bufAS.data()));
        tor(tf, tu, ts);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from unaligned pointer") {
        F tf(simd::unaligned(bufAF.data()));
        U tu(simd::unaligned(bufAU.data()));
        S ts(simd::unaligned(bufAS.data()));
        tor(tf, tu, ts);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from simd::storage") {
        simd::storage<F> storf;
        simd::storage<U> storu;
        simd::storage<S> stors;
        storf = bufAF;
        storu = bufAU;
        stors = bufAS;
        F tf(storf);
        U tu(storu);
        S ts(stors);
        tor(tf, tu, ts);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from simd::aos_storage") {
        simd::aos_storage<F, 7> aos_storf;
        simd::aos_storage<U, 8> aos_storu;
        simd::aos_storage<S, 9> aos_stors;
        for (int i = 0; i < F::W; ++i) aos_storf[i] = bufAF[i];
        for (int i = 0; i < U::W; ++i) aos_storu[i] = bufAU[i];
        for (int i = 0; i < S::W; ++i) aos_stors[i] = bufAS[i];
        F tf(aos_storf);
        U tu(aos_storu);
        S ts(aos_stors);
        tor(tf, tu, ts);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from uval, sval, zero, etc. (simd::init family)") {
        {
            F tf(simd::fval(1.2345678f));
            U tu(simd::fval(1.2345678f));
            S ts(simd::fval(1.2345678f));
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tof(val) == 1.2345678f);
            for (auto val : ru) REQUIRE(simd::tof(val) == 1.2345678f);
            for (auto val : rs) REQUIRE(simd::tof(val) == 1.2345678f);
        }
        {
            F tf(simd::uval(0xdeadbeef));
            U tu(simd::uval(0xdeadbeef));
            S ts(simd::uval(0xdeadbeef));
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tou(val) == 0xdeadbeef);
            for (auto val : ru) REQUIRE(simd::tou(val) == 0xdeadbeef);
            for (auto val : rs) REQUIRE(simd::tou(val) == 0xdeadbeef);
        }
        {
            F tf(simd::sval(-123456789));
            U tu(simd::sval(-123456789));
            S ts(simd::sval(-123456789));
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tos(val) == -123456789);
            for (auto val : ru) REQUIRE(simd::tos(val) == -123456789);
            for (auto val : rs) REQUIRE(simd::tos(val) == -123456789);
        }
        {
            F tf(simd::zero());
            U tu(simd::zero());
            S ts(simd::zero());
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tou(val) == 0x00000000);
            for (auto val : ru) REQUIRE(simd::tou(val) == 0x00000000);
            for (auto val : rs) REQUIRE(simd::tou(val) == 0x00000000);
        }
        {
            F tf(simd::all_bits());
            U tu(simd::all_bits());
            S ts(simd::all_bits());
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tou(val) == 0xffffffff);
            for (auto val : ru) REQUIRE(simd::tou(val) == 0xffffffff);
            for (auto val : rs) REQUIRE(simd::tou(val) == 0xffffffff);
        }
        {
            F tf(simd::sign_bit());
            U tu(simd::sign_bit());
            S ts(simd::sign_bit());
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tou(val) == 0x80000000);
            for (auto val : ru) REQUIRE(simd::tou(val) == 0x80000000);
            for (auto val : rs) REQUIRE(simd::tou(val) == 0x80000000);
        }
        {
            F tf(simd::abs_mask());
            U tu(simd::abs_mask());
            S ts(simd::abs_mask());
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tou(val) == 0x7fffffff);
            for (auto val : ru) REQUIRE(simd::tou(val) == 0x7fffffff);
            for (auto val : rs) REQUIRE(simd::tou(val) == 0x7fffffff);
        }
    }
}


TEST_CASE("AVX implicit construction", "[simd_t][x86][avx]") {
    alignas(F)F::array_e rf = bufZF;
    alignas(U)U::array_e ru = bufZU;
    alignas(S)S::array_e rs = bufZS;
    auto implicit_test = [&rf, &ru, &rs](const F& tf, const U& tu, const S& ts) {
        tf.aligned_store(rf.data());
        tu.aligned_store(ru.data());
        ts.aligned_store(rs.data());
    };

    SECTION("from e_t") {
        implicit_test(1.2345678f, 123456789U, -123456789);
        for (auto val : rf) REQUIRE(val == 1.2345678f);
        for (auto val : ru) REQUIRE(val == 123456789U);
        for (auto val : rs) REQUIRE(val == -123456789);
    }
    SECTION("from mm_t") {
        implicit_test(
            _mm256_load_ps(bufAF.data()),
            _mm256_load_ps((float*)bufAU.data()),
            _mm256_load_ps((float*)bufAS.data()));
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from aligned pointer") {
        implicit_test(
            simd::aligned(bufAF.data()),
            simd::aligned(bufAU.data()),
            simd::aligned(bufAS.data()));
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from unaligned pointer") {
        implicit_test(
            simd::unaligned(bufAF.data()),
            simd::unaligned(bufAU.data()),
            simd::unaligned(bufAS.data()));
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from simd::storage") {
        simd::storage<F> storf;
        simd::storage<U> storu;
        simd::storage<S> stors;
        storf = bufAF;
        storu = bufAU;
        stors = bufAS;
        implicit_test(storf, storu, stors);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from simd::aos_storage") {
        simd::aos_storage<F, 7> aos_storf;
        simd::aos_storage<U, 8> aos_storu;
        simd::aos_storage<S, 9> aos_stors;
        for (int i = 0; i < F::W; ++i) aos_storf[i] = bufAF[i];
        for (int i = 0; i < U::W; ++i) aos_storu[i] = bufAU[i];
        for (int i = 0; i < S::W; ++i) aos_stors[i] = bufAS[i];
        implicit_test(aos_storf, aos_storu, aos_stors);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from uval, sval, zero, etc. (simd::init family)") {
        implicit_test(simd::fval(1.2345678f), simd::fval(1.2345678f), simd::fval(1.2345678f));
        for (auto val : rf) REQUIRE(simd::tof(val) == 1.2345678f);
        for (auto val : ru) REQUIRE(simd::tof(val) == 1.2345678f);
        for (auto val : rs) REQUIRE(simd::tof(val) == 1.2345678f);
        implicit_test(simd::uval(0xdeadbeef), simd::uval(0xdeadbeef), simd::uval(0xdeadbeef));
        for (auto val : rf) REQUIRE(simd::tou(val) == 0xdeadbeef);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0xdeadbeef);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0xdeadbeef);
        implicit_test(simd::sval(-123456789), simd::sval(-123456789), simd::sval(-123456789));
        for (auto val : rf) REQUIRE(simd::tos(val) == -123456789);
        for (auto val : ru) REQUIRE(simd::tos(val) == -123456789);
        for (auto val : rs) REQUIRE(simd::tos(val) == -123456789);
        implicit_test(simd::zero(), simd::zero(), simd::zero());
        for (auto val : rf) REQUIRE(simd::tou(val) == 0x00000000);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0x00000000);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0x00000000);
        implicit_test(simd::all_bits(), simd::all_bits(), simd::all_bits());
        for (auto val : rf) REQUIRE(simd::tou(val) == 0xffffffff);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0xffffffff);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0xffffffff);
        implicit_test(simd::sign_bit(), simd::sign_bit(), simd::sign_bit());
        for (auto val : rf) REQUIRE(simd::tou(val) == 0x80000000);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0x80000000);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0x80000000);
        implicit_test(simd::abs_mask(), simd::abs_mask(), simd::abs_mask());
        for (auto val : rf) REQUIRE(simd::tou(val) == 0x7fffffff);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0x7fffffff);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0x7fffffff);
    }
}

TEST_CASE("AVX assignment", "[simd_t][x86][avx]") {
    alignas(F)F::array_e rf = bufZF;
    alignas(U)U::array_e ru = bufZU;
    alignas(S)S::array_e rs = bufZS;
    F tf;
    U tu;
    S ts;
    auto tor = [&rf, &tf, &ru, &tu, &rs, &ts]() {
        tf.aligned_store(rf.data());
        tu.aligned_store(ru.data());
        ts.aligned_store(rs.data());
    };

    SECTION("from e_t") {
        tf = 1.2345678f;
        tu = 123456789U;
        ts = -123456789;
        tor();
        for (auto val : rf) REQUIRE(val == 1.2345678f);
        for (auto val : ru) REQUIRE(val == 123456789U);
        for (auto val : rs) REQUIRE(val == -123456789);
    }
    SECTION("from mm_t") {
        tf = _mm256_load_ps(bufAF.data());
        tu = _mm256_load_ps((float*)bufAU.data());
        ts = _mm256_load_ps((float*)bufAS.data());
        tor();
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from aligned pointer") {
        tf = simd::aligned(bufAF.data());
        tu = simd::aligned(bufAU.data());
        ts = simd::aligned(bufAS.data());
        tor();
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from unaligned pointer") {
        tf = simd::unaligned(bufAF.data());
        tu = simd::unaligned(bufAU.data());
        ts = simd::unaligned(bufAS.data());
        tor();
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from simd::storage") {
        simd::storage<F> storf;
        simd::storage<U> storu;
        simd::storage<S> stors;
        storf = bufAF;
        storu = bufAU;
        stors = bufAS;
        tf = storf;
        tu = storu;
        ts = stors;
        tor();
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from simd::aos_storage") {
        simd::aos_storage<F, 7> aos_storf;
        simd::aos_storage<U, 8> aos_storu;
        simd::aos_storage<S, 9> aos_stors;
        for (int i = 0; i < F::W; ++i) aos_storf[i] = bufAF[i];
        for (int i = 0; i < U::W; ++i) aos_storu[i] = bufAU[i];
        for (int i = 0; i < S::W; ++i) aos_stors[i] = bufAS[i];
        tf = aos_storf;
        tu = aos_storu;
        ts = aos_stors;
        tor();
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from uval, sval, zero, etc. (simd::init family)") {
        tf = simd::fval(1.2345678f);
        tu = simd::fval(1.2345678f);
        ts = simd::fval(1.2345678f);
        tor();
        for (auto val : rf) REQUIRE(simd::tof(val) == 1.2345678f);
        for (auto val : ru) REQUIRE(simd::tof(val) == 1.2345678f);
        for (auto val : rs) REQUIRE(simd::tof(val) == 1.2345678f);
        tf = simd::uval(0xdeadbeef);
        tu = simd::uval(0xdeadbeef);
        ts = simd::uval(0xdeadbeef);
        tor();
        for (auto val : rf) REQUIRE(simd::tou(val) == 0xdeadbeef);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0xdeadbeef);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0xdeadbeef);
        tf = simd::sval(-123456789);
        tu = simd::sval(-123456789);
        ts = simd::sval(-123456789);
        tor();
        for (auto val : rf) REQUIRE(simd::tos(val) == -123456789);
        for (auto val : ru) REQUIRE(simd::tos(val) == -123456789);
        for (auto val : rs) REQUIRE(simd::tos(val) == -123456789);
        tf = simd::zero();
        tu = simd::zero();
        ts = simd::zero();
        tor();
        for (auto val : rf) REQUIRE(simd::tou(val) == 0x00000000);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0x00000000);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0x00000000);
        tf = simd::all_bits();
        tu = simd::all_bits();
        ts = simd::all_bits();
        tor();
        for (auto val : rf) REQUIRE(simd::tou(val) == 0xffffffff);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0xffffffff);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0xffffffff);
        tf = simd::sign_bit();
        tu = simd::sign_bit();
        ts = simd::sign_bit();
        tor();
        for (auto val : rf) REQUIRE(simd::tou(val) == 0x80000000);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0x80000000);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0x80000000);
        tf = simd::abs_mask();
        tu = simd::abs_mask();
        ts = simd::abs_mask();
        tor();
        for (auto val : rf) REQUIRE(simd::tou(val) == 0x7fffffff);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0x7fffffff);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0x7fffffff);
    }
}

TEST_CASE("AVX arithmetic", "[simd_t][x86][avx]") {
    alignas(F)F::array_e r;
    alignas(F)F::array_e e;
    F a = simd::aligned(bufAF.data());
    F b = simd::aligned(bufBF.data());
    auto tor = [&r](const F& t) {
        t.aligned_store(r.data());
    };

    SECTION("unary plus") {
        std::transform(begin(bufAF), end(bufAF), begin(e), [](F::e_t a) { return +a; });
        tor(+a);
        REQUIRE(r == e);
    }
    SECTION("unary minus") {
        std::transform(begin(bufAF), end(bufAF), begin(e), std::negate<F::e_t>());
        tor(-a);
        REQUIRE(r == e);
    }
    SECTION("plus") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), std::plus<F::e_t>());
        tor(a + b);
        REQUIRE(r == e);
        a += b;
        tor(a);
        REQUIRE(r == e);
    }
    SECTION("minus") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), std::minus<F::e_t>());
        tor(a - b);
        REQUIRE(r == e);
        a -= b;
        tor(a);
        REQUIRE(r == e);
    }
    SECTION("multiplies") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), std::multiplies<F::e_t>());
        tor(a * b);
        REQUIRE(r == e);
        a *= b;
        tor(a);
        REQUIRE(r == e);
    }
    SECTION("divides") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), std::divides<F::e_t>());
        tor(a / b);
        REQUIRE(r == e);
        a /= b;
        tor(a);
        REQUIRE(r == e);
    }
    SECTION("absolute value") {
        std::transform(begin(bufAF), end(bufAF), begin(e), [](F::e_t a) { return std::abs(a); });
        tor(abs(a));
        REQUIRE(r == e);
    }
    SECTION("signum") {
        std::transform(begin(bufAF), end(bufAF), begin(e), [](F::e_t a) { return std::signbit(a)? F::e_t(-1) : F::e_t(1); });
        tor(signum(a));
        REQUIRE(r == e);
    }
    SECTION("square root") {
        std::transform(begin(bufAF), end(bufAF), begin(e), [](F::e_t a) { return std::sqrt(std::abs(a)); });
        tor(sqrt(abs(a)));
        for (int i = 0; i < F::W; ++i) REQUIRE(r[i] == Approx(e[i]));
    }
    SECTION("fast reciprocal") {
        std::transform(begin(bufAF), end(bufAF), begin(e), [](F::e_t a) { return 1 / a; });
        tor(rcp(a));
        for (int i = 0; i < F::W; ++i) REQUIRE(r[i] == Approx(e[i]).epsilon(0.001));
    }
    SECTION("fast reciprocal square root") {
        std::transform(begin(bufAF), end(bufAF), begin(e), [](F::e_t a) { return 1 / std::sqrt(std::abs(a)); });
        tor(rsqrt(abs(a)));
        for (int i = 0; i < F::W; ++i) REQUIRE(r[i] == Approx(e[i]).epsilon(0.001));
    }
}

TEST_CASE("AVX comparison", "[simd_t][x86][avx]") {
    auto if_ = [](bool in) { return in ? 0xffffffffU : 0x00000000U; };
    alignas(U)U::array_e r;
    alignas(U)U::array_e e;
    F a = simd::aligned(bufAF.data());
    F b = simd::aligned(bufBF.data());
    auto tor = [&r](const U& t) {
        t.aligned_store(r.data());
    };

    SECTION("equal to") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), [&if_](F::e_t a, F::e_t b) {
            return if_(a == b);
        });
        tor(a == b);
        REQUIRE(r == e);
    }
    SECTION("not equal to") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), [&if_](F::e_t a, F::e_t b) {
            return if_(a != b);
        });
        tor(a != b);
        REQUIRE(r == e);
    }
    SECTION("greater") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), [&if_](F::e_t a, F::e_t b) {
            return if_(a > b);
        });
        tor(a > b);
        REQUIRE(r == e);
    }
    SECTION("less") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), [&if_](F::e_t a, F::e_t b) {
            return if_(a < b);
        });
        tor(a < b);
        REQUIRE(r == e);
    }
    SECTION("greater equal") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), [&if_](F::e_t a, F::e_t b) {
            return if_(a >= b);
        });
        tor(a >= b);
        REQUIRE(r == e);
    }
    SECTION("less equal") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), [&if_](F::e_t a, F::e_t b) {
            return if_(a <= b);
        });
        tor(a <= b);
        REQUIRE(r == e);
    }
}

TEST_CASE("AVX bitwise", "[simd_t][x86][avx]") {
    alignas(U)U::array_e r;
    alignas(U)U::array_e e;
    U a = simd::aligned(bufAU.data());
    U b = simd::aligned(bufBU.data());
    auto tor = [&r](const U& t) {
        t.aligned_store(r.data());
    };

    SECTION("bit and") {
        std::transform(begin(bufAU), end(bufAU), begin(bufBU), begin(e), [](U::e_t a, U::e_t b) {
            return simd::tou(a) & simd::tou(b);
        });
        tor(a & b);
        REQUIRE(r == e);
        a &= b;
        tor(a);
        REQUIRE(r == e);
    }
    SECTION("bit or") {
        std::transform(begin(bufAU), end(bufAU), begin(bufBU), begin(e), [](U::e_t a, U::e_t b) {
            return simd::tou(a) | simd::tou(b);
        });
        tor(a | b);
        REQUIRE(r == e);
        a |= b;
        tor(a);
        REQUIRE(r == e);
    }
    SECTION("bit xor") {
        std::transform(begin(bufAU), end(bufAU), begin(bufBU), begin(e), [](U::e_t a, U::e_t b) {
            return simd::tou(a) ^ simd::tou(b);
        });
        tor(a ^ b);
        REQUIRE(r == e);
        a ^= b;
        tor(a);
        REQUIRE(r == e);
    }
    SECTION("bit not") {
        std::transform(begin(bufAU), end(bufAU), begin(e), [](U::e_t a) {
            return ~simd::tou(a);
        });
        tor(~a);
        REQUIRE(r == e);
    }
    SECTION("complex expr") {
        std::transform(begin(bufAU), end(bufAU), begin(bufBU), begin(e), [](U::e_t af, U::e_t bf) {
            auto a = simd::tou(af);
            auto b = simd::tou(bf);
            return ~((~a & ~b) | (~a ^ ~b));
        });
        tor(~((~a & ~b) | (~a ^ ~b)));
        REQUIRE(r == e);
    }
}

TEST_CASE("AVX horizontal operations", "[simd_t][x86][avx]") {
    F a = simd::aligned(bufAF.data());
    F::e_t v, v0;
    uint32_t idx;

    SECTION("max") {
        v = a.reduce(simd::max).first_element();
        REQUIRE(v == 2.22944568f);
        idx = (v == a).front();
        REQUIRE(idx == 3);
        v = max_mask(a, a != v).reduce(simd::max).first_element();
        REQUIRE(v == 0.70154146f);
        idx = (v == a).front();
        REQUIRE(idx == 1);
    }
    SECTION("min") {
        v = a.reduce(simd::min).first_element();
        REQUIRE(v == -2.05181630f);
        idx = (v == a).front();
        REQUIRE(idx == 2);
        v = min_mask(a, a != v).reduce(simd::min).first_element();
        REQUIRE(v == -1.57705702f);
        idx = (v == a).front();
        REQUIRE(idx == 5);
    }
    SECTION("sum") {
        v0 = std::accumulate(begin(bufAF), end(bufAF), F::e_t(0));
        v = a.reduce(simd::operator+).first_element();
        REQUIRE(v == Approx(v0));
        v = sum_mask(a, simd::zero()).reduce(simd::operator+).first_element();
        REQUIRE(v == 0);
    }
    SECTION("product") {
        v0 = std::accumulate(begin(bufAF), end(bufAF), F::e_t(1), std::multiplies<F::e_t>());
        v = a.reduce(simd::operator*).first_element();
        REQUIRE(v == Approx(v0));
        v = product_mask(a, simd::zero()).reduce(simd::operator*).first_element();
        REQUIRE(v == 1);
    }
}

TEST_CASE("AVX conditional", "[simd_t][x86][avx]") {
    F af = simd::aligned(bufAF.data());
    U au = simd::aligned(bufAU.data());
    S as = simd::aligned(bufAS.data());
    F bf = simd::aligned(bufBF.data());
    U bu = simd::aligned(bufBU.data());
    S bs = simd::aligned(bufBS.data());

    U sel = af >= bf;
    bool sel0[F::W];
    for (auto& val : sel0) val = false;
    for (auto idx : sel) sel0[idx] = true;

    simd::storage<F> rf(cond(sel, af, bf));
    simd::storage<U> ru(cond(sel, au, bu));
    simd::storage<S> rs(cond(sel, as, bs));

    for (int i = 0; i < F::W; ++i) {
        REQUIRE((rf[i]) == (sel0[i] ? bufAF[i] : bufBF[i]));
        REQUIRE((ru[i]) == (sel0[i] ? bufAU[i] : bufBU[i]));
        REQUIRE((rs[i]) == (sel0[i] ? bufAS[i] : bufBS[i]));
    }
}
