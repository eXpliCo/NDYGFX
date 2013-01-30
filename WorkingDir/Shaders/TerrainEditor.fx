//-----------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska H�gskola.
//	
//	This shader renders geometry (deferred with 4 GBuffers) with up to 4 textures(blendmap).
//-----------------------------------------------------------------------------------------
#include "stdafx.fx"

//-----------------------------------------------------------------------------------------
//	Global variables (non-numeric values cannot be added to a constantbuffer.)
//-----------------------------------------------------------------------------------------
//Textures used to make the blend map
Texture2D tex0; //R-channel in blendmap0. 
Texture2D tex1; //G-channel in blendmap0. 
Texture2D tex2; //B-channel in blendmap0. 
Texture2D tex3; //A-channel in blendmap0.
Texture2D tex4; //R-channel in blendmap1. 
Texture2D tex5; //G-channel in blendmap2. 
Texture2D tex6; //B-channel in blendmap3. 
Texture2D tex7; //A-channel in blendmap4.
Texture2D<float4> blendMap0; //**TILLMAN TODO: testa s�nka format 
Texture2D<float4> blendMap1; //**TILLMAN TODO: testa s�nka format 
//Texture2D<uint> AIMap;
Texture2D AIMap; //Format = DXGI_FORMAT_R8_UNORM.
//Texture2D<int> AIMap; //**uint


//-----------------------------------------------------------------------------------------
// Constant buffers
//-----------------------------------------------------------------------------------------
cbuffer PerObject
{
	//Matrices
	matrix	WVP;
	matrix	worldMatrix;
	matrix	worldMatrixInverseTranspose;

	//Texture
	bool	textured;
	bool	blendMapped;
	uint	nrOfBlendMaps;
	float	textureScale;

	//Material
	float	specularPower;
	float3	specularColor;
	float3	diffuseColor;

	//AI(editor)
	bool useAIMap;
	float nodesPerSide;
	float AIGridThickness;
};
//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------
struct VSIn
{
	float3 pos		: POSITION;
	float2 tex		: TEXCOORD;
	float3 norm		: NORMAL;
	float3 color	: COLOR;
};

struct PSSceneIn
{
	float4 pos		: SV_POSITION; 
	float2 tex		: TEXCOORD;
	float3 norm		: NORMAL;
	float3 color	: COLOR;

	float4 posW		: POSITION;	//world position 
};

struct PSOut			
{
	float4 Texture			: SV_TARGET0;	//Texture XYZ, unused W
	float4 NormalAndDepth	: SV_TARGET1;	//Normal XYZ, depth W
	float4 Position			: SV_TARGET2;	//Position XYZ, unused W
	float4 Specular			: SV_TARGET3;	//Specular XYZ, specular power W
};

float3 RenderTextured(float scale, float2 tex, bool useBlendMap)
{
	//Sample R,G,B,A textures
	float2 texCoord = scale * tex;
	float3 tex0Color = tex0.Sample(LinearWrapSampler, texCoord).rgb; 
	float3 tex1Color = tex1.Sample(LinearWrapSampler, texCoord).rgb; 
	float3 tex2Color = tex2.Sample(LinearWrapSampler, texCoord).rgb; 
	float3 tex3Color = tex3.Sample(LinearWrapSampler, texCoord).rgb; 
	
	if(useBlendMap)
	{
		//Sample blend map texture
		float4 blendMap0Color = blendMap0.Sample(LinearClampSampler, tex); 
		
		// Is blendmap 0 empty?
		if ( length(blendMap0Color) > 0.0 )
		{	
			// Are we using multiple blendmaps?
			if(nrOfBlendMaps == 2)
			{
				// Is blendmap 2 empty?
				float4 blendMap1Color = blendMap1.Sample(LinearClampSampler, tex);
				if ( length(blendMap1Color) > 0.0 )
				{
					float distance = sqrt(
										pow(blendMap0Color.r, 2.0) + pow(blendMap1Color.r, 2.0) + 
										pow(blendMap0Color.g, 2.0) + pow(blendMap1Color.g, 2.0) + 
										pow(blendMap0Color.b, 2.0) + pow(blendMap1Color.b, 2.0) + 
										pow(blendMap0Color.a, 2.0) + pow(blendMap1Color.a, 2.0) 
										);
					
					float totalLength = length(blendMap0Color) + length(blendMap1Color);
					float4 blendValues0 = normalize(blendMap0Color) * length(blendMap0Color) / totalLength;
					float4 blendValues1 = normalize(blendMap1Color) * length(blendMap1Color) / totalLength;
					
					// Scale color for each texture by the weight in the blendmap and scale to [0,1]
					float3 tex0Color = tex0.Sample(LinearWrapSampler, texCoord).rgb * blendValues0.r;
					float3 tex1Color = tex1.Sample(LinearWrapSampler, texCoord).rgb * blendValues0.g;
					float3 tex2Color = tex2.Sample(LinearWrapSampler, texCoord).rgb * blendValues0.b;
					float3 tex3Color = tex3.Sample(LinearWrapSampler, texCoord).rgb * blendValues0.a;
				
					// Scale color for each texture by the weight in the blendmap and scale to [0,1]
					float3 tex4Color = tex4.Sample(LinearWrapSampler, texCoord).rgb * blendValues1.r; 
					float3 tex5Color = tex5.Sample(LinearWrapSampler, texCoord).rgb * blendValues1.g; 
					float3 tex6Color = tex6.Sample(LinearWrapSampler, texCoord).rgb * blendValues1.b; 
					float3 tex7Color = tex7.Sample(LinearWrapSampler, texCoord).rgb * blendValues1.a; 
		
					//Blendmapped color 
					return saturate(tex0Color + tex1Color + tex2Color + tex3Color + tex4Color + tex5Color + tex6Color + tex7Color) * diffuseColor;
				}
				else
				{
					// Only Blendmap 0
					float4 blendValues0 = normalize(blendMap0Color); //normalize (don't use texture scaling).
			
					float3 tex0Color = tex0.Sample(LinearWrapSampler, texCoord).rgb * blendValues0.r;
					float3 tex1Color = tex1.Sample(LinearWrapSampler, texCoord).rgb * blendValues0.g;
					float3 tex2Color = tex2.Sample(LinearWrapSampler, texCoord).rgb * blendValues0.b;
					float3 tex3Color = tex3.Sample(LinearWrapSampler, texCoord).rgb * blendValues0.a;
					
					// Blendmapped color 
					return saturate(tex0Color + tex1Color + tex2Color + tex3Color) * diffuseColor;
				}
			}
			else
			{
				// Only Blendmap 0
				float4 blendValues0 = normalize(blendMap0Color); //normalize (don't use texture scaling).
			
				float3 tex0Color = tex0.Sample(LinearWrapSampler, texCoord).rgb * blendValues0.r;
				float3 tex1Color = tex1.Sample(LinearWrapSampler, texCoord).rgb * blendValues0.g;
				float3 tex2Color = tex2.Sample(LinearWrapSampler, texCoord).rgb * blendValues0.b;
				float3 tex3Color = tex3.Sample(LinearWrapSampler, texCoord).rgb * blendValues0.a;
				
				// Blendmapped color 
				return saturate(tex0Color + tex1Color + tex2Color + tex3Color) * diffuseColor;
			}
		}
		else
		{
			// Only Blendmap 1
			float4 blendMap1Color = blendMap1.Sample(LinearClampSampler, tex); //normalize (don't use texture scaling).
			if ( length(blendMap1Color) > 0.0 )
			{
				float4 blendValues1 = normalize(blendMap0Color); //normalize (don't use texture scaling).
			
				float3 tex4Color = tex4.Sample(LinearWrapSampler, texCoord).rgb * blendValues1.r;
				float3 tex5Color = tex5.Sample(LinearWrapSampler, texCoord).rgb * blendValues1.g;
				float3 tex6Color = tex6.Sample(LinearWrapSampler, texCoord).rgb * blendValues1.b;
				float3 tex7Color = tex7.Sample(LinearWrapSampler, texCoord).rgb * blendValues1.a;
				
				// Blendmapped color 
				return saturate(tex4Color + tex5Color + tex6Color + tex7Color) * diffuseColor;
			}
		}
	}
	
	return saturate((tex0Color + tex1Color + tex2Color + tex3Color) * diffuseColor);
}

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	PSSceneIn output = (PSSceneIn)0;
	output.pos = mul(float4(input.pos, 1.0f), WVP);
	output.tex = input.tex;
	output.norm = normalize(mul(input.norm, (float3x3)worldMatrixInverseTranspose));
	output.color = input.color;
	output.posW = mul(float4(input.pos, 1.0f), worldMatrix);

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSScene
//-----------------------------------------------------------------------------------------
PSOut PSScene(PSSceneIn input) : SV_Target
{	
	PSOut output = (PSOut)0; 

	//Texture RT
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	if(useAIMap) //AIMap color
	{
		float boolColor = AIMap.Sample(PointClampSampler, input.tex).r;

		if(textured)
		{
			finalColor = RenderTextured(textureScale, input.tex, blendMapped);
		}
		//If node is blocked, add 50% redness (true = 1 = red).
		if(boolColor == 1.0f)
		{
			finalColor.r += 0.5f; //Render non-traversable/blocked nodes
			finalColor.r *= 0.66666666f;
		}

		//Render grid
		float deltaXY = 1.0f / nodesPerSide;
		float texX = fmod(input.tex.x, deltaXY);
		float texY = fmod(input.tex.y, deltaXY);

		if(texX > deltaXY - (AIGridThickness * 0.5f) || texX < (AIGridThickness * 0.5f) || 
			texY > deltaXY - (AIGridThickness * 0.5f) || texY < (AIGridThickness * 0.5f)) 
		{
			finalColor.rgb = 1.0f;
		}
	}
	else if(textured) //Texture color(s)
	{
		finalColor = RenderTextured(textureScale, input.tex, blendMapped);
	}
	else //Geometry color
	{
		finalColor = input.color; 
	}

	//RENDERTARGET DATA:
	//Texture RT
	output.Texture.xyz = finalColor;
	output.Texture.w = -1.0f;

	//NormalAndDepth RT
	output.NormalAndDepth = float4(input.norm, input.pos.z / input.pos.w);	
	float depth = length(CameraPosition.xyz - input.posW.xyz) / FarClip;		// Haxfix
	output.NormalAndDepth.w = depth;

	//Position RT
	output.Position = input.posW;
	
	//Specular RT
	output.Specular.xyz = specularColor;
	output.Specular.w = specularPower;
	
	return output;
}


//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 TerrainEditorTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    

		SetDepthStencilState( EnableDepth, 0 );
	    SetRasterizerState( BackCulling );
    }  
}