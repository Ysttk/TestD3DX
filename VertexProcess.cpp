#include "DXUT.h"
#include "VetexProcess.h"
#include "Global.h"

struct CUSTOMVERTEX
{
	FLOAT x,y,z;
	DWORD color;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

CUSTOMVERTEX vertices[] =
{
	{ 250.0f,  250.0f, 0.5f,  0xffff0000, }, // x, y, z, rhw, color
	{ 50.0f, 50.0f, 0.5f,  0xff00ff00, },
	{  250.0f, 50.0f, 0.5f, 0xff0000ff, },
	{ 50.0f, 50.0f, 1000.0f, 0xff00ff00, },
	{  250.0f, 50.0f, 1000.0f, 0xff0000ff, },
	{  50.0f, 250.0f, 1000.0f, 0xffff0000, },
};


HRESULT ProcessVertex(IDirect3DDevice9* pDevice)
{


	IDirect3DVertexBuffer9* pVB;
	if (!pDevice) return E_FAIL;

	if (FAILED(g_pDevice->CreateVertexBuffer(6*sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &pVB, NULL))) return E_FAIL;

	VOID* pVertices;
	if (FAILED(pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0))) return E_FAIL;

	memcpy(pVertices, vertices, sizeof(vertices));
	pVB->Unlock();




	pDevice->SetStreamSource(0, pVB, 0, sizeof(CUSTOMVERTEX));

	pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	pVB->Release();

	return S_OK;
}

//��ν�ġ���������ʵ������һ�����󣬱�������������һ������ڸ�������������������ϵ���ı任����ˣ�������Ƥ��������ʵ���������ȱ�����Ƥ����ľֲ����꣨�����ĳ���������Ȼ��
//ͨ���ݹ�����ۼ����и������ı任����󽫴˱任Ӧ�õ���Ƥ���������ϣ���ɶ�����������֡������仯��

struct D3DXFRAME_DERIVED : public D3DXFRAME
{
	D3DXMATRIXA16 CombinedTransformationMatrix;	//�þ��󱣴汾����ÿ֡�������������ϵ���˶�����
	// ÿ֡��ʼʱ�����ȼ�����������ϵ���ӽǱ任��Ȼ����ö������ƶ���g_pAnimationController->AdvanceTime�������������й����е�TransformationMatrix����
	// �����ݱ仯�����������ϵ��ÿ�������仯���TransformationMatrix�õ�û������������������������ϵ��CombinedTransformationMatrix����
};

//--------------------------------------------------------------------------------------
// Name: struct D3DXMESHCONTAINER_DERIVED
// Desc: Structure derived from D3DXMESHCONTAINER so we can add some app-specific
//       info that will be stored with each mesh
//--------------------------------------------------------------------------------------
struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER
{
	LPDIRECT3DTEXTURE9* ppTextures;       // array of textures, entries are NULL if no texture specified    
										//base�е�pMaterials���ָ����ʹ��Texture����ô�Ͱ�TextureFileNameָ������ͼ�����������ڸýṹ�У����û�У���ô����ֱ������pMaterialsԪ��Ϊ��Ҫʹ�õĲ��ʣ����ʰ�����diffuse, ambient, specular, emissive, power)

	// SkinMesh info             
	LPD3DXMESH pOrigMesh;	//��������base�е�MeshData��MeshData����ÿ֡�ñ任�������任����������꣨����֮ǰ��Ҫ����ص�����׼���£�����û����������Mesh��Ҫ���㱣���������ȣ�
	D3DXMATRIX* pBoneOffsetMatrices;	//��ѯpSkinInfo�ӿڽ�������ع����ľ��󱣴浽��������

	LPD3DXATTRIBUTERANGE pAttributeTable;
	
	DWORD NumInfl;		//��ѯSkinInfo->ConvertToBlendedMesh�ӿ����õ�ÿ��������౻NumInfl������Ӱ��
	LPD3DXBUFFER pBoneCombinationBuf;	//D3DXBONECOMBINATION*���ͣ�ÿ��Ԫ��Ϊһ��draw call��Ԫ������draw����㶥����յ㶥�㡢����յ�mesh�Լ��漰���Ĺ�����
	DWORD NumAttributeGroups; //��ѯSkinInfo->ConvertToBlendedMesh�ӿڷ���pBoneCombinationBuf�ܹ��ж��ٸ�Ԫ��

	D3DXMATRIX** ppBoneMatrixPtrs;	//SetupBoneMatrixPointersOnMesh�����б���ָ�������D3DXFRAME_DERIVED����CombinedTransformationMatrix��ַ��ÿ֡����ʱ��ֱ�ӷ���CombinedTransformationMatrix�õ���֡�����������������ϵ�ı任
	DWORD iAttributeSW;     // used to denote the split between SW and HW if necessary for non-indexed skinning
	DWORD NumPaletteEntries;
	bool UseSoftwareVP;

};