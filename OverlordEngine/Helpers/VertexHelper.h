//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.113
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
#pragma once

struct VertexPosCol
{
	public:

	VertexPosCol(){};
	VertexPosCol(XMFLOAT3 pos, XMFLOAT4 col):
		Position(pos), Color(col){}

	XMFLOAT3 Position;
	XMFLOAT4 Color;
};

struct VertexPosNormCol
{
public:

	VertexPosNormCol(){};
	VertexPosNormCol(XMFLOAT3 pos, XMFLOAT3 norm, XMFLOAT4 col):
		Position(pos), Normal(norm), Color(col){}

	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT4 Color;
};

struct QuadPosNormCol
{
public:

	QuadPosNormCol(){};
	QuadPosNormCol(VertexPosNormCol vert1, VertexPosNormCol vert2, VertexPosNormCol vert3, VertexPosNormCol vert4):
		Vertex1(vert1), Vertex2(vert2), Vertex3(vert3), Vertex4(vert4){}

	VertexPosNormCol Vertex1;
	VertexPosNormCol Vertex2;
	VertexPosNormCol Vertex3;
	VertexPosNormCol Vertex4;
};

struct TrianglePosNormCol
{
public:

	TrianglePosNormCol(){};
	TrianglePosNormCol(VertexPosNormCol vert1, VertexPosNormCol vert2, VertexPosNormCol vert3):
		Vertex1(vert1), Vertex2(vert2), Vertex3(vert3){}

	VertexPosNormCol Vertex1;
	VertexPosNormCol Vertex2;
	VertexPosNormCol Vertex3;
};

// UPDATE PP
struct VertexPosTex
{
	public:

	VertexPosTex(){};
	VertexPosTex(XMFLOAT3 pos, XMFLOAT2 uv):
		Position(pos), UV(uv){}

	XMFLOAT3 Position;
	XMFLOAT2 UV;
};

struct QuadPosTex
{
public:

	QuadPosTex(){};
	QuadPosTex(VertexPosTex vert1, VertexPosTex vert2, VertexPosTex vert3, VertexPosTex vert4):
		Vertex1(vert1), Vertex2(vert2), Vertex3(vert3), Vertex4(vert4){}

	VertexPosTex Vertex1;
	VertexPosTex Vertex2;
	VertexPosTex Vertex3;
	VertexPosTex Vertex4;
};

struct SkinnedVertex 
{ 
	SkinnedVertex(XMFLOAT3 position, XMFLOAT3 normal, XMFLOAT4 color, float weight1, float weight2) 
		: TransformedVertex(position, normal, color)
		, OriginalVertex(position, normal, color)
		, WeightBone1(weight1)
		, WeightBone2(weight2)
		{}

	VertexPosNormCol TransformedVertex;
	VertexPosNormCol OriginalVertex;
	float WeightBone1;
	float WeightBone2;
};