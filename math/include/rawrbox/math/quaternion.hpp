﻿#pragma once

#include <rawrbox/math/vector2.hpp>
#include <rawrbox/math/vector3.hpp>

#include <algorithm>

namespace rawrBOX {
	template <class NumberType>
	class Quaternion_t {
	private:
		using QuaternionType = Quaternion_t<NumberType>;

	public:
		NumberType x = 0, y = 0, z = 0, w = 0;

		Quaternion_t() = default;
		Quaternion_t(Vector2_t<NumberType> first, Vector2_t<NumberType> second = 0) : x(first.x), y(first.y), z(second.x), w(second.y) {}
		Quaternion_t(Vector3_t<NumberType> vec, NumberType _w = 0) : x(vec.x), y(vec.y), z(vec.z), w(_w) {}
		Quaternion_t(NumberType _x, NumberType _y = 0, NumberType _z = 0, NumberType _w = 0) : x(_x), y(_y), z(_z), w(_w) {}

		QuaternionType operator- (const QuaternionType& other) const {
			return QuaternionType(x - other.x, y - other.y, z - other.z, w - other.w);
		}

		QuaternionType operator+ (const QuaternionType& other) const {
			return QuaternionType(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		QuaternionType operator* (const QuaternionType& other) const {
			return QuaternionType(x * other.x, y * other.y, z * other.z, w * other.w);
		}

		QuaternionType operator/ (const QuaternionType& other) const {
			return QuaternionType(x / other.x, y / other.y, z / other.z, w / other.w);
		}

		QuaternionType operator- (NumberType other) const {
			return QuaternionType(x - other, y - other, z - other, w - other);
		}

		QuaternionType operator+ (NumberType other) const {
			return QuaternionType(x + other, y + other, z + other, w + other);
		}

		QuaternionType operator* (NumberType other) const {
			return QuaternionType(x * other, y * other, z * other, w * other);
		}

		QuaternionType operator/ (NumberType other) const {
			return QuaternionType(x / other, y / other, z / other, w / other);
		}

		NumberType length() const {
			return static_cast<NumberType>(std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2) + std::pow(w, 2)));
		}

		QuaternionType normalized() const {
			return (*this) / length();
		}

		QuaternionType lerp(const QuaternionType& other, NumberType timestep) const {
			if((*this) == other) return other;
			QuaternionType ret;

			ret.x = static_cast<NumberType>(static_cast<float>(x) + static_cast<float>(other.x - x) * timestep);
			ret.y = static_cast<NumberType>(static_cast<float>(y) + static_cast<float>(other.y - y) * timestep);
			ret.z = static_cast<NumberType>(static_cast<float>(z) + static_cast<float>(other.z - z) * timestep);
			ret.w = static_cast<NumberType>(static_cast<float>(w) + static_cast<float>(other.w - w) * timestep);

			return ret;
		}

		QuaternionType& operator-= (const QuaternionType& other) {
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
			return *this;
		}

		QuaternionType& operator+= (const QuaternionType& other) {
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}

		QuaternionType& operator*= (const QuaternionType& other) {
			x *= other.x;
			y *= other.y;
			z *= other.z;
			w *= other.w;
			return *this;
		}

		QuaternionType& operator/= (const QuaternionType& other) {
			x /= other.x;
			y /= other.y;
			z /= other.z;
			w /= other.w;
			return *this;
		}

		QuaternionType& operator-= (NumberType other) {
			x -= other;
			y -= other;
			z -= other;
			w -= other;
			return *this;
		}

		QuaternionType& operator+= (NumberType other) {
			x += other;
			y += other;
			z += other;
			w += other;
			return *this;
		}

		QuaternionType& operator*= (NumberType other) {
			x *= other;
			y *= other;
			z *= other;
			w *= other;
			return *this;
		}

		QuaternionType& operator/= (NumberType other) {
			x /= other;
			y /= other;
			z /= other;
			w /= other;
			return *this;
		}

		QuaternionType operator-() const {
			return QuaternionType(-x, -y, -z, -w);
		}

		bool operator== (const QuaternionType& other) const {
			return x == other.x && y == other.y && z == other.z && w == other.w;
		}

		bool operator!= (const QuaternionType& other) const {
			return !operator==(other);
		}
	};

	using Quaterniond = Quaternion_t<double>;
	using Quaternionf = Quaternion_t<float>;
	using Quaternioni = Quaternion_t<int>;
	using Quaternion = Quaternionf;

}
