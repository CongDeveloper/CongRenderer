// CongRenderer.cpp: 定义应用程序的入口点。
//

#include "CongRenderer.h"

using namespace std;


int main() {
	Model model("testModel0");
	Data data(model);

	data.resolution = Vector2Int(1920,1080);
	data.modelPos = Vector3(0,0,0);
	data.modelRot = Vector3(0, 180, 180);
	data.modelScale = 4*Vector3(1, 1, 1);
	data.camWorldPos = Vector3(0, 0, -10);
	data.camDir = Vector3(0, 0, 1);
	data.camUp = Vector3(0, 1, 0);
	data.fovy = 60;
	data.near = -1;
	data.far = -60;
	data.lightIntensity = 1;
	data.lightWorldPos = Vector3(0, 0, -8);
	data.lightColor = Color32(255,255,255,255);
	data.ambient = Color32(10, 10, 10, 10);
	data.diffuseK = 5;
	data.specularK = 5;
	data.specularBasePower = 5;
	data.isTangentSpaceNormalMap = true;

	auto& img = Render(data);
	img.write_tga_file("out_img.tga");
	return 0;
}

