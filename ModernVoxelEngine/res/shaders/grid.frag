#version 450

layout(location = 0) in vec3 nearPoint;
layout(location = 1) in vec3 farPoint;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 proj;
    mat4 view;
    mat4 projInverse;
    mat4 viewInverse;
    vec4 lightPos;
	vec4 viewPos;
} ubo;

// 1. 无限平面生成网格纹理的函数
float grid(vec3 worldPos, float scale) {
    vec2 coord = worldPos.xz * scale; // 只取 XZ 平面
    vec2 derivative = fwidth(coord);  // 关键：计算屏幕空间的导数
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1.0);
    float minimumx = min(derivative.x, 1.0);
    
    // 颜色变化：让线有一点柔和的边缘
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    
    // 在远处淡出 z-fighting
    if(line > 1.0) { 
       color.a = 0.0;
    }
    return color.a;
}

void main() {
    // 2. 射线-平面求交 (Ray-Plane Intersection)
    // 平面方程：y = 0 (我们设地平面高度为 0)
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);

    // 如果 t < 0，说明视线是朝上看的，没打中地板，丢弃
    // 如果 t > 1 (或很大)，说明视线虽然朝下，但在远平面之外（对于无限平面可以忽略这个，只看 t>0）
    if (t < 0.0) {
        discard; 
    }

    // 3. 还原世界坐标
    vec3 worldPos = nearPoint + t * (farPoint - nearPoint);

    // 4. 写入深度 (可选)
    // 因为这是全屏三角形，默认深度是平面的深度。我们需要手动算一下地板的真实深度
    // 否则地板会永远画在物体最上面或最下面
    vec4 clipPos = ubo.proj * ubo.view * vec4(worldPos, 1.0);
    gl_FragDepth = clipPos.z / clipPos.w;

    // 5. 绘制网格
    // 我们可以叠加两层网格：一层大的(1.0)，一层小的(10.0)
    float g1 = grid(worldPos, 1.0);  // 大格子
    float g2 = grid(worldPos, 10.0); // 小格子

    // 混合逻辑：远处淡出小格子，只留大格子，避免摩尔纹
    // 计算相机到像素点的距离
    float dist = distance(vec3(ubo.viewInverse[3]), worldPos); // 相机位置在 ViewInv 的最后一列
    
    // 简单的线性淡出 (Linear Fog)
    float fade = 1.0 - smoothstep(0.0, 50.0, dist); // 50米后小格子消失

    float finalAlpha = max(g1, g2 * fade);
    
    // 加上距离衰减，让地平线处完全透明
    float horizonFade = 1.0 - smoothstep(20.0, 100.0, dist); 
    finalAlpha *= horizonFade;
     
    if (finalAlpha < 0.01) discard;
    //outColor = vec4(0, 0, 1, 1);
    outColor = vec4(vec3(0.8), finalAlpha); // 灰色线条
}