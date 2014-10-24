Texture2D gTexture;
int Iterations = 5;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
/// Create Rasterizer State (Backface culling) 

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

struct VS_INPUT_STRUCT
{
    float3 Pos : POSITION;
	float2 TexCoord : TEXCOORD0;

};
struct PS_INPUT_STRUCT
{
    float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};

PS_INPUT_STRUCT VS(VS_INPUT_STRUCT input)
{
	PS_INPUT_STRUCT output = (PS_INPUT_STRUCT)0;
	// Set the Position
	// Set the Text coord
	output.Pos = float4(input.Pos, 1);
	output.TexCoord = input.TexCoord;
	return output;
}

float4 PS(PS_INPUT_STRUCT input):SV_TARGET
{
	float3 color = float3(0,0,0);
	float colorOpacity = 1;
	
	// Step 1: find the dimensions of the texture (the texture has a method for that)
	uint width, height;
	gTexture.GetDimensions(width, height);

	// Step 2: calculate dx and dy
	float dx = 1.0f / width;
	float dy = 1.0f / height;
	
	// Step 3: Create a double for loop (5 iterations each)
	//		   Inside the look, calculate the offset in each direction. Make sure not to take every pixel but move by 2 pixels each time
	//			Do a texture lookup using your previously calculated uv coordinates + the offset, and add to the final color
	
	float4 finalColor;
	if(Iterations > 1)
	{
		int it = Iterations;
		float offsetX = input.TexCoord.x - it * dx;
		float offsetY = input.TexCoord.y - it * dy;
		for(int i = 0; i < it; ++i)
		{
			for(int j = 0; j < it; ++j)
			{
				float2 uv;
				uv.x = offsetX + i * 2 * dx;
				uv.y = offsetY + j * 2 * dy;
				finalColor += gTexture.Sample(samLinear, uv);
			}
		}

		finalColor = finalColor / (it * it);
		float val = pow(0.02f * Iterations, 1.4f);
		finalColor -= float4(0, val, val, 0);
	}
	else
	{
		finalColor = gTexture.Sample(samLinear, input.TexCoord);
	}

	// Step 5: return the final color
	return finalColor;
}

technique11 Blur
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