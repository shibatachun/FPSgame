#version 450

layout (set = 0, binding = 0) uniform UBOScene
{
    mat4 projection;
    mat4 view;
    mat4 projectionInverse;
    mat4 viewInverse;
    vec4 lightPos;   // xyz = light position, w = intensity（可选）
    vec4 viewPos;    // xyz = camera position
} uboScene;

// 简单材质参数（你可以在 CPU 那边填）
layout (set = 0, binding = 1) uniform UBOMaterial
{
    float metallic;
    float roughness;
    float ao;
    float padding;   // 对齐用
} uboMat;

layout (set = 1, binding = 0) uniform sampler2D samplerColorMap;   // baseColor / albedo
layout (set = 1, binding = 1) uniform sampler2D samplerNormalMap;  // tangent-space normal

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec3 inT;
layout (location = 4) in vec3 inB;
layout (location = 5) in vec3 inN;

layout (location = 0) out vec4 outFragColor;

// ------------ PBR 辅助函数 --------------

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a  = roughness * roughness;
    float a2 = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / max(denom, 1e-4);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Disney 近似

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / max(denom, 1e-4);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// ------------ main ------------

void main()
{
    // baseColor：贴图 * 顶点色
    vec4 baseTex = texture(samplerColorMap, inUV);
    vec3 albedo  = baseTex.rgb * inColor;
    // 视作 sRGB → 线性（简单做法，可按需要关掉）
    albedo = pow(albedo, vec3(2.2));

    // tangent space normal → world space
    vec3 mapN = texture(samplerNormalMap, inUV).xyz * 2.0 - 1.0;
    mat3 TBN  = mat3(normalize(inT), normalize(inB), normalize(inN));
    vec3 N    = normalize(TBN * mapN);

    vec3 V = normalize(uboScene.viewPos.xyz - inWorldPos);
    //vec3 L = normalize(uboScene.lightPos.xyz - inWorldPos);
    vec3 L = normalize(uboScene.lightPos.xyz);  
    vec3 H = normalize(V + L);

    float distance  = length(uboScene.lightPos.xyz - inWorldPos);
    float attenuation = 1.0 / (distance * distance); // 点光源衰减
    vec3  lightColor  = vec3(1.0);                   // 你可以改成 UBO 里传颜色
    float lightIntensity = (uboScene.lightPos.w != 0.0) ? uboScene.lightPos.w : 10.0;
    vec3  radiance = lightColor * attenuation * lightIntensity;

    float metallic  = uboMat.metallic;
    float roughness = clamp(uboMat.roughness, 0.04, 1.0);
    float ao        = uboMat.ao;

    // F0：金属用 albedo，非金属大约 0.04
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 nominator   = NDF * G * F;
    float denom      = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 1e-4;
    vec3 specular    = nominator / denom;

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    // 简单环境光（可以以后换 IBL）
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // 简单 tone mapping + gamma
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    outFragColor = vec4(color, baseTex.a);
    //outFragColor = vec4( normalize(uboScene.lightPos.xyz) * 0.5 + 0.5 , 1.0 );
    //outFragColor = vec4(1,0,0,1);
}