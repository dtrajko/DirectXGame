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

	VertexMesh(Vector3D position, Vector2D texcoord, Vector3D normal) :
		m_position(position),
		m_texcoord(texcoord),
		m_normal(normal)
	{
	}

	VertexMesh(const VertexMesh& vertexMesh) :
		m_position(vertexMesh.m_position),
		m_texcoord(vertexMesh.m_texcoord),
		m_normal(vertexMesh.m_normal)
	{
	}

	~VertexMesh()
	{
	}

public:

	Vector3D m_position;
	Vector2D m_texcoord;
	Vector3D m_normal;

};
