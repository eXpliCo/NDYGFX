
#include "stdafx.fx"
#include "SSAO.fx"
#include "PPHighlightArea.fx"

// Marcus L�wegren

// For textures
Texture2D Texture;
Texture2D NormalAndDepth;
Texture2D Position;
Texture2D Specular;
SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

Texture2D ShadowMap[10];
Texture2D CascadedShadowMap[10];
SamplerState shadowMapSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};

DepthStencilState DisableDepthWrite
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};
BlendState SrcAlphaBlendingAdd 
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
//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------
cbuffer ef
{
	//Single sampling
	float SMAP_DX; 

	//PCF sampling
	bool usePCF;
	float PCF_SIZE;
	float PCF_SIZE_SQUARED;

	//Multiple sampling
	bool blendCascades;
	float blendStrength;

	//Both
	uint nrOfCascades;
	float4 cascadeFarPlanes;
};




struct VSIn
{
	float4 Pos : POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float4 Color : COLOR;
};

struct PSSceneIn
{
	float4 Pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

//-----------------------------------------------------------------------------------------
// State Structures
//-----------------------------------------------------------------------------------------
RasterizerState NoCulling
{
	CullMode = NONE;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VSIn VSScene(VSIn input)
{
	 // Transform the shadow texture coordinates for all the cascades.
	//  Output.vTexShadow = mul( Input.vPosition, m_mShadow ); //**TILLMAN


	return input;
}


// GS
[maxvertexcount(4)]
void GS( point VSIn input[1], inout TriangleStream<PSSceneIn> triStream )
{
	PSSceneIn output;

	//bottom left
	output.Pos = float4(-1, -1, 0, 1);
	output.tex = float2(0,1);
	triStream.Append(output);
	
	//bottom right
	output.Pos = float4(1, -1, 0, 1);
	output.tex = float2(1,1);
	triStream.Append(output);

	//top left
	output.Pos = float4(-1, 1, 0, 1);
	output.tex = float2(0,0);
	triStream.Append(output);

	//top right
	output.Pos = float4(1, 1, 0, 1);
	output.tex = float2(1,0);
	triStream.Append(output);
}

uint FindCascadeToBlendWith(float shadowMapTextureCoordViewSpaceDepth)
{
	//anv�nda cascadeFarPlanes**
	return -1;
}
float SampleCascades(uint cascadeIndex, uint otherCascadeIndex, float2 shadowMapTextureCoords, float shadowMapdepth) 
{
	float CSM_SHADOW_EPSILON = 0.002f; //**�ndra beroende p� CSM level/slice**
	float shadow = 1.0f; 
	
	if(blendCascades) //global variable
	{
		//**if(usePCF)??**
	}
	else
	{
		if(usePCF)
		{
			for(float s = 0; s < PCF_SIZE; s++)
			{
				for(float q = 0; q < PCF_SIZE; q++)
				{
					shadow += (CascadedShadowMap[cascadeIndex].SampleLevel(shadowMapSampler, shadowMapTextureCoords + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < shadowMapdepth) ? 0.0f : 1.0f;
				}
			}
			shadow *= PCF_SIZE_SQUARED;
		}
		else
		{
			shadow = (CascadedShadowMap[cascadeIndex].SampleLevel(shadowMapSampler, shadowMapTextureCoords + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < shadowMapdepth) ? 0.0f : 1.0f;
		}
	}

	/**ev: if(shadowMapTextureCoords.x < 0 || shadowMapTextureCoords.x > 1 || shadowMapTextureCoords.y < 0 || shadowMapTextureCoords.y > 1)
		{
			shadow = 1.0f;
		}
		else if(depth > 1.0f)
		{
			shadow = 1.0f;
		}***/
	

	return shadow;
}
		


//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSScene(PSSceneIn input) : SV_Target
{	
	float3 DiffuseColor = Texture.Sample(linearSampler, input.tex).xyz;	

	float4 NormsAndDepth = NormalAndDepth.Sample(linearSampler, input.tex);
	
	float4 WorldPos = Position.Sample(linearSampler, input.tex);

	float4 AmbientLight = SceneAmbientLight;

	float SpecularPower = Specular.Sample(linearSampler, input.tex).w;
	float4 SpecularColor = float4(Specular.Sample(linearSampler, input.tex).xyz, 1.0f);

	float diffuseLighting = 0.0f;
	float specLighting = 0.0f;
	
	for(int i = 0; i < NrOfLights; i++)
	{
		float3 LightDirection = WorldPos.xyz - lights[i].LightPosition.xyz;
		float DistanceToLight = length(LightDirection);
		LightDirection = normalize(LightDirection);

		// Diff light
		float difflight = saturate(dot(NormsAndDepth.xyz, -LightDirection));

		// Spec Light
		float3 h = normalize(normalize(CameraPosition.xyz - WorldPos.xyz) - LightDirection);
		float speclight = pow(saturate(dot(h, NormsAndDepth.xyz)), SpecularPower);


		// Shadow Mappings
		float4 posLight = mul(WorldPos, lights[i].LightViewProj);
		posLight.xy /= posLight.w;


		//Compute shadow map tex coord
		float2 smTex = float2(0.5f*posLight.x, -0.5f*posLight.y) + 0.5f;

		// Compute pixel depth for shadowing.
		float depth = posLight.z / posLight.w;

		float SHADOW_EPSILON = 0.00001f;			////////////// PUT THIS WHERE?


		//float PCF_SIZE = 3.0f;								////// Not able to move this to cbuffer, why?


		// PCF
		float shadow = 0.0f;
		if(smTex.x < 0 || smTex.x > 1 || smTex.y < 0 || smTex.y > 1)
			shadow = 1.0f;
		else if(depth > 1.0f)
			shadow = 1.0f;
		else
		{
			for(float s = 0; s < PCF_SIZE; s++)
			{
				for(float q = 0; q < PCF_SIZE; q++)
				{
					shadow += (ShadowMap[i].SampleLevel(shadowMapSampler, smTex + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
				}
			}
			shadow *= PCF_SIZE_SQUARED;
		}


		// Fall off test 3
		float coef = (lights[i].LightIntensity / 1000.0f) + (DistanceToLight * DistanceToLight) / (lights[i].LightIntensity * lights[i].LightIntensity);

		difflight /= coef;

		/// For slower fall-off on spec light. Doesnt work with quad-based culling tho.
		//if(coef > 10.0f)
		//	speclight /= coef / 10.0f;
			
		speclight /= coef;
		
		difflight *= shadow;
		speclight *= shadow;
		
		diffuseLighting += difflight;
		specLighting += speclight;
	}
	
	if(NrOfLights > 0)
	{
		diffuseLighting = saturate(diffuseLighting / NrOfLights);
		specLighting = saturate(specLighting / NrOfLights);
	}
	
	//TILLMAN START OF CSM
	// Sun
	if(UseSun)
	{
		// Diff light
		float diffLight = saturate(dot(NormsAndDepth.xyz, -sun.Direction)) * sun.LightIntensity;
		// Spec Light
		float3 h = normalize(normalize(CameraPosition.xyz - WorldPos.xyz) - sun.Direction);
		float specLight = pow(saturate(dot(h, NormsAndDepth.xyz)), SpecularPower) * sun.LightIntensity;

		
		//I PS:
		//Determine the proper shadow map.
		//Transforms the texture coordinates if necessary.
		//Samples the cascade.
		//Lights the pixel.

		
		
		//Transform the texture coordinates to texel space. **Tillman - ev. ers�tta/skicka med inversmatris?**
		/*float4 shadowMapTextureCoordViewSpace = input.shadowMapTextureCoordViewSpace; //vertex shader**
		float4 lightPosition = mul(WorldPos, cascades[cascadeIndex].viewProj); //Transform pixel from world space to **Tillman**[-w,w].
		lightPosition.xy /= lightPosition.w; //Transform to **Tillman**[-1,1].
		float2 texTexelSpace = float2(0.5f * lightPosition.x, -0.5f * lightPosition.y) + 0.5f; //Transform to texel space [0,1].
			*/

		//Determine the shadow map to use.
		uint cascadeIndex = 0;
		/*for(int i = 0; i < nrOfCascades; i++)
		{
			//Check if the shadow map tex coord (in view space) is inside the frustum.
			if(shadowMapTextureCoordViewSpace.z < cascadeFarPlanes[i]) //(cascadeFarPlanes contains the z-distance(in view space)).
			{
				cascadeIndex = i;
				break; //Stop looping.
			}
		}

		//Determine the second shadow map to use if blending between cascades shall be done.
		uint otherCascadeIndex = -1;
		bool blendCascades = false; //**TILLMAN TMP**
		if(blendCascades)
		{
			otherCascadeIndex = FindCascadeToBlendWith(shadowMapTextureCoordViewSpace.z); //**internt: cascadeFarPlanes**
		}

		//Sample the cascade(s)
		shadow = SampleCascades(cascadeIndex, otherCascadeIndex, shadowMapTextureCoord, shadowMapdepth); //**internt blendCascades
		
		*/



		
		
		float distancePixel = length(CameraPosition.xyz - WorldPos.xyz);
		if(distancePixel > cascadeFarPlanes.x * 1.1f) //**tillman - m�ste kolla om pixeln �r innanf�r frustumet, och inte radie fr�n kamera
		{
			cascadeIndex = 1;
		}
		if(distancePixel > cascadeFarPlanes.y * 1.1f) //**tillman - m�ste kolla om pixeln �r innanf�r frustumet, och inte radie fr�n kamera
		{
			cascadeIndex = 2;
		}
		if(distancePixel > cascadeFarPlanes.z * 1.1f) //**tillman - m�ste kolla om pixeln �r innanf�r frustumet, och inte radie fr�n kamera
		{
			cascadeIndex = 3;
		}

		float4 posLight = mul(WorldPos, cascades[cascadeIndex].viewProj); //**TILLMAN "i"
		posLight.xy /= posLight.w;
		float2 smTex = float2(0.5f*posLight.x, -0.5f*posLight.y) + 0.5f;
		
		// Compute pixel depth for shadowing.
		float depth = posLight.z / posLight.w;

		//A value of 0.004f is required to remove all self-shadowing. //**TILLMAN - 0.004f f�r helt, 0.002f f�r self?**
		float CSM_SHADOW_EPSILON = 0.002f;			////////////// PUT THIS WHERE?

		/*float strength = 1.0f;
		uint nrOfCSMs = 3;
		float z = strength * n pow(f / n),i / nrOfCSMs) + (1 - strength) * (n + (i / nrOfCSMs) * (f - n);
		*/

		//float PCF_SIZE = 3.0f;								////// Not able to move this to cbuffer, why?

		// PCF
		float shadow = 0.0f;
		if(smTex.x < 0 || smTex.x > 1 || smTex.y < 0 || smTex.y > 1)
		{
			shadow = 1.0f;
		}
		else if(depth > 1.0f)
		{
			shadow = 1.0f;
		}
		else
		{
			for(float s = 0; s < PCF_SIZE; s++)
			{
				for(float q = 0; q < PCF_SIZE; q++)
				{
					if(cascadeIndex == 0)
					{
						shadow += (CascadedShadowMap[0].SampleLevel(shadowMapSampler, smTex + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
					}
					if(cascadeIndex == 1)
					{
						shadow += (CascadedShadowMap[1].SampleLevel(shadowMapSampler, smTex + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
					}
					if(cascadeIndex == 2)
					{
						shadow += (CascadedShadowMap[2].SampleLevel(shadowMapSampler, smTex + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
					}
					if(cascadeIndex == 3)
					{
						shadow += (CascadedShadowMap[3].SampleLevel(shadowMapSampler, smTex + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
					}
					//**TILLMAN test loop: ** shadow += (CascadedShadowMap[cascadeIndex].SampleLevel(shadowMapSampler, smTex + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
					
				}
			}
			shadow *= PCF_SIZE_SQUARED;
		}

		
		
		diffLight *= shadow;
		specLight *= shadow;
		
		//**tillman end of CSM

		diffuseLighting += diffLight;
		specLighting += specLight;

		diffuseLighting = saturate(diffuseLighting);
		specLighting = saturate(specLighting);
	}

	// Reduction of ambientlight if diffuse and spec is high to prevent oversaturation.
	AmbientLight /= 1.0f + diffuseLighting + specLighting;	

	float4 finalColor = float4((							
		AmbientLight.xyz * DiffuseColor + 
		DiffuseColor * diffuseLighting + 
		SpecularColor.xyz * specLighting), 
		1.0f);


	
	// Haxfix, want it above but I lose 75% of my FPS then (??!?!? :S:S:S:S:S)
	if(NormsAndDepth.w < -0.5f)		// All pixels that has a negative depth means that there is no geometry, therefor go without lightcalcs.
		finalColor = float4(DiffuseColor, 1.0f);

	if(NormsAndDepth.w > 1.0f)		// All pixels that has a greater than 1 depth means that there is no geometry and there is skybox, therefor go without lightcalcs.
		finalColor = float4(DiffuseColor, 1.0f);
	
	

	
	
	//temp:
	//ev. todo: if player is on red team, reduce redness and increase blueness**
	
	
	uint TeamColor = (uint)Texture.Sample(linearSampler, input.tex).w;
	//Exlude skybox
	if(NormsAndDepth.w < 1.00001f)
	{
		//Exclude nullColor
		if(TeamColor != 0)
		{
			switch(TeamColor)
			{
				case 1: finalColor.xyz += (WHITE.xyz * 0.2f); break;
				case 2: finalColor.xyz += (BLACK.xyz * 0.2f); break;
				case 3: finalColor.xyz += (RED.xyz * 0.2f); break;
				case 4: finalColor.xyz += (GREEN.xyz * 0.2f); break;
				case 5: finalColor.xyz += (BLUE.xyz * 0.2f); break;
				case 6: finalColor.xyz += (YELLOW.xyz * 0.2f); break;
				case 7: finalColor.xyz += (CYAN.xyz * 0.2f); break;
				case 8: finalColor.xyz += (MAGENTA.xyz * 0.2f); break;
			}
			finalColor.xyz *= (5.0f / 6.0f);
		}
	}


		
	//if(finalColor.a >= 0.00001f && finalColor.a <= 0.9999f) //**tillman - haxl�sning?**
	//{
		//finalColor = SSAO(input.tex, NormalAndDepth, Position); //**tillman opt(position tex)**
	
	finalColor.rgb = HighlightArea(WorldPos.xz, finalColor.rgb); //Position is in world space
	finalColor.a = 1.0f;
		
	//}



	///////////////////////////////////////////////////////////////////
	//							Basic fog:							//
	//////////////////////////////////////////////////////////////////
	finalColor = saturate(finalColor);
	float fogDepth = NormsAndDepth.w;
	if(fogDepth > 0.75f)
	{
		float fogfactor = (fogDepth - 0.75f) * 4.1f;	// Linear scale the last 25% of farclip, but a little more 
		finalColor = lerp(finalColor, float4(0.45f, 0.45f, 0.45f, 1.0f), saturate(fogfactor));
	}
			

	
	return saturate(finalColor);
}

//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 BasicTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    

		SetDepthStencilState( DisableDepthWrite, 0 );
	    SetRasterizerState( NoCulling );
		
		SetBlendState( SrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }  
}