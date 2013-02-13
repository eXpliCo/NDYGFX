#pragma once

#include "Array.h"
#include "Mesh.h"
#include "iFBXMesh.h"

// FBX
#include "BTHFBX\Shared\FBX\FBXSceneD3D.h"

#ifdef _DEBUG
#pragma comment(lib, "BTHFbxD.lib")
#else
#pragma comment(lib, "BTHFbx.lib")
#endif

#pragma warning ( push )
#pragma warning ( disable : 4250 ) //  warning C4250: 'FBXMesh' : inherits via dominance

class FBXMesh : public Mesh, public virtual iFBXMesh
{
private:
	FBXSceneD3D* scene;


public:
	FBXMesh(D3DXVECTOR3 pos);
	virtual ~FBXMesh();

	virtual MaloW::Array<MeshStrip*>* GetStrips();

	virtual bool LoadFromFile(string file, IBTHFbx* fbx, ID3D11Device* dev, ID3D11DeviceContext* devCont );

	void Update(float dt);
	void Render(float dt, D3DXMATRIX camProj, D3DXMATRIX camView, Shader* shad, ID3D11DeviceContext* devCont);

	virtual void SetAnimation(int ani);

	//BTHFBX_RAY_BOX_RESULT RayVsScene(const BTHFBX_RAY& ray, BTHFBX_MATRIX* worldMatrix);
};

#pragma warning ( pop )