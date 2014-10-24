float Desaturation;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Mirror;
    AddressV = Mirror;
};

DepthStencilState DepthEnabled
{
	DepthEnable = TRUE;
	//DepthWriteMask = 1;
};

RasterizerState BackfaceCulling
{
	//FillMode = SOLID;
	CullMode = BACK;
};

Texture2D gTexture;

struct VS_INPUT_STRUCT
{
	float3 Pos : POSITION;
    float2 Tex : TEXCOORD;

};
struct PS_INPUT_STRUCT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD1;

};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT_STRUCT VS( VS_INPUT_STRUCT input )
{
	PS_INPUT_STRUCT output = (PS_INPUT_STRUCT)0;
	// Set the Position
	// Set the Text coord

	output.Pos = float4(input.Pos, 1);
	output.Tex = input.Tex;

	return output;
}
//--------------------------------------------------------------------------------------
// Pixel XMeshShader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT_STRUCT input): SV_Target
{
    // Step 1: sample the texture
	// Step 2: calculate the mean value
	// Step 3: return the color
	float4 finalColor = gTexture.Sample(samLinear, input.Tex);
	float meanValue = (finalColor.x + finalColor.y + finalColor.z) / 3;

	//float r = finalColor.x;
	float r = lerp(finalColor.x, meanValue, Desaturation);
	float g = lerp(finalColor.y, meanValue, Desaturation);
	float b = lerp(finalColor.z, meanValue, Desaturation);

	//float4 meanCol = float4(meanValue, meanValue, meanValue, 1);
	//finalColor = lerp(finalColor, meanCol, Desaturation);

	finalColor = float4(r, g, b, 1);

    return finalColor;
}

technique10 Grayscale
{
    pass P0
    {          
        // Set states
		SetRasterizerState(BackfaceCulling);
		
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		SetDepthStencilState(DepthEnabled, 0);
    }
}

