#include "XTest.h"
#include "DirectInputClass.h"
#include <fstream>
#include "GeometryData.h"
#include "../MMDFormats/MikuMikuFormats/Pmx.h"

std::ofstream debug("debug.txt");
extern DirectInput *gDInput;

bool g_pause = true;

//----------------------------------------------------------------------
//App initialize£¬include read and compile shader, define vertex layout
//create geometry objects, vertex buffer, index buffer,
//initialize camera, material and light
//----------------------------------------------------------------------
void XTest::OnInit()
{
	HRESULT hr;
	ID3DBlob *pVSBlob = NULL;
	ID3DBlob *pPSBlob = NULL;
	//compile shader
	CompileShader(L"shader.hlsl", "VS", "vs_5_0", &pVSBlob);
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &vertex_shader_d3dptr_);
	// Compile the pixel shader
	CompileShader(L"shader.hlsl", "PS", "ps_5_0", &pPSBlob);
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &pixel_shader_d3dptr_);
	pPSBlob->Release();
	if (FAILED(hr))
		return;
	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"INDEX", 0, DXGI_FORMAT_R16_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"INDEX", 1, DXGI_FORMAT_R16_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"INDEX", 2, DXGI_FORMAT_R16_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"INDEX", 3, DXGI_FORMAT_R16_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"WEIGHT", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"WEIGHT", 1, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"WEIGHT", 2, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"WEIGHT", 3, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
	                                    pVSBlob->GetBufferSize(), &vertex_layout_d3dptr_);
	pVSBlob->Release();
	if (FAILED(hr))
		return;

	// Set the input layout
	_pImmediateContext->IASetInputLayout(vertex_layout_d3dptr_);


	// Load Model

	motion_controller_.LoadModel(L"..\\data\\tda\\", L"tda.pmx", L"tda2.vmd");


	std::vector<MyMeshData::Vertex> vertices;
	auto &model = motion_controller_.getModel();
	auto vs = model.vertices.get();
	for (int i = 0; i < model.vertex_count; ++i)
	{
		auto it = vs + i;
		//std::unique_ptr<pmx::PmxVertexSkinning> p2(std::move(it->skinning));
		auto sk = (pmx::PmxVertexSkinningBDEF1*) it->skinning.get();
		auto sk2 = (pmx::PmxVertexSkinningBDEF2*) it->skinning.get();
		auto sk4 = (pmx::PmxVertexSkinningBDEF4*) it->skinning.get();
		auto sks = (pmx::PmxVertexSkinningSDEF*) it->skinning.get();
		switch (it->skinning_type)
		{
		case pmx::PmxVertexSkinningType::BDEF1:
			vertices.push_back(MyMeshData::Vertex(it->positon, it->normal, it->uv, sk->bone_index, 0, 0, 0, 1, 0, 0, 0));
			break;
		case pmx::PmxVertexSkinningType::BDEF2:
			vertices.push_back(MyMeshData::Vertex(it->positon, it->normal, it->uv, sk2->bone_index1, sk2->bone_index2, 0, 0, sk2->bone_weight, 1 - sk2->bone_weight, 0, 0));
			break;
		case pmx::PmxVertexSkinningType::BDEF4:
			vertices.push_back(MyMeshData::Vertex(it->positon, it->normal, it->uv, sk4->bone_index1, sk4->bone_index2, sk4->bone_index3, sk4->bone_index4, sk4->bone_weight1, sk4->bone_weight2, sk4->bone_weight3, sk4->bone_weight3));
			//assert(sk4->bone_weight1 + sk4->bone_weight2 + sk4->bone_weight3 + sk4->bone_weight4 == 1.0f);
			break;
		case pmx::PmxVertexSkinningType::SDEF:
			vertices.push_back(MyMeshData::Vertex(it->positon, it->normal, it->uv, sks->bone_index1, sks->bone_index2, 0, 0, sks->bone_weight, 1 - sks->bone_weight, 0, 0));
			break;
		default:
			//vertices.push_back(MyMeshData::Vertex(it->positon, it->normal, it->uv, 0, 0, 0, 0, 1, 0, 0,0));
			break;
		}

		//if (sk->bone_index1 >= motion_controller_.bone_count_)
		//	sk->bone_index1 -= motion_controller_.bone_count_;

		//if (sk->bone_index2 >= motion_controller_.bone_count_)
		//	sk->bone_index2 -= motion_controller_.bone_count_;

		//if (sk->bone_index3 >= motion_controller_.bone_count_)
		//	sk->bone_index3 -= motion_controller_.bone_count_;

		//if (sk->bone_index4 >= motion_controller_.bone_count_)
		//	sk->bone_index4 -= motion_controller_.bone_count_;
	}


	UINT totalVertexCount = vertices.size();

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(MyMeshData::Vertex) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;


	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &vertices[0];
	hr = _pd3dDevice->CreateBuffer(&vbd, &InitData, &vertex_buffer_d3dptr_);
	if (FAILED(hr))
		return;

	// Set vertex buffer
	UINT stride = sizeof(MyMeshData::Vertex);
	UINT offset = 0;
	_pImmediateContext->IASetVertexBuffers(0, 1, &vertex_buffer_d3dptr_, &stride, &offset);
	//
	// Pack the indices of all the meshes into one index buffer.
	//
	std::vector<UINT32> indices;
	for (int i = 0; i < model.index_count; ++i)
	{
		indices.push_back(model.indices.get()[i]);
	}


	UINT totalIndexCount = indices.size();
	mesh_index_count_ = totalIndexCount;
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(UINT32) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &indices[0];
	hr = _pd3dDevice->CreateBuffer(&ibd, &initData, &index_buffer_d3dptr_);
	if (FAILED(hr))
		return;
	// Set index buffer
	_pImmediateContext->IASetIndexBuffer(index_buffer_d3dptr_, DXGI_FORMAT_R32_UINT, 0);
	// Set primitive topology
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Create the constant buffer for transform
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, NULL, &constant_buffer_d3dptr_);
	if (FAILED(hr))
		return;


	// Initialize the world matrix
	world_m_ = XMMatrixIdentity();

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(2.0f, 2.0f, 20.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 10.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//view_m_ = XMMatrixLookAtLH(Eye, At, Up);
	origin = Camera(Eye, At, Up);
	camera_ptr_ = &origin;
	// Initialize the projection matrix
	//proje = XMMatrixPerspectiveFovLH(XM_PIDIV2, _aspectRatio, 0.01f, 100.0f);
	camera_ptr_->setLens(XM_PIDIV4, _aspectRatio, 0.01f, 1000.0f);

	dir_light_.Ambient = XMFLOAT4(0.1f, 0.22f, 0.42f, 1.0f);
	dir_light_.Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dir_light_.Direction = XMFLOAT3(1.0f, 1.0f, 1.0f);

	material_.Ambient = XMFLOAT4(0.5f, 0.78f, 0.98f, 1.0f);
	material_.Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	material_.Reflect = XMFLOAT4(0.3f, 0.5f, 0.7f, 1.0f);

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = _pd3dDevice->CreateSamplerState(&samplerDesc, &sample_state_d3dptr_);
	for (int i = 0; i < model.texture_count; ++i)
	{
		auto path = motion_controller_.getBasePath() + model.textures.get()[i];
		if (path.substr(path.size() - 3, 3) == L"tga")
			path.replace(path.size() - 3, 3, L"dds");
		ID3D11ShaderResourceView *tmp;
		hr = D3DX11CreateShaderResourceViewFromFile(_pd3dDevice, path.c_str(), NULL, NULL, &tmp, NULL);
		if (FAILED(hr))
			texture_array.push_back(nullptr);
		else
		{
			texture_array.push_back(tmp);
		}
	}
}

//--------------------------------------------------------------------
// Update camera, and random shoot ball
//--------------------------------------------------------------------
void XTest::OnUpdate()
{
	//get I/O input
	gDInput->poll();
	// Update our time
	static float t = 0.0f;
	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();
	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.0f;

	if (gDInput->keyDown(DIK_P))
	{
		g_pause = !g_pause;
	}

	camera_ptr_->update(t);
	//update camera
	if (g_pause || gDInput->keyDown(DIK_LEFT))
	{
		if (t >= 1 / 30.0f)
		{
			motion_controller_.advanceTime();
			dwTimeStart = dwTimeCur;
		}
	}
	motion_controller_.updateBoneAnimation();
	motion_controller_.updateMatrix();
	constant_buffer_.mDirLight = dir_light_;
	constant_buffer_.mMaterial = material_;
	constant_buffer_.mEyeW = XMFLOAT3(20.0f, 20.0f, 20.0f);
	for (int i = 0; i < motion_controller_.skeleton_matrix.size(); ++i)
	{
		constant_buffer_.skeleton[i] = motion_controller_.skeleton_matrix[i];
	}
	//_pImmediateContext->UpdateSubresource(constant_buffer_d3dptr_, 0, NULL, &constant_buffer_, 0, 0);
}

//--------------------------------------------------------------------
// render all objects
//--------------------------------------------------------------------
void XTest::OnRender()
{
	// Just clear the backbuffer
	float ClearColor[4] = {0.5f, 0.5f, 0.5f, 1.0f}; //red,green,blue,alpha
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	_pImmediateContext->VSSetShader(vertex_shader_d3dptr_, NULL, 0);

	_pImmediateContext->VSSetConstantBuffers(0, 1, &constant_buffer_d3dptr_);
	_pImmediateContext->PSSetShader(pixel_shader_d3dptr_, NULL, 0);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &constant_buffer_d3dptr_);
	_pImmediateContext->PSSetSamplers(0, 1, &sample_state_d3dptr_);


	// render object

	XMFLOAT4X4 mw(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	XMMATRIX mWorld = XMLoadFloat4x4(&mw);

	constant_buffer_.mWorldInverseTranspose = XMMatrixTranspose(XMMatrixInverse(0, mWorld));
	//constant_buffer_.mWorldViewProjection = XMMatrixTranspose(proje) * XMMatrixTranspose(view_m_) * mWorld;
	constant_buffer_.mWorldViewProjection = XMMatrixTranspose(camera_ptr_->proj()) * XMMatrixTranspose(camera_ptr_->view()) * mWorld;
	constant_buffer_.mView = camera_ptr_->view();
	_pImmediateContext->UpdateSubresource(constant_buffer_d3dptr_, 0, NULL, &constant_buffer_, 0, 0);
	int total_index = 0;
	auto &model = motion_controller_.getModel();
	for (int i = 0; i < model.material_count; ++i)
	{
		if (model.materials.get()[i].diffuse_texture_index < 0)
			_pImmediateContext->PSSetShaderResources(0, 1, &texture_array[0]);
		else
			_pImmediateContext->PSSetShaderResources(0, 1, &texture_array[model.materials.get()[i].diffuse_texture_index]);
		_pImmediateContext->DrawIndexed(model.materials.get()[i].index_count, total_index, 0);
		total_index += model.materials.get()[i].index_count;
	}

	_pSwapChain->Present(1, 0);
}


void XTest::OnDestroy()
{
	SAFE_RELEASE(constant_buffer_d3dptr_);
	SAFE_RELEASE(index_buffer_d3dptr_);
	SAFE_RELEASE(vertex_buffer_d3dptr_);
	SAFE_RELEASE(vertex_layout_d3dptr_);
	SAFE_RELEASE(pixel_shader_d3dptr_);
	SAFE_RELEASE(vertex_buffer_d3dptr_);
	SAFE_RELEASE(sample_state_d3dptr_);
	for (auto i = 0; i < texture_array.size(); ++i)
	{
		SAFE_RELEASE(texture_array[i]);
	}
}

bool XTest::OnEvent(MSG msg)
{
	return true;
}
