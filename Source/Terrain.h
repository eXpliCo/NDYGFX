/*



*/

#ifndef TERRAIN_H
#define TERRAIN_H

#include "iTerrain.h"

#include "Array.h"
#include "Vertex.h"
#include "Buffer.h"
#include "Material.h"
#include "TextureResource.h"
#include "ResourceManager.h"


struct BlendMap
{
	bool						HasChanged;
	unsigned int				Size;
	float const*				Data; //values in data shall not be changed, only the pointer.
	ID3D11ShaderResourceView*	SRV;
	
	BlendMap() {HasChanged = false; Size = 0; Data = NULL;  SRV = NULL; }
	BlendMap(unsigned int size, float* data) {HasChanged = true; Size = size; Data = data; SRV = NULL; }
	virtual ~BlendMap() { Data = NULL; if(SRV) SRV->Release(); SRV = NULL; }

};
struct BoundingBox
{
	Vector3 Size;
	Vector3 MinPos;
	Vector3 MaxPos; 

	BoundingBox(const Vector3 &Size = Vector3(), const Vector3 &MinPos = Vector3(), const Vector3 &MaxPos = Vector3()) : Size(Size), MinPos(MinPos), MaxPos(MaxPos) {}
};

class Terrain : public iTerrain
{
	private:
		bool						zIsCulled;
		unsigned int				zSize; //size of mesh (width & height)
		D3DXVECTOR3					zPos;
		D3DXVECTOR3					zScale;
		D3DXMATRIX					zWorldMatrix;

		bool						zHeightMapHasChanged;
		int							zNrOfVertices;
		Vertex*						zVertices;	
		Buffer*						zVertexBuffer;
		int							zNrOfIndices;
		int*						zIndices; 
		Buffer*						zIndexBuffer;

		Material*					zMaterial;
		D3D_PRIMITIVE_TOPOLOGY		zTopology;

		float						zTextureScale;
		int							zNrOfTextures;
		TextureResource**			zTextureResources;
		bool						zTextureResourceHasChanged[4];
		string						zTextureResourceToLoadFileName[4]; //**TILLMAN
		BlendMap*					zBlendMap;
		bool						zRecreateBoundingBox;
		BoundingBox					zBoundingBox;
		

	private:
		void CreateMesh();
		//Calculates normals for each vertex in the height map
		void CalculateNormals();


	public:
		Terrain();
		Terrain(D3DXVECTOR3 pos, D3DXVECTOR3 scale, unsigned int size);
		virtual ~Terrain();

		//Get
		bool IsCulled() const { return this->zIsCulled; }
		unsigned int GetSize() const { return this->zSize; }
		virtual Vector3 GetPosition() const;
		D3DXVECTOR3 GetScale() const { return this->zScale; }
		D3DXMATRIX GetWorldMatrix() const { return this->zWorldMatrix; }

		bool HasHeightMapChanged() const { return this->zHeightMapHasChanged; }
		int GetNrOfVertices() const { return this->zNrOfVertices; }
		Vertex* GetVerticesPointer() const { return this->zVertices; }
		Buffer* GetVertexBufferPointer() const { return this->zVertexBuffer; }
		int GetNrOfIndices() const { return this->zNrOfIndices; }
		int* GetIndicesPointer() const { return this->zIndices; }
		Buffer* GetIndexBufferPointer() const { return this->zIndexBuffer; }

		Material* GetMaterial() const { return this->zMaterial; }
		D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return this->zTopology; }

		float GetTextureScale() const { return this->zTextureScale; }
		int GetNrOfTextures() const { return this->zNrOfTextures; }
		TextureResource* GetTexture(unsigned int index) const { return this->zTextureResources[index]; }
		bool HasTextureResourceChanged(unsigned int index) const { return this->zTextureResourceHasChanged[index]; }
		string GetTextureResourceToLoadFileName(unsigned int index) const { return this->zTextureResourceToLoadFileName[index]; }
		//string GetTextureFileName(unsigned int index) { return this->zTextureFileNames[index]; }
		BlendMap* GetBlendMapPointer() { return this->zBlendMap; }
		BoundingBox GetBoundingBox() { if(this->zRecreateBoundingBox) { RecreateBoundingBox(); } return this->zBoundingBox; }

		//Set
		void SetCulled(bool cull) { this->zIsCulled = cull; }
		void SetScale(D3DXVECTOR3 scale) { this->zScale = scale; }
		void HeightMapHasChanged(bool has) { this->zHeightMapHasChanged = has; }
		void SetVertexBuffer(Buffer* vertexBuffer) { this->zVertexBuffer = vertexBuffer; }
		void SetIndexBuffer(Buffer* indexBuffer) { this->zIndexBuffer = indexBuffer; }
		void SetTexture(unsigned int index, TextureResource* textureResource) const { this->zTextureResources[index] = textureResource; }
		void TextureResourceHasChanged(unsigned int index, bool has) { this->zTextureResourceHasChanged[index] = has; this->zTextureResourceToLoadFileName[index] = ""; }
		

		//Other
		//Is used internally when needed, but can be used from the outside for debugging.
		void RecreateWorldMatrix();
		void RecreateBoundingBox();
		
		//bool LoadTexture();
		//bool LoadTextures()



		//iTerrain interface functions
		//Get
		virtual float GetYPositionAt(float x, float z) const throw(...); 

		//Set
		virtual void SetScale(const Vector3& scale);
		virtual void SetHeightMap(float const* const data); 
		virtual void SetTextures(char const* const* const fileNames);
		virtual void SetBlendMap(unsigned int size, float const* const data);
		virtual void SetDiffuseColor(const Vector3& color);
		virtual void SetTextureScale(float textureScale);

		// MaloW ToDO
		// virtual void SetAIMap(int size, float const* data);
		// Do just like blendmap, maybe even use the struct blendmap, and send it to GPU just like it
		// And then render it. think about performance tho, this shouldnt affect client-side.


};

#endif