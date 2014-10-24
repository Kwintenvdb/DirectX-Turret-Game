// The World View Projection Matrix
float4x4 m_MatrixWorldViewProj : WORLDVIEWPROJECTION;
float4x4 m_MatrixViewInv : VIEWINVERSE;
float4x4 World : WORLD;
float4x4 View : VIEW;
float4x4 Projection : PROJECTION;
//float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f); 
float3 gLightDirection = float3(-0.5f, -0.75f, 0.6f); 

cbuffer cbPerFrame
{
	float CurrentLayer;		// shell index: value between 0 and 1
	float MaxHairLength;	// maximum hair length
	float Density;			// amount of "hairs" drawn
	float3 Comb;			// hair "comb" vector
	float MaxOpacity;		// max opacity of the fins
};
Texture2D FurTexture;	// noise texture (alpha values)
Texture2D ColTexture;	// fur pattern texture
Texture2D FinTexture;	// opacity texture for fins
Texture2D NoiseTexture;	// random rgb noise
bool bUseNoise = true;

SamplerState FurSampler
{
	Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState ColorSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

RasterizerState FrontCulling
{ 
	CullMode = Front; 
};

RasterizerState NoCulling
{
	CullMode = None;
};

BlendState EnableBlending
{
	BlendEnable[0] = true;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

BlendState AlphaBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState DisableDepthWriting
{
	DepthEnable = true;
    DepthWriteMask = ZERO;
};

DepthStencilState EnableDepthWriting
{
	DepthEnable = true;
    DepthWriteMask = ALL;
};

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal	: NORMAL;
    float2 TexCoord : TEXCOORD;
};
 
struct VSOutput
{
    float4 Position	: SV_POSITION;
	float3 Normal	: TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
	float3 EyeVec	: TEXCOORD2;
};

struct GSOutput
{
    float4 Position	: SV_POSITION;  
	float3 Normal	: NORMAL;
    float2 TexCoord	: TEXCOORD;
	float2 TexCol	: TEXCOORD1;
	float  Opacity	: OPACITY;
};

VSOutput MeshVS(VSInput input)
{
	VSOutput output = (VSOutput)0;
	output.Position = mul(float4(input.Position, 1), m_MatrixWorldViewProj);
	output.Normal = normalize(mul(input.Normal, (float3x3)World));
	//output.Normal = input.Normal;
	output.TexCoord = input.TexCoord;
	float3 pw = mul(float4(input.Position, 1), World).xyz;
	output.EyeVec = (m_MatrixViewInv[3].xyz - pw);
	return output;
}

VSOutput ShellVS(VSInput input)
{
	VSOutput output = (VSOutput)0;

	float mv = 1;
	if(bUseNoise)
	{
		float4 noise = NoiseTexture.SampleLevel(FurSampler, input.TexCoord, 0);
		float mv = (noise.r + noise.g + noise.b) / 3;
		mv = pow(mv, 2);
	}

	float3 pos = input.Position + input.Normal * MaxHairLength * CurrentLayer * mv;
	float4 worldPos = mul(float4(pos,1), m_MatrixWorldViewProj);

	float combFactor = pow(CurrentLayer, 3);
	worldPos.xyz += Comb * combFactor;

	output.Position = worldPos;
	output.Normal	= normalize(mul(input.Normal, (float3x3)World));
	float3 pw		= mul(float4(input.Position, 1), World).xyz;
	output.EyeVec	= (m_MatrixViewInv[3].xyz - pw);
	output.TexCoord = input.TexCoord;
	return output;
}

VSOutput FinVS(VSInput input)
{
	VSOutput output = (VSOutput)0;
	output.Position = float4(input.Position, 1);
	//output.Position = mul(float4(input.Position,1),m_MatrixWorldViewProj);
	float3 pw		= mul(float4(input.Position, 1), World).xyz;
	output.EyeVec	= (m_MatrixViewInv[3].xyz - pw);
	//output.Normal	= input.Normal;
	output.Normal	= normalize(mul(input.Normal, (float3x3)World));
	output.TexCoord = input.TexCoord;
	return output;
}

GSOutput CreateVertex(float3 position, float3 normal, float2 texCoord, float2 texCol, float opac)
{
	GSOutput vertex;
	vertex.Position	= mul(float4(position,1), m_MatrixWorldViewProj);
	vertex.Normal	= normal;
	vertex.TexCoord	= texCoord;
	vertex.TexCol	= texCol;
	vertex.Opacity	= opac;
	return vertex;
}

void MakeFin(VSOutput v[2], float3 direction, float opac, inout TriangleStream<GSOutput> TriStream)
{
	float opacity = opac * MaxOpacity;

	float3 normal = normalize((v[0].Normal + v[1].Normal) / 2);
	float length = 1.25f * MaxHairLength;
	float3 basePoint = (v[0].Position.xyz + v[1].Position.xyz) / 2;
	float3 top = basePoint + normal * length * Comb;

	float3 bleft  = basePoint - direction		;//+ Comb;
	float3 bright = basePoint + direction		;//+ Comb;
	float3 tleft  = bleft + normal * length		+ Comb;
	float3 tright = bright + normal * length	+ Comb;

	float3 s0 = top - bleft;
	float3 s1 = top - bright;
	float3 finNormal = normalize(cross(s0,s1));
	
	float2 texCol = v[1].TexCoord;
	float2 tc = { 0, 1 };
	TriStream.Append(CreateVertex(bleft, finNormal, tc, texCol, opacity));
	tc = float2(0, 0.1);
	TriStream.Append(CreateVertex(tleft, finNormal, tc, texCol, opacity));
	tc = float2(1, 1);
	texCol = v[0].TexCoord;
	TriStream.Append(CreateVertex(bright, finNormal, tc, texCol, opacity));
	tc = float2(1, 0.1);
	TriStream.Append(CreateVertex(tright, finNormal, tc, texCol, opacity));

	TriStream.RestartStrip();
}

[maxvertexcount(4)]
void FinGS(line VSOutput input[2], inout TriangleStream<GSOutput> TriStream)
{   
	float3 eyeVec = normalize((input[0].EyeVec + input[1].EyeVec) / 2);
	float3 lineNormal = normalize((input[0].Normal + input[1].Normal) / 2);
	float eyeDot = dot(lineNormal, eyeVec);

	float maxOffset = 0.075f;
	if(eyeDot < maxOffset && eyeDot > -maxOffset)
	{
		float opacity = 1 - (eyeDot / maxOffset);
		float3 dir = normalize(cross(lineNormal, eyeVec));
		MakeFin(input, dir, opacity, TriStream);
	}
}
//void FinGS(triangle VSOutput input[3], inout TriangleStream<GS_OUTPUT_FINS> TriStream)
//{   
//	float3 eyeVec = normalize(input[0].EyeVec);
//
//    float3 triNormal = normalize(cross(input[0].Position - input[1].Position,
//                                       input[2].Position - input[1].Position));
//	float eyeDot = dot(triNormal, eyeVec);
//
//	if(eyeDot < 0.2 && eyeDot > -0.4)
//	{
//		float3 dir = normalize(cross(triNormal, eyeVec));
//		MakeFin(input, dir, TriStream);
//	}
//}

float4 MeshPS(VSOutput input) : SV_Target
{
	float4 color = ColTexture.Sample(ColorSampler, input.TexCoord);
	float shade = 0.2f;
	color.rgb *= shade;

	//specular
	float power = dot(input.Normal, -gLightDirection);
	float intensity = pow(saturate(power), 5);
	float4 specColor = float4(float3(1,1,1) * intensity, color.a);

	return color + specColor / 6.5f;
}

float4 ShellPS(VSOutput input) : SV_Target
{
	float4 furPixel = FurTexture.Sample(FurSampler, input.TexCoord * Density);
	float4 color = ColTexture.Sample(ColorSampler, input.TexCoord);
	float shade = lerp(0.4f, 1, CurrentLayer);
	
	color *= shade;
	color.a = furPixel.a * (1 - CurrentLayer);
	if(color.a == 0) discard;

	// HalfLambert Diffuse + specular
	float diffuseStrength = dot(input.Normal, -gLightDirection);
	float intensity = pow(saturate(diffuseStrength), 5);
	intensity = lerp(0, intensity, CurrentLayer);
	float4 specColor = float4(float3(1,1,1) * intensity, color.a);

	diffuseStrength = diffuseStrength * 0.5f + 0.5f;
	diffuseStrength = saturate(diffuseStrength);
	color.rgb = color.rgb * diffuseStrength;
	return color + specColor / 2;
}

float4 FinPS(GSOutput input) : SV_Target
{
	float4 offset = FinTexture.Sample(ColorSampler, input.TexCoord);
	float2 tex = input.TexCol;
	//tex -= (offset.xy - 0.5f);

	float4 color = ColTexture.Sample(ColorSampler, tex);		// fur color
	float4 opacity = FinTexture.Sample(ColorSampler, input.TexCoord);	// fin opacity texture
	float transparency = pow(input.TexCoord.y, 1.5f);	// exponential transparency decrease from bottom to top
	transparency = saturate(transparency);
	transparency *= 0.75f;
	color.a = opacity.a * input.Opacity * transparency;

	if(color.a < 0.05f) discard;
	return color;
}

technique11 DefaultTechnique {

	pass p0 {
		SetDepthStencilState(EnableDepthWriting, 0);
		SetRasterizerState(FrontCulling);

		SetVertexShader(CompileShader(vs_4_0, MeshVS()));
		SetPixelShader(CompileShader(ps_4_0, MeshPS()));
	}

	pass p1 {	
		SetDepthStencilState(DisableDepthWriting, 0);
		SetRasterizerState(FrontCulling);	
		SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader(CompileShader(vs_4_0, ShellVS()));
		SetPixelShader(CompileShader(ps_4_0, ShellPS()));
	}

	pass p2 {	
		SetDepthStencilState(DisableDepthWriting, 0);
		SetRasterizerState(NoCulling);	
		SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader(CompileShader(vs_4_0, FinVS()));
		SetGeometryShader(CompileShader(gs_4_0, FinGS()));
		SetPixelShader(CompileShader(ps_4_0, FinPS()));
	}
}