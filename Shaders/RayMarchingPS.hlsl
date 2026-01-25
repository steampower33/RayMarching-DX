cbuffer Constants : register(b0)
{
    float iTime;
    float2 iResolution;
    float iFogDensity;

    float3 iCameraPos;
    float iConeHeight;

    float3 iCameraForward;
    float iConeRadius;

    float3 iCameraRight;
    float padding1;

    float3 iCameraUp;
    float padding2;

    float3 iFogColor;
    float padding3;

    float3 iSunDir;
    float padding4;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// --- SDF Functions ---

// Signed Distance Function for a capped cone
float sdCappedCone(float3 p, float h, float r1, float r2)
{
    float2 q = float2(length(p.xz), p.y);
    float2 k1 = float2(r2, h);
    float2 k2 = float2(r2 - r1, 2.0 * h);
    float2 ca = float2(q.x - min(q.x, (q.y < 0.0) ? r1 : r2), abs(q.y) - h);
    float2 cb = q - k1 + k2 * clamp(dot(k1 - q, k2) / dot(k2, k2), 0.0, 1.0);
    float s = (cb.x < 0.0 && ca.y < 0.0) ? -1.0 : 1.0;
    return s * sqrt(min(dot(ca, ca), dot(cb, cb)));
}

// Scene definition (Mapping all objects)
float map(float3 p)
{
    // 1. Plane SDF (Floor at Y = 0)
    float dPlane = p.y;

    // 2. Cone SDF
    float height = 2.0;
    float radius = 1.0;
    float3 posCone = p - float3(0.0, height * 0.5, 0.0); // Lift up so it sits on the plane
    float dCone = sdCappedCone(posCone, height * 0.5, radius, 0.0);

    // Return the minimum distance (Union of plane and cone)
    return min(dPlane, dCone);
}

// --- Normals ---
float3 GetNormal(float3 p)
{
    float e = 0.001;
    float3 n = float3(
        map(p + float3(e, 0, 0)) - map(p - float3(e, 0, 0)),
        map(p + float3(0, e, 0)) - map(p - float3(0, e, 0)),
        map(p + float3(0, 0, e)) - map(p - float3(0, 0, e))
    );
    return normalize(n);
}

// --- Ray Marching ---
float RayMarch(float3 ro, float3 rd)
{
    float dO = 0.0;
    for (int i = 0; i < 128; i++) // Standard iteration count
    {
        float3 p = ro + rd * dO;
        float dS = map(p);
        if (abs(dS) < 0.001 || dO > 100.0)
            break;
        dO += dS;
    }
    return dO;
}

// --- Pixel Shader ---
float4 main(VS_OUTPUT input) : SV_Target
{
    // Screen Coordinate Setup
    float2 uv = (input.uv - 0.5) * 2.0;
    uv.x *= iResolution.x / iResolution.y;
    uv.y = -uv.y;

    // Camera setup from C++ constants
    float3 ro = iCameraPos;
    float3 fwd = normalize(iCameraForward);
    float3 right = normalize(iCameraRight);
    float3 up = normalize(iCameraUp);
    
    // Ray Direction
    float3 rd = normalize(uv.x * right + uv.y * up + 2.0 * fwd);

    // GPU Hang Safety
    if (any(isnan(rd)) || any(isinf(rd)))
        return float4(0, 0, 0, 1);

    float d = RayMarch(ro, rd);

    // Default Background Color
    float3 col = float3(0.1, 0.1, 0.15);

    if (d < 100.0)
    {
        float3 p = ro + rd * d;
        float3 n = GetNormal(p);
        float3 sunDir = normalize(iSunDir);

        // Simple Lambertian Diffuse Lighting
        float diff = max(dot(n, sunDir), 0.1); // 0.1 is minimum ambient

        // Simple Coloring (Gray for floor, Orange for cone)
        float3 objCol = (p.y < 0.01) ? float3(0.2, 0.2, 0.2) : float3(1.0, 0.5, 0.2);
        
        col = objCol * diff;
    }
    
    // Simple Gamma Correction
    col = pow(col, 1.0 / 2.2);

    return float4(col, 1.0);
}