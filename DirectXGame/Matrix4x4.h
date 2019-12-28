#pragma once

#include "Vector3D.h"

#include <iostream>
#include <memory>

class Matrix4x4
{

public:

	Matrix4x4()
	{
		setIdentity();
	}

	void setIdentity()
	{
		::memset(m_mat, 0, sizeof(float) * 16);
		m_mat[0][0] = 1;
		m_mat[1][1] = 1;
		m_mat[2][2] = 1;
		m_mat[3][3] = 1;
	}

	void setTranslation(const Vector3D& translation)
	{
		// setIdentity();
		m_mat[3][0] = translation.m_x;
		m_mat[3][1] = translation.m_y;
		m_mat[3][2] = translation.m_z;
	}

	void setRotationX(float x)
	{
		m_mat[1][1] =  cos(x);
		m_mat[1][2] =  sin(x);
		m_mat[2][1] = -sin(x);
		m_mat[2][2] =  cos(x);
	}

	void setRotationY(float y)
	{
		m_mat[0][0] =  cos(y);
		m_mat[0][2] = -sin(y);
		m_mat[2][0] =  sin(y);
		m_mat[2][2] =  cos(y);
	}

	void setRotationZ(float z)
	{
		m_mat[0][0] =  cos(z);
		m_mat[0][1] =  sin(z);
		m_mat[1][0] = -sin(z);
		m_mat[1][1] =  cos(z);
	}

	void setRotation(const Vector3D& rot)
	{
		setRotationX(rot.m_x);
		setRotationY(rot.m_y);
		setRotationZ(rot.m_z);
	}

	void setScale(const Vector3D& scale)
	{
		// setIdentity();
		m_mat[0][0] = scale.m_x;
		m_mat[1][1] = scale.m_y;
		m_mat[2][2] = scale.m_z;
	}

	void operator *=(const Matrix4x4& other)
	{
		Matrix4x4 out;
		for (short i = 0; i < 4; i++)
		{
			for (short j = 0; j < 4; j++)
			{
				out.m_mat[i][j] =
					m_mat[i][0] * other.m_mat[0][j] +
					m_mat[i][1] * other.m_mat[1][j] +
					m_mat[i][2] * other.m_mat[2][j] +
					m_mat[i][3] * other.m_mat[3][j];
			}
		}
		::memcpy(m_mat, out.m_mat, sizeof(float) * 16);
	}

	void setOrthoLH(float width, float height, float near_plane, float far_plane)
	{
		setIdentity();
		m_mat[0][0] = 2.0f / width;
		m_mat[1][1] = 2.0f / height;
		m_mat[2][2] = 1.0f / (far_plane - near_plane);
		m_mat[3][2] = -(near_plane / (far_plane - near_plane));
	}

	void print()
	{
		std::cout << "[00]: " << m_mat[0][0] << " [10]: " << m_mat[1][0] << " [20]: " << m_mat[2][0] << " [30]: " << m_mat[3][0] << std::endl;
		std::cout << "[01]: " << m_mat[0][1] << " [12]: " << m_mat[1][1] << " [21]: " << m_mat[2][1] << " [31]: " << m_mat[3][1] << std::endl;
		std::cout << "[02]: " << m_mat[0][2] << " [13]: " << m_mat[1][2] << " [22]: " << m_mat[2][2] << " [32]: " << m_mat[3][2] << std::endl;
		std::cout << "[03]: " << m_mat[0][3] << " [14]: " << m_mat[1][3] << " [23]: " << m_mat[2][3] << " [33]: " << m_mat[3][3] << std::endl;
	}

	~Matrix4x4()
	{

	}

private:

	float m_mat[4][4] = {};

};
