#include "MathUtil.h"
#include "GLUtil.hpp"
#include "Model.h"
#include "math.h"

#pragma once


// 数据
class Data {
public:
    Data(Model& model):model(model){}

    Vector2Int resolution;

    // Model
    Vector3 modelPos;
    Vector3 modelRot;
    Vector3 modelScale;

    // View
    Vector3 camWorldPos;
    Vector3 camDir;
    Vector3 camUp;

    // Project
    float aspect() { return (float)resolution.x / resolution.y; }
    float fovy;
    float near;
    float far;

    // Viewport
    int width() { return resolution.x; }
    int height() { return resolution.y; }

    // light
    float lightIntensity;
    Vector3 lightWorldPos;
    Color32 lightColor;

    // model, map
    Model model;

    // shader data
    Color32 ambient;
    float diffuseK = 1;
    float specularK = 1;
    float specularBasePower = 1;
    bool isTangentSpaceNormalMap = true; // 是否是切线空间法线贴图

    Vector3 camViewPos() { return Vector3::Zero(); }

    // temp
    Matrix4x4 modelMat;
    Matrix4x4 viewMat;
    Matrix4x4 projMat;
    Matrix4x4 mvp;
    Matrix4x4 viewportMat;
    Matrix4x4 normalTranslateMat;
    Vector3 lightNdcPos;
    Vector3 lightViewPos;
    Vector3 camNdcPos;
    int length;
};

// 顶点
class Vertex {
public :
    int ifacet, ivert;
    Vector2 uv;
    Vector3 ndcPos;
    Vector3 worldPos;
    Vector3 viewPos;
    Vector3 normal; // world
    Vector4 homScreenPos; // 齐次屏幕坐标
    Vector2 screenPos;

};

// 片段
class Frag {
public :
    Vertex* verts; // 3 vertex of triangle
    Vector2Int screenPos;
    Vector3 barCoo;
    Vector2 uv;
};


#pragma region Render Pipeline

TGAImage Render(Data& data);

void InitData(Data& data);

void VertexShader(Vertex& v, Data& data);

bool TestFacet(Vertex verts[]);
bool IsOutside(Vertex verts[]);
bool IsBackward(Vertex verts[]);

void ProjToScreen(Vertex verts[], Data& data);

void Rasterize(Vertex verts[], Data& data, float zBuffer[], TGAImage& frameBuffer);
Vector3 NdcVertBarCoo(Vector3& screenBarCoo, Vertex verts[]);

bool TestFrag(Frag& frag, float zBuffer[], Data& data);

void FragShader(Frag& frag, Data& data, TGAImage& frameBuffer);
Vector3 CalNormalWithNormalMap(Frag& frag, Data& data);
void GetTB(Vertex& p0, Vertex& p1, Vertex& p2, Vector3& N, Vector3& T, Vector3& B);
void GetTB2(Vertex& p0, Vertex& p1, Vertex& p2, Vector3& N, Vector3& T, Vector3& B);


TGAImage Render(Data& data) {

    // init zbuffer
    data.length = data.width() * data.height();
    float* zBuffer = new float[data.length];
    for (int i = 0; i < data.length; i++) {
        zBuffer[i] = -FLT_MAX;
    }

    TGAImage frameBuffer(data.width(), data.height(), Format::RGBA);

    InitData(data);
    Vertex verts[3];
    verts[0].ivert = 0;
    verts[1].ivert = 1;
    verts[2].ivert = 2;
    for (int i = 0; i < data.model.facetCount(); i++) {
        for (int j = 0; j < 3; j++) {
            verts[j].ifacet = i;
            VertexShader(verts[j], data);
        }

        if (!TestFacet(verts)) continue;

        ProjToScreen(verts, data);
        Rasterize(verts, data, zBuffer, frameBuffer);
    }

    return frameBuffer;
}

void InitData(Data& data) {
    // 矩阵
    data.modelMat = ModelMat(data.modelPos, data.modelRot, data.modelScale);
    data.viewMat = ViewMat(data.camWorldPos, data.camDir, data.camUp);
    data.projMat = PerspectProjMat(data.fovy, data.aspect(), data.near, data.far);
    data.mvp = data.projMat * data.viewMat * data.modelMat;
    data.viewportMat = ViewportMat(data.width(), data.height());

    data.lightNdcPos = TranslatePoint(data.projMat * data.viewMat, data.lightWorldPos);
    data.lightViewPos = TranslatePoint(data.viewMat, data.lightWorldPos);
    data.camNdcPos = TranslatePoint(data.projMat, Vector3::Zero());
    data.normalTranslateMat = (data.viewMat * data.modelMat).Inverse().Transpose(); // 法线变换矩阵=mv的逆的转置
}

// 顶点着色:顶点uv，顶点ndc坐标，顶点法线
void VertexShader(Vertex& v, Data& data) {
    // ndc 坐标
    auto& localPos = data.model.vertPos(v.ifacet, v.ivert);
    v.ndcPos = TranslatePoint(data.mvp, localPos);
    v.viewPos = TranslatePoint(data.viewMat * data.modelMat, localPos);
    v.worldPos = TranslatePoint(data.modelMat, localPos);

    // uv
    v.uv = data.model.vertUV(v.ifacet, v.ivert);

    // 顶点法线
    auto& vertNormal = data.model.vertNormal(v.ifacet, v.ivert).Normalized();
    v.normal = TranslateDir(data.normalTranslateMat, vertNormal);
}

bool TestFacet(Vertex verts[]) {
    return !IsOutside(verts) && !IsBackward(verts);
}

// 裁剪，背面剔除
bool IsOutside(Vertex verts[]) {
    for (int i = 0; i < 3; i++)
    {
        auto& v = verts[i];
        auto& p = v.ndcPos;
        if (p.x >= -1 && p.x <= 1 && p.y >= -1 && p.y <= 1 && p.z >= -1 && p.z <= 1) return false;
    }

    return true;
}

bool IsBackward(Vertex verts[]) {
    // 三角形正对相机，Dot((p1-p0)x(p2-p0),camDir)<0
    auto& normal = Vector3::Cross((verts[1].ndcPos - verts[0].ndcPos), (verts[2].ndcPos - verts[0].ndcPos));
    auto& ndcCamDir = Vector3::Back();
    return Vector3::Dot(normal, ndcCamDir) >= 0;
}

// 屏幕映射
void ProjToScreen(Vertex verts[], Data& data) {
    for (int i = 0; i < 3; i++)
    {
        auto& v = verts[i];
        v.homScreenPos = data.viewportMat * HomogeneousCoordinate(v.ndcPos, true);
        v.screenPos = HomogeneousDivide(v.homScreenPos);
    }
}

// 光栅化: 片段屏幕坐标，片段重心坐标
void Rasterize(Vertex verts[], Data& data, float zBuffer[], TGAImage& frameBuffer) {
    auto& frag = Frag();

    // 包围盒
    int xmin, xmax, ymin, ymax;
    getBoundingBox(verts[0].screenPos, verts[1].screenPos, verts[2].screenPos, xmin, xmax, ymin, ymax);

    // for each pixel in bounding box
    for (int x = xmin; x <= xmax; x++) {
        for (int y = ymin; y <= ymax; y++) {
            auto& fragScreenPos = Vector2Int(x, y);

            auto& screenBarCoo = barycentricCoordinate((Vector2)fragScreenPos, verts[0].screenPos, verts[1].screenPos, verts[2].screenPos);

            // 剔除三角形外的片段
            if (screenBarCoo.x < 0 || screenBarCoo.y < 0 || screenBarCoo.z < 0) continue;

            frag.verts = verts;
            frag.screenPos = fragScreenPos;
            frag.barCoo = NdcVertBarCoo(screenBarCoo, verts);

            if (!TestFrag(frag, zBuffer, data)) continue;
            FragShader(frag, data, frameBuffer);
        }
    }
}

Vector3 NdcVertBarCoo(Vector3& screenBarCoo, Vertex verts[]) {
    auto& ret = Vector3(screenBarCoo.x / verts[0].homScreenPos.w, screenBarCoo.y / verts[1].homScreenPos.w,
        screenBarCoo.z / verts[2].homScreenPos.w);
    auto sum = ret.x + ret.y + ret.z;
    ret = ret/sum;
    return ret;
}

bool TestFrag(Frag& frag, float zBuffer[], Data& data) {
    // 裁剪
    if (frag.screenPos.x < 0 || frag.screenPos.x >= data.width() || frag.screenPos.y < 0 || frag.screenPos.y >= data.height())
        return false;

    // 重心坐标合法性
    // if (frag.barCoo.x < 0 || frag.barCoo.x > 1 || frag.barCoo.y < 0 || frag.barCoo.y > 1 ||frag.barCoo.z < 0 || frag.barCoo.z > 1) 
    //     return false;

    // 深度测试
    auto index = frag.screenPos.x + frag.screenPos.y * data.width();
    if (index >= data.length || index < 0) return false;
    auto depth = Lerp(frag.barCoo, frag.verts[0].ndcPos.z, frag.verts[1].ndcPos.z, frag.verts[2].ndcPos.z);
    if (zBuffer[index] >= depth) return false;
    zBuffer[index] = depth;
    return true;
}

// 片段着色
void FragShader(Frag& frag, Data& data, TGAImage& frameBuffer) {
    // prepare
    auto& fragViewPos = Lerp(frag.barCoo, frag.verts[0].viewPos, frag.verts[1].viewPos, frag.verts[2].viewPos);
    frag.uv = Lerp(frag.barCoo, frag.verts[0].uv, frag.verts[1].uv, frag.verts[2].uv);

    auto& mapColor = data.model.diffuseMap(frag.uv);
    auto mapSpecPower = data.model.specularMap(frag.uv);
    auto& normal = CalNormalWithNormalMap(frag, data);
    auto& vertToLight = (data.lightViewPos - fragViewPos).Normalized();
    auto distanceToLight = (data.lightViewPos - fragViewPos).Magnitude();
    auto& pointToCam = (data.camViewPos() - fragViewPos).Normalized();

    // diffuse
    auto& diffuseAffectByNormal = max(0.f, Vector3::Dot(normal, vertToLight));
    auto diffuse = data.diffuseK * data.lightIntensity / distanceToLight * diffuseAffectByNormal;

    auto& specularAffectByNormal = max(0.f, Vector3::Dot(normal, (pointToCam + vertToLight).Normalized()));
    auto p = data.specularBasePower + mapSpecPower;
    auto specular = data.specularK * data.lightIntensity / distanceToLight * pow(specularAffectByNormal, p);

    auto& color = mapColor * (diffuse + specular) + data.ambient;
    color.a() = 255;
    frameBuffer.set(frag.screenPos.x, frag.screenPos.y, color);
}

Vector3 CalNormalWithNormalMap(Frag& frag, Data& data) {
    if (!data.isTangentSpaceNormalMap) {
        // 使用模型空间法线贴图
        auto& localNormal = data.model.normalMap(frag.uv).Normalized();
        return TranslateDir(data.viewMat * data.modelMat, localNormal);
    }

    auto& p0 = frag.verts[0];
    auto& p1 = frag.verts[1];
    auto& p2 = frag.verts[2];
    auto& N = Lerp(frag.barCoo, p0.normal, p1.normal, p2.normal).Normalized();

    // return N;
    Vector3 T, B;
    GetTB(p0, p1, p2, N, T, B);
    // GetTB2(p0, p1, p2, N, out var T, out var B);

    float mDat[4][4]{
            {T.x,B.x,N.x,0},
            {T.y,B.y,N.y,0},
            {T.z,B.z,N.z,0},
            {0,0,0,1},
    };
    auto& M = Matrix4x4(mDat);

    auto& mapNormal = data.model.normalMap(frag.uv).Normalized();
    auto& normal = TranslateDir(M, mapNormal);
    return normal;
}

/* TBN 公式
    // 构建triangle平面的方向向量 (position delta, δ)
    CELL::float3  deltaPos1 = v1 - v0;
    CELL::float3  deltaPos2 = v2 - v0;
    // 构建UV平面的两个方向的向量 (uv delta, δ)
    CELL::float2 deltaUV1   = uv1 - uv0;
    CELL::float2 deltaUV2   = uv2 - uv0;

    float   r  = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);  // uv叉积作底
    CELL::float3 tangent    = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r; // 得出切线
    CELL::float3 binormal   = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r; // 得出副切线

*/
static void GetTB(Vertex& p0, Vertex& p1, Vertex& p2, Vector3& N, Vector3& T, Vector3& B) {
    auto& deltaPos1 = p1.viewPos - p0.viewPos;
    auto& deltaPos2 = p2.viewPos - p0.viewPos;
    auto& deltaUV1 = p1.uv - p0.uv;
    auto& deltaUV2 = p2.uv - p0.uv;

    auto den = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    T = ((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * den).Normalized();
    B = ((deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * den).Normalized();
    // B = (Vector3.Cross(T,N)).normalized;
}

/* TBN 公式
mat<3, 3> AI = mat<3, 3>{ {ndc_tri.col(1) - ndc_tri.col(0), ndc_tri.col(2) - ndc_tri.col(0), bn} }.invert();
vec3 i = AI * vec3(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
vec3 j = AI * vec3(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);
mat<3, 3> B = mat<3, 3>{ {i.normalize(), j.normalize(), bn} }.transpose();
 */
static void GetTB2(Vertex& p0, Vertex& p1, Vertex& p2, Vector3& N, Vector3& T, Vector3& B) {
    auto& E1 = p1.viewPos - p0.viewPos;
    auto& E2 = p2.viewPos - p0.viewPos;
    auto deltaU1 = p1.uv.x - p0.uv.x;
    auto deltaU2 = p2.uv.x - p0.uv.x;
    auto deltaV1 = p1.uv.y - p0.uv.y;
    auto deltaV2 = p2.uv.y - p0.uv.y;
    float AIData[4][4]{
        {E1.x,E1.y,E1.z,0},
        {E2.x,E2.y,E2.z,0},
        {N.x,N.y,N.z,0},
        {0,0,0,1}
    };
    auto& AI = Matrix4x4(AIData).Inverse();
    T = TranslateDir(AI, Vector3(deltaU1, deltaU2, 0));
    B = TranslateDir(AI, Vector3(deltaV1, deltaV2, 0));
    // B = Vector3.Cross(N, T);
}

#pragma endregion
