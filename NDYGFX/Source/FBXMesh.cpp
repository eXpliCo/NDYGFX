#include "FBXMesh.h"


FBXMesh::FBXMesh( D3DXVECTOR3 pos ) : Mesh(pos)
{
	this->zScene = new FBXSceneD3D();
}

FBXMesh::~FBXMesh()
{
	if(this->zScene)
		delete this->zScene;
}

MaloW::Array<MeshStrip*>* FBXMesh::GetStrips()
{
	return NULL;
}

void FBXMesh::Update( float dt )
{
	this->zScene->Update(0);

	// Move Bound Meshes
	for( auto i = zBoundMeshes.begin(); i != zBoundMeshes.end(); ++i )
	{
		float x, y, z;

		// Bone Position
		if ( GetBonePosition(i->second, x, y, z) )
		{
			i->first->SetPosition(Vector3(x, y, z));
		}
	}
}

void FBXMesh::Render(float dt, D3DXMATRIX camProj, D3DXMATRIX camView, Shader* shad, ID3D11DeviceContext* devCont)
{
	this->RecreateWorldMatrix();
	D3DXMATRIX world = this->GetWorldMatrix();
	this->zScene->Render(0, world, camProj, camView, shad, devCont );
}

bool FBXMesh::LoadFromFile( string file, IBTHFbx* fbx, ID3D11Device* dev, ID3D11DeviceContext* devCont )
{
	this->zScene->Init(file.c_str(), fbx, dev, devCont);
	this->zScene->GetAnimationController()->SetCurrentAnimation(0);
	this->zScene->GetAnimationController()->Play();
	return true;
}

bool FBXMesh::SetAnimation( unsigned int ani )
{
	if ( !this->zScene->GetAnimationController()->SetCurrentAnimation(ani) )
	{	
		MaloW::Debug(
			"Tried to apply animation at index " + 
			MaloW::convertNrToString(ani) + 
			" to an FBX mesh that only has " + 
			MaloW::convertNrToString(this->zScene->GetAnimationController()->GetAnimationCount()) + 
			" animations.");
		
		return false;
	}

	return true;
}

bool FBXMesh::SetAnimation( const char* name )
{
	if ( !this->zScene->GetAnimationController()->SetCurrentAnimation(name) )
	{	
		MaloW::Debug(
			"Tried to apply animation at index " + 
			std::string(name) + 
			" to an FBX mesh that only has " + 
			MaloW::convertNrToString(this->zScene->GetAnimationController()->GetAnimationCount()) + 
			" animations.");
		
		return false;
	}

	return true;
}

bool FBXMesh::BindMesh(const char* boneName, iMesh* mesh)
{
	// Find Bone
	if ( zScene )
	{
		IBTHFbxSkeletonBone* bone = zScene->GetSkeleton()->GetBone(boneName);

		if ( bone )
		{
			zBoundMeshes[mesh] = boneName;
			return true;
		}
	}

	return false;
}

void FBXMesh::UnbindMesh(iMesh* mesh)
{
	auto i = zBoundMeshes.find(mesh);
	if ( i != zBoundMeshes.end() )
		zBoundMeshes.erase(i);
}

bool FBXMesh::GetBonePosition(const std::string& name, float& x, float& y, float& z)
{
	if ( zScene )
	{
		IBTHFbxSkeletonBone* bone = zScene->GetSkeleton()->GetBone(name.c_str());

		if ( bone )
		{
			// 4x4 Matrix
			const float* matrix;
			matrix = bone->GetCombinedTransform();

			D3DXVECTOR4 worldPos;
			worldPos.x = matrix[12] * -1.0f;
			worldPos.y = matrix[13];
			worldPos.z = matrix[14];
			worldPos.w = 1.0;

			D3DXMATRIX worldMat = GetWorldMatrix();

			D3DXVec4Transform(&worldPos, &worldPos, &worldMat);

			x = worldPos.x;
			y = worldPos.y;
			z = worldPos.z;

			return true;
		}
	}

	return false;
}

void FBXMesh::RenderShadow( float dt, D3DXMATRIX lightViewProj, Shader* shad, ID3D11DeviceContext* devCont )
{
	this->RecreateWorldMatrix();
	D3DXMATRIX world = this->GetWorldMatrix();
	this->zScene->RenderShadow(0, world, lightViewProj, shad, devCont );
}
