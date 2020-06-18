#include "static_mesh.h"
#include <Shlwapi.h>
#include <fstream>
#include <vector>
#include <functional>

//#include <fbxsdk.h>
//using namespace fbxsdk;

#include "misc.h"
#include"texture.h"
#include"shader.h"
#include"camera_manager.h"

void fbxamatrix_to_xmfloat4x4(const FbxAMatrix& fbxamatrix, DirectX::XMFLOAT4X4& xmfloat4x4)
{
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
}

StaticMesh::StaticMesh(ID3D11Device* device, const char* fileName, SHADER_TYPE shaderType, bool pathOrganize, int organizeType)
{
	std::string fbxName = fileName;
    size_t engineer = fbxName.find_last_of(".") + 1;
    std::string name = fbxName.substr(0, engineer);
	name += "bin";
	if (PathFileExistsA((name).c_str()))
	{
		std::ifstream ifs;
		
		ifs.open((name).c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);

	}
	else
	{
		CreateMesh(device, fileName, pathOrganize, organizeType);
		std::ofstream ofs;
		ofs.open((name).c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}
	CreateBuffers(device);
	CreateShaderResourceView(device,shaderType);
	CreateShader(device, shaderType);
}




void StaticMesh::SetShaderResouceView(ID3D11DeviceContext* context, Subset& subset)
{
	switch (mShaderType)
	{
	case SHADER_TYPE::USEALLY:
		context->PSSetShaderResources(0, 1, subset.diffuse.mShaderResourceView.GetAddressOf());
		break;
	case SHADER_TYPE::NORMAL:
		context->PSSetShaderResources(0, 1, subset.diffuse.mShaderResourceView.GetAddressOf());
		context->PSSetShaderResources(1, 1, subset.normal.mShaderResourceView.GetAddressOf());
		context->PSSetShaderResources(2, 1, subset.bump.mShaderResourceView.GetAddressOf());
		break;
	}
}

void StaticMesh::CreateMesh(ID3D11Device* device, const char* fileName, bool pathOrganize, int organizeType)
{
	// Create the FBX SDK manager
	FbxManager* manager = FbxManager::Create();

	// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h.
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

	// Create an importer.
	FbxImporter* importer = FbxImporter::Create(manager, "");

	// Initialize the importer.
	bool importStatus = false;
	importStatus = importer->Initialize(fileName, -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

	// Create a new scene so it can be populated by the imported file.
	FbxScene* scene = FbxScene::Create(manager, "");

	// Import the contents of the file into the scene.
	importStatus = importer->Import(scene);
	_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

	// Convert mesh, NURBS and patch into triangle mesh
	fbxsdk::FbxGeometryConverter geometryConverter(manager);
	geometryConverter.Triangulate(scene, /*replace*/true);

	// Fetch node attributes and materials under this node recursively. Currently only mesh.	
	std::vector<FbxNode*> fetchedMeshes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node)
	{
		if (node)
		{
			FbxNodeAttribute* fbxNodeAttribute = node->GetNodeAttribute();
			if (fbxNodeAttribute)
			{
				switch (fbxNodeAttribute->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					fetchedMeshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
			{
				traverse(node->GetChild(i));
			}
		}
	};
	traverse(scene->GetRootNode());

	meshes.resize(fetchedMeshes.size());

	//FbxMesh *fbx_mesh = fetched_meshes.at(0)->GetMesh();  // Currently only one mesh.
	for (size_t i = 0; i < fetchedMeshes.size(); i++)
	{
		FbxMesh* fbxMesh = fetchedMeshes.at(i)->GetMesh();
		Mesh& mesh = meshes.at(i);

		FbxAMatrix globalTransform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
		fbxamatrix_to_xmfloat4x4(globalTransform, mesh.globalTransform);

		// Fetch material properties.
		const int numberOfMaterials = fbxMesh->GetNode()->GetMaterialCount();

		//subsets.resize(number_of_materials);
		//subsets.resize(number_of_materials > 0 ? number_of_materials : 1);

		mesh.subsets.resize(numberOfMaterials > 0 ? numberOfMaterials : 1);
		for (int indexOfMaterial = 0; indexOfMaterial < numberOfMaterials; ++indexOfMaterial)
		{
			//subset &subset = subsets.at(index_of_material);
			Subset& subset = mesh.subsets.at(indexOfMaterial);

			const FbxSurfaceMaterial* surfaceMaterial = fbxMesh->GetNode()->GetMaterial(indexOfMaterial);

			FbxTettureNameLoad(FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, subset.diffuse, surfaceMaterial, fileName, pathOrganize,organizeType);
			FbxTettureNameLoad(FbxSurfaceMaterial::sNormalMap, FbxSurfaceMaterial::sBumpFactor, subset.normal, surfaceMaterial, fileName, pathOrganize, organizeType);
			FbxTettureNameLoad(FbxSurfaceMaterial::sBump, FbxSurfaceMaterial::sBumpFactor, subset.bump, surfaceMaterial, fileName, pathOrganize, organizeType);
		}

		// Count the polygon count of each material
		if (numberOfMaterials > 0)
		{
			// Count the faces of each material
			const int numberOfPolygons = fbxMesh->GetPolygonCount();
			for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; ++indexOfPolygon)
			{
				const u_int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);

				//subsets.at(material_index).index_count += 3;
				mesh.subsets.at(materialIndex).indexCount += 3;

			}

			// Record the offset (how many vertex)
			int offset = 0;

			//for (subset &subset : subsets)
			for (Subset& subset : mesh.subsets)
			{
				subset.indexStart = offset;
				offset += subset.indexCount;
				// This will be used as counter in the following procedures, reset to zero
				subset.indexCount = 0;
			}
		}

		// Fetch mesh data
		u_int vertexCount = 0;

		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);

		const FbxVector4* arrayOfControlPoints = fbxMesh->GetControlPoints();
		const int numberOfPolygons = fbxMesh->GetPolygonCount();
		mesh.indices.resize(numberOfPolygons * 3);
		Face f;
		for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; indexOfPolygon++)
		{
			// The material for current face.
			int indexOfMaterial = 0;
			if (numberOfMaterials > 0)
			{
				indexOfMaterial = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);
			}

			// Where should I save the vertex attribute index, according to the material
			//subset &subset = subsets.at(index_of_material);
			Subset& subset = mesh.subsets.at(indexOfMaterial);
			const int indexOffset = subset.indexStart + subset.indexCount;
			f.materialIndex = indexOfMaterial;
			for (int indexOfVertex = 0; indexOfVertex < 3; indexOfVertex++)
			{
				Vertex vertex;
				const int indexOfControlPoint = fbxMesh->GetPolygonVertex(indexOfPolygon, indexOfVertex);
				vertex.position.x = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][0]);
				vertex.position.y = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][1]);
				vertex.position.z = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][2]);

				if (fbxMesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(indexOfPolygon, indexOfVertex, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}

				if (fbxMesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmapped_uv;
					fbxMesh->GetPolygonVertexUV(indexOfPolygon, indexOfVertex, uvNames[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}
				//�ʂ��Ƃ̒��_�f�[�^
				DirectX::XMVECTOR facePosition = DirectX::XMLoadFloat3(&vertex.position);
				DirectX::XMMATRIX fW = DirectX::XMLoadFloat4x4(&mesh.globalTransform);
				DirectX::XMVECTOR fLP = DirectX::XMVector3TransformCoord(facePosition, fW);
				DirectX::XMStoreFloat3(&f.position[indexOfVertex], fLP);
				mesh.vertices.push_back(vertex);
				mesh.indices.at(indexOffset + indexOfVertex) = static_cast<u_int>(vertexCount);
				vertexCount += 1;
			}
			subset.indexCount += 3;
			//�ʃf�[�^�̕ۑ�
			mesh.faces.push_back(f);
		}
		//create_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
	}
	manager->Destroy();

}



void StaticMesh::FbxTettureNameLoad(const char* property_name, const char* factor_name, Material& material,
	const FbxSurfaceMaterial* surfaceMaterial, const char* fileName, bool pathOrganize, int organizeType)
{
	const FbxProperty property = surfaceMaterial->FindProperty(property_name);
	const FbxProperty factor = surfaceMaterial->FindProperty(factor_name);
	if (property.IsValid() && factor.IsValid())
	{
		FbxDouble3 color = property.Get<FbxDouble3>();
		double f = factor.Get<FbxDouble>();
		material.color.x = static_cast<float>(color[0] * f);
		material.color.y = static_cast<float>(color[1] * f);
		material.color.z = static_cast<float>(color[2] * f);
		material.color.w = 1.0f;
	}
	if (property.IsValid())
	{
		const int numberOfTextures = property.GetSrcObjectCount<FbxFileTexture>();
		if (numberOfTextures)
		{
			const FbxFileTexture* fileTexture = property.GetSrcObject<FbxFileTexture>();
			if (fileTexture)
			{
				const char* filename = fileTexture->GetRelativeFileName();

				wchar_t textureUnicode[256];

				if (pathOrganize)
				{
					char file[256], textureFileName[256], textureFileType[256];

					_splitpath_s(filename, nullptr, 0, nullptr, 0, textureFileName, 256, textureFileType, 256);

					if (strcmp(textureFileType, ".tga") == 0)
					{
						switch (organizeType)
						{
						case 0:
							strcat_s(textureFileType, 256, ".png");
							break;
						case 1:
							strcpy_s(textureFileType, 256, ".png");
							break;
						case 2:
							break;
						}
					}
					char dirname[256];
					::_splitpath_s(fileName, nullptr, 0, dirname, 256, nullptr, 0, nullptr, 0);

					::_makepath_s(file, 256, nullptr, dirname, textureFileName, textureFileType);


					MultiByteToWideChar(CP_ACP, 0, file, static_cast<int>(strlen(file) + 1), textureUnicode, 1024);

				}
				else
				{
					wchar_t fbxUnicode[256];
					MultiByteToWideChar(CP_ACP, 0, fileName, static_cast<int>(strlen(fileName) + 1), fbxUnicode, 1024);
					MultiByteToWideChar(CP_ACP, 0, fileTexture->GetFileName(), static_cast<int>(strlen(fileTexture->GetFileName()) + 1), textureUnicode, 1024);
					combine_resource_path(textureUnicode, fbxUnicode, textureUnicode);
				}
				material.textureName = textureUnicode;





				//material.texture_filename = texture_unicode;
			}
		}
	}

}

void StaticMesh::CreateBuffers(ID3D11Device* device)
{
	HRESULT hr;

	for (auto& mesh : meshes)
	{
		//���_�o�b�t�@�̐���
		{
			D3D11_BUFFER_DESC desc = {};
			D3D11_SUBRESOURCE_DATA data = {};

			desc.ByteWidth = sizeof(Vertex) * mesh.vertices.size();
			//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			data.pSysMem = mesh.vertices.data();
			data.SysMemPitch = 0; //Not use for vertex buffers.mm 
			data.SysMemSlicePitch = 0; //Not use for vertex buffers.

			hr = device->CreateBuffer(&desc, &data, mesh.vertexBuffer.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		//�C���e�b�N�X�o�b�t�@�̐���
		{
			D3D11_BUFFER_DESC desc = {};
			D3D11_SUBRESOURCE_DATA data = {};

			desc.ByteWidth = sizeof(u_int) * mesh.indices.size();
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			data.pSysMem = mesh.indices.data();
			data.SysMemPitch = 0; //Not use for index buffers.
			data.SysMemSlicePitch = 0; //Not use for index buffers.
			hr = device->CreateBuffer(&desc, &data, mesh.indexBuffer.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}
}

void StaticMesh::CreateShaderResourceView(ID3D11Device* device, SHADER_TYPE shaderType)
{
	for (auto& mesh : meshes)
	{
		for (auto& subset : mesh.subsets)
		{
			if (wcscmp(subset.diffuse.textureName.c_str(), L"") != 0)
			{
				D3D11_TEXTURE2D_DESC texture2dDesc;
				load_texture_from_file(device, subset.diffuse.textureName.c_str(), subset.diffuse.mShaderResourceView.GetAddressOf(), &texture2dDesc);
			}
			if (!subset.diffuse.mShaderResourceView)
			{
				make_dummy_texture(device, subset.diffuse.mShaderResourceView.GetAddressOf());
			}
			if (wcscmp(subset.normal.textureName.c_str(), L"") != 0&& shaderType==SHADER_TYPE::NORMAL)
			{
				D3D11_TEXTURE2D_DESC texture2dDesc;
				load_texture_from_file(device, subset.normal.textureName.c_str(), subset.normal.mShaderResourceView.GetAddressOf(), &texture2dDesc);
			}
			if (!subset.normal.mShaderResourceView)
			{
				make_dummy_texture(device, subset.normal.mShaderResourceView.GetAddressOf());
			}
			if (wcscmp(subset.bump.textureName.c_str(), L"") != 0 && shaderType == SHADER_TYPE::NORMAL)
			{
				D3D11_TEXTURE2D_DESC texture2dDesc;
				load_texture_from_file(device, subset.bump.textureName.c_str(), subset.bump.mShaderResourceView.GetAddressOf(), &texture2dDesc);
			}
			if (!subset.bump.mShaderResourceView)
			{
				make_dummy_texture(device, subset.bump.mShaderResourceView.GetAddressOf());
			}

		}
	}
}

void StaticMesh::CreateShader(ID3D11Device* device, SHADER_TYPE shaderType)
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	std::string vsName = { "Data/shader/" };
	std::string psName = { "Data/shader/" };
	switch (shaderType)
	{
	case SHADER_TYPE::USEALLY:
		vsName += "static_mesh_vs.cso";
		psName += "static_mesh_ps.cso";
		break;
	case SHADER_TYPE::NORMAL:
		vsName += "static_mesh_normal_vs.cso";
		psName += "static_mesh_normal_ps.cso";
		break;
	}
	create_vs_from_cso(device, vsName.c_str(), mVSShader.GetAddressOf(), mInput.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	/*if(shaderType!=SHADER_TYPE::SHADOW)*/create_ps_from_cso(device, psName.c_str(), mPSShader.GetAddressOf());
	mShaderType = shaderType;
}

int StaticMesh::RayPick(const VECTOR3F& startPosition, const VECTOR3F& endPosition, VECTOR3F* outPosition, VECTOR3F* outNormal, float* outLength)
{
	return 0;
}
void StaticMesh::ChangeShader(ID3D11Device* device, SHADER_TYPE shaderType, std::vector<TextureMapData> data)
{
	for (auto& mesh : meshes)
	{
		for (auto& subset : mesh.subsets)
		{
			for (auto& d : data)
			{
				if (wcscmp(subset.diffuse.textureName.c_str(), d.deffuseName) == 0)
				{
					subset.normal.textureName = d.normalName;
					subset.bump.textureName = d.bumpName;
					break;
				}
			}
		}
	}
	CreateShaderResourceView(device, shaderType);
	CreateShader(device, shaderType);
}
/************************************************/
//             �`��N���X
/************************************************/
MeshRender::MeshRender(ID3D11Device* device)
{
	HRESULT hr = S_OK;

	//shadow shader
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		Microsoft::WRL::ComPtr<ID3D11InputLayout>input;
		create_vs_from_cso(device, "Data/shader/static_mesh_shadow_vs.cso", mShadowVSShader.GetAddressOf(), input.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	}

	// create rasterizer state : solid mode
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizerDesc, mRasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// �[�x�X�e���V���X�e�[�g�̐ݒ�
	{
		//D3D11_DEPTH_STENCIL_DESC desc;
		//::memset(&desc, 0, sizeof(desc));
		//desc.DepthEnable = true;
		//desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		//desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		//hr = device->CreateDepthStencilState(&desc, mDepthStencilState.GetAddressOf());

		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.StencilEnable = FALSE;
		depth_stencil_desc.StencilReadMask = 0xFF;
		depth_stencil_desc.StencilWriteMask = 0xFF;
		depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, mDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	// create constant buffer
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(CbScene);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		hr = device->CreateBuffer(&bufferDesc, nullptr, mCbScene.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		bufferDesc.ByteWidth = sizeof(CbObj);

		hr = device->CreateBuffer(&bufferDesc, nullptr, mCbObj.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}

	// create sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	memcpy(samplerDesc.BorderColor, &VECTOR4F(0.0f, 0.0f, 0.0f, 0.0f), sizeof(VECTOR4F));
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;
	//D3D11_SAMPLER_DESC samplerDesc = {};
	//samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	//samplerDesc.MipLODBias = 0;
	//samplerDesc.MaxAnisotropy = 16;
	//samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//memcpy(samplerDesc.BorderColor, &VECTOR4F(0.0f, 0.0f, 0.0f, 0.0f), sizeof(VECTOR4F));
	//samplerDesc.MinLOD = 0;
	//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&samplerDesc, mDiffuseSamplerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

	hr = device->CreateSamplerState(&samplerDesc, mNormalSamplerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void MeshRender::ShadowBegin(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	context->VSSetShader(mShadowVSShader.Get(), nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);

	ID3D11Buffer* constant_buffers[] =
	{
		mCbScene.Get(),
		mCbObj.Get(),
	};
	context->VSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	context->RSSetState(mRasterizerState.Get());
	CbScene cbScene;

	cbScene.lightDirection = VECTOR4F(0,0,0,0);
	cbScene.view = view;
	cbScene.projection = projection;
	cbScene.lightColor = VECTOR4F(1, 1, 1, 1);
	cbScene.ambientColor = VECTOR4F(1, 1, 1, 1);
	cbScene.eyePosition = VECTOR4F(0,0,0,0);
	context->UpdateSubresource(mCbScene.Get(), 0, 0, &cbScene, 0, 0);

}

void MeshRender::ShadowRender(ID3D11DeviceContext* context, StaticMesh* obj, const FLOAT4X4& world)
{
	context->IASetInputLayout(obj->GetInputLayout().Get());

	for (StaticMesh::Mesh& mesh : obj->meshes)
	{
		u_int stride = sizeof(StaticMesh::Vertex);
		u_int offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		CbObj data;

		DirectX::XMStoreFloat4x4(&data.world, DirectX::XMLoadFloat4x4(&mesh.globalTransform) * DirectX::XMLoadFloat4x4(&world));
		context->UpdateSubresource(mCbObj.Get(), 0, 0, &data, 0, 0);
		for (StaticMesh::Subset& subset : mesh.subsets)
		{
			context->DrawIndexed(subset.indexCount, subset.indexStart, 0);
		}
	}
}

void MeshRender::ShadowEnd(ID3D11DeviceContext* context)
{
}

void MeshRender::Begin(ID3D11DeviceContext* context, const VECTOR4F& light, const FLOAT4X4& view, const FLOAT4X4& projection)
{

	ID3D11Buffer* constant_buffers[] =
	{
		mCbScene.Get(),
		mCbObj.Get(),
	};
	context->VSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
	context->PSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);

	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	context->RSSetState(mRasterizerState.Get());
	context->PSSetSamplers(0, 1, mDiffuseSamplerState.GetAddressOf());
	context->PSSetSamplers(1, 1, mNormalSamplerState.GetAddressOf());

	CbScene cbScene;

	cbScene.lightDirection = light;
	cbScene.view = view;
	cbScene.projection = projection;
	cbScene.lightColor = VECTOR4F(1, 1, 1, 1);
	cbScene.ambientColor = VECTOR4F(1, 1, 1, 1);
	cbScene.eyePosition = VECTOR4F(pCamera.GetCamera()->GetEye().x, pCamera.GetCamera()->GetEye().y, pCamera.GetCamera()->GetEye().z, 1.0f);
	context->UpdateSubresource(mCbScene.Get(), 0, 0, &cbScene, 0, 0);

}

void MeshRender::Render(ID3D11DeviceContext* context, StaticMesh* obj, const FLOAT4X4& world, const VECTOR4F color)
{
	context->VSSetShader(obj->GetVSShader().Get(), nullptr, 0);
	context->PSSetShader(obj->GetPSShader().Get(), nullptr, 0);
	context->IASetInputLayout(obj->GetInputLayout().Get());

	for (StaticMesh::Mesh& mesh : obj->meshes)
	{
		u_int stride = sizeof(StaticMesh::Vertex);
		u_int offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		CbObj data;

		DirectX::XMStoreFloat4x4(&data.world, DirectX::XMLoadFloat4x4(&mesh.globalTransform) * DirectX::XMLoadFloat4x4(&world));

		for (StaticMesh::Subset& subset : mesh.subsets)
		{
			data.color.x = subset.diffuse.color.x * color.x;
			data.color.y = subset.diffuse.color.y * color.y;
			data.color.z = subset.diffuse.color.z * color.z;
			data.color.w = color.w;
			context->UpdateSubresource(mCbObj.Get(), 0, 0, &data, 0, 0);
			obj->SetShaderResouceView(context, subset);
			context->DrawIndexed(subset.indexCount, subset.indexStart, 0);
		}
	}
	ID3D11ShaderResourceView* srv = nullptr;
	for (int i = 0;i < 3;i++)
	{
		context->PSSetShaderResources(i, 1, &srv);
	}

}

void MeshRender::End(ID3D11DeviceContext* context)
{
}
