#pragma once

#include "FBXModelPartD3D.h"

class FBXModelPartDataD3D
{
public:
	Buffer*						mVB_Position;
	Buffer*						mVB_Normal;
	Buffer*						mVB_Tangent;
	Buffer*						mVB_TexCoord;
	Buffer*						mVB_BlendWeights;
	Buffer*						mIB;

	Texture*					mDiffuseTexture;
	Texture*					mNormalTexture;

	FBXModelPartDataD3D();
	~FBXModelPartDataD3D();

	void Init(class FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, Device3D* device3D);
};

typedef std::map<std::string, FBXModelPartDataD3D*> MODEL_PART_DATA_MAP;
class FBXModelPartDataD3DManager
{
	MODEL_PART_DATA_MAP	mModelParts;
	FBXModelPartDataD3DManager() {}
	~FBXModelPartDataD3DManager();

	static FBXModelPartDataD3DManager* instance;
public:
	static FBXModelPartDataD3DManager* GetInstance();
	static void DeleteInstance();

	FBXModelPartDataD3D* GetModelData(class FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, int partIndex);

	void Cleanup();
};