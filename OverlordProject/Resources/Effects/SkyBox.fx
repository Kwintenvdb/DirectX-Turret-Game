SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

TextureCube m_CubeMap : CubeMap;

cbuffer cbChangesEveryFrame
{
	matrix matWorldViewProj : WorldViewProjection;
}

struct VS_IN
{
	float3 posL : POSITION;
};

struct VS_OUT
{
	float4 posH : SV_POSITION;
	float3 texC : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUT VS( VS_IN vIn )
{

	VS_OUT vOut = (VS_OUT)0;

	// 1.
	// set z = w so that z/w = 1 (skydome always on far plane).
	//Calculate posH
	vOut.posH= mul( float4(vIn.posL,0.0f), matWorldViewProj).xyww;


	//2.
	// use local vertex position as cubemap lookup vector
	// Set tex coord
	vOut.texC = vIn.posL;

	return vOut;
}
//--------------------------------------------------------------------------------------
// Pixel XMeshShader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUT pIn): SV_Target
{
	//3. Sample cubemap
	return m_CubeMap.Sample(samLinear, pIn.texC);
}

RasterizerState NoCull
{
	CullMode = NONE;
};

DepthStencilState LessEqualDSS
{
	// Make sure the depth function is LESS_EQUAL and not just LESS.
    // Otherwise, the normalized depth values at z = 1 (NDC) will
    // fail the depth test if the depth buffer was cleared to 1.
	//4. Set DepthFunc to Less Equal
	DepthFunc = LESS_EQUAL;
};


technique10 Render
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		// 5. Set Rasterizer state and Depth Stencil state

		SetRasterizerState(NoCull);
        SetDepthStencilState(LessEqualDSS, 0);
    }
}