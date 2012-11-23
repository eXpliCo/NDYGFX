#pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "iMesh.h"
#include "iLight.h"
#include "iImage.h"
#include "iText.h"
#include "iAnimatedMesh.h"

extern "C"
{
	class DECLDIR GraphicsEngine
	{
	public:
		GraphicsEngine() { };
		virtual ~GraphicsEngine() { };

		static GraphicsEngine* eng;

		virtual iMesh* CreateMesh(const char* filename, const Vector3& pos) = 0;


		virtual iLight* CreateLight(Vector3 pos) = 0;
		virtual void DeleteLight(iLight* light) = 0;

		virtual iImage* CreateImage(Vector2 pos, Vector2 dimensions, const char* texture) = 0;
		virtual void DeleteImage(iImage* delImg) = 0;

		virtual iText* CreateText(const char* text, Vector2 pos, float size, const char* fontTexturePath) = 0;
		virtual void DeleteText(iText* deltxt) = 0;

		virtual iMesh* CreateStaticMesh(const char* filename, const Vector3& pos) = 0;
		virtual iAnimatedMesh* CreateAnimatedMesh(const char* filename, const Vector3& pos) = 0;

		virtual float Update() = 0;
	};

	/*
	StaticMesh* CreateStaticMesh(string filename, D3DXVECTOR3 pos, MaterialType material);
	StaticMesh* CreateStaticMesh(string filename, D3DXVECTOR3 pos, Material* material);
	StaticMesh* CreateStaticMesh(string filename, D3DXVECTOR3 pos);
	void DeleteStaticMesh(StaticMesh* mesh) { this->dx->DeleteStaticMesh(mesh); }

	AnimatedMesh* CreateAnimatedMesh(string filename, D3DXVECTOR3 pos);
	void DeleteAnimatedMesh(AnimatedMesh* mesh) { this->dx->DeleteAnimatedMesh(mesh); }

	Terrain* CreateTerrain(D3DXVECTOR3 position, D3DXVECTOR3 dimension, std::string texture, string heightmap, int vertexSize = 256);

	void CreateSkyBox(string texture);



	GraphicsEngineParams GetEngineParameters() const { return this->parameters; }

	float Update();

	void LoadingScreen(string BackgroundTexture = "", string ProgressBarTexture = "", float FadeBlackInInTime = 0.0f, float FadeBlackInOutTime = 0.0f, float FadeBlackOutInTime = 0.0f, float FadeBlackOutOutTime = 0.0f);

	bool isRunning();
	void StartRendering() { this->dx->StartRender = true; }

	Camera* GetCamera() const { return this->cam; } //{ return this->dx->GetCamera(); }
	MaloW::KeyListener* GetKeyListener() const { return this->kl; }
	HWND GetWindowHandle() const { return this->hWnd; }


	EXTRAS:
	SetParamater - To allow change of parameters after creation


	WRAPPER:
	StaticMesh*	-alex DONE
	AnimatedMesh* -alex DONE
	Light* -malow	DONE
	Terrain* -alex
	Image* -malow	DONE
	Text* -malow
	Camera* -malow
	MaloW::KeyListener* -malow
	GraphicsEngineParams		--- Done
	*/

	DECLDIR GraphicsEngine* CreateGraphicsEngineInWindow(unsigned int hWnd, const char* configFile);
	DECLDIR GraphicsEngine* CreateGraphicsEngine(unsigned int hInstance, const char* configFile);
	DECLDIR GraphicsEngine* GetGraphicsEngine();
}