float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gViewInv : VIEWINVERSE;
float4x4 gView : VIEW;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float4 gColor;

struct VS_INPUT{
	float3 pos : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float3 lightVec : TEXCOORD1;
};

RasterizerState BackCulling
{
	FillMode = SOLID;
	CullMode = NONE;
};

BlendState EnableBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){
	VS_OUTPUT output;

	float3 pw = mul(float4(input.pos, 1), gWorld).xyz;
	output.lightVec = (gViewInv[3].xyz - pw);

	output.pos = mul (float4(input.pos,1.0f), gWorldViewProj);

	output.normal = normalize(mul(input.normal, (float3x3)gWorld));

	output.color = input.color;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

	//float3 color_rgb= input.color.rgb;
	float3 color_rgb = gColor.rgb;
	float color_a = input.color.a;
	
	//HalfLambert Diffuse :)
	//float3 light = normalize(input.lightVec);
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;
	
	return float4( color_rgb , color_a );
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(BackCulling);
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

technique11 TransparencyTech
{
	pass P0
	{
		SetRasterizerState(BackCulling);
		SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}

