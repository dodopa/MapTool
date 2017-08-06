#pragma once

#include <atlstr.h>
#include <vector>
#include <atlconv.h>
#include <fstream>
#include <iostream>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <DirectXCollision.h>

using namespace std;
using namespace DirectX;

namespace MyStruct
{
	struct VectorType
	{
		float x, y, z;
	};

	struct FontType
	{
		float left, right;
		int size;
	};

	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
		float r, g, b;
	};


	/* Descriptions */
	// OnNewFile�� ���� �޾ƿ� ������ ������ ���� ������ ���� ����ü�Դϴ�.
	struct TERRAIN_DESC
	{
		int nCell;
		int nTile;
		int nTexture;

		char *baseTextureName;
		std::vector<std::string> *textureNames;

		int textureCurSel;	// Cursor Select
	};


	/* Models */
	// ������ ���� ������ ���� ����ü�Դϴ�.
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float tu2, tv2;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		float r, g, b;
	};

	struct SkyModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};


	/* Vertices */
	struct TerrainVertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
		XMFLOAT3 color;
		XMFLOAT2 texture2;
	};
	
	// �� ���� ������ ���� �ӽ� ���� ����ü�Դϴ�.
	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VertexType
	{
		XMFLOAT3 position;
	};

	struct ColorVertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	struct TextureVertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};


	/* Buffers */
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

	struct ColorBufferType
	{
		XMFLOAT4 apexColor;
		XMFLOAT4 centerColor;
	};

	struct PixelBufferType
	{
		XMFLOAT4 pixelColor;
	};
}