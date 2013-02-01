#pragma once


// Implementation Class
#include "GraphicsEngine.h"

/*
Wrapper-class for the graphics engine. Contains the interface that user use from the outside.
Singleton.
*/

#include "DxManager.h"
#include "KeyListener.h"
#include "GraphicsEngineParameters.h"
#include "FPSCamera.h"
#include "RTSCamera.h"
#include "TRDCamera.h"
#include "Image.h"
#include "CamRecording.h"
#include "PhysicsEngine.h"

// Class for communication between processes for loading meshes
class LoadMeshEvent : public MaloW::ProcessEvent
{
private:
	string fileName;
	StaticMesh* mesh;
	AnimatedMesh* ani;
	Material* mat;
	bool selfdelete;

public:
	LoadMeshEvent(string fileName, StaticMesh* mesh, AnimatedMesh* ani, Material* mat) 
	{ 
		this->fileName = fileName; 
		this->mesh = mesh; 
		this->mat = mat;
		this->ani = ani;
		this->selfdelete = true;
	}
	virtual ~LoadMeshEvent() 
	{
		if(this->selfdelete) 
		{
			if(this->mesh) 
				delete this->mesh;
			if(this->mat)
				delete this->mat;
			if(this->ani)
				delete this->ani;
		}
	}
	string GetFileName() { return this->fileName; }
	StaticMesh* GetStaticMesh() { this->selfdelete = false; return this->mesh; }
	AnimatedMesh* GetAnimatedMesh() { this->selfdelete = false; return this->ani; }
	Material* GetMaterial() { this->selfdelete = false; return this->mat; }
};

class GraphicsEngineImp : public MaloW::Process, public GraphicsEngine
{
private:
	DxManager* dx;
	HINSTANCE hInstance;
	HWND hWnd;
	KeyListener* kl;
	Camera* cam;
	PhysicsEngine* physx;

	int fpsLast;
	int prevFrameCount;
	float fpsTimer;

	bool keepRunning;
	bool loading;

	GraphicsEngineParams parameters;

	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	void InitObjects();

	bool isManagingMyOwnWindow;


	float PCFreq;
	__int64 prevTimeStamp;


public:
	GraphicsEngineImp(GraphicsEngineParams params, HINSTANCE hInstance, int nCmdShow);
	GraphicsEngineImp(GraphicsEngineParams params, HWND hWnd);
	virtual ~GraphicsEngineImp();

	// Special String Used For File Drops
	string specialString;
	virtual const char* GetSpecialString();

	/*  Inherited from interface  */
	virtual iMesh* CreateMesh(const char* filename, const Vector3& pos);
	virtual void DeleteMesh(iMesh* &delMesh);
	virtual iMesh* CreateStaticMesh(const char* filename, const Vector3& pos);
	virtual iAnimatedMesh* CreateAnimatedMesh(const char* filename, const Vector3& pos);

	// Create And Delete Terrain
	virtual iTerrain* CreateTerrain(const Vector3& pos, const Vector3& scale, const unsigned int& size);
	virtual void DeleteTerrain( iTerrain*& terrain );

	// Lights
	virtual iLight* CreateLight(Vector3 pos);
	virtual void DeleteLight(iLight* &light);

	// Images
	virtual iImage* CreateImage(Vector2 pos, Vector2 dimensions, const char* texture);
	virtual void DeleteImage(iImage* &delImg);

	// Text
	virtual iText* CreateText(const char* text, Vector2 pos, float size, const char* fontTexturePath);
	virtual void DeleteText(iText* &deltxt);

	// Skybox
	virtual void CreateSkyBox(const char* texture);

	// Camera
	virtual iCamera* GetCamera() const;
	virtual iCamera* ChangeCamera(CameraType newCamType);

	// Key Listener
	virtual iKeyListener* GetKeyListener() const;

	// Phyics Engine
	virtual iPhysicsEngine* GetPhysicsEngine() const;

	// Logic
	virtual float Update();
	virtual bool IsRunning();
	virtual void StartRendering() { this->dx->StartRender = true; }
	virtual void ResizeGraphicsEngine(unsigned int width, unsigned int height);

	// Settings
	virtual const iGraphicsEngineParams& GetEngineParameters() const;
	virtual void UseShadow(bool useShadow);
	virtual void SetSpecialCircle(float innerRadius, float outerRadius, Vector2& targetPos) const;

	virtual void PreLoadMeshes(unsigned int nrOfMeshes, char const* const* const meshesFileNames);


	virtual void SetFPSMax(float maxFPS) { this->dx->SetFPSMAX(maxFPS); }
	virtual void SetSunLightProperties(Vector3 direction, Vector3 lightColor = Vector3(1.0f, 1.0f, 1.0f), float intensity = 1.0f);
	virtual Vector3 GetSunLightDirection() const;
	virtual Vector3 GetSunLightColor() const;
	virtual float GetSunLightIntensity() const;
	virtual void SetSunLightDisabled();
	virtual void SetSceneAmbientLight(Vector3 ambientLight);
	virtual Vector3 GetSceneAmbientLight() const;

	/*! 
	Takes control of the thread and renders a loading-screen with a progress bar. Returns once all objects that have been sent to load is loaded.
	To use it first make all CreateMesh()-calls that you need and then call LoadingScreen(.,.) directly after, and it will return once all the meshes are
	created and being rendered in the background. */
	virtual void LoadingScreen(const char* BackgroundTexture = "", const char* ProgressBarTexture = "", float FadeBlackInInTime = 0.0f, float FadeBlackInOutTime = 0.0f, float FadeBlackOutInTime = 0.0f, float FadeBlackOutOutTime = 0.0f);

	virtual iWaterPlane* CreateWaterPlane(Vector3& pos, const char* texture);
	virtual void DeleteWaterPlane(iWaterPlane* del);

	/*  Non-inherited functions */

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	/*! Creates a Mesh and starts rendering it every frame. Return is a pointer to the Mesh created. To stop rendering it call DeleteMesh on the engine with the Mesh as parameter. */
	StaticMesh* CreateStaticMesh(string filename, D3DXVECTOR3 pos, MaterialType material);
	StaticMesh* CreateStaticMesh(string filename, D3DXVECTOR3 pos, Material* material);
	StaticMesh* CreateStaticMesh(string filename, D3DXVECTOR3 pos);

	AnimatedMesh* CreateAnimatedMesh(string filename, D3DXVECTOR3 pos);
	void DeleteAnimatedMesh(AnimatedMesh* mesh) { this->dx->DeleteAnimatedMesh(mesh); }

	Light* CreateLight(D3DXVECTOR3 pos, bool UseShadowMap = true);
	void DeleteLight(Light* light) { this->dx->DeleteLight(light); }
	Terrain* CreateTerrain(D3DXVECTOR3 position, D3DXVECTOR3 dimension, std::string texture, string heightmap, int vertexSize = 256);

	/*! Stops rendering the Mesh and internally deletes it and the pointer will become NULL. Return depends on if the Mesh was sucessfully removed. */
	void DeleteStaticMesh(StaticMesh* mesh) { this->dx->DeleteStaticMesh(mesh); }

	/*! Create an Image and starts rendering it every frame. Return is a pointer to the image created. Coordinates are in screen-space. 
	To stop rendering it call DeleteImage on the engine with the image as parameter. */
	Image* CreateImage(D3DXVECTOR2 position, D3DXVECTOR2 dimensions, string texture);

	/*! Stops rendering the Image and internally deletes it and the pointer will become NULL. Return depends on if the Image was sucessfully removed. */
	bool DeleteImage(Image* delImage);

	Text* CreateText(string text, D3DXVECTOR2 position, float size, string fontTexturePath);
	bool DeleteText(Text* delText);


	void SetManagingWindow(bool manage) { this->isManagingMyOwnWindow = manage; }
	bool GetManagingWindow() const { return this->isManagingMyOwnWindow; }

	const GraphicsEngineParams& GetEngineParams() const { return this->parameters; }

	// Get's
	Camera* GetCam() const { return this->cam; } //{ return this->dx->GetCamera(); }
	KeyListener* GetKeyList() const { return this->kl; }
	HWND GetWindowHandle() const { return this->hWnd; }

	void CreateSmokeEffect() { this->dx->CreateSmokeEffect(); }
	virtual void Life();
};