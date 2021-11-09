#pragma once
#include "mathUtil.h";
#include "tgaimage.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem> // C++17 standard header file name

using namespace std;

struct Facet {
	// 3个顶点在obj文件中的顶点序号
	vector<int> verts{ 0,0,0 };
	// 3个顶点在obj文件中的uv序号
	vector<int> uv{ 0,0,0 };
	// 3个顶点在obj文件中的法线序号
	vector<int> normals{ 0,0,0 };
};

class Model
{
public:
	Model(string model_dir);
	int vertCount();
	int facetCount();
	Vector3 vertPos(const int ifacet, const int ivert);
	Vector2 vertUV(const int ifacet, const int ivert);
	Vector3 vertNormal(const int ifacet, const int ivert);
	Color32 diffuseMap(const Vector2& uv);
	Vector3 normalMap(const Vector2& uv);
	float specularMap(const Vector2& uv);
	void testPrint();

	vector<Facet> facets;
	vector<Vector3> verts;
	vector<Vector2> uv;
	vector<Vector3> normals;

private:
	void readObjFile(string file);
	void readDiffuseMap(string file);
	void readNormalMap(string file);
	void readSpecularMap(string file);


	TGAImage diffuse_map;
	TGAImage norm_map;
	TGAImage specular_map;
};

