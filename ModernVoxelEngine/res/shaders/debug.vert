#version 450

// 顶点输入
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;   // 模型空间 normal
// 如果有 uv 也可以照样传，但白模不会用到
// layout(location = 2) in vec2 inUV;

// UBO：变换矩阵（按你自己的布局改）
layout(set = 0, binding = 0) uniform CameraUBO {
	mat4 projection;
	mat4 view;
	mat4 projectionInverse;
	mat4 viewInverse;
	vec4 lightPos;
	vec4 viewPos;
} ubo;
layout (push_constant) uniform PushConsts{
	mat4 model;
} primitive;

// 传给 FS 的数据
layout(location = 0) out vec3 vWorldPos;
layout(location = 1) out vec3 vWorldNormal;
layout(location = 2) out vec3 vlightPos;

void main()
{
    // 模型 -> 世界
    vec4 worldPos = primitive.model * vec4(inPosition, 1.0);
    vWorldPos = worldPos.xyz;

    // 模型 normal -> 世界 normal
	vWorldNormal = normalize(inNormal);
	vlightPos = normalize(ubo.lightPos.xyz - vWorldPos);
    gl_Position = ubo.projection * ubo.view * vec4(inPosition, 1.0);
}