#version 450


layout(location = 0) in vec3 vWorldPos;
layout(location = 1) in vec3 vWorldNormal;
layout(location = 2) in vec3 vLightDir;

layout(location = 0) out vec4 outColor;

// 为了匹配 Vulkan pipeline layout 的 texture binding

void main()
{
    vec3 N = normalize(vWorldNormal);
    vec3 L = normalize(vLightDir);

    // 白模基础色
    vec3 baseColor = vec3(0.9);  // 稍微偏灰白，更易看形状

    // Lambert 漫反射
    float NdotL = max(dot(N, L), 0.0);

    // 设置一点环境光避免完全黑
    float ambient = 0.2;

    vec3 color = baseColor * (ambient + NdotL);

    outColor = vec4(color, 1.0);
    //outColor = vec4(1,0,0,1);
}