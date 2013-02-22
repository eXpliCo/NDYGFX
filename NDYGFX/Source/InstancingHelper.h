#pragma once

//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska H�gskola.
// 
//	Helper class for hardware instancing of billboards and meshes.
//	Requirements:	ID3D11Device*, ID3D11DeviceContext*.
//	
//--------------------------------------------------------------------------------------------------

#include "Vertex.h"
#include "DirectX.h"
#include <vector>
#include "Billboard.h"
#include "Mesh.h"


struct BillboardData
{
	Vertex							s_Vertex;
	ID3D11ShaderResourceView*		s_SRV; 
	
	BillboardData() : s_Vertex(), s_SRV(NULL) {}
	BillboardData(Vertex vertex, ID3D11ShaderResourceView* srv) : s_Vertex(vertex), s_SRV(srv) {}
};
struct BillboardGroup
{
	unsigned int				s_Size; 
	unsigned int				s_StartLocation;
	ID3D11ShaderResourceView*	s_SRV;
	
	BillboardGroup() : s_Size(0), s_StartLocation(0), s_SRV(NULL) {}
	BillboardGroup(unsigned int size, unsigned int startLocation, ID3D11ShaderResourceView* srv)
		: s_Size(size), s_StartLocation(startLocation), s_SRV(srv) {}
};

/*
struct MeshData
{
	struct InstancedDataStruct
	{
		D3DXMATRIX	s_WorldMatrix; //translation, rotation, scale
		D3DXMATRIX	s_WorldInverseTransposeMatrix; 

		InstancedDataStruct() 
		{
			D3DXMatrixIdentity(&s_WorldMatrix);
			s_WorldInverseTransposeMatrix = s_WorldMatrix;
		}
	} InstancedData;

	MeshStripsResource*	 s_MeshStripsResource;

	MeshData() : s_MeshStripsResource(NULL) {}
	MeshData(MeshStripsResource* meshStripsResource) : s_MeshStripsResource(meshStripsResource) {}
};*/
struct StripData
{
	struct InstancedDataStruct
	{
		D3DXMATRIX	s_WorldMatrix; //translation, rotation, scale
		//D3DXMATRIX	s_WorldInverseTransposeMatrix; 

		InstancedDataStruct() 
		{
			D3DXMatrixIdentity(&s_WorldMatrix);
			//s_WorldInverseTransposeMatrix = s_WorldMatrix;
		}
	} InstancedData;

	MeshStrip*	 s_MeshStrip;

	StripData() : s_MeshStrip(NULL) {}
	StripData(MeshStrip* meshStrip) : s_MeshStrip(meshStrip) {}
};
/*
struct MeshGroup 
{
	unsigned int		s_StartLocation;
	unsigned int		s_Size; 
	//MeshStripsResource*	s_MeshStripsResource;
	MeshStrip*			s_MeshStrip;
	MeshGroup() : s_StartLocation(0), s_Size(0), s_MeshStrip(NULL) {}// s_MeshStripsResource(NULL) {}
	MeshGroup(unsigned int startLocation, unsigned int size, MeshStrip* meshStrip)
		: s_StartLocation(startLocation), s_Size(size), s_MeshStrip(meshStrip) {}
};
*/

struct StripGroup
{
	unsigned int		s_StartLocation;
	unsigned int		s_Size; 
	//Buffer*				s_VertexBuffer;
	//TextureResource*	s_TextureResource;
	MeshStrip*			s_MeshStrip;

	StripGroup() : s_StartLocation(0), s_Size(0), s_MeshStrip(NULL) {}
	StripGroup(unsigned int startLocation, unsigned int size, MeshStrip* meshStrip)
		: s_StartLocation(startLocation), s_Size(size), s_MeshStrip(meshStrip)
	{}
};

bool SortBillboardData(BillboardData billboardLeft, BillboardData billboardRight);
//bool SortMeshData(MeshData meshLeft, MeshData meshRight);
bool SortStripData(StripData stripLeft, StripData stripRight);

class InstancingHelper
{
	private:
		ID3D11Device*			g_Device;
		ID3D11DeviceContext*	g_DeviceContext;

	private:
		//BILLBOARD
		//Counter
		unsigned int zBillboardInstanceBufferSize;
		//Billboard data
		std::vector<BillboardData> zBillboardData;
		//Instance group 
		std::vector<BillboardGroup> zBillboardGroups;
		//Instance Buffer
		ID3D11Buffer* zBillboardInstanceBuffer; //Shall contain vertex data of billboardData.



		//MESHES(MESHSTRIPS)
		//Counters
		unsigned int zStripInstanceBufferSize;

		//Vector containing strip data of all meshes that are added each frame.
		std::vector<StripData> zStripData;

		//Instance group (groups of strips that share the same texture)
		std::vector<StripGroup> zStripGroups;

		//Instance Buffer (containing all instanced data)
		ID3D11Buffer* zStripInstanceBuffer; 
		
    private:
	    void ExpandBillboardInstanceBuffer();
		void ExpandStripInstanceBuffer();

	public:
		InstancingHelper();
		virtual~InstancingHelper();

		HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

		//BILLBOARD
		unsigned int GetNrOfBillboards() { return this->zBillboardData.size(); }
		unsigned int GetNrOfBillboardGroups() { return this->zBillboardGroups.size(); }
		unsigned int GetBillboardDataCapacity() { return this->zBillboardData.capacity(); }
		unsigned int GetBillboardGroupCapacity() { return this->zBillboardGroups.capacity(); }
		BillboardData GetBillboardData(unsigned int index) { return this->zBillboardData[index]; }
		BillboardGroup GetBillboardGroup(unsigned int index) { return this->zBillboardGroups[index]; }
		ID3D11Buffer* GetBillboardInstanceBuffer() { return this->zBillboardInstanceBuffer; }  

		void AddBillboard(const Billboard* const billboard);
		/*	Sorts, creates instance groups and updates the instance buffer.	*/
		void PreRenderBillboards();
		void PostRenderBillboards() { this->zBillboardData.clear(); this->zBillboardGroups.clear(); }
		


		//MESH
		unsigned int GetNrOfStrips() { return this->zStripData.size(); }
		unsigned int GetNrOfStripGroups() { return this->zStripGroups.size(); }
		unsigned int GetStripDataCapacity() { return this->zStripData.capacity(); }
		unsigned int GetStripGroupCapacity() { return this->zStripGroups.capacity(); }
		StripData GetStripData(unsigned int index) { return this->zStripData[index]; }
		StripGroup GetStripGroup(unsigned int index) { return this->zStripGroups[index]; }
		ID3D11Buffer* GetStripInstanceBuffer() { return this->zStripInstanceBuffer; }  

		void AddMesh(Mesh* mesh);
		/*	Sorts, creates instance groups and updates the instance buffer.	*/
		void PreRenderStrips();
		void PostRenderStrips() { this->zStripData.clear(); this->zStripGroups.clear(); }

};