#ifndef GEOMETRYDATA_H_
#define GEOMETRYDATA_H_
//------------------------------------------------------------------------------------
// generate cube mesh data and ball mesh data
//------------------------------------------------------------------------------------
#include <vector>
#include <DirectXMath.h>

typedef unsigned short USHORT;
using namespace DirectX;

namespace MyMeshData
{
	struct Vertex
	{
		Vertex()
		{
		}

		Vertex(const XMFLOAT3& p)
			: Position(p){}

		Vertex(
			float px, float py, float pz,float u,float v)
			: Position(px, py, pz),TexCoord(u,v){}
		Vertex(float position[3],float normal[3],float texcoord[2], int index0, int index1, int index2, int index3, float weight0, float weight1, float weight2,float weight3)
			: Position(position[0],position[1],position[2]),Normal(normal[0],normal[1],normal[2]),
			TexCoord(texcoord[0],texcoord[1]),indeces(index0,index1,index2,index3),
			weights(weight0,weight1,weight2,weight3)
			{};
		//Vertex(VertexCommon v) : Position(v.pos.toXMFloat()), TexCoord(v.uv.toXMFloat()), Normal(v.normal_v.toXMFloat()), Binormal(v.binormal.toXMFloat()), Tangent(v.tangent.toXMFloat())
		//{
		//};

		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 TexCoord;
		XMINT4 indeces;
		XMFLOAT4 weights;
		//XMFLOAT3 Binormal;
		//XMFLOAT3 Tangent;
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<USHORT> Indices;
	};
}

#endif
