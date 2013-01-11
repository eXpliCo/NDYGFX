﻿#include "DxManager.h"

#include "DxManagerDebugging.h"

HRESULT DxManager::Update(float)
{
	// update subsystems
	//ps.update(deltaTime);
	//this->camera->Update(deltaTime);				// Moved to Life below to counter update spasms due to thread-collision

	return S_OK;
}

void DxManager::HandleTerrainEvent(TerrainEvent* me)
{
	string msg = me->getMessage();
	if(msg == "Add Terrain")
	{
		this->terrains.add(me->GetTerrain());
	}
	else if(msg == "Delete Terrain")
	{
		Terrain* terrain = me->GetTerrain();
		for(int i = 0; i < this->terrains.size(); i++)
		{
			if(this->terrains[i] == terrain)
			{
				delete this->terrains.getAndRemove(i);
				terrain = NULL;
			}
		}
	}
}

void DxManager::HandleMeshEvent(MeshEvent* me)
{
	string msg = me->getMessage();
	if(msg == "Add Mesh")
	{
		this->objects.add(me->GetStaticMesh());
	}
	else if(msg == "Delete Mesh")
	{
		StaticMesh* mesh = me->GetStaticMesh();
		for(int i = 0; i < this->objects.size(); i++)
		{
			if(this->objects[i] == mesh)
			{
				delete this->objects.getAndRemove(i);
				mesh = NULL;
			}
		}
	}
	if(msg == "Add AniMesh")
	{
		this->animations.add(me->GetAnimatedMesh());
	}
	else if(msg == "Delete AniMesh")
	{
		AnimatedMesh* mesh = me->GetAnimatedMesh();
		for(int i = 0; i < this->animations.size(); i++)
		{
			if(this->animations[i] == mesh)
			{
				delete this->animations.getAndRemove(i);
				mesh = NULL;
			}
		}
	}
}

void DxManager::HandleLightEvent(LightEvent* le)
{
	string msg = le->getMessage();
	if(msg == "Add Light with shadows")
	{
		le->GetLight()->InitShadowMap(this->Dx_Device, this->params.ShadowMapSettings);
		this->lights.add(le->GetLight());
	}
	else if(msg == "Add Light")
	{
		this->lights.add(le->GetLight());
	}
	else if(msg == "Delete Light")
	{
		Light* light = le->GetLight();
		for(int i = 0; i < this->lights.size(); i++)
		{
			if(this->lights[i] == light)
			{
				//delete this->lights.getAndRemove(i);
				light = NULL;
			}
		}
	}
}

void DxManager::HandleImageEvent(ImageEvent* ie)
{
	string msg = ie->getMessage();
	if(msg == "Add Image")
		this->images.add(ie->GetImage());
	else if(msg == "Delete Image")
	{
		Image* img = ie->GetImage();
		for(int i = 0; i < this->images.size(); i++)
		{
			if(this->images[i] == img)
			{
				delete this->images.getAndRemove(i);
				img = NULL;
			}
		}
	}
}

void DxManager::HandleTextEvent(TextEvent* te)
{
	string msg = te->getMessage();
	if(msg == "Add Text")
		this->texts.add(te->GetText());
	else if(msg == "Delete Text")
	{
		Text* txt = te->GetText();
		for(int i = 0; i < this->texts.size(); i++)
		{
			if(this->texts[i] == txt)
			{
				delete this->texts.getAndRemove(i);
				txt = NULL;
			}
		}
	}
}

void DxManager::Life()
{
	//Engine Start Splash screen.
	Image* img = new Image(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2((float)this->params.windowWidth, (float)this->params.windowHeight));
	Texture* tex = NULL;
	tex = GetResourceManager()->CreateTextureFromFile("Media/LoadingScreen/StartingSplash.png");
	img->SetTexture(tex);
	this->images.add(img);

	while(!this->StartRender)
	{
		this->Render();
		this->framecount++;
	}
	this->DeleteImage(img);
	img = NULL;
	Sleep(100);


	while(this->stayAlive)
	{
		while(MaloW::ProcessEvent* ev = this->PeekEvent())
		{
			if(dynamic_cast<RendererEvent*>(ev) != NULL)
			{
				string msg = ((RendererEvent*)ev)->getMessage();
				
				// ResizeEvent
				if(dynamic_cast<ResizeEvent*>(ev) != NULL)
				{
					this->ResizeRenderer((ResizeEvent*)ev);
				}

				//TerrainEvent
				else if(dynamic_cast<TerrainEvent*>(ev) != NULL)
				{
					this->HandleTerrainEvent((TerrainEvent*)ev);
				}

				// MeshEvent
				else if(dynamic_cast<MeshEvent*>(ev) != NULL)
				{
					this->HandleMeshEvent((MeshEvent*)ev);
				}

				// LightEvent
				else if(dynamic_cast<LightEvent*>(ev) != NULL)
				{
					this->HandleLightEvent((LightEvent*)ev);
				}

				// ImageEvent
				else if(dynamic_cast<ImageEvent*>(ev) != NULL)
				{
					this->HandleImageEvent((ImageEvent*)ev);
				}

				// TextEvent
				else if(dynamic_cast<TextEvent*>(ev) != NULL)
				{
					this->HandleTextEvent((TextEvent*)ev);
				}
			}
			delete ev;
		}
		this->camera->Update(this->TimerAnimation - this->LastCamUpdate);
		this->LastCamUpdate = this->TimerAnimation;
		this->Render();
		this->framecount++;
	}
}

void DxManager::RenderParticles()
{
	/*
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);

	//Matrixes
	D3DXMATRIX world, view, proj, wvp;
	view = this->camera->GetViewMatrix();
	proj = this->camera->GetProjectionMatrix();

	// Draw particleEffects
	MaloW::Array<ParticleMesh*>* psMeshes = this->ps.getMeshes();
	for(int i = 0; i < psMeshes->size(); i++)
	{
		Object3D* obj = this->createParticleObject(psMeshes->get(i));
		
		// Set matrixes
		world = psMeshes->get(i)->GetWorldMatrix();
		wvp = world * view * proj;
		Shader_Particle->SetMatrix("WVP", wvp);
		Shader_Particle->SetMatrix("worldMatrix", world);

		Dx_DeviceContext->IASetPrimitiveTopology(obj->GetTopology());

		Buffer* verts = obj->GetVertBuff();
		if(verts)
			verts->Apply();

		if(ID3D11ShaderResourceView* texture = obj->GetTexture())
		{
			Shader_Particle->SetBool("textured", true);
			Shader_Particle->SetResource("tex2D", texture);
		}
		else
			Shader_Particle->SetBool("textured", false);

		Buffer* inds = obj->GetIndsBuff();
		if(inds)
			inds->Apply();

		Shader_Particle->Apply(0);

		// draw
		if(inds)
			Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
		else
			Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
	}

	// Unbind resources:
	Shader_Particle->SetResource("tex2D", NULL);
	Shader_Particle->Apply(0);

	delete psMeshes;
	*/
}

void DxManager::RenderShadowMap()
{
	// Set sun-settings
	this->Shader_DeferredLightning->SetBool("UseSun", this->useSun);
	if(this->useSun) //**tillman**
	{
		this->Shader_DeferredLightning->SetStructMemberAsFloat4("sun", "Direction", D3DXVECTOR4(this->sun.direction, 0.0f));
		this->Shader_DeferredLightning->SetStructMemberAsFloat4("sun", "LightColor", D3DXVECTOR4(this->sun.lightColor, 0.0f));
		this->Shader_DeferredLightning->SetStructMemberAsFloat("sun", "LightIntensity", this->sun.intensity);
		Shader_ShadowMap->Apply(0);		// Dont know why the fuck this has to be here, but it does, otherwise textures wont be sent when rendering objects
	}
	// If special circle is used
	if(this->specialCircleParams.x) //if inner radius > 0, then send/set data
	{
		this->Shader_DeferredLightning->SetFloat4("dataPPHA", this->specialCircleParams);
	}

	// Generate and send shadowmaps to the main-shader
	if(!this->lights.size())
	{
		for (int l = 0; l < this->lights.size(); l++)
		{
			Dx_DeviceContext->OMSetRenderTargets(0, 0, this->lights[l]->GetShadowMapDSV());
			D3D11_VIEWPORT wp = this->lights[l]->GetShadowMapViewPort();
			Dx_DeviceContext->RSSetViewports(1, &wp);
			Dx_DeviceContext->ClearDepthStencilView(this->lights[l]->GetShadowMapDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);
			for(int i = 0; i < this->objects.size(); i++)
			{
				if(!this->objects[i]->IsUsingInvisibility())
				{
					MaloW::Array<MeshStrip*>* strips = this->objects[i]->GetStrips();
					D3DXMATRIX wvp = this->objects[i]->GetWorldMatrix() * this->lights[l]->GetViewProjMatrix();
					this->Shader_ShadowMap->SetMatrix("LightWVP", wvp);
				
					for(int u = 0; u < strips->size(); u++)
					{
						Object3D* obj = strips->get(u)->GetRenderObject();
						Dx_DeviceContext->IASetPrimitiveTopology(obj->GetTopology());
						Buffer* verts = obj->GetVertBuff();
						if(verts)
							verts->Apply();
						Shader_ShadowMap->SetBool("textured", false);

						Buffer* inds = obj->GetIndsBuff();
						if(inds)
							inds->Apply();

						Shader_ShadowMap->Apply(0);

					
						// draw
						if(inds)
							Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
						else
							Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
					}
				}
			}
		
			//animated meshes
			for(int i = 0; i < this->animations.size(); i++)
			{
				if(!this->animations[i]->IsUsingInvisibility())
				{
					KeyFrame* one = NULL;
					KeyFrame* two = NULL;
					float t = 0.0f;
					this->animations[i]->SetCurrentTime(this->TimerAnimation);
					this->animations[i]->GetCurrentKeyFrames(&one, &two, t);
					MaloW::Array<MeshStrip*>* stripsOne = one->strips;
					MaloW::Array<MeshStrip*>* stripsTwo = two->strips;

					//set shader data (per object)
					D3DXMATRIX wvp = this->animations[i]->GetWorldMatrix() * this->lights[l]->GetViewProjMatrix();
					this->Shader_ShadowMapAnimated->SetMatrix("LightWVP", wvp);
					this->Shader_ShadowMapAnimated->SetFloat("t", t);

					for(int u = 0; u < stripsOne->size(); u++) 
					{
						Object3D* objOne = stripsOne->get(u)->GetRenderObject();
						Object3D* objTwo = stripsTwo->get(u)->GetRenderObject();

						this->Dx_DeviceContext->IASetPrimitiveTopology(objOne->GetTopology()); 

						Buffer* vertsOne = objOne->GetVertBuff();
						Buffer* vertsTwo = objTwo->GetVertBuff();

						ID3D11Buffer* vertexBuffers [] = {vertsOne->GetBufferPointer(), vertsTwo->GetBufferPointer()};
						UINT strides [] = {sizeof(Vertex), sizeof(Vertex)};
						UINT offsets [] = {0, 0};

						this->Dx_DeviceContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);

						Shader_ShadowMapAnimated->Apply(0);
						this->Dx_DeviceContext->Draw(vertsOne->GetElementCount(), 0);
					}
				}
			}
		
			D3DXMATRIX lvp = this->lights[l]->GetViewProjMatrix();
		
		
			// Forward
			//this->Shader_ForwardRendering->SetResourceAtIndex(l, "ShadowMap", this->lights[l]->GetShadowMapSRV());
			//this->Shader_ForwardRendering->SetStructMemberAtIndexAsMatrix(l, "lights", "LightViewProj", lvp);
			//this->Shader_ForwardRendering->SetStructMemberAtIndexAsFloat4(l, "lights", "LightPosition", D3DXVECTOR4(this->lights[l]->GetPosition(), 1));
			//this->Shader_ForwardRendering->SetStructMemberAtIndexAsFloat4(l, "lights", "LightColor", D3DXVECTOR4(this->lights[l]->GetColor(), 1));
			//this->Shader_ForwardRendering->SetStructMemberAtIndexAsFloat(l, "lights", "LightIntensity", this->lights[l]->GetIntensity());
		

			// For deferred:
			this->Shader_DeferredLightning->SetResourceAtIndex(l, "ShadowMap", this->lights[l]->GetShadowMapSRV());
			//**tillman**
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsMatrix(l, "lights", "LightViewProj", lvp);
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsFloat4(l, "lights", "LightPosition", D3DXVECTOR4(this->lights[l]->GetPosition(), 1));
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsFloat4(l, "lights", "LightColor", D3DXVECTOR4(this->lights[l]->GetColor(), 1));
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsFloat(l, "lights", "LightIntensity", this->lights[l]->GetIntensity());
		
		
			// For deferred quad:
			//this->Shader_DeferredQuad->SetResourceAtIndex(l, "ShadowMap", this->lights[l]->GetShadowMapSRV());
			//this->Shader_DeferredQuad->SetStructMemberAtIndexAsMatrix(l, "lights", "LightViewProj", lvp);
			//this->Shader_DeferredQuad->SetStructMemberAtIndexAsFloat4(l, "lights", "LightPosition", D3DXVECTOR4(this->lights[l]->GetPosition(), 1));
			//this->Shader_DeferredQuad->SetStructMemberAtIndexAsFloat4(l, "lights", "LightColor", D3DXVECTOR4(this->lights[l]->GetColor(), 1));
			//this->Shader_DeferredQuad->SetStructMemberAtIndexAsFloat(l, "lights", "LightIntensity", this->lights[l]->GetIntensity());
		
		}
	}
	
	float PCF_SIZE = (float)this->params.ShadowMapSettings + 1;
	float PCF_SQUARED = 1 / (PCF_SIZE * PCF_SIZE);

	/*
	// Forward
	this->Shader_ForwardRendering->SetFloat("PCF_SIZE", PCF_SIZE);
	this->Shader_ForwardRendering->SetFloat("PCF_SIZE_SQUARED", PCF_SQUARED);
	this->Shader_ForwardRendering->SetFloat("SMAP_DX", 1.0f / (256 * pow(2.0f, this->params.ShadowMapSettings/2)));
	this->Shader_ForwardRendering->SetFloat("NrOfLights", (float)this->lights.size());
	*/

	
	// Deferred:
	this->Shader_DeferredLightning->SetFloat("SMAP_DX", 1.0f / (256 * pow(2.0f, this->params.ShadowMapSettings/2)));
	this->Shader_DeferredLightning->SetFloat("PCF_SIZE", PCF_SIZE);
	this->Shader_DeferredLightning->SetFloat("PCF_SIZE_SQUARED", PCF_SQUARED);
	//this->Shader_DeferredLightning->SetFloat("SMAP_DX", 1.0f / 256.0f);
	this->Shader_DeferredLightning->SetFloat("NrOfLights", (float)this->lights.size()); //**tillman**
	
	/*
	// for deferred quad:
	this->Shader_DeferredQuad->SetFloat("PCF_SIZE", PCF_SIZE);
	this->Shader_DeferredQuad->SetFloat("PCF_SIZE_SQUARED", PCF_SQUARED);
	this->Shader_DeferredQuad->SetFloat("SMAP_DX", 1.0f / (256 * pow(2.0f, this->params.ShadowMapSettings/2)));
	this->Shader_DeferredQuad->SetFloat("NrOfLights", (float)this->lights.size());
	*/
}


void DxManager::RenderImages()
{
	/*
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	*/
	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	

	for(int i = 0; i < this->images.size(); i++)
	{
		Image* img = this->images[i];
		// if Convert from screenspace is needed, which it isnt.
		this->Shader_BillBoard->SetFloat("posx", (img->GetPosition().x / this->params.windowWidth) * 2 - 1);
		this->Shader_BillBoard->SetFloat("posy", 2 - (img->GetPosition().y / this->params.windowHeight) * 2 - 1);
		this->Shader_BillBoard->SetFloat("dimx", (img->GetDimensions().x / this->params.windowWidth) * 2);
		this->Shader_BillBoard->SetFloat("dimy", -(img->GetDimensions().y / this->params.windowHeight) * 2);
		this->Shader_BillBoard->SetFloat("opacity", img->GetOpacity());
		
		/*// if -1 to 1
		this->Shader_BillBoard->SetFloat("posx", img->GetPosition().x);
		this->Shader_BillBoard->SetFloat("posy", img->GetPosition().y);
		this->Shader_BillBoard->SetFloat("dimx", img->GetDimensions().x);
		this->Shader_BillBoard->SetFloat("dimy", img->GetDimensions().y);
		*/
		if(img->GetTexture() != NULL)
		{
			this->Shader_BillBoard->SetResource("tex2D", img->GetTexture()->GetSRVPointer());
		}
		this->Shader_BillBoard->Apply(0);
		this->Dx_DeviceContext->Draw(1, 0);
	}
	this->Shader_BillBoard->SetResource("tex2D", NULL);
	this->Shader_BillBoard->Apply(0);
}

void DxManager::RenderText()
{
	/*
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	*/
	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	for(int i = 0; i < this->texts.size(); i++)
	{
		Text* txt = this->texts[i];
		// if Convert from screenspace is needed, which it is
		this->Shader_Text->SetFloat("posx", (txt->GetPosition().x / this->params.windowWidth) * 2 - 1);
		this->Shader_Text->SetFloat("posy", 2 - (txt->GetPosition().y / this->params.windowHeight) * 2 - 1);

		this->Shader_Text->SetFloat("size", txt->GetSize());
		this->Shader_Text->SetFloat("windowWidth", (float)this->params.windowWidth);
		this->Shader_Text->SetFloat("windowHeight", (float)this->params.windowHeight);
		
		// Im only using ASCI 30 - 100, to reduce data sent I only send those 70 as 0-70. Therefor the t = 30 and t - 30
		static bool once = true;
		if(once)
		{
			for(int t = 30; t < 100; t++)
			{
				this->Shader_Text->SetFloatAtIndex(t - 30, "charTex", (float)(int)txt->GetFont()->charTexCoords[t]);
				this->Shader_Text->SetFloatAtIndex(t - 30, "charWidth", (float)(int)txt->GetFont()->charWidth[t]);
			}
			once = false;
		}


		this->Shader_Text->SetResource("tex2D", txt->GetFont()->texture->GetSRVPointer());


		string drawText = txt->GetText();

		//
		if(drawText.size() > 40)
			drawText = drawText.substr(0, 40);

		this->Shader_Text->SetFloat("NrOfChars", (float)drawText.size());
		for(int t = 0; t < (int)drawText.size(); t++)
		{
			// Im only using ASCI 30 - 100, to reduce data sent I only send those 70 as 0-70. Therefor the -30
			this->Shader_Text->SetFloatAtIndex(t, "text", (float)(int)drawText[t] - 30);
		}

		//

		/*
		bool go = true;
		do
		{
			int chars = 40;
			if(drawText.size() <= chars)
			{
				go = false;
				this->Shader_Text->SetFloat("NrOfChars", (float)drawText.size());

				for(int t = 0; t < drawText.size(); t++)
				{
					this->Shader_Text->SetFloatAtIndex(t, "text", (float)(int)drawText[t]);
				}
			}
			else
			{
				string temp = drawText.substr(0, 40);
				drawText = drawText.substr(41);


				this->Shader_Text->SetFloat("NrOfChars", (float)temp.size());

				for(int t = 0; t < temp.size(); t++)
				{
					this->Shader_Text->SetFloatAtIndex(t, "text", (float)(int)temp[t]);
				}
			}
		}
		while(go);

		*/


		this->Shader_Text->Apply(0);

		this->Dx_DeviceContext->Draw(1, 0);
	}
	this->Shader_Text->SetResource("tex2D", NULL);
	this->Shader_Text->Apply(0);
}

void DxManager::RenderCascadedShadowMap()
{
	this->csm->PreRender(this->sun.direction, this->camera);
	
	for (int l = 0; l < this->csm->GetNrOfCascadeLevels(); l++)
	{
		Dx_DeviceContext->OMSetRenderTargets(0, 0, this->csm->GetShadowMapDSV(l));
		D3D11_VIEWPORT wp = this->csm->GetShadowMapViewPort(l);
		Dx_DeviceContext->RSSetViewports(1, &wp);
		Dx_DeviceContext->ClearDepthStencilView(this->csm->GetShadowMapDSV(l), D3D11_CLEAR_DEPTH, 1.0f, 0);
		
		for(int i = 0; i < this->objects.size(); i++)
		{
			if(!this->objects[i]->IsUsingInvisibility())
			{
				MaloW::Array<MeshStrip*>* strips = this->objects[i]->GetStrips();
				D3DXMATRIX wvp = this->objects[i]->GetWorldMatrix() * this->csm->GetViewProjMatrix(l);
				this->Shader_ShadowMap->SetMatrix("LightWVP", wvp);

				for(int u = 0; u < strips->size(); u++)
				{
					Object3D* obj = strips->get(u)->GetRenderObject();
					Dx_DeviceContext->IASetPrimitiveTopology(obj->GetTopology());
					Buffer* verts = obj->GetVertBuff();
					if(verts)
						verts->Apply();
					Shader_ShadowMap->SetBool("textured", false);

					Buffer* inds = obj->GetIndsBuff();
					if(inds)
						inds->Apply();

					Shader_ShadowMap->Apply(0);


					// draw
					if(inds)
						Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
					else
						Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
				}
			}
		}
		
		D3DXMATRIX lvp = this->csm->GetViewProjMatrix(l);

		// For deferred:
		this->Shader_DeferredLightning->SetResourceAtIndex(l, "CascadedShadowMap", this->csm->GetShadowMapSRV(l));
		//**tillman**
		this->Shader_DeferredLightning->SetStructMemberAtIndexAsMatrix(l, "cascades", "viewProj", lvp);
	}
	

	float PCF_SIZE = (float)this->params.ShadowMapSettings + 1;
	float PCF_SQUARED = 1 / (PCF_SIZE * PCF_SIZE);

	this->Shader_DeferredLightning->SetFloat("SMAP_DX", 1.0f / (256 * pow(2.0f, this->params.ShadowMapSettings/2)));
	this->Shader_DeferredLightning->SetFloat("PCF_SIZE", PCF_SIZE);
	this->Shader_DeferredLightning->SetFloat("PCF_SIZE_SQUARED", PCF_SQUARED);
	this->Shader_DeferredLightning->SetFloat("NrOfCascades", (float)this->csm->GetNrOfCascadeLevels());

	this->Shader_DeferredLightning->SetFloat4("CascadeLevels", D3DXVECTOR4(this->csm->GetSplitDepth(0),
			this->csm->GetSplitDepth(1), this->csm->GetSplitDepth(2), this->csm->GetSplitDepth(3)));
}

HRESULT DxManager::Render()
{
	if(this->RendererSleep > 0)
		Sleep(this->RendererSleep);

	// Timer
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	float diff = (li.QuadPart - prevTimeStamp) / this->PCFreq;
	this->prevTimeStamp = li.QuadPart;

	this->TimerAnimation += diff;// / 1000.0f;	

	this->PreRender();


	this->RenderShadowMap();
	this->RenderCascadedShadowMap();
	
	//this->RenderForward();


	this->RenderDeferredGeometry();

	
	//this->RenderQuadDeferred();
	//this->RenderDeferredTexture();

	this->RenderDeferredPerPixel();
	
	if(this->invisibleGeometry)
		this->RenderInvisibilityEffect(); 
	
	this->RenderDeferredSkybox();

	this->RenderParticles();

	this->RenderImages();

	this->RenderText();

	this->RenderAntiAliasing();

	// Debugging:
	// Debug: Render Normals
	//MaloW::Array<StaticMesh*>* meshes = &this->objects;
	//DrawNormals(meshes, this->Dx_Device, this->Dx_DeviceContext, this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix());
	//MaloW::Array<Terrain*>* terrains = &this->terrains;
	//DrawNormals(terrains, this->Dx_Device, this->Dx_DeviceContext, this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix());
	// Debug: Render Wireframe
	//MaloW::Array<StaticMesh*>* meshes = &this->objects;
	//DrawWireFrame(meshes, this->Dx_Device, this->Dx_DeviceContext, this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix());

	/*
	// Render RTs pictures
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);	
	for(int q = 0; q < this->NrOfRenderTargets; q++)
		DrawScreenSpaceBillboardDebug(this->Dx_DeviceContext, this->Shader_BillBoard, this->Dx_GbufferSRVs[q], q); 
	*/
	
	
	// Render shadowmap pictures:
	//for(int q = 0; q < this->lights.size(); q++)
		//DrawScreenSpaceBillboardDebug(this->Dx_DeviceContext, this->Shader_BillBoard, this->lights[q]->GetShadowMapSRV(), q); 
	//for(int q = 0; q < this->csm->GetNrOfCascadeLevels(); q++)
		//DrawScreenSpaceBillboardDebug(this->Dx_DeviceContext, this->Shader_BillBoard, this->csm->GetShadowMapSRV(q), q); 

	
	
	if(FAILED(Dx_SwapChain->Present( 0, 0 )))
		return E_FAIL;

	return S_OK;
}
