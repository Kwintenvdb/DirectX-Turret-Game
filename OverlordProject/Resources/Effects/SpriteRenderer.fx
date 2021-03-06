float4x4 gTransform : WorldViewProjection;
Texture2D gSpriteTexture;
float2 gTextureSize;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

BlendState EnableBlending 
{     
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

DepthStencilState NoDepth
{
	DepthEnable = FALSE;
};

RasterizerState BackCulling 
{ 
	CullMode = BACK; 
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
	uint TextureId: TEXCOORD0;
	float4 TransformData : POSITION; //PosX, PosY, Depth (PosZ), Rotation
	float4 TransformData2 : POSITION1; //PivotX, PivotY, ScaleX, ScaleY
	float4 Color: COLOR;	
};

struct GS_DATA
{
	float4 Position : SV_POSITION;
	float4 Color: COLOR;
	float2 TexCoord: TEXCOORD0;
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, float rotation, float2 rotCosSin, float2 offset, float2 pivotOffset)
{
	if (rotation != 0)
	{

		pos -= float3(pivotOffset,0);
		float3 origpos = pos;
		pos.x = (origpos.x * rotCosSin.x)-(origpos.y * rotCosSin.y);
		pos.y = (origpos.y * rotCosSin.x)+(origpos.x * rotCosSin.y);
		//Step 3.
		//Do rotation calculations
		//Transform to origin
		//Rotate
		//Retransform to initial position
		
	}
	else
	{
		//Step 2.
		//No rotation calculations (no need to do the rotation calculations if there is no rotation applied > redundant operations)
		//Just apply the pivot offset
		pos -= float3( pivotOffset,0 );
	}

	pos += float3(offset, 0);
	//Geometry Vertex Output
	GS_DATA geomData = (GS_DATA) 0;
	geomData.Position = mul(float4(pos, 1.0f), gTransform);
	geomData.Color = col;
	geomData.TexCoord = texCoord;
	triStream.Append(geomData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//Given Data (Vertex Data)
	float3 position = vertex[0].TransformData.xyz; //Extract the position data from the VS_DATA vertex struct
	float2 offset = float2( vertex[0].TransformData.x, vertex[0].TransformData.y ); //Extract the offset data from the VS_DATA vertex struct (initial X and Y position)
	float rotation = vertex[0].TransformData.w; //Extract the rotation data from the VS_DATA vertex struct
	float2 pivot = float2( vertex[0].TransformData2.x, vertex[0].TransformData2.y ); //Extract the pivot data from the VS_DATA vertex struct
	float2 scale = float2( vertex[0].TransformData2.z, vertex[0].TransformData2.w ); //Extract the scale data from the VS_DATA vertex struct
	float2 texCoord = float2(0, 0); //Initial Texture Coordinate
	
	float2 pivotoffset = pivot*gTextureSize*scale;
	//...

	
	// LT----------RT //TringleStrip (LT > RT > LB, LB > RB > RT)
	// |          / |
	// |       /    |
	// |    /       |
	// | /          |
	// LB----------RB

	float2 rotCosSin=float2(0,0);
	if( rotation!=0 )
	{
		rotCosSin.x = cos( rotation );
		rotCosSin.y = sin( rotation );
	}

	//VERTEX 1 [LT]
	position.x=0;
	position.y=0;
	CreateVertex(triStream, position, vertex[0].Color, float2(0,0), rotation, rotCosSin, offset, pivotoffset ); //Change the color data too!

	//VERTEX 2 [RT]
	position.x = gTextureSize.x*scale.x;
	CreateVertex(triStream, position, vertex[0].Color, float2(1,0), rotation, rotCosSin, offset, pivotoffset ); //Change the color data too!

	//VERTEX 3 [LB]
	position.y = gTextureSize.y*scale.y;
	position.x = 0;
	CreateVertex(triStream, position, vertex[0].Color, float2(0,1), rotation, rotCosSin, offset, pivotoffset ); //Change the color data too!

	//VERTEX 4 [RB]
	position.x = gTextureSize.x * scale.x;
	CreateVertex(triStream, position, vertex[0].Color, float2(1,1), rotation, rotCosSin, offset, pivotoffset ); //Change the color data too!
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET {

	return gSpriteTexture.Sample(samPoint, input.TexCoord) * input.Color;	
}

// Default Technique
technique11 Default {

	pass p0 {
		SetRasterizerState(BackCulling);
		SetBlendState(EnableBlending,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		//SetDepthStencilState(NoDepth,0);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}
