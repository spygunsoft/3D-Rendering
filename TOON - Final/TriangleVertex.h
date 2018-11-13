#pragma once
#ifndef TRIANGLEVERTEX_H
#define TRIANGLEVERTEX_H
class TriangleVertex
{
public:
	TriangleVertex();
	~TriangleVertex();
	float *Vertex;
	float *Normal;
	float uv[2];
	GzColor color;  //originalColor
	GzColor ambientColor;
	GzColor diffuseColor;
	GzColor specularColor;
};

TriangleVertex::TriangleVertex()
{
}

TriangleVertex::~TriangleVertex()
{
}



#endif // !TRIANGLEVERTEX_H

