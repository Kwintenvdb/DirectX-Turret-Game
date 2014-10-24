//************
// VARIABLES *
//************
cbuffer cbPerObject
{
	float4x4 m_MatrixWorldViewProj : WORLDVIEWPROJECTION;
	float4x4 m_MatrixWorld : WORLD;
	float3 m_LightDir={0.2f,-1.0f,0.2f};
}

RasterizerState FrontCulling 
{ 
	CullMode = NONE; 
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// of Mirror of Clamp of Border
    AddressV = Wrap;// of Mirror of Clamp of Border
};

Texture2D m_TextureDiffuse;

//**********
// STRUCTS *
//**********
struct VS_DATA
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct GS_DATA
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
};

//****************
// VERTEX SHADER *
//****************
VS_DATA MainVS(VS_DATA vsData)
{
	//GS_DATA temp = (GS_DATA)0;
	//temp.Position = mul(float4(vsData.Position,1),m_MatrixWorldViewProj);
	//temp.Normal = mul(vsData.Normal,(float3x3)m_MatrixWorld);
	//temp.TexCoord = vsData.TexCoord;

	return vsData;
}

//***************
// GEOM SHADER  *
//***************

GS_DATA CreateVertex(float3 position, float3 normal, float2 texCoord)
{
	GS_DATA vertex;
	vertex.Position = mul(float4(position,1),m_MatrixWorldViewProj);
	vertex.Normal = mul(normal,(float3x3)m_MatrixWorld);
	vertex.TexCoord = texCoord;

	return vertex;
}

[maxvertexcount(9)]
void MainGS(triangle VS_DATA gIn[3], inout TriangleStream<GS_DATA> triStream)
{
	GS_DATA gOut;
	for(int i = 0; i < 3; ++i)
	{
		gOut.Position = mul(float4(gIn[i].Position,1),m_MatrixWorldViewProj);
		gOut.Normal = mul(gIn[i].Normal,(float3x3)m_MatrixWorld);
		gOut.TexCoord = gIn[i].TexCoord;

		triStream.Append(gOut);
	}

	float3 basePoint = (gIn[0].Position + gIn[1].Position + gIn[2].Position) / 3;

	float3 normal = (gIn[0].Normal + gIn[1].Normal + gIn[2].Normal) / 3;

	float3 top = basePoint + (0.5f*normal);

	float3 spikeDirection = (gIn[2].Position - gIn[0].Position) * 0.3f;

	float3 left = basePoint - spikeDirection;
	float3 right = basePoint + spikeDirection;

	float3 s0 = top - left;
	float3 s1 = top - right;
	float3 spikeNormal = normalize(cross(s0,s1));

	triStream.RestartStrip();

	triStream.Append(CreateVertex(left,spikeNormal,gIn[0].TexCoord));
	triStream.Append(CreateVertex(top,spikeNormal,gIn[1].TexCoord));
	triStream.Append(CreateVertex(right,spikeNormal,gIn[2].TexCoord));

	triStream.RestartStrip();
}

//***************
// PIXEL SHADER *
//***************
float4 MainPS(GS_DATA input) : SV_TARGET 
{
	float4 diffuseColor = m_TextureDiffuse.Sample( samLinear,input.TexCoord );
	float3 color_rgb = diffuseColor.rgb;
	float color_a = diffuseColor.a;
	
	//HalfLambert Diffuse :)
	//float diffuseStrength = dot(input.normal, -m_LightDir);
	//diffuseStrength = diffuseStrength * 0.5 + 0.5;
	//diffuseStrength = saturate(diffuseStrength);
	//color_rgb = color_rgb * diffuseStrength;

	return float4( color_rgb , color_a );
	//return float4(1,1,1,1);
}


//*************
// TECHNIQUES *
//*************
technique10 DefaultTechnique 
{
	pass p0 {
		SetRasterizerState(FrontCulling);	
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}