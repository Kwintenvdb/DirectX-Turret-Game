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
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "MeshFilterLoader.h"
#include "..\Helpers\BinaryReader.h"
#include "..\Helpers\EffectHelper.h"
#include <algorithm>

#define OVM_vMAJOR 1
#define OVM_vMINOR 1

MeshFilter* MeshFilterLoader::LoadContent(const wstring& assetFile)
{
	auto binReader = new BinaryReader();
	binReader->Open(assetFile);

	if(!binReader->Exists())
		return nullptr;

	//READ OVM FILE
	int versionMajor = binReader->Read<char>();
	int versionMinor = binReader->Read<char>();

	if (versionMajor != OVM_vMAJOR || versionMinor != OVM_vMINOR)
	{
		Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::Load() > Wrong OVM version\n\tFile: \"%s\" \n\tExpected version %i.%i, not %i.%i.", assetFile.c_str(), OVM_vMAJOR, OVM_vMINOR, versionMajor, versionMinor);
		//wstringstream ss;
		//ss<<L"MeshDataLoader::Load() > Wrong OVM version\nFile: ";
		//ss<<assetFile;
		//ss << L"\nExpected version " << OVM_vMAJOR << L"." << OVM_vMINOR << L", not " << versionMajor << L"." << versionMinor;
		//Logger::LogWarning(ss.str());

		delete binReader;
		return nullptr;
	}

	UINT vertexCount = 0;
	UINT indexCount = 0;

	auto pMesh = new MeshFilter();

	for (;;)
	{
		auto meshDataType = (MeshDataType)binReader->Read<char>();
		if(meshDataType == MeshDataType::END)
			break;

		auto dataOffset = binReader->Read<UINT>();

		switch(meshDataType)
		{
			//HEADER
		case MeshDataType::HEADER:
			{
				pMesh->m_MeshName = binReader->ReadString();
				vertexCount = binReader->Read<UINT>();
				indexCount = binReader->Read<UINT>();

				pMesh->m_VertexCount = vertexCount;
				pMesh->m_IndexCount = indexCount;
			}
			break;
		case MeshDataType::POSITIONS:
			{
				pMesh->m_HasElement |= ILSemantic::POSITION;

				for(UINT i = 0; i<vertexCount; ++i)
				{
					XMFLOAT3 pos;
					pos.x = binReader->Read<float>();
					pos.y = binReader->Read<float>();
					pos.z = binReader->Read<float>();
					pMesh->m_Positions.push_back(pos);
				}
			}
			break;
		case MeshDataType::INDICES:
			{
				for(UINT i = 0; i<indexCount; ++i)
				{
					pMesh->m_Indices.push_back(binReader->Read<DWORD>());
				}
			}
			break;
		case MeshDataType::NORMALS:
			{
				pMesh->m_HasElement |= ILSemantic::NORMAL;

				for (UINT i = 0; i<vertexCount; ++i)
				{
					XMFLOAT3 normal;
					normal.x = binReader->Read<float>();
					normal.y = binReader->Read<float>();
					normal.z = binReader->Read<float>();
					pMesh->m_Normals.push_back(normal);
				}
			}
			break;
		case MeshDataType::TANGENTS:
			{
				pMesh->m_HasElement |= ILSemantic::TANGENT;

				for (UINT i = 0; i<vertexCount; ++i)
				{
					XMFLOAT3 tangent;
					tangent.x = binReader->Read<float>();
					tangent.y = binReader->Read<float>();
					tangent.z = binReader->Read<float>();
					pMesh->m_Tangents.push_back(tangent);
				}
			}
			break;
		case MeshDataType::BINORMALS:
			{
				pMesh->m_HasElement |= ILSemantic::BINORMAL;

				for (UINT i = 0; i<vertexCount; ++i)
				{
					XMFLOAT3 binormal;
					binormal.x = binReader->Read<float>();
					binormal.y = binReader->Read<float>();
					binormal.z = binReader->Read<float>();
					pMesh->m_Binormals.push_back(binormal);
				}
			}
			break;
		case MeshDataType::TEXCOORDS:
			{
				pMesh->m_HasElement |= ILSemantic::TEXCOORD;

				for (UINT i = 0; i<vertexCount; ++i)
				{
					XMFLOAT2 tc;
					tc.x = binReader->Read<float>();
					tc.y = binReader->Read<float>();
					pMesh->m_TexCoords.push_back(tc);
				}
			}
			break;
		case MeshDataType::COLORS:
			{
				pMesh->m_HasElement |= ILSemantic::COLOR;

				for (UINT i = 0; i<vertexCount; ++i)
				{
					XMFLOAT4 color;
					color.x = binReader->Read<float>();
					color.y = binReader->Read<float>();
					color.z = binReader->Read<float>();
					color.w = binReader->Read<float>();
					pMesh->m_Colors.push_back(color);
				}
			}
			break;
		case MeshDataType::BLENDINDICES:
			{
				pMesh->m_HasElement |= ILSemantic::BLENDINDICES;
				
				//Start parsing the BlendIndices for every vertex
				//and add them to the corresponding vector
				//pMesh->m_BlendIndices

				for (UINT i = 0; i<vertexCount; ++i)
				{
					XMFLOAT4 blendIndices;
					blendIndices.x = binReader->Read<float>();
					blendIndices.y = binReader->Read<float>();
					blendIndices.z = binReader->Read<float>();
					blendIndices.w = binReader->Read<float>();
					pMesh->m_BlendIndices.push_back(blendIndices);
				}

			}
			break;
		case MeshDataType::BLENDWEIGHTS:
			{
				pMesh->m_HasElement |= ILSemantic::BLENDWEIGHTS;

				//Start parsing the BlendWeights for every vertex
				//and add them to the corresponding vector
				//pMesh->m_BlendWeights

				for (UINT i = 0; i<vertexCount; ++i)
				{
					XMFLOAT4 blendWeights;
					blendWeights.x = binReader->Read<float>();
					blendWeights.y = binReader->Read<float>();
					blendWeights.z = binReader->Read<float>();
					blendWeights.w = binReader->Read<float>();
					pMesh->m_BlendWeights.push_back(blendWeights);
				}

			}
			break;
		case MeshDataType::ANIMATIONCLIPS:
			{
				pMesh->m_HasAnimations = true;
				
				//Start parsing the AnimationClips
				UINT clipCount = binReader->Read<USHORT>();
				
				//1. Read the clipCount
				for(UINT x = 0 ; x < clipCount; x++)
				{
					AnimationClip clip;
					clip.Name = binReader->ReadString();
					clip.Duration = binReader->Read<float>();
					clip.TicksPerSecond = binReader->Read<float>();
					
					UINT keyCount = binReader->Read<USHORT>();

					for(UINT y = 0; y < keyCount; y++)
					{
						AnimationKey key;

						key.Tick = binReader->Read<float>();
						
						UINT boneTransformCount = binReader->Read<USHORT>();

						for(UINT z = 0; z < boneTransformCount; z++)
						{
							XMFLOAT4X4 transform;

							transform._11 = binReader->Read<float>();
							transform._12 = binReader->Read<float>();
							transform._13 = binReader->Read<float>();
							transform._14 = binReader->Read<float>();
							transform._21 = binReader->Read<float>();
							transform._22 = binReader->Read<float>();
							transform._23 = binReader->Read<float>();
							transform._24 = binReader->Read<float>();
							transform._31 = binReader->Read<float>();
							transform._32 = binReader->Read<float>();
							transform._33 = binReader->Read<float>();
							transform._34 = binReader->Read<float>();
							transform._41 = binReader->Read<float>();
							transform._42 = binReader->Read<float>();
							transform._43 = binReader->Read<float>();
							transform._44 = binReader->Read<float>();

							key.BoneTransforms.push_back(transform);

						}

						clip.Keys.push_back(key);

					}

					pMesh->m_AnimationClips.push_back(clip);

				}
				//2. For every clip
					//3. Create a AnimationClip object (clip)
					//4. Read/Assign the ClipName
					//5. Read/Assign the ClipDuration
					//6. Read/Assign the TicksPerSecond
					//7. Read the KeyCount for this clip
						//8. For every key
						//9. Create a AnimationKey object (key)
						//10. Read/Assign the Tick
						//11. Read the TransformCount
							//12. For every transform
							//13. Create a XMFLOAT4X4
							//14. The following 16 floats are the matrix values, they are stored by row
							// float0 = readFloat (ROW1) (_11)
							// float1 = readFloat (ROW1) (_12)
							// float2 = readFloat (ROW1)
							// float3 = readFloat (ROW1)
							// float4 = readFloat (ROW2) (_21)
							//...
							// float15 = readFloat (ROW4) (_44)
							//
							//MATRIX:
							// [ float0	float1	float2	float3 ]
							// [ float4	...		...		...	   ]
							// [ ...	...		...		...	   ]
							// [ ...	...		...		float15]
						//15. Add The matrix to the BoneTransform vector of the key
					//16. Add the key to the key vector of the clip
				//17. Add the clip to the AnimationClip vector of the MeshFilter (pMesh->m_AnimationClips)
				
			}
			break;
		case MeshDataType::SKELETON:
			{
				//1. Read/Assign the boneCount (pMesh->m_BoneCount
				pMesh->m_BoneCount = binReader->Read<UINT>();
				//2. Move the buffer to the next block position (don't forget that we already moved the reader ;) )
				binReader->MoveBufferPosition(dataOffset - sizeof(UINT));
			}
			break;
		default:
			binReader->MoveBufferPosition(dataOffset);
			break;
		}
	}

	delete binReader;

	return pMesh;
}

void MeshFilterLoader::Destroy(MeshFilter* objToDestroy)
{
	SafeDelete(objToDestroy);
}
