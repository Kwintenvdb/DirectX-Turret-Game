/*
DAE Ubershader Prototype
*Overlord Style*
*Overlord Brecht Kets*
*Overlord Thomas Goussaert*

**This Shader Contains:

- Diffuse (Texture & Color)
	- Regular Diffuse
- Specular
	- Specular Level (Texture & Value)
	- Shininess (Value)
	- Models
		- Blinn
		- Phong
- Ambient (Color)
- EnvironmentMapping (Cubemap)
	- Reflection
		- Fresnel Falloff
	- Refraction
- Normal (Texture)
- Opacity (Texture & Value)

-Techniques
	- WithAlphaBlending
	- WithoutAlphaBlending
*/

//GLOBAL MATRICES
//***************
// The World View Projection Matrix
float4x4 m_MatrixWorldViewProj : WORLDVIEWPROJECTION;
// The ViewInverse Matrix - the third row contains the camera position!
float4x4 m_MatrixViewInv : VIEWINVERSE;
// The World Matrix
float4x4 m_MatrixWorld : WORLD;

//STATES
//******
RasterizerState BackCulling 
{ 
	CullMode = FRONT; 
};

BlendState EnableBlending 
{     
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

//SAMPLER STATES
//**************
SamplerState m_TextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
 	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
};

//LIGHT
//*****
float3 m_LightDirection:DIRECTION
<
	string UIName = "Light Direction";
	string Object = "TargetLight";
> = float3(0.577f, 0.577f, 0.577f);

//DIFFUSE
//*******
bool m_bDiffuseTexture
<
	string UIName = "Diffuse Texture";
	string UIWidget = "Bool";
> = false;

float4 m_ColorDiffuse
<
	string UIName = "Diffuse Color";
	string UIWidget = "Color";
> = float4(1,1,1,1);

Texture2D m_TextureDiffuse
<
	string UIName = "Diffuse Texture";
	string UIWidget = "Texture";
>;

//SPECULAR
//********
float4 m_ColorSpecular
<
	string UIName = "Specular Color";
	string UIWidget = "Color";
> = float4(1,1,1,1);

Texture2D m_TextureSpecularLevel
<
	string UIName = "Specular Level Texture";
	string UIWidget = "Texture";
>;

bool m_bSpecularLevelTexture
<
	string UIName = "Specular Level Texture";
	string UIWidget = "Bool";
> = false;

int m_Shininess<
	string UIName = "Shininess";
	string UIWidget = "Slider";
	float UIMin = 1;
	float UIMax = 100;
	float UIStep = 0.1f;
> = 15;

//AMBIENT
//*******
float4 m_ColorAmbient
<
	string UIName = "Ambient Color";
	string UIWidget = "Color";
> = float4(0,0,0,1);

float m_AmbientIntensity
<
	string UIName = "Ambient Intensity";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
>  = 0.0f;

//NORMAL MAPPING
//**************
bool m_FlipGreenChannel
<
	string UIName = "Flip Green Channel";
	string UIWidget = "Bool";
> = false;

bool m_bNormalMapping
<
	string UIName = "Normal Mapping";
	string UIWidget = "Bool";
> = false;

Texture2D m_TextureNormal
<
	string UIName = "Normal Texture";
	string UIWidget = "Texture";
>;

//ENVIRONMENT MAPPING
//*******************
TextureCube m_CubeEnvironment
<
	string UIName = "Environment Cube";
	string ResourceType = "Cube";
>;

bool m_bEnvironmentMapping
<
	string UIName = "Environment Mapping";
	string UIWidget = "Bool";
> = false;

float m_ReflectionStrength<
	string UIName = "Reflection Strength";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
	float UIStep = 0.1;
>  = 0.0f;

float m_RefractionStrength<
	string UIName = "Refraction Strength";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
	float UIStep = 0.1;
>  = 0.0f;

float m_RefractionIndex<
	string UIName = "Refraction Index";
>  = 0.3f;

//OPACITY
//***************
float m_Opacity<
	string UIName = "Opacity";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
>  = 1.0f;

bool m_bOpacityMap
<
	string UIName = "Opacity Map";
	string UIWidget = "Bool";
> = false;

Texture2D m_TextureOpacity
<
	string UIName = "Opacity Map";
	string UIWidget = "Texture";
>;


//SPECULAR MODELS
//***************
bool m_SpecularBlinn
<
	string UIName = "Specular Blinn";
	string UIWidget = "Bool";
> = false;

bool m_SpecularPhong
<
	string UIName = "Specular Phong";
	string UIWidget = "Bool";
> = false;

//FRESNEL FALLOFF
//***************
bool m_bFresnelFallOff
<
	string UIName = "Fresnel FallOff";
	string UIWidget = "Bool";
> = false;


float4 m_ColorFresnel
<
	string UIName = "Fresnel Color";
	string UIWidget = "Color";
> = float4(1,1,1,1);

float m_FresnelPower<
	string UIName = "Fresnel Power";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 100;
	float UIStep = 0.1;
>  = 1.0f;

float m_FresnelMultiplier<
	string UIName = "Fresnel Multiplier";
	string UIWidget = "slider";
	float UIMin = 1;
	float UIMax = 100;
	float UIStep = 0.1;
>  = 1.0;

float m_FresnelHardness<
	string UIName = "Fresnel Hardness";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 100;
	float UIStep = 0.1;
>  = 0;

//VS IN & OUT
//***********
struct VS_Input
{
	float3 Position: POSITION;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

struct VS_Output
{
	float4 Position: SV_POSITION;
	float4 WorldPosition: COLOR0;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

float3 CalculateSpecularBlinn(float3 viewDirection, float3 normal, float2 texCoord)
{
	float3 hn = normalize(viewDirection + m_LightDirection);
	float specularStrength = dot(-normal, hn);
	specularStrength = saturate(specularStrength);
	specularStrength = pow(specularStrength,m_Shininess);
	float3 specColor =  m_ColorSpecular * specularStrength;
	
	//Use a Texture to control the specular level?
	if(m_bSpecularLevelTexture)
		specColor*= m_TextureSpecularLevel.Sample(m_TextureSampler, texCoord).r;
		
	return specColor;
}

float3 CalculateSpecularPhong(float3 viewDirection, float3 normal, float2 texCoord)
{
	float3 reflectedVector = reflect(m_LightDirection, normal);
	float specularStrength = dot(-viewDirection, reflectedVector);
	specularStrength = saturate(specularStrength);
	specularStrength = pow(specularStrength,m_Shininess);
	float3 specColor =  m_ColorSpecular * specularStrength;
	
	//Use a Texture to control the specular level?
	if(m_bSpecularLevelTexture)
		specColor*= m_TextureSpecularLevel.Sample(m_TextureSampler, texCoord).r;
		
	return specColor;
}

float3 CalculateSpecular(float3 viewDirection, float3 normal, float2 texCoord)
{
	float3 specColor = float3(0,0,0);
	
	if (m_SpecularBlinn)
		specColor += CalculateSpecularBlinn(viewDirection, normal, texCoord);
		
	if (m_SpecularPhong)
		specColor += CalculateSpecularPhong(viewDirection, normal, texCoord);
				
	return specColor;
}

float3 CalculateNormal(float3 tangent, float3 normal, float2 texCoord)
{
	float3 newNormal = normal;
	
	if(m_bNormalMapping)
	{
		float3 binormal = cross(tangent, normal);
		binormal = normalize(binormal);
		if (m_FlipGreenChannel)
		{
			binormal = -binormal;
		}
		float3x3 localAxis = float3x3(tangent, binormal, normal);
	
		float3 sampledNormal = 2.0f * m_TextureNormal.Sample(m_TextureSampler,texCoord)-1.0f;
		newNormal =  mul(sampledNormal, localAxis);
	}
	
	return newNormal;
}

float3 CalculateDiffuse(float3 normal, float2 texCoord)
{
	float3 diffColor = m_ColorDiffuse;
	
	float diffuseStrength = dot(-normal, m_LightDirection);
	diffuseStrength = saturate(diffuseStrength);
	diffColor *= diffuseStrength;
	
	//Use a texture to define the diffuse color?
	if(m_bDiffuseTexture)
		diffColor *= m_TextureDiffuse.Sample(m_TextureSampler, texCoord);
		
	return diffColor;
}

float3 CalculateFresnelFalloff(float3 normal, float3 viewDirection, float3 environmentColor)
{
	if(m_bFresnelFallOff)
	{
		float fresnel = saturate(abs(dot(normal,viewDirection)));
		fresnel = 1 - fresnel;
		fresnel = pow(fresnel,m_FresnelPower);
		fresnel = saturate(fresnel)*m_FresnelMultiplier;
		
		float fresnelMask =1.0f;
		fresnelMask = 1 - saturate(dot(float3(0.0,1.0,0.0),normal));
		fresnelMask = pow(fresnelMask, m_FresnelHardness);
		fresnel *= fresnelMask;
		
		if(m_bEnvironmentMapping)
			return environmentColor*fresnel;
		else
			return m_ColorFresnel*fresnel;
	}
	
	if(m_bEnvironmentMapping)
		return environmentColor;
	
	return float3(0,0,0);
}

float3 CalculateEnvironment(float3 viewDirection, float3 normal)
{
	float3 environmentColor = float3(0,0,0);
	if(m_bEnvironmentMapping)
	{
		float3 reflectedVector = reflect(viewDirection,normal);
		float3 refractedVector = refract(viewDirection,normal,m_RefractionIndex);
		environmentColor = (m_CubeEnvironment.Sample(m_TextureSampler,reflectedVector) * m_ReflectionStrength)
							+(m_CubeEnvironment.Sample(m_TextureSampler,refractedVector) * m_RefractionStrength);	
	}	
	
	return environmentColor;
}

float CalculateOpacity(float2 texCoord)
{
	float opacity = m_Opacity;
	if (m_bOpacityMap)
		opacity *= m_TextureOpacity.Sample(m_TextureSampler, texCoord).r;
	
	return opacity;
}

// The main vertex shader
VS_Output MainVS(VS_Input input) {
	
	VS_Output output = (VS_Output)0;
	
	output.Position = mul(float4(input.Position, 1.0), m_MatrixWorldViewProj);
	output.WorldPosition = mul(float4(input.Position,1.0), m_MatrixWorld);
	output.Normal = mul(input.Normal, (float3x3)m_MatrixWorld);
	output.Tangent = mul(input.Tangent, (float3x3)m_MatrixWorld);
	output.TexCoord = input.TexCoord;
	
	return output;
}

// The main pixel shader
float4 MainPS(VS_Output input) : SV_TARGET {
	// NORMALIZE
	input.Normal = normalize(input.Normal);
	input.Tangent = normalize(input.Tangent);
	input.TexCoord = normalize(input.TexCoord);
	
	float3 viewDirection = normalize(input.WorldPosition.xyz - m_MatrixViewInv[3].xyz);
	
	//NORMAL
	float3 newNormal = CalculateNormal(input.Tangent, input.Normal, input.TexCoord);
		
	//SPECULAR
	float3 specColor = CalculateSpecular(viewDirection, newNormal, input.TexCoord);
		
	//DIFFUSE
	float3 diffColor = CalculateDiffuse(newNormal, input.TexCoord);
		
	//AMBIENT
	float3 ambientColor = m_ColorAmbient * m_AmbientIntensity;
	
	//ENVIRONMENT MAPPING
	float3 environmentColor = CalculateEnvironment(viewDirection, newNormal);
	
	//FRESNEL FALLOFF
	environmentColor = CalculateFresnelFalloff(newNormal, viewDirection, environmentColor);
		
	//FINAL COLOR CALCULATION
	float3 finalColor = diffColor + specColor + environmentColor + ambientColor;
	
	//OPACITY
	float opacity = CalculateOpacity(input.TexCoord);
	
	return float4(finalColor,opacity);
}

// Default Technique
technique10 WithAlphaBlending {
	pass p0 {
		SetRasterizerState(BackCulling);
		SetBlendState(EnableBlending,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}

// Default Technique
technique10 WithoutAlphaBlending {
	pass p0 {
		SetRasterizerState(BackCulling);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}

