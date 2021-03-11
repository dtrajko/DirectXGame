#pragma once

#include <iostream>


class Vector3D
{

public:
	Vector3D(): m_x(0), m_y(0), m_z(0)
	{
	}

	Vector3D(float x, float y, float z) : m_x(x), m_y(y), m_z(z)
	{
	}

	Vector3D(const Vector3D& vector) : m_x(vector.m_x), m_y(vector.m_y), m_z(vector.m_z)
	{
	}

	static Vector3D lerp(Vector3D& start, Vector3D& end, float delta)
	{
		Vector3D v;
		v.m_x = start.m_x * (1.0f - delta) + end.m_x * delta;
		v.m_y = start.m_y * (1.0f - delta) + end.m_y * delta;
		v.m_z = start.m_z * (1.0f - delta) + end.m_z * delta;
		return v;
	}

	Vector3D operator *(float scalar) const
	{
		return Vector3D(m_x * scalar, m_y * scalar, m_z * scalar);
	}

	Vector3D operator +(const Vector3D other) const
	{
		return Vector3D(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z);
	}

	Vector3D operator -(const Vector3D other) const
	{
		return Vector3D(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z);
	}

	static Vector3D normalize(const Vector3D& vec)
	{
		Vector3D res;
		float len = sqrt(vec.m_x * vec.m_x) + (vec.m_y * vec.m_y) + (vec.m_z * vec.m_z);
		if (!len) {
			return Vector3D();
		}

		res.m_x = vec.m_x / len;
		res.m_y = vec.m_y / len;
		res.m_z = vec.m_z / len;

		return res;
	}

	static Vector3D cross(const Vector3D& v1, const Vector3D& v2)
	{
		Vector3D res;
		res.m_x = (v1.m_y * v2.m_z) - (v1.m_z * v2.m_y);
		res.m_y = (v1.m_z * v2.m_x) - (v1.m_x * v2.m_z);
		res.m_z = (v1.m_x * v2.m_y) - (v1.m_y * v2.m_x);
		return res;
	}

	void print()
	{
		std::cout << "[x]: " << m_x << " [y]: " << m_y << " [z]: " << m_z << std::endl;
	}

	~Vector3D()
	{
	}

public:

	float m_x;
	float m_y;
	float m_z;

};
