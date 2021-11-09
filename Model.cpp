
#include "Model.h";

using namespace std::experimental::filesystem::v1;
using namespace filesystem;
using namespace std;

static bool startWith(const string& s, const string& s1) {
	return s.compare(0, s1.length(), s1) == 0;
}

static Color32 getColorWithUV(const TGAImage& img, const float u, const float v) {
	auto x = img.get_width() * u;
	auto y = img.get_height() * v;
	return img.get(x, y);
}

void Model::readObjFile(string file)
{
	ifstream in(file);
	if (in.fail())return;
	string line;
	while (!in.eof())
	{
		getline(in, line);
		stringstream ss (line);
		if (startWith(line, "v ")) {
			ss.ignore(2);
			float x, y, z;
			ss >> x >> y >> z;
			verts.push_back(Vector3( x, y, z ));
		}
		else if (startWith(line, "vt ")) {
			ss.ignore(3);
			float u,v;
			ss >> u >> v;
			uv.push_back(Vector2(u, v));
		}
		else if (startWith(line, "vn ")) {
			ss.ignore(3);
			float x, y, z;
			ss >> x >> y >> z;
			normals.push_back(Vector3(x, y, z));
		}
		else if (startWith(line, "f ")) {
			Facet f;
			ss.ignore(2);
			for (int i = 0; i < 3; i++)
			{
				ss >> f.verts[i];
				f.verts[i]--;
				ss.ignore(1);
				ss >> f.uv[i];
				f.uv[i]--;
				ss.ignore(1);
				ss >> f.normals[i];
				f.normals[i]--;
			}
			facets.push_back(f);
		}
	}
	in.close();
}

void Model::readDiffuseMap(string file)
{
	diffuse_map.read_tga_file(file);
}

void Model::readNormalMap(string file)
{
	norm_map.read_tga_file(file);
}

void Model::readSpecularMap(string file)
{
	specular_map.read_tga_file(file);
}

void Model::testPrint()
{
	cout << "# verts" << endl;
	for each (auto v in verts)
	{
		cout << v.x << " " << v.y << " " << v.z << endl;
	}
	cout << "# uv" << endl;
	for each (auto v in uv)
	{
		cout << v.x << " " << v.y << endl;
	}
	cout << "# normals" << endl;
	for each (auto v in normals)
	{
		cout << v.x << " " << v.y << " " << v.z << endl;
	}
	cout << "# facets" << endl;
	for each (auto v in facets)
	{
		for (int i = 0; i < 3; i++)
		{
			cout << v.verts[i] << "/" << v.uv[i] << "/" << v.normals[i] << " ";
		}
		cout << endl;
	}
}

Model::Model(string model_dir)
{
	for (auto &v : directory_iterator(model_dir))
	{
		auto ext = v.path().extension().string();
		auto name = v.path().filename().string();
		auto path = v.path().string();
		if (ext == ".obj") {
			readObjFile(path);
		}
		else if (ext == ".tga") {
			if (name.find("diffuse") != string::npos) {
				readDiffuseMap(path);
			}
			else if (name.find("nm_tangent") != string::npos) {
				readNormalMap(path);
			}
			else if (name.find("spec") != string::npos) {
				readSpecularMap(path);
			}
		}
	}
}

int Model::vertCount()
{
	return (int)verts.size();
}

int Model::facetCount()
{
	return (int)facets.size();
}

Vector3 Model::vertPos(const int ifacet, const int ivert)
{
	auto& i = facets[ifacet].verts[ivert];
	if (i >= verts.size()) {
		cout << "Error:vertOfFacet," + to_string(i) << endl;
		return Vector3(0, 0, 0);
	}
	return verts[i];
}

Vector2 Model::vertUV(const int ifacet, const int ivert)
{
	return uv[facets[ifacet].uv[ivert]];
}

Vector3 Model::vertNormal(const int ifacet, const int ivert)
{
	return normals[facets[ifacet].normals[ivert]];
}

Color32 Model::diffuseMap(const Vector2& uv)
{
	return getColorWithUV(diffuse_map, uv.x, uv.y);
}

static float color2normal(const uint8_t rgb) {
	return 2 * rgb / 255.f - 1;
}
Vector3 Model::normalMap(const Vector2& uv)
{
	auto color = getColorWithUV(norm_map, uv.x, uv.y);
	auto x = color2normal(color.r());
	auto y = color2normal(color.g());
	auto z = color2normal(color.b());
	return Vector3(x,y,z);
}

float Model::specularMap(const Vector2& uv)
{
	auto color = getColorWithUV(specular_map, uv.x, uv.y);
	return color.r();
}
