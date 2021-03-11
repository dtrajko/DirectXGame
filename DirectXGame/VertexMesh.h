#pragma once

#include <iostream>

#include "Vector3D.h"
#include "Vector2D.h"


class VertexMesh
{

public:
	VertexMesh() : m_position(), m_texcoord(), m_normal()
	{
	}

	VertexMesh(const Vector3D& position, const Vector2D& texcoord, const Vector3D& normal,
		const Vector3D& tangent, const Vector3D& binormal) :
		m_position(position),
		m_texcoord(texcoord),
		m_normal(normal),
		m_tangent(tangent),
		m_binormal(binormal)
	{
	}

	VertexMesh(const VertexMesh& vertexMesh) :
		m_position(vertexMesh.m_position),
		m_texcoord(vertexMesh.m_texcoord),
		m_normal(vertexMesh.m_normal),
		m_tangent(vertexMesh.m_tangent),
		m_binormal(vertexMesh.m_binormal)
	{
	}

	~VertexMesh()
	{
	}

public:

	Vector3D m_position;
	Vector2D m_texcoord;
	Vector3D m_normal;
	Vector3D m_tangent;
	Vector3D m_binormal;

};
