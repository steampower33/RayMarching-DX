#include "Common.hlsli"

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float3 getSky(float3 rd)
{
    float3 sc = lerp(float3(1.0, 1.0, 1.0), float3(0.1, 0.5, 1.0), clamp(rd.y, -1.0, 1.0) * 0.5 + 0.5);
    return sc;
}

// Ray-Sphere Intersection Function
// ro: Ray Origin, rd: Ray Direction, rad: Sphere Radius
// tMin: Entry distance, tMax: Exit distance
bool intersectSphere(float3 ro, float3 rd, float rad, out float tMin, out float tMax)
{
    // Cloud container is usually centered at (0,0,0) for simplicity
    // If you want to move it, use: float3 oc = ro - sphereCenter;
    float3 oc = ro;

    float b = dot(oc, rd);
    float c = dot(oc, oc) - rad * rad;
    float h = b * b - c;
    
    if (h < 0.0)
        return false;
    
    h = sqrt(h);
    tMin = -b - h;
    tMax = -b + h;
    
    return true;
}

float getDensity(float3 p, float radius)
{
    float d = length(p);
    
    float density = saturate((radius - d) / radius);

    return density;
}

float3 getCloud(float3 ro, float3 rd)
{
    float tMin, tMax;
    float sphereRadius = 3.0;
    
    if (!intersectSphere(ro, rd, sphereRadius, tMin, tMax))
        return float3(0, 0, 0);
    
    tMin = max(tMin, 0.0);
    
    float3 col = float3(0, 0, 0);
    float transmittance = 1.0;
    
    for (float t = tMin; t < tMax; t += iStepSize)
    {
        float3 pos = ro + rd * t;
        float density = getDensity(pos, sphereRadius);

        if (density > 0.01)
        {
            col += density;
        }
    }
    
    return col;
}

float4 main(VS_OUTPUT input) : SV_Target
{
    float2 p = (input.uv - 0.5) * 2.0;
    p.x *= iResolution.x / iResolution.y;
    p.y = -p.y;
    
    float3 ro = iCameraPos;
    float3 fwd = iCameraForward;
    float3 right = iCameraRight;
    float3 up = iCameraUp;
    
    const float fl = 2.5;
    float3 rd = normalize(p.x * right + p.y * up + fl * fwd);
    
    float3 skyColor = getSky(rd);
    float3 cloudColor = getCloud(ro, rd);

    float3 c = skyColor + cloudColor;
    
    c = pow(c, 0.4545);
    
    return float4(c, 1.0);
}