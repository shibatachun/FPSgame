#version 450


// -------------------------------
// UBO from CPU
// -------------------------------
layout(set = 0, binding = 0) uniform SkyData {
    mat4 projInverse;
    mat4 viewInverse;
    vec4 sunDirection;   // xyz = direction, w unused
    vec2 screenSize;
    float padding1;
    float padding2;
} ubo;

layout(location = 0) out vec4 outColor;


// ---------------------------------------------------------
// Compute world-space ray direction from fragment position
// ---------------------------------------------------------
vec3 ComputeRayDir()
{
    // Convert pixel coord ¡ú NDC [-1,1]
    vec2 uv = gl_FragCoord.xy / ubo.screenSize * 2.0 - 1.0;

    // Clip space
    vec4 clipPos = vec4(uv, 1.0, 1.0);

    // View space
    vec4 viewPos = ubo.projInverse * clipPos;
    viewPos /= viewPos.w;

    // World space
    vec4 worldPos = ubo.viewInverse * viewPos;
    return normalize(worldPos.xyz);
}


// ---------------------------------------------------------
// Phase functions for Rayleigh / Mie scattering
// ---------------------------------------------------------
float RayleighPhase(float cosTheta)
{
    return (3.0 / (16.0 * 3.1415926)) * (1.0 + cosTheta * cosTheta);
}

float MiePhase(float cosTheta, float g)
{
    float g2 = g * g;
    float denom = pow(1.0 + g2 - 2.0 * g * cosTheta, 1.5);
    return (3.0 / (8.0 * 3.1415926)) * ((1.0 - g2) * (1.0 + cosTheta*cosTheta)) / (denom * 2.0);
}


// ---------------------------------------------------------
// Simple physical-ish sky model (fast version)
// ---------------------------------------------------------
vec3 CalculateSky(vec3 rayDir, vec3 sunDir)
{
    float cosTheta = dot(rayDir, sunDir);

    // Rayleigh & Mie coefficients (scaled for visual look)
    vec3 betaR = vec3(5.5e-6, 13.0e-6, 22.4e-6) * 30000.0;  // blue-ish
    float betaM = 2.0e-5 * 1500.0;

    float rayleigh = RayleighPhase(cosTheta);
    float mie      = MiePhase(cosTheta, 0.76);

    vec3 scattered =
          betaR * rayleigh * 15.0 +
          vec3(betaM * mie) * 0.15;

    // Sun height influences overall brightness
    float sunHeight = max(0.0, sunDir.y);
    float brightness = clamp(sunHeight * 1.5, 0.0, 1.0);

    scattered *= brightness;

    return scattered;
}


// ---------------------------------------------------------
// Sun Disk + Glow
// ---------------------------------------------------------
vec3 CalculateSun(vec3 rayDir, vec3 sunDir)
{
    float cosTheta = max(dot(rayDir, sunDir), 0.0);

    // Sun disk size
    float sunDisk = exp(-pow(1.0 - cosTheta, 2.0) / 0.00005);

    // Glow
    float glow = exp(-pow(1.0 - cosTheta, 2.0) / 0.01);

    vec3 sunColor = vec3(1.0, 0.95, 0.85);

    return sunColor * (sunDisk * 40.0 + glow * 3.0);
}


// ---------------------------------------------------------
// Final shader
// ---------------------------------------------------------
void main()
{
    vec3 rayDir = ComputeRayDir();
    vec3 sunDir = normalize(ubo.sunDirection.xyz);

    // Debug: visualize rayDir
    // outColor = vec4(abs(rayDir), 1.0); return;

    vec3 sky = CalculateSky(rayDir, sunDir);
    vec3 sun = CalculateSun(-rayDir, sunDir);
    float sunHeight = max(0.0, sunDir.y);

    vec3 color = sky + sun;
    float brightness = 0.3 + sunHeight * 0.7;
    color *= brightness;

    vec3 ambientFloor = vec3(0.03, 0.04, 0.06);
    color = max(color, ambientFloor);
    outColor = vec4(color, 1.0);
}