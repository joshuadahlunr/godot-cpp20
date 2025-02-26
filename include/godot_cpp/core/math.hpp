/**************************************************************************/
/*  math.hpp                                                              */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef GODOT_MATH_HPP
#define GODOT_MATH_HPP

#include <godot_cpp/core/defs.hpp>

#include <gdextension_interface.h>

#include <cmath>

namespace godot {

#define Math_SQRT12 0.7071067811865475244008443621048490
#define Math_SQRT2 1.4142135623730950488016887242
#define Math_LN2 0.6931471805599453094172321215
#define Math_PI 3.1415926535897932384626433833
#define Math_TAU 6.2831853071795864769252867666
#define Math_E 2.7182818284590452353602874714
#define Math_INF INFINITY
#define Math_NAN NAN

// Make room for our constexpr's below by overriding potential system-specific macros.
#undef ABS
#undef SIGN
#undef MIN
#undef MAX
#undef CLAMP

// Generic ABS function, for math uses please use Math::abs.
template <typename T>
constexpr T ABS(T m_v) {
	return m_v < 0 ? -m_v : m_v;
}

template <typename T>
constexpr const T SIGN(const T m_v) {
	return m_v == 0 ? 0.0f : (m_v < 0 ? -1.0f : +1.0f);
}

template <typename T, typename T2>
constexpr auto MIN(const T m_a, const T2 m_b) {
	return m_a < m_b ? m_a : m_b;
}

template <typename T, typename T2>
constexpr auto MAX(const T m_a, const T2 m_b) {
	return m_a > m_b ? m_a : m_b;
}

template <typename T, typename T2, typename T3>
constexpr auto CLAMP(const T m_a, const T2 m_min, const T3 m_max) {
	return m_a < m_min ? m_min : (m_a > m_max ? m_max : m_a);
}

// Generic swap template.
#ifndef SWAP
#define SWAP(m_x, m_y) __swap_tmpl((m_x), (m_y))
template <typename T>
inline void __swap_tmpl(T &x, T &y) {
	T aux = x;
	x = y;
	y = aux;
}
#endif // SWAP

/* Functions to handle powers of 2 and shifting. */

// Function to find the next power of 2 to an integer.
static _FORCE_INLINE_ unsigned int next_power_of_2(unsigned int x) {
	if (x == 0) {
		return 0;
	}

	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;

	return ++x;
}

// Function to find the previous power of 2 to an integer.
static _FORCE_INLINE_ unsigned int previous_power_of_2(unsigned int x) {
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x - (x >> 1);
}

// Function to find the closest power of 2 to an integer.
static _FORCE_INLINE_ unsigned int closest_power_of_2(unsigned int x) {
	unsigned int nx = next_power_of_2(x);
	unsigned int px = previous_power_of_2(x);
	return (nx - x) > (x - px) ? px : nx;
}

// Get a shift value from a power of 2.
static inline int get_shift_from_power_of_2(unsigned int p_bits) {
	for (unsigned int i = 0; i < 32; i++) {
		if (p_bits == (unsigned int)(1 << i)) {
			return i;
		}
	}

	return -1;
}

template <typename T>
static _FORCE_INLINE_ T nearest_power_of_2_templated(T x) {
	--x;

	// The number of operations on x is the base two logarithm
	// of the number of bits in the type. Add three to account
	// for sizeof(T) being in bytes.
	size_t num = get_shift_from_power_of_2(sizeof(T)) + 3;

	// If the compiler is smart, it unrolls this loop.
	// If it's dumb, this is a bit slow.
	for (size_t i = 0; i < num; i++) {
		x |= x >> (1 << i);
	}

	return ++x;
}

// Function to find the nearest (bigger) power of 2 to an integer.
static inline unsigned int nearest_shift(unsigned int p_number) {
	for (int i = 30; i >= 0; i--) {
		if (p_number & (1 << i)) {
			return i + 1;
		}
	}

	return 0;
}

// constexpr function to find the floored log2 of a number
template <typename T>
constexpr T floor_log2(T x) {
	return x < 2 ? x : 1 + floor_log2(x >> 1);
}

// Get the number of bits needed to represent the number.
// IE, if you pass in 8, you will get 4.
// If you want to know how many bits are needed to store 8 values however, pass in (8 - 1).
template <typename T>
constexpr T get_num_bits(T x) {
	return floor_log2(x);
}

// Swap 16, 32 and 64 bits value for endianness.
#if defined(__GNUC__)
#define BSWAP16(x) __builtin_bswap16(x)
#define BSWAP32(x) __builtin_bswap32(x)
#define BSWAP64(x) __builtin_bswap64(x)
#else
static inline uint16_t BSWAP16(uint16_t x) {
	return (x >> 8) | (x << 8);
}

static inline uint32_t BSWAP32(uint32_t x) {
	return ((x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24));
}

static inline uint64_t BSWAP64(uint64_t x) {
	x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
	x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
	x = (x & 0x00FF00FF00FF00FF) << 8 | (x & 0xFF00FF00FF00FF00) >> 8;
	return x;
}
#endif

namespace Math {

// This epsilon should match the one used by Godot for consistency.
// Using `f` when `real_t` is float.
#define CMP_EPSILON 0.00001f
#define CMP_EPSILON2 (CMP_EPSILON * CMP_EPSILON)

// This epsilon is for values related to a unit size (scalar or vector len).
#ifdef PRECISE_MATH_CHECKS
#define UNIT_EPSILON 0.00001
#else
// Tolerate some more floating point error normally.
#define UNIT_EPSILON 0.001
#endif

// Functions reproduced as in Godot's source code `math_funcs.h`.
// Some are overloads to automatically support changing real_t into either double or float in the way Godot does.

inline double deg_to_rad(double p_y) {
	return p_y * Math_PI / 180.0;
}
inline float deg_to_rad(float p_y) {
	return p_y * static_cast<float>(Math_PI) / 180.f;
}

inline double rad_to_deg(double p_y) {
	return p_y * 180.0 / Math_PI;
}
inline float rad_to_deg(float p_y) {
	return p_y * 180.f / static_cast<float>(Math_PI);
}

class Degree;

/**
 * Radian type (allows automatic worry free conversion between radians and degrees)
 */
class Radian {
	real_t value;
public:
	Radian(const real_t radian) : value(radian) {}
	Radian(const Radian& other) : Radian(other.value) {}
	Radian() : Radian(0) {}
	Radian(const Degree d); 

	Radian& operator=(const Radian& other) = default;

	operator real_t() const { return value; }

	// Radian operator+(const Radian other) const { return Radian{value + other.value}; }
	// Radian operator-(const Radian other) const { return Radian{value - other.value}; }
	// Radian operator*(const Radian other) const { return Radian{value * other.value}; }
	// Radian operator/(const Radian other) const { return Radian{value / other.value}; }

	Radian& operator+=(const Radian other) { *this = *this + other; return *this; }
	Radian& operator-=(const Radian other) { *this = *this - other; return *this; }
	Radian& operator*=(const Radian other) { *this = *this * other; return *this; }
	Radian& operator/=(const Radian other) { *this = *this / other; return *this; }
};

/**
 * Degree type (allows automatic worry free conversion between radians and degrees)
 */
class Degree {
	real_t value;
public:
	Degree(real_t degree) : value(degree) {}
	Degree(const Degree& other) : Degree(other.value) {}
	Degree() : Degree(0) {}
	Degree(const Radian r) : Degree(Math::rad_to_deg(real_t(r))) {}

	Degree& operator=(const Degree& other) = default;

	operator real_t() const { return value; }

	// Degree operator+(const Degree other) const { return Degree{value + other.value}; }
	// Degree operator-(const Degree other) const { return Degree{value - other.value}; }
	// Degree operator*(const Degree other) const { return Degree{value * other.value}; }
	// Degree operator/(const Degree other) const { return Degree{value / other.value}; }

	Degree& operator+=(const Degree other) { *this = *this + other; return *this; }
	Degree& operator-=(const Degree other) { *this = *this - other; return *this; }
	Degree& operator*=(const Degree other) { *this = *this * other; return *this; }
	Degree& operator/=(const Degree other) { *this = *this / other; return *this; }

	real_t RadianValue() { return Radian(*this); }
};

inline Radian::Radian(const Degree d) : Radian(Math::deg_to_rad(real_t(d))) {}

inline double fmod(double p_x, double p_y) {
	return ::fmod(p_x, p_y);
}
inline float fmod(float p_x, float p_y) {
	return ::fmodf(p_x, p_y);
}

inline double fposmod(double p_x, double p_y) {
	double value = Math::fmod(p_x, p_y);
	if ((value < 0 && p_y > 0) || (value > 0 && p_y < 0)) {
		value += p_y;
	}
	value += 0.0;
	return value;
}
inline float fposmod(float p_x, float p_y) {
	float value = Math::fmod(p_x, p_y);
	if ((value < 0 && p_y > 0) || (value > 0 && p_y < 0)) {
		value += p_y;
	}
	value += 0.0f;
	return value;
}

inline float fposmodp(float p_x, float p_y) {
	float value = Math::fmod(p_x, p_y);
	if (value < 0) {
		value += p_y;
	}
	value += 0.0f;
	return value;
}
inline double fposmodp(double p_x, double p_y) {
	double value = Math::fmod(p_x, p_y);
	if (value < 0) {
		value += p_y;
	}
	value += 0.0;
	return value;
}

inline int64_t posmod(int64_t p_x, int64_t p_y) {
	int64_t value = p_x % p_y;
	if ((value < 0 && p_y > 0) || (value > 0 && p_y < 0)) {
		value += p_y;
	}
	return value;
}

inline double floor(double p_x) {
	return ::floor(p_x);
}
inline float floor(float p_x) {
	return ::floorf(p_x);
}

inline double ceil(double p_x) {
	return ::ceil(p_x);
}
inline float ceil(float p_x) {
	return ::ceilf(p_x);
}

inline double exp(double p_x) {
	return ::exp(p_x);
}
inline float exp(float p_x) {
	return ::expf(p_x);
}

inline real_t sin(Radian p_x) {
	return ::sin(p_x);
}

inline real_t cos(Radian p_x) {
	return ::cos(p_x);
}

inline real_t tan(Radian p_x) {
	return ::tan(p_x);
}

inline real_t sinh(Radian p_x) {
	return ::sinh(p_x);
}

inline real_t sinc(Radian p_x) {
	return p_x == 0 ? 1 : ::sin(p_x) / p_x;
}

inline real_t sincn(Radian p_x) {
	return sinc(Math_PI * p_x);
}

inline real_t cosh(Radian p_x) {
	return ::cosh(p_x);
}

inline real_t tanh(Radian p_x) {
	return ::tanh(p_x);
}

inline Radian asin(Radian p_x) {
	return ::asin(p_x);
}

inline Radian acos(double p_x) {
	return ::acos(p_x);
}
inline Radian acos(float p_x) {
	return ::acosf(p_x);
}

inline Radian atan(double p_x) {
	return ::atan(p_x);
}
inline Radian atan(float p_x) {
	return ::atanf(p_x);
}

inline Radian atan2(double p_y, double p_x) {
	return ::atan2(p_y, p_x);
}
inline Radian atan2(float p_y, float p_x) {
	return ::atan2f(p_y, p_x);
}

inline double sqrt(double p_x) {
	return ::sqrt(p_x);
}
inline float sqrt(float p_x) {
	return ::sqrtf(p_x);
}

inline double pow(double p_x, double p_y) {
	return ::pow(p_x, p_y);
}
inline float pow(float p_x, float p_y) {
	return ::powf(p_x, p_y);
}

inline double log(double p_x) {
	return ::log(p_x);
}
inline float log(float p_x) {
	return ::logf(p_x);
}

inline float lerp(float minv, float maxv, float t) {
	return minv + t * (maxv - minv);
}
inline double lerp(double minv, double maxv, double t) {
	return minv + t * (maxv - minv);
}

inline double lerp_angle(double p_from, double p_to, double p_weight) {
	double difference = fmod(p_to - p_from, Math_TAU);
	double distance = fmod(2.0 * difference, Math_TAU) - difference;
	return p_from + distance * p_weight;
}
inline float lerp_angle(float p_from, float p_to, float p_weight) {
	float difference = fmod(p_to - p_from, (float)Math_TAU);
	float distance = fmod(2.0f * difference, (float)Math_TAU) - difference;
	return p_from + distance * p_weight;
}
inline Radian lerp_angle(Radian p_from, Radian p_to, real_t p_weight) {
	Radian difference = fmod(p_to - p_from, (real_t)Math_TAU);
	Radian distance = fmod((real_t)2.0 * difference, (real_t)Math_TAU) - difference;
	return p_from + distance * p_weight;
}
inline Degree lerp_angle(Degree p_from, Degree p_to, real_t p_weight) {
	Degree difference = fmod(p_to - p_from, (real_t)Math_TAU);
	Degree distance = fmod((real_t)2.0 * difference, (real_t)Math_TAU) - difference;
	return p_from + distance * p_weight;
}

inline double cubic_interpolate(double p_from, double p_to, double p_pre, double p_post, double p_weight) {
	return 0.5 *
			((p_from * 2.0) +
					(-p_pre + p_to) * p_weight +
					(2.0 * p_pre - 5.0 * p_from + 4.0 * p_to - p_post) * (p_weight * p_weight) +
					(-p_pre + 3.0 * p_from - 3.0 * p_to + p_post) * (p_weight * p_weight * p_weight));
}

inline float cubic_interpolate(float p_from, float p_to, float p_pre, float p_post, float p_weight) {
	return 0.5f *
			((p_from * 2.0f) +
					(-p_pre + p_to) * p_weight +
					(2.0f * p_pre - 5.0f * p_from + 4.0f * p_to - p_post) * (p_weight * p_weight) +
					(-p_pre + 3.0f * p_from - 3.0f * p_to + p_post) * (p_weight * p_weight * p_weight));
}

inline double cubic_interpolate_angle(double p_from, double p_to, double p_pre, double p_post, double p_weight) {
	double from_rot = fmod(p_from, Math_TAU);

	double pre_diff = fmod(p_pre - from_rot, Math_TAU);
	double pre_rot = from_rot + fmod(2.0 * pre_diff, Math_TAU) - pre_diff;

	double to_diff = fmod(p_to - from_rot, Math_TAU);
	double to_rot = from_rot + fmod(2.0 * to_diff, Math_TAU) - to_diff;

	double post_diff = fmod(p_post - to_rot, Math_TAU);
	double post_rot = to_rot + fmod(2.0 * post_diff, Math_TAU) - post_diff;

	return cubic_interpolate(from_rot, to_rot, pre_rot, post_rot, p_weight);
}

inline float cubic_interpolate_angle(float p_from, float p_to, float p_pre, float p_post, float p_weight) {
	float from_rot = fmod(p_from, (float)Math_TAU);

	float pre_diff = fmod(p_pre - from_rot, (float)Math_TAU);
	float pre_rot = from_rot + fmod(2.0f * pre_diff, (float)Math_TAU) - pre_diff;

	float to_diff = fmod(p_to - from_rot, (float)Math_TAU);
	float to_rot = from_rot + fmod(2.0f * to_diff, (float)Math_TAU) - to_diff;

	float post_diff = fmod(p_post - to_rot, (float)Math_TAU);
	float post_rot = to_rot + fmod(2.0f * post_diff, (float)Math_TAU) - post_diff;

	return cubic_interpolate(from_rot, to_rot, pre_rot, post_rot, p_weight);
}

inline Radian cubic_interpolate_angle(Radian p_from, Radian p_to, Radian p_pre, Radian p_post, real_t p_weight) {
	Radian from_rot = fmod(p_from, (real_t)Math_TAU);

	Radian pre_diff = fmod(p_pre - from_rot, (real_t)Math_TAU);
	Radian pre_rot = from_rot + fmod(2.0f * pre_diff, (real_t)Math_TAU) - pre_diff;

	Radian to_diff = fmod(p_to - from_rot, (real_t)Math_TAU);
	Radian to_rot = from_rot + fmod(2.0f * to_diff, (real_t)Math_TAU) - to_diff;

	Radian post_diff = fmod(p_post - to_rot, (real_t)Math_TAU);
	Radian post_rot = to_rot + fmod(2.0f * post_diff, (real_t)Math_TAU) - post_diff;

	return cubic_interpolate(from_rot, to_rot, pre_rot, post_rot, p_weight);
}

inline Degree cubic_interpolate_angle(Degree p_from, Degree p_to, Degree p_pre, Degree p_post, real_t p_weight) {
	Degree from_rot = fmod(p_from, (real_t)Math_TAU);

	Degree pre_diff = fmod(p_pre - from_rot, (real_t)Math_TAU);
	Degree pre_rot = from_rot + fmod(2.0f * pre_diff, (real_t)Math_TAU) - pre_diff;

	Degree to_diff = fmod(p_to - from_rot, (real_t)Math_TAU);
	Degree to_rot = from_rot + fmod(2.0f * to_diff, (real_t)Math_TAU) - to_diff;

	Degree post_diff = fmod(p_post - to_rot, (real_t)Math_TAU);
	Degree post_rot = to_rot + fmod(2.0f * post_diff, (real_t)Math_TAU) - post_diff;

	return cubic_interpolate(from_rot, to_rot, pre_rot, post_rot, p_weight);
}

inline double cubic_interpolate_in_time(double p_from, double p_to, double p_pre, double p_post, double p_weight,
		double p_to_t, double p_pre_t, double p_post_t) {
	/* Barry-Goldman method */
	double t = Math::lerp(0.0, p_to_t, p_weight);
	double a1 = Math::lerp(p_pre, p_from, p_pre_t == 0 ? 0.0 : (t - p_pre_t) / -p_pre_t);
	double a2 = Math::lerp(p_from, p_to, p_to_t == 0 ? 0.5 : t / p_to_t);
	double a3 = Math::lerp(p_to, p_post, p_post_t - p_to_t == 0 ? 1.0 : (t - p_to_t) / (p_post_t - p_to_t));
	double b1 = Math::lerp(a1, a2, p_to_t - p_pre_t == 0 ? 0.0 : (t - p_pre_t) / (p_to_t - p_pre_t));
	double b2 = Math::lerp(a2, a3, p_post_t == 0 ? 1.0 : t / p_post_t);
	return Math::lerp(b1, b2, p_to_t == 0 ? 0.5 : t / p_to_t);
}

inline float cubic_interpolate_in_time(float p_from, float p_to, float p_pre, float p_post, float p_weight,
		float p_to_t, float p_pre_t, float p_post_t) {
	/* Barry-Goldman method */
	float t = Math::lerp(0.0f, p_to_t, p_weight);
	float a1 = Math::lerp(p_pre, p_from, p_pre_t == 0 ? 0.0f : (t - p_pre_t) / -p_pre_t);
	float a2 = Math::lerp(p_from, p_to, p_to_t == 0 ? 0.5f : t / p_to_t);
	float a3 = Math::lerp(p_to, p_post, p_post_t - p_to_t == 0 ? 1.0f : (t - p_to_t) / (p_post_t - p_to_t));
	float b1 = Math::lerp(a1, a2, p_to_t - p_pre_t == 0 ? 0.0f : (t - p_pre_t) / (p_to_t - p_pre_t));
	float b2 = Math::lerp(a2, a3, p_post_t == 0 ? 1.0f : t / p_post_t);
	return Math::lerp(b1, b2, p_to_t == 0 ? 0.5f : t / p_to_t);
}

inline double cubic_interpolate_angle_in_time(double p_from, double p_to, double p_pre, double p_post, double p_weight,
		double p_to_t, double p_pre_t, double p_post_t) {
	double from_rot = fmod(p_from, Math_TAU);

	double pre_diff = fmod(p_pre - from_rot, Math_TAU);
	double pre_rot = from_rot + fmod(2.0 * pre_diff, Math_TAU) - pre_diff;

	double to_diff = fmod(p_to - from_rot, Math_TAU);
	double to_rot = from_rot + fmod(2.0 * to_diff, Math_TAU) - to_diff;

	double post_diff = fmod(p_post - to_rot, Math_TAU);
	double post_rot = to_rot + fmod(2.0 * post_diff, Math_TAU) - post_diff;

	return cubic_interpolate_in_time(from_rot, to_rot, pre_rot, post_rot, p_weight, p_to_t, p_pre_t, p_post_t);
}

inline float cubic_interpolate_angle_in_time(float p_from, float p_to, float p_pre, float p_post, float p_weight,
		float p_to_t, float p_pre_t, float p_post_t) {
	float from_rot = fmod(p_from, (float)Math_TAU);

	float pre_diff = fmod(p_pre - from_rot, (float)Math_TAU);
	float pre_rot = from_rot + fmod(2.0f * pre_diff, (float)Math_TAU) - pre_diff;

	float to_diff = fmod(p_to - from_rot, (float)Math_TAU);
	float to_rot = from_rot + fmod(2.0f * to_diff, (float)Math_TAU) - to_diff;

	float post_diff = fmod(p_post - to_rot, (float)Math_TAU);
	float post_rot = to_rot + fmod(2.0f * post_diff, (float)Math_TAU) - post_diff;

	return cubic_interpolate_in_time(from_rot, to_rot, pre_rot, post_rot, p_weight, p_to_t, p_pre_t, p_post_t);
}

inline Radian cubic_interpolate_angle_in_time(Radian p_from, Radian p_to, Radian p_pre, Radian p_post, real_t p_weight,
		Radian p_to_t, Radian p_pre_t, Radian p_post_t) {
	Radian from_rot = fmod(p_from, (real_t)Math_TAU);

	Radian pre_diff = fmod(p_pre - from_rot, (real_t)Math_TAU);
	Radian pre_rot = from_rot + fmod(2.0f * pre_diff, (real_t)Math_TAU) - pre_diff;

	Radian to_diff = fmod(p_to - from_rot, (real_t)Math_TAU);
	Radian to_rot = from_rot + fmod(2.0f * to_diff, (float)Math_TAU) - to_diff;

	Radian post_diff = fmod(p_post - to_rot, (real_t)Math_TAU);
	Radian post_rot = to_rot + fmod(2.0f * post_diff, (real_t)Math_TAU) - post_diff;

	return cubic_interpolate_in_time(from_rot, to_rot, pre_rot, post_rot, p_weight, p_to_t, p_pre_t, p_post_t);
}

inline Degree cubic_interpolate_angle_in_time(Degree p_from, Degree p_to, Degree p_pre, Degree p_post, real_t p_weight,
		Degree p_to_t, Degree p_pre_t, Degree p_post_t) {
	Degree from_rot = fmod(p_from, (real_t)Math_TAU);

	Degree pre_diff = fmod(p_pre - from_rot, (real_t)Math_TAU);
	Degree pre_rot = from_rot + fmod(2.0f * pre_diff, (real_t)Math_TAU) - pre_diff;

	Degree to_diff = fmod(p_to - from_rot, (real_t)Math_TAU);
	Degree to_rot = from_rot + fmod(2.0f * to_diff, (float)Math_TAU) - to_diff;

	Degree post_diff = fmod(p_post - to_rot, (real_t)Math_TAU);
	Degree post_rot = to_rot + fmod(2.0f * post_diff, (real_t)Math_TAU) - post_diff;

	return cubic_interpolate_in_time(from_rot, to_rot, pre_rot, post_rot, p_weight, p_to_t, p_pre_t, p_post_t);
}

inline double bezier_interpolate(double p_start, double p_control_1, double p_control_2, double p_end, double p_t) {
	/* Formula from Wikipedia article on Bezier curves. */
	double omt = (1.0 - p_t);
	double omt2 = omt * omt;
	double omt3 = omt2 * omt;
	double t2 = p_t * p_t;
	double t3 = t2 * p_t;

	return p_start * omt3 + p_control_1 * omt2 * p_t * 3.0 + p_control_2 * omt * t2 * 3.0 + p_end * t3;
}

inline float bezier_interpolate(float p_start, float p_control_1, float p_control_2, float p_end, float p_t) {
	/* Formula from Wikipedia article on Bezier curves. */
	float omt = (1.0f - p_t);
	float omt2 = omt * omt;
	float omt3 = omt2 * omt;
	float t2 = p_t * p_t;
	float t3 = t2 * p_t;

	return p_start * omt3 + p_control_1 * omt2 * p_t * 3.0f + p_control_2 * omt * t2 * 3.0f + p_end * t3;
}

template <typename T>
inline T clamp(T x, T minv, T maxv) {
	if (x < minv) {
		return minv;
	}
	if (x > maxv) {
		return maxv;
	}
	return x;
}

template <typename T>
inline T min(T a, T b) {
	return a < b ? a : b;
}

template <typename T>
inline T max(T a, T b) {
	return a > b ? a : b;
}

template <typename T>
inline T sign(T x) {
	return static_cast<T>(SIGN(x));
}

template <typename T>
inline T abs(T x) {
	return std::abs(x);
}

inline double inverse_lerp(double p_from, double p_to, double p_value) {
	return (p_value - p_from) / (p_to - p_from);
}
inline float inverse_lerp(float p_from, float p_to, float p_value) {
	return (p_value - p_from) / (p_to - p_from);
}
inline Radian inverse_lerp(Radian p_from, Radian p_to, double p_value) {
	return (p_value - p_from) / (p_to - p_from);
}
inline Degree inverse_lerp(Degree p_from, Degree p_to, double p_value) {
	return (p_value - p_from) / (p_to - p_from);
}

inline double remap(double p_value, double p_istart, double p_istop, double p_ostart, double p_ostop) {
	return Math::lerp(p_ostart, p_ostop, Math::inverse_lerp(p_istart, p_istop, p_value));
}
inline float remap(float p_value, float p_istart, float p_istop, float p_ostart, float p_ostop) {
	return Math::lerp(p_ostart, p_ostop, Math::inverse_lerp(p_istart, p_istop, p_value));
}

inline bool is_nan(float p_val) {
	return std::isnan(p_val);
}

inline bool is_nan(double p_val) {
	return std::isnan(p_val);
}

inline bool is_inf(float p_val) {
	return std::isinf(p_val);
}

inline bool is_inf(double p_val) {
	return std::isinf(p_val);
}

inline bool is_finite(float p_val) {
	return std::isfinite(p_val);
}

inline bool is_finite(double p_val) {
	return std::isfinite(p_val);
}

inline bool is_equal_approx(float a, float b) {
	// Check for exact equality first, required to handle "infinity" values.
	if (a == b) {
		return true;
	}
	// Then check for approximate equality.
	float tolerance = (float)CMP_EPSILON * abs(a);
	if (tolerance < (float)CMP_EPSILON) {
		tolerance = (float)CMP_EPSILON;
	}
	return abs(a - b) < tolerance;
}

inline bool is_equal_approx(float a, float b, float tolerance) {
	// Check for exact equality first, required to handle "infinity" values.
	if (a == b) {
		return true;
	}
	// Then check for approximate equality.
	return abs(a - b) < tolerance;
}

inline bool is_zero_approx(float s) {
	return abs(s) < (float)CMP_EPSILON;
}

inline bool is_equal_approx(double a, double b) {
	// Check for exact equality first, required to handle "infinity" values.
	if (a == b) {
		return true;
	}
	// Then check for approximate equality.
	double tolerance = CMP_EPSILON * abs(a);
	if (tolerance < CMP_EPSILON) {
		tolerance = CMP_EPSILON;
	}
	return abs(a - b) < tolerance;
}

inline bool is_equal_approx(double a, double b, double tolerance) {
	// Check for exact equality first, required to handle "infinity" values.
	if (a == b) {
		return true;
	}
	// Then check for approximate equality.
	return abs(a - b) < tolerance;
}

inline bool is_zero_approx(double s) {
	return abs(s) < CMP_EPSILON;
}

inline bool is_equal_approx(Radian a, Radian b) {
	// Check for exact equality first, required to handle "infinity" values.
	if (a == b) {
		return true;
	}
	// Then check for approximate equality.
	real_t tolerance = (real_t)CMP_EPSILON * abs(a);
	if (tolerance < (real_t)CMP_EPSILON) {
		tolerance = (real_t)CMP_EPSILON;
	}
	return abs(a - b) < tolerance;
}

inline bool is_equal_approx(Radian a, Radian b, real_t tolerance) {
	// Check for exact equality first, required to handle "infinity" values.
	if (a == b) {
		return true;
	}
	// Then check for approximate equality.
	return abs(a - b) < tolerance;
}

inline bool is_zero_approx(Radian s) {
	return abs(s) < (real_t)CMP_EPSILON;
}

inline bool is_equal_approx(Degree a, Degree b) {
	// Check for exact equality first, required to handle "infinity" values.
	if (a == b) {
		return true;
	}
	// Then check for approximate equality.
	real_t tolerance = (real_t)CMP_EPSILON * abs(a);
	if (tolerance < (real_t)CMP_EPSILON) {
		tolerance = (real_t)CMP_EPSILON;
	}
	return abs(a - b) < tolerance;
}

inline bool is_equal_approx(Degree a, Degree b, real_t tolerance) {
	// Check for exact equality first, required to handle "infinity" values.
	if (a == b) {
		return true;
	}
	// Then check for approximate equality.
	return abs(a - b) < tolerance;
}

inline bool is_zero_approx(Degree s) {
	return abs(s) < (real_t)CMP_EPSILON;
}

inline float absf(float g) {
	union {
		float f;
		uint32_t i;
	} u;

	u.f = g;
	u.i &= 2147483647u;
	return u.f;
}

inline double absd(double g) {
	union {
		double d;
		uint64_t i;
	} u;
	u.d = g;
	u.i &= (uint64_t)9223372036854775807ull;
	return u.d;
}

inline double smoothstep(double p_from, double p_to, double p_weight) {
	if (is_equal_approx(static_cast<real_t>(p_from), static_cast<real_t>(p_to))) {
		return p_from;
	}
	double x = clamp((p_weight - p_from) / (p_to - p_from), 0.0, 1.0);
	return x * x * (3.0 - 2.0 * x);
}
inline float smoothstep(float p_from, float p_to, float p_weight) {
	if (is_equal_approx(p_from, p_to)) {
		return p_from;
	}
	float x = clamp((p_weight - p_from) / (p_to - p_from), 0.0f, 1.0f);
	return x * x * (3.0f - 2.0f * x);
}

inline double move_toward(double p_from, double p_to, double p_delta) {
	return std::abs(p_to - p_from) <= p_delta ? p_to : p_from + sign(p_to - p_from) * p_delta;
}

inline float move_toward(float p_from, float p_to, float p_delta) {
	return std::abs(p_to - p_from) <= p_delta ? p_to : p_from + sign(p_to - p_from) * p_delta;
}

inline double linear2db(double p_linear) {
	return log(p_linear) * 8.6858896380650365530225783783321;
}
inline float linear2db(float p_linear) {
	return log(p_linear) * 8.6858896380650365530225783783321f;
}

inline double db2linear(double p_db) {
	return exp(p_db * 0.11512925464970228420089957273422);
}
inline float db2linear(float p_db) {
	return exp(p_db * 0.11512925464970228420089957273422f);
}

inline double round(double p_val) {
	return (p_val >= 0) ? floor(p_val + 0.5) : -floor(-p_val + 0.5);
}
inline float round(float p_val) {
	return (p_val >= 0) ? floor(p_val + 0.5f) : -floor(-p_val + 0.5f);
}

inline int64_t wrapi(int64_t value, int64_t min, int64_t max) {
	int64_t range = max - min;
	return range == 0 ? min : min + ((((value - min) % range) + range) % range);
}

inline float wrapf(real_t value, real_t min, real_t max) {
	const real_t range = max - min;
	return is_zero_approx(range) ? min : value - (range * floor((value - min) / range));
}

inline Radian wrapf(Radian value, Radian min, Radian max) {
	const Radian range = max - min;
	return is_zero_approx(range) ? min : Radian(value - (range * floor((value - min) / range)));
}

inline Degree wrapf(Degree value, Degree min, Degree max) {
	const Degree range = max - min;
	return is_zero_approx(range) ? min : Degree(value - (range * floor((value - min) / range)));
}

inline Radian angle_wrap(Radian value) {
	return wrapf((Degree)value, (Degree)0, (Degree)360);
}
inline Degree angle_wrap(Degree value) {
	return wrapf(value, (Degree)0, (Degree)360);
}

inline float fract(float value) {
	return value - floor(value);
}

inline double fract(double value) {
	return value - floor(value);
}

inline float pingpong(float value, float length) {
	return (length != 0.0f) ? abs(fract((value - length) / (length * 2.0f)) * length * 2.0f - length) : 0.0f;
}

inline double pingpong(double value, double length) {
	return (length != 0.0) ? abs(fract((value - length) / (length * 2.0)) * length * 2.0 - length) : 0.0;
}

// This function should be as fast as possible and rounding mode should not matter.
inline int fast_ftoi(float a) {
	static int b;

#if (defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0603) || WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP // windows 8 phone?
	b = (int)((a > 0.0) ? (a + 0.5) : (a - 0.5));

#elif defined(_MSC_VER) && _MSC_VER < 1800
	__asm fld a __asm fistp b
	/*#elif defined( __GNUC__ ) && ( defined( __i386__ ) || defined( __x86_64__ ) )
	// use AT&T inline assembly style, document that
	// we use memory as output (=m) and input (m)
	__asm__ __volatile__ (
	"flds %1        \n\t"
	"fistpl %0      \n\t"
	: "=m" (b)
	: "m" (a));*/

#else
	b = lrintf(a); // assuming everything but msvc 2012 or earlier has lrint
#endif
	return b;
}

inline double snapped(double p_value, double p_step) {
	if (p_step != 0) {
		p_value = Math::floor(p_value / p_step + 0.5) * p_step;
	}
	return p_value;
}

inline float snap_scalar(float p_offset, float p_step, float p_target) {
	return p_step != 0 ? Math::snapped(p_target - p_offset, p_step) + p_offset : p_target;
}

inline float snap_scalar_separation(float p_offset, float p_step, float p_target, float p_separation) {
	if (p_step != 0) {
		float a = Math::snapped(p_target - p_offset, p_step + p_separation) + p_offset;
		float b = a;
		if (p_target >= 0) {
			b -= p_separation;
		} else {
			b += p_step;
		}
		return (Math::abs(p_target - a) < Math::abs(p_target - b)) ? a : b;
	}
	return p_target;
}

} // namespace Math
} // namespace godot

#endif // GODOT_MATH_HPP
