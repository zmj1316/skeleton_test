#ifndef PHYSXTEST_H_
#define PHYSXTEST_H_
//------------------------------------------------------------------------------------
// APP.h
//------------------------------------------------------------------------------------
#include "MYAPP.h"
#include "Camera.h"
#include "../MMDFormats/MikuMikuFormats/Pmx.h"
#include "../MMDFormats/MikuMikuFormats/Vmd.h"
#include "MotionController.h"

using namespace DirectX;

class XTest : public MYAPP
{
public:
	XTest(std::wstring windowName, UINT windowWidth, UINT windowHeight, HINSTANCE hInstance, int nCmdShow)
		: MYAPP(windowName, windowWidth, windowHeight, hInstance, nCmdShow)
	{
		XTest::OnInit();
	};

	~XTest()
	{
	}

	//light
	struct DirectionalLight
	{
		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT3 Direction;
		uint32_t padding0;

	};

	//material
	struct Material
	{
		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Reflect;
	};

	//dx11 constant buffer
	struct ConstantBuffer
	{
		XMMATRIX skeleton[512];
		XMMATRIX mWorldViewProjection;
		XMMATRIX mWorld;
		XMMATRIX mView;
		XMMATRIX mWorldInverseTranspose;

		DirectionalLight mDirLight;
		Material mMaterial;
		XMFLOAT3 mEyeW;
		uint32_t padding0;
	};

	//rewrite framework function
	void OnInit();
	void OnUpdate();
	void OnRender();
	void OnDestroy();
	bool OnEvent(MSG msg);

	//void RenderPxActors(PxRigidActor** actors, const PxU32 numActors);
	//void RenderGeometry(const PxGeometryHolder& h, XMMATRIX& mWorld);
private:
	static const int texture_max_ = 28;
	static const int bones_max = 256;
	//DX11 
	ID3D11VertexShader *vertex_shader_d3dptr_ = NULL;
	ID3D11PixelShader *pixel_shader_d3dptr_ = NULL;
	ID3D11InputLayout *vertex_layout_d3dptr_ = NULL;
	ID3D11Buffer *vertex_buffer_d3dptr_ = NULL;
	ID3D11Buffer *index_buffer_d3dptr_ = NULL;
	ID3D11Buffer *constant_buffer_d3dptr_ = NULL;
	ID3D11SamplerState *sample_state_d3dptr_ = NULL;


	//ID3D11ShaderResourceView *texture_array_d3dptr_[texture_max_];
	std::vector<ID3D11ShaderResourceView *> texture_array;

	XMMATRIX world_m_;
	XMMATRIX view_m_;
	XMMATRIX projection_m_;
	Camera *camera_ptr_;
	Camera origin;
	DirectionalLight dir_light_;
	Material material_;
	ConstantBuffer constant_buffer_;
	////vertex and index buffer offset
	//int						mBoxVertexOffset;
	//int						mBallVertexOffset;
	//int						mGroundVertexOffset;
	//int                     mBoxIndexOffset;
	//int						mBallIndexOffset;
	//int						mGroundIndexOffset;
	//int						mBoxIndexCount;
	//int						mBallIndexCount;
	//int						mGroundIndexCount;

	int mesh_index_count_;

	MotionController motion_controller_;
	////physx world
	//PhysXWorld              g_myPxWorld;
};

#endif
