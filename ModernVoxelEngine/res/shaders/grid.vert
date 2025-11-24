#version 450

// 假设你有这样一个 UBO 存相机矩阵
layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 proj;
    mat4 view;
    mat4 projInverse;
    mat4 viewInverse; // 必须传逆矩阵，用于还原世界坐标
    vec4 lightPos;
	vec4 viewPos;
} ubo;

layout(location = 0) out vec3 outNearPoint;
layout(location = 1) out vec3 outFarPoint;

// 这个函数用来把屏幕空间的点（x,y, depth）反投影回世界空间
vec3 UnprojectPoint(float x, float y, float z, mat4 viewInv, mat4 projInv) {
    vec4 clipSpace = vec4(x, y, z, 1.0);
    vec4 viewSpace = projInv * clipSpace;
    viewSpace /= viewSpace.w; // Perspective divide
    vec4 worldSpace = viewInv * viewSpace;
    return worldSpace.xyz;
}

void main() {
    // 骚操作：根据 Index 生成覆盖全屏的三角形 UV (0,0) -> (2,2)
    // 这样裁剪后正好覆盖屏幕 [-1, 1]
    vec2 gridUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    vec2 clipPos = gridUV * 2.0f - 1.0f;

    gl_Position = vec4(clipPos, 0.0, 1.0);

    // 关键点：我们在 VS 里算出视线射向“近平面”和“远平面”的两个点
    // 这样在 FS 里插值后，就能得到当前像素的一条射线
    outNearPoint = UnprojectPoint(clipPos.x, clipPos.y, 0.0, ubo.viewInverse, ubo.projInverse);
    outFarPoint  = UnprojectPoint(clipPos.x, clipPos.y, 1.0, ubo.viewInverse, ubo.projInverse);
}