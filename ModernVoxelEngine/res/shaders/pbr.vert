#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inColor;
layout (location = 4) in vec4 inTangent;   // xyz = tangent, w = handedness

layout (set = 0, binding = 0) uniform UBOScene
{
    mat4 projection;
    mat4 view;
    mat4 projectionInverse;
    mat4 viewInverse;
    vec4 lightPos;   // xyz = position, w 可当强度用
    vec4 viewPos;    // xyz = camera pos
} uboScene;

// 模型矩阵 push constant
layout (push_constant) uniform PushConsts{
    mat4 model;
} primitive;

// 输出到 fragment 的插值量
layout (location = 0) out vec3 outWorldPos;
layout (location = 1) out vec2 outUV;
layout (location = 2) out vec3 outColor;
layout (location = 3) out vec3 outT;
layout (location = 4) out vec3 outB;
layout (location = 5) out vec3 outN;

void main()
{
    // world space 位置
    vec4 worldPos = primitive.model * vec4(inPos, 1.0);
    outWorldPos = worldPos.xyz;

    // 如果没有非均匀缩放，mat3(model) 就够用了；
    // 有非均匀缩放的话改成 mat3(transpose(inverse(primitive.model)))
    mat3 normalMat = mat3(primitive.model);

    vec3 N = normalize(normalMat * inNormal);
    vec3 T = normalize(normalMat * inTangent.xyz);
    vec3 B = normalize(cross(N, T) * inTangent.w);

    outT = T;
    outB = B;
    outN = N;

    outUV    = inUV;
    outColor = inColor;

    gl_Position = uboScene.projection * uboScene.view * worldPos;
    //gl_Position = uboScene.projection * uboScene.view * vec4(inPos, 1.0);
     //gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}