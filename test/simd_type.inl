//
// common SIMD type tests
//
// following macros are expected to be defined
// SIMD_TYPE -- name of the SIMD type as a string
// SIMD_TEST_TAG -- catch tests tag(s) as a string
// SIMD_WIDTH -- expected SIMD width
// SIMD_LOAD_AF -- expression that extracts vector_t from bufAF
// SIMD_LOAD_AU -- expression that extracts vector_t from bufAU
// SIMD_LOAD_AS -- expression that extracts vector_t from bufAS
//

#define VAL(TYPE) \
    std::declval< TYPE >()

#define HAS_METHOD(WHO, NAME_ARGS, RETURN_TYPE) \
    (std::is_same< std::remove_cv<std::remove_reference< \
    decltype( VAL(WHO) . NAME_ARGS ) >::type>::type, \
    RETURN_TYPE >::value)

#define ASSERT(COND) \
    static_assert( COND , SIMD_TYPE " compile-time check" )

ASSERT(F::width == SIMD_WIDTH);
ASSERT(U::width == SIMD_WIDTH);
ASSERT(S::width == SIMD_WIDTH);
ASSERT((std::is_same<F::scalar_t, float>::value));
ASSERT((std::is_same<U::scalar_t, uint32_t>::value));
ASSERT((std::is_same<S::scalar_t, int32_t>::value));
ASSERT((std::is_same<U::mask_t, sd::mask<U>>::value));
ASSERT((std::is_same<F::storage_t, sd::storage<F>>::value));
ASSERT((std::is_same<U::storage_t, sd::storage<U>>::value));
ASSERT((std::is_same<S::storage_t, sd::storage<S>>::value));
ASSERT(sizeof(F) == sizeof(F::vector_t));
ASSERT(sizeof(U) == sizeof(U::vector_t));
ASSERT(sizeof(S) == sizeof(S::vector_t));
ASSERT(alignof(F) == alignof(F::vector_t));
ASSERT(alignof(U) == alignof(U::vector_t));
ASSERT(alignof(S) == alignof(S::vector_t));
ASSERT(sizeof(F::scalar_t) * F::width == sizeof(F));
ASSERT(sizeof(U::scalar_t) * U::width == sizeof(U));
ASSERT(sizeof(S::scalar_t) * S::width == sizeof(S));
ASSERT((std::is_trivially_copyable<F>::value));
ASSERT((std::is_trivially_copyable<U>::value));
ASSERT((std::is_trivially_copyable<S>::value));
ASSERT((sd::is_simd_type<F>::value));
ASSERT((sd::is_simd_type<U>::value));
ASSERT((sd::is_simd_type<S>::value));
ASSERT(HAS_METHOD(const F, self(), F));
ASSERT(HAS_METHOD(const U, self(), U));
ASSERT(HAS_METHOD(const S, self(), S));
ASSERT(HAS_METHOD(const F, eval(), F));
ASSERT(HAS_METHOD(const U, eval(), U));
ASSERT(HAS_METHOD(const S, eval(), S));
ASSERT(HAS_METHOD(const F, data(), F::vector_t));
ASSERT(HAS_METHOD(const U, data(), U::vector_t));
ASSERT(HAS_METHOD(const S, data(), S::vector_t));
ASSERT(HAS_METHOD(F, aligned_load(VAL(F::scalar_t*)), void));
ASSERT(HAS_METHOD(U, aligned_load(VAL(U::scalar_t*)), void));
ASSERT(HAS_METHOD(S, aligned_load(VAL(S::scalar_t*)), void));
ASSERT(HAS_METHOD(const F, aligned_store(VAL(F::scalar_t*)), void));
ASSERT(HAS_METHOD(const U, aligned_store(VAL(U::scalar_t*)), void));
ASSERT(HAS_METHOD(const S, aligned_store(VAL(S::scalar_t*)), void));
ASSERT(HAS_METHOD(F, unaligned_load(VAL(F::scalar_t*)), void));
ASSERT(HAS_METHOD(U, unaligned_load(VAL(U::scalar_t*)), void));
ASSERT(HAS_METHOD(S, unaligned_load(VAL(S::scalar_t*)), void));
ASSERT(HAS_METHOD(const F, unaligned_store(VAL(F::scalar_t*)), void));
ASSERT(HAS_METHOD(const U, unaligned_store(VAL(U::scalar_t*)), void));
ASSERT(HAS_METHOD(const S, unaligned_store(VAL(S::scalar_t*)), void));
ASSERT(HAS_METHOD(F, interleaved_load(VAL(F::scalar_t*), VAL(std::size_t)), void));
ASSERT(HAS_METHOD(U, interleaved_load(VAL(U::scalar_t*), VAL(std::size_t)), void));
ASSERT(HAS_METHOD(S, interleaved_load(VAL(S::scalar_t*), VAL(std::size_t)), void));
ASSERT(HAS_METHOD(const F, interleaved_store(VAL(F::scalar_t*), VAL(std::size_t)), void));
ASSERT(HAS_METHOD(const U, interleaved_store(VAL(U::scalar_t*), VAL(std::size_t)), void));
ASSERT(HAS_METHOD(const S, interleaved_store(VAL(S::scalar_t*), VAL(std::size_t)), void));
ASSERT(HAS_METHOD(const F, reduce(VAL(F::binary_op_t)), F));
ASSERT(HAS_METHOD(const U, reduce(VAL(U::binary_op_t)), U));
ASSERT(HAS_METHOD(const S, reduce(VAL(S::binary_op_t)), S));
ASSERT(HAS_METHOD(const U, mask(), U::mask_t));
ASSERT(HAS_METHOD(const F, first_element(), F::scalar_t));
ASSERT(HAS_METHOD(const U, first_element(), U::scalar_t));
ASSERT(HAS_METHOD(const S, first_element(), S::scalar_t));

TEST_CASE(SIMD_TYPE " explicit construction", SIMD_TEST_TAG) {
    F::storage_t rf = bufZF;
    U::storage_t ru = bufZU;
    S::storage_t rs = bufZS;

    auto tor = [&rf, &ru, &rs](const F& tf, const U& tu, const S& ts) {
        rf = tf; ru = tu; rs = ts;
    };

    SECTION("from scalar_t") {
        F tf(1.2345678f);
        U tu(123456789U);
        S ts(-123456789);
        tor(tf, tu, ts);
        for (auto val : rf) REQUIRE(val == 1.2345678f);
        for (auto val : ru) REQUIRE(val == 123456789U);
        for (auto val : rs) REQUIRE(val == -123456789);
    }
    SECTION("from vector_t") {
        F tf(SIMD_LOAD_AF);
        U tu(SIMD_LOAD_AU);
        S ts(SIMD_LOAD_AS);
        tor(tf, tu, ts);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from aligned pointer") {
        F tf(sd::aligned(bufAF.data()));
        U tu(sd::aligned(bufAU.data()));
        S ts(sd::aligned(bufAS.data()));
        tor(tf, tu, ts);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from unaligned pointer") {
        F tf(sd::unaligned(bufAF.data()));
        U tu(sd::unaligned(bufAU.data()));
        S ts(sd::unaligned(bufAS.data()));
        tor(tf, tu, ts);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from storage_t") {
        F::storage_t storf;
        U::storage_t storu;
        S::storage_t stors;
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
    SECTION("from uval, sval, zero, etc. (sd::init family)") {
        {
            F tf(sd::fval(1.2345678f));
            U tu(sd::fval(1.2345678f));
            S ts(sd::fval(1.2345678f));
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(sd::tof(val) == 1.2345678f);
            for (auto val : ru) REQUIRE(sd::tof(val) == 1.2345678f);
            for (auto val : rs) REQUIRE(sd::tof(val) == 1.2345678f);
        }
        {
            F tf(sd::uval(0xdeadbeef));
            U tu(sd::uval(0xdeadbeef));
            S ts(sd::uval(0xdeadbeef));
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(sd::tou(val) == 0xdeadbeef);
            for (auto val : ru) REQUIRE(sd::tou(val) == 0xdeadbeef);
            for (auto val : rs) REQUIRE(sd::tou(val) == 0xdeadbeef);
        }
        {
            F tf(sd::sval(-123456789));
            U tu(sd::sval(-123456789));
            S ts(sd::sval(-123456789));
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(sd::tos(val) == -123456789);
            for (auto val : ru) REQUIRE(sd::tos(val) == -123456789);
            for (auto val : rs) REQUIRE(sd::tos(val) == -123456789);
        }
        {
            F tf(sd::zero());
            U tu(sd::zero());
            S ts(sd::zero());
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(sd::tou(val) == 0x00000000);
            for (auto val : ru) REQUIRE(sd::tou(val) == 0x00000000);
            for (auto val : rs) REQUIRE(sd::tou(val) == 0x00000000);
        }
        {
            F tf(sd::all_bits());
            U tu(sd::all_bits());
            S ts(sd::all_bits());
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(sd::tou(val) == 0xffffffff);
            for (auto val : ru) REQUIRE(sd::tou(val) == 0xffffffff);
            for (auto val : rs) REQUIRE(sd::tou(val) == 0xffffffff);
        }
        {
            F tf(sd::sign_bit());
            U tu(sd::sign_bit());
            S ts(sd::sign_bit());
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(sd::tou(val) == 0x80000000);
            for (auto val : ru) REQUIRE(sd::tou(val) == 0x80000000);
            for (auto val : rs) REQUIRE(sd::tou(val) == 0x80000000);
        }
        {
            F tf(sd::abs_mask());
            U tu(sd::abs_mask());
            S ts(sd::abs_mask());
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(sd::tou(val) == 0x7fffffff);
            for (auto val : ru) REQUIRE(sd::tou(val) == 0x7fffffff);
            for (auto val : rs) REQUIRE(sd::tou(val) == 0x7fffffff);
        }
    }
}


TEST_CASE(SIMD_TYPE " implicit construction", SIMD_TEST_TAG) {
    F::storage_t rf = bufZF;
    U::storage_t ru = bufZU;
    S::storage_t rs = bufZS;

    auto implicit_test = [&rf, &ru, &rs](const F& tf, const U& tu, const S& ts) {
        rf = tf; ru = tu; rs = ts;
    };

    SECTION("from scalar_t") {
        implicit_test(1.2345678f, 123456789U, -123456789);
        for (auto val : rf) REQUIRE(val == 1.2345678f);
        for (auto val : ru) REQUIRE(val == 123456789U);
        for (auto val : rs) REQUIRE(val == -123456789);
    }
    SECTION("from vector_t") {
        implicit_test(
            SIMD_LOAD_AF,
            SIMD_LOAD_AU,
            SIMD_LOAD_AS);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from aligned pointer") {
        implicit_test(
            sd::aligned(bufAF.data()),
            sd::aligned(bufAU.data()),
            sd::aligned(bufAS.data()));
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from unaligned pointer") {
        implicit_test(
            sd::unaligned(bufAF.data()),
            sd::unaligned(bufAU.data()),
            sd::unaligned(bufAS.data()));
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from storage_t") {
        F::storage_t storf;
        U::storage_t storu;
        S::storage_t stors;
        storf = bufAF;
        storu = bufAU;
        stors = bufAS;
        implicit_test(storf, storu, stors);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from uval, sval, zero, etc. (sd::init family)") {
        implicit_test(sd::fval(1.2345678f), sd::fval(1.2345678f), sd::fval(1.2345678f));
        for (auto val : rf) REQUIRE(sd::tof(val) == 1.2345678f);
        for (auto val : ru) REQUIRE(sd::tof(val) == 1.2345678f);
        for (auto val : rs) REQUIRE(sd::tof(val) == 1.2345678f);
        implicit_test(sd::uval(0xdeadbeef), sd::uval(0xdeadbeef), sd::uval(0xdeadbeef));
        for (auto val : rf) REQUIRE(sd::tou(val) == 0xdeadbeef);
        for (auto val : ru) REQUIRE(sd::tou(val) == 0xdeadbeef);
        for (auto val : rs) REQUIRE(sd::tou(val) == 0xdeadbeef);
        implicit_test(sd::sval(-123456789), sd::sval(-123456789), sd::sval(-123456789));
        for (auto val : rf) REQUIRE(sd::tos(val) == -123456789);
        for (auto val : ru) REQUIRE(sd::tos(val) == -123456789);
        for (auto val : rs) REQUIRE(sd::tos(val) == -123456789);
        implicit_test(sd::zero(), sd::zero(), sd::zero());
        for (auto val : rf) REQUIRE(sd::tou(val) == 0x00000000);
        for (auto val : ru) REQUIRE(sd::tou(val) == 0x00000000);
        for (auto val : rs) REQUIRE(sd::tou(val) == 0x00000000);
        implicit_test(sd::all_bits(), sd::all_bits(), sd::all_bits());
        for (auto val : rf) REQUIRE(sd::tou(val) == 0xffffffff);
        for (auto val : ru) REQUIRE(sd::tou(val) == 0xffffffff);
        for (auto val : rs) REQUIRE(sd::tou(val) == 0xffffffff);
        implicit_test(sd::sign_bit(), sd::sign_bit(), sd::sign_bit());
        for (auto val : rf) REQUIRE(sd::tou(val) == 0x80000000);
        for (auto val : ru) REQUIRE(sd::tou(val) == 0x80000000);
        for (auto val : rs) REQUIRE(sd::tou(val) == 0x80000000);
        implicit_test(sd::abs_mask(), sd::abs_mask(), sd::abs_mask());
        for (auto val : rf) REQUIRE(sd::tou(val) == 0x7fffffff);
        for (auto val : ru) REQUIRE(sd::tou(val) == 0x7fffffff);
        for (auto val : rs) REQUIRE(sd::tou(val) == 0x7fffffff);
    }
}

TEST_CASE(SIMD_TYPE " assignment", SIMD_TEST_TAG) {
    F::storage_t rf = bufZF;
    U::storage_t ru = bufZU;
    S::storage_t rs = bufZS;
    F tf; U tu; S ts;

    auto tor = [&rf, &tf, &ru, &tu, &rs, &ts]() {
        rf = tf;
        ru = tu;
        rs = ts;
    };

    SECTION("from scalar_t") {
        tf = 1.2345678f;
        tu = 123456789U;
        ts = -123456789;
        tor();
        for (auto val : rf) REQUIRE(val == 1.2345678f);
        for (auto val : ru) REQUIRE(val == 123456789U);
        for (auto val : rs) REQUIRE(val == -123456789);
    }
    SECTION("from vector_t") {
        tf = SIMD_LOAD_AF;
        tu = SIMD_LOAD_AU;
        ts = SIMD_LOAD_AS;
        tor();
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from aligned pointer") {
        tf = bufAF;
        tu = bufAU;
        ts = bufAS;
        tor();
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from unaligned pointer") {
        tf = bufAF;
        tu = bufAU;
        ts = bufAS;
        tor();
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from storage_t") {
        F::storage_t storf;
        U::storage_t storu;
        S::storage_t stors;
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
    SECTION("from uval, sval, zero, etc. (sd::init family)") {
        tf = sd::fval(1.2345678f);
        tu = sd::fval(1.2345678f);
        ts = sd::fval(1.2345678f);
        tor();
        for (auto val : rf) REQUIRE(sd::tof(val) == 1.2345678f);
        for (auto val : ru) REQUIRE(sd::tof(val) == 1.2345678f);
        for (auto val : rs) REQUIRE(sd::tof(val) == 1.2345678f);
        tf = sd::uval(0xdeadbeef);
        tu = sd::uval(0xdeadbeef);
        ts = sd::uval(0xdeadbeef);
        tor();
        for (auto val : rf) REQUIRE(sd::tou(val) == 0xdeadbeef);
        for (auto val : ru) REQUIRE(sd::tou(val) == 0xdeadbeef);
        for (auto val : rs) REQUIRE(sd::tou(val) == 0xdeadbeef);
        tf = sd::sval(-123456789);
        tu = sd::sval(-123456789);
        ts = sd::sval(-123456789);
        tor();
        for (auto val : rf) REQUIRE(sd::tos(val) == -123456789);
        for (auto val : ru) REQUIRE(sd::tos(val) == -123456789);
        for (auto val : rs) REQUIRE(sd::tos(val) == -123456789);
        tf = sd::zero();
        tu = sd::zero();
        ts = sd::zero();
        tor();
        for (auto val : rf) REQUIRE(sd::tou(val) == 0x00000000);
        for (auto val : ru) REQUIRE(sd::tou(val) == 0x00000000);
        for (auto val : rs) REQUIRE(sd::tou(val) == 0x00000000);
        tf = sd::all_bits();
        tu = sd::all_bits();
        ts = sd::all_bits();
        tor();
        for (auto val : rf) REQUIRE(sd::tou(val) == 0xffffffff);
        for (auto val : ru) REQUIRE(sd::tou(val) == 0xffffffff);
        for (auto val : rs) REQUIRE(sd::tou(val) == 0xffffffff);
        tf = sd::sign_bit();
        tu = sd::sign_bit();
        ts = sd::sign_bit();
        tor();
        for (auto val : rf) REQUIRE(sd::tou(val) == 0x80000000);
        for (auto val : ru) REQUIRE(sd::tou(val) == 0x80000000);
        for (auto val : rs) REQUIRE(sd::tou(val) == 0x80000000);
        tf = sd::abs_mask();
        tu = sd::abs_mask();
        ts = sd::abs_mask();
        tor();
        for (auto val : rf) REQUIRE(sd::tou(val) == 0x7fffffff);
        for (auto val : ru) REQUIRE(sd::tou(val) == 0x7fffffff);
        for (auto val : rs) REQUIRE(sd::tou(val) == 0x7fffffff);
    }
}

TEST_CASE(SIMD_TYPE " type conversion", SIMD_TEST_TAG) {
    SECTION("int to float") {
        F::storage_t expected, result;
        std::transform(begin(bufAS), end(bufAS), begin(expected), [](S::scalar_t a) {
            return static_cast<F::scalar_t>(a);
        });
        S in = bufAS;
        result = F(in);
        REQUIRE(result == expected);
    }
    SECTION("float to int") {
        S::storage_t expected, result;
        std::transform(begin(bufAF), end(bufAF), begin(expected), [](F::scalar_t a) {
            return sd::round_to_int32(a);
        });
        F in = bufAF;
        result = S(in);
        REQUIRE(result == expected);
    }
    SECTION("int to uint") {
        U::storage_t expected, result;
        std::transform(begin(bufAS), end(bufAS), begin(expected), [](S::scalar_t a) {
            return static_cast<U::scalar_t>(a);
        });
        S in = bufAS;
        result = U(in);
        REQUIRE(result == expected);
    }
    SECTION("uint to int") {
        S::storage_t expected, result;
        std::transform(begin(bufAU), end(bufAU), begin(expected), [](U::scalar_t a) {
            return sd::round_to_int32(a);
        });
        U in = bufAU;
        result = S(in);
        REQUIRE(result == expected);
    }
}

TEST_CASE(SIMD_TYPE " float arithmetic", SIMD_TEST_TAG) {
    using scalar_t = F::scalar_t;
    F::storage_t r, e;
    F a = bufAF;
    F b = bufBF;

    auto expect1 = [&e](scalar_t(*f)(scalar_t)) {
        std::transform(begin(bufAF), end(bufAF), begin(e), f);
    };
    auto expect = [&e](scalar_t(*f)(scalar_t, scalar_t)) {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), f);
    };

    SECTION("unary plus") {
        expect1([](scalar_t a) { return +a; });
        r = +a;
        REQUIRE(r == e);
    }
    SECTION("unary minus") {
        expect1([](scalar_t a) { return -a; });
        r = -a;
        REQUIRE(r == e);
    }
    SECTION("plus") {
        expect([](scalar_t a, scalar_t b) { return a + b; });
        r = a + b;
        REQUIRE(r == e);
        a += b;
        r = a;
        REQUIRE(r == e);
    }
    SECTION("minus") {
        expect([](scalar_t a, scalar_t b) { return a - b; });
        r = a - b;
        REQUIRE(r == e);
        a -= b;
        r = a;
        REQUIRE(r == e);
    }
    SECTION("multiplies") {
        expect([](scalar_t a, scalar_t b) { return a * b; });
        r = a * b;
        REQUIRE(r == e);
        a *= b;
        r = a;
        REQUIRE(r == e);
    }
    SECTION("divides") {
        expect([](scalar_t a, scalar_t b) { return a / b; });
        r = a / b;
        REQUIRE(r == e);
        a /= b;
        r = a;
        REQUIRE(r == e);
    }
    SECTION("minimum") {
        expect([](scalar_t a, scalar_t b) { return std::min(a, b); });
        r = min(a, b);
        REQUIRE(r == e);
    }
    SECTION("maximum") {
        expect([](scalar_t a, scalar_t b) { return std::max(a, b); });
        r = max(a, b);
        REQUIRE(r == e);
    }
    SECTION("absolute value") {
        expect1([](scalar_t a) { return std::abs(a); });
        r = abs(a);
        REQUIRE(r == e);
    }
    SECTION("signum") {
        expect1([](scalar_t a) { return std::signbit(a) ? scalar_t(-1) : scalar_t(1); });
        r = signum(a);
        REQUIRE(r == e);
    }
    SECTION("square root") {
        expect1([](scalar_t a) { return std::sqrt(std::abs(a)); });
        r = sqrt(abs(a));
        for (int i = 0; i < F::width; ++i) REQUIRE(r[i] == Approx(e[i]));
    }
    SECTION("fast reciprocal") {
        expect1([](scalar_t a) { return 1 / a; });
        r = rcp(a);
        for (int i = 0; i < F::width; ++i) REQUIRE(r[i] == Approx(e[i]).epsilon(0.001));
    }
    SECTION("fast reciprocal square root") {
        expect1([](scalar_t a) { return 1 / std::sqrt(std::abs(a)); });
        r = rsqrt(abs(a));
        for (int i = 0; i < F::width; ++i) REQUIRE(r[i] == Approx(e[i]).epsilon(0.001));
    }
    SECTION("rhs of compound can be implicitly constructed") {
        a = b;
        a += 1.23f; b = b + 1.23f;
        REQUIRE(all(a == b));
        a -= 2.34f; b = b - 2.34f;
        REQUIRE(all(a == b));
        a *= 3.45f; b = b * 3.45f;
        REQUIRE(all(a == b));
        a /= 4.56f; b = b / 4.56f;
        REQUIRE(all(a == b));
    }
}

TEST_CASE(SIMD_TYPE " uint arithmetic", SIMD_TEST_TAG) {
    using scalar_t = U::scalar_t;
    U::storage_t r, e;
    U a = bufAU;
    U b = bufBU;

    auto expect1 = [&e](scalar_t(*f)(scalar_t)) {
        std::transform(begin(bufAU), end(bufAU), begin(e), f);
    };
    auto expect = [&e](scalar_t(*f)(scalar_t, scalar_t)) {
        std::transform(begin(bufAU), end(bufAU), begin(bufBU), begin(e), f);
    };

    SECTION("bit not") {
        expect1([](scalar_t a) { return ~a; });
        r = ~a;
        REQUIRE(r == e);
    }
    SECTION("bit and") {
        expect([](scalar_t a, scalar_t b) { return a & b; });
        r = a & b;
        REQUIRE(r == e);
        a &= b;
        r = a;
        REQUIRE(r == e);
    }
    SECTION("bit or") {
        expect([](scalar_t a, scalar_t b) { return a | b; });
        r = a | b;
        REQUIRE(r == e);
        a |= b;
        r = a;
        REQUIRE(r == e);
    }
    SECTION("bit xor") {
        expect([](scalar_t a, scalar_t b) { return a ^ b; });
        r = a ^ b;
        REQUIRE(r == e);
        a ^= b;
        r = a;
        REQUIRE(r == e);
    }
    SECTION("bit andnot") {
        expect([](scalar_t a, scalar_t b) { return a & ~b; });
        r = a & ~b;
        REQUIRE(r == e);
        r = andnot(a, b);
        REQUIRE(r == e);
    }
    SECTION("complex expr") {
        expect([](scalar_t a, scalar_t b) { return ~((~a & ~b) | (~a ^ ~b)); });
        r = ~((~a & ~b) | (~a ^ ~b));
        REQUIRE(r == e);
    }
    SECTION("rhs of compound can be implicitly constructed") {
        a = b;
        a &= 0xdeadbeefU; b = b & 0xdeadbeefU;
        REQUIRE(all(a == b));
        a |= 0xf0f0f0f0U; b = b | 0xf0f0f0f0U;
        REQUIRE(all(a == b));
        a ^= 0x1234abcdU; b = b ^ 0x1234abcdU;
        REQUIRE(all(a == b));
    }
}

TEST_CASE(SIMD_TYPE " int arithmetic", SIMD_TEST_TAG) {
    using scalar_t = S::scalar_t;
    S::storage_t r, e;
    S a = bufAS;
    S b = bufBS;

    auto expect1 = [&e](scalar_t(*f)(scalar_t)) {
        std::transform(begin(bufAS), end(bufAS), begin(e), f);
    };
    auto expect = [&e](scalar_t(*f)(scalar_t, scalar_t)) {
        std::transform(begin(bufAS), end(bufAS), begin(bufBS), begin(e), f);
    };

    SECTION("unary plus") {
        expect1([](scalar_t a) { return +a; });
        r = +a;
        REQUIRE(r == e);
    }
    SECTION("unary minus") {
        expect1([](scalar_t a) { return -a; });
        r = -a;
        REQUIRE(r == e);
    }
    SECTION("plus") {
        expect([](scalar_t a, scalar_t b) { return a + b; });
        r = a + b;
        REQUIRE(r == e);
        a += b;
        r = a;
        REQUIRE(r == e);
    }
    SECTION("minus") {
        expect([](scalar_t a, scalar_t b) { return a - b; });
        r = a - b;
        REQUIRE(r == e);
        a -= b;
        r = a;
        REQUIRE(r == e);
    }
    SECTION("multiplies") {
        expect([](scalar_t a, scalar_t b) { return a * b; });
        r = a * b;
        REQUIRE(r == e);
        a *= b;
        r = a;
        REQUIRE(r == e);
    }
    SECTION("minimum") {
        expect([](scalar_t a, scalar_t b) { return std::min(a, b); });
        r = min(a, b);
        REQUIRE(r == e);
    }
    SECTION("maximum") {
        expect([](scalar_t a, scalar_t b) { return std::max(a, b); });
        r = max(a, b);
        REQUIRE(r == e);
    }
    SECTION("absolute value") {
        expect1([](scalar_t a) { return std::abs(a); });
        r = abs(a);
        REQUIRE(r == e);
    }
    SECTION("signum") {
        expect1([](scalar_t a) { return (a < 0) ? scalar_t(-1) : scalar_t(1); });
        r = signum(a);
        REQUIRE(r == e);
    }
    SECTION("rhs of compound can be implicitly constructed") {
        a = b;
        a += 123; b = b + 123;
        REQUIRE(all(a == b));
        a -= 234; b = b - 234;
        REQUIRE(all(a == b));
        a *= 345; b = b * 345;
        REQUIRE(all(a == b));
    }
}

TEST_CASE(SIMD_TYPE " float comparison", SIMD_TEST_TAG) {
    using scalar_t = F::scalar_t;
    U::storage_t r, e;
    F a = bufAF;
    F b = bufBF;

    auto expect0 = [&e](bool v) {
        std::fill(begin(e), end(e), v ? ~0U : 0U);
    };
    auto expect = [&e](bool(*f)(scalar_t, scalar_t)) {
        auto f2 = [f](scalar_t a, scalar_t b) {
            return f(a, b) ? ~0U : 0U;
        };
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), f2);
    };

    SECTION("equal to") {
        expect([](scalar_t a, scalar_t b) { return a == b; });
        r = a == b;
        REQUIRE(r == e);
        expect0(true);
        r = a == a;
        REQUIRE(r == e);
    }
    SECTION("not equal to") {
        expect([](scalar_t a, scalar_t b) { return a != b; });
        r = a != b;
        REQUIRE(r == e);
        expect0(false);
        r = a != a;
        REQUIRE(r == e);
    }
    SECTION("greater") {
        expect([](scalar_t a, scalar_t b) { return a > b; });
        r = a > b;
        REQUIRE(r == e);
        expect0(false);
        r = a > a;
        REQUIRE(r == e);
    }
    SECTION("less") {
        expect([](scalar_t a, scalar_t b) { return a < b; });
        r = a < b;
        REQUIRE(r == e);
        expect0(false);
        r = a < a;
        REQUIRE(r == e);
    }
    SECTION("greater equal") {
        expect([](scalar_t a, scalar_t b) { return a >= b; });
        r = a >= b;
        REQUIRE(r == e);
        expect0(true);
        r = a >= a;
        REQUIRE(r == e);
    }
    SECTION("less equal") {
        expect([](scalar_t a, scalar_t b) { return a <= b; });
        r = a <= b;
        REQUIRE(r == e);
        expect0(true);
        r = a <= a;
        REQUIRE(r == e);
    }
}

TEST_CASE(SIMD_TYPE " uint comparison", SIMD_TEST_TAG) {
    using scalar_t = U::scalar_t;
    U::storage_t r, e;
    U a = bufAU;
    U b = bufBU;

    auto expect0 = [&e](bool v) {
        std::fill(begin(e), end(e), v ? ~0U : 0U);
    };
    auto expect = [&e](bool(*f)(scalar_t, scalar_t)) {
        auto f2 = [f](scalar_t a, scalar_t b) {
            return f(a, b) ? ~0U : 0U;
        };
        std::transform(begin(bufAU), end(bufAU), begin(bufBU), begin(e), f2);
    };

    SECTION("equal to") {
        expect([](scalar_t a, scalar_t b) { return a == b; });
        r = a == b;
        REQUIRE(r == e);
        expect0(true);
        r = a == a;
        REQUIRE(r == e);
    }
    SECTION("not equal to") {
        expect([](scalar_t a, scalar_t b) { return a != b; });
        r = a != b;
        REQUIRE(r == e);
        expect0(false);
        r = a != a;
        REQUIRE(r == e);
    }
}

TEST_CASE(SIMD_TYPE " int comparison", SIMD_TEST_TAG) {
    using scalar_t = S::scalar_t;
    U::storage_t r, e;
    S a = bufAS;
    S b = bufBS;

    auto expect0 = [&e](bool v) {
        std::fill(begin(e), end(e), v ? ~0U : 0U);
    };
    auto expect = [&e](bool(*f)(scalar_t, scalar_t)) {
        auto f2 = [f](scalar_t a, scalar_t b) {
            return f(a, b) ? ~0U : 0U;
        };
        std::transform(begin(bufAS), end(bufAS), begin(bufBS), begin(e), f2);
    };

    SECTION("equal to") {
        expect([](scalar_t a, scalar_t b) { return a == b; });
        r = a == b;
        REQUIRE(r == e);
        expect0(true);
        r = a == a;
        REQUIRE(r == e);
    }
    SECTION("not equal to") {
        expect([](scalar_t a, scalar_t b) { return a != b; });
        r = a != b;
        REQUIRE(r == e);
        expect0(false);
        r = a != a;
        REQUIRE(r == e);
    }
    SECTION("greater") {
        expect([](scalar_t a, scalar_t b) { return a > b; });
        r = a > b;
        REQUIRE(r == e);
        expect0(false);
        r = a > a;
        REQUIRE(r == e);
    }
    SECTION("less") {
        expect([](scalar_t a, scalar_t b) { return a < b; });
        r = a < b;
        REQUIRE(r == e);
        expect0(false);
        r = a < a;
        REQUIRE(r == e);
    }
    SECTION("greater equal") {
        expect([](scalar_t a, scalar_t b) { return a >= b; });
        r = a >= b;
        REQUIRE(r == e);
        expect0(true);
        r = a >= a;
        REQUIRE(r == e);
    }
    SECTION("less equal") {
        expect([](scalar_t a, scalar_t b) { return a <= b; });
        r = a <= b;
        REQUIRE(r == e);
        expect0(true);
        r = a <= a;
        REQUIRE(r == e);
    }
}

TEST_CASE(SIMD_TYPE " horizontal operations", SIMD_TEST_TAG) {
    using scalar_t = F::scalar_t;
    F a = bufAF;
    scalar_t r, e;

    SECTION("max") {
        constexpr scalar_t inf = std::numeric_limits<scalar_t>::infinity();
        auto max = [](scalar_t l, scalar_t r) { return std::max(l, r); };
        e = std::accumulate(begin(bufAF), end(bufAF), -inf, max);
        r = a.reduce(sd::max).first_element();
        REQUIRE(r == e);
    }
    SECTION("min") {
        constexpr scalar_t inf = std::numeric_limits<scalar_t>::infinity();
        auto min = [](scalar_t l, scalar_t r) { return std::min(l, r); };
        e = std::accumulate(begin(bufAF), end(bufAF), inf, min);
        r = a.reduce(sd::min).first_element();
        REQUIRE(r == e);
    }
    SECTION("sum") {
        e = std::accumulate(begin(bufAF), end(bufAF), scalar_t(0));
        r = a.reduce(sd::operator+).first_element();
        REQUIRE(r == Approx(e));
    }
    SECTION("product") {
        auto prod = std::multiplies<scalar_t>();
        e = std::accumulate(begin(bufAF), end(bufAF), scalar_t(1), prod);
        r = a.reduce(sd::operator*).first_element();
        REQUIRE(r == Approx(e));
    }
}

TEST_CASE(SIMD_TYPE " conditional", SIMD_TEST_TAG) {
    F af = bufAF;
    U au = bufAU;
    S as = bufAS;
    F bf = bufBF;
    U bu = bufBU;
    S bs = bufBS;

    U sel = af >= bf;
    auto mask = sel.mask();

    F::storage_t rf(cond(sel, af, bf));
    U::storage_t ru(cond(sel, au, bu));
    S::storage_t rs(cond(sel, as, bs));

    for (int i = 0; i < F::width; ++i) {
        REQUIRE((rf[i]) == (mask[i] ? bufAF[i] : bufBF[i]));
        REQUIRE((ru[i]) == (mask[i] ? bufAU[i] : bufBU[i]));
        REQUIRE((rs[i]) == (mask[i] ? bufAS[i] : bufBS[i]));
    }
}

TEST_CASE(SIMD_TYPE " mask() method", SIMD_TEST_TAG) {
    SECTION("mask() itself") {
        auto expected = [](const U::storage_t& s) {
            U::mask_t res(0U);
            for (auto i = 0U; i < s.size(); ++i) {
                if (s[i] & (1U << 31)) {
                    res |= U::mask_t(1U << i);
                }
            }
            return res;
        };

        REQUIRE(expected(bufAU) == U(bufAU).mask());
        REQUIRE(expected(bufBU) == U(bufBU).mask());
    }
    SECTION("any, all") {
        U a = bufAU;
        U b = bufBU;
        REQUIRE(any(a) == a.mask().any());
        REQUIRE(any(b) == b.mask().any());
        REQUIRE(all(a) == a.mask().all());
        REQUIRE(all(b) == b.mask().all());
    }
}
