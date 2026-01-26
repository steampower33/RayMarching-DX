// --- Procedural Noise Functions ---

// Simple hash for pseudo-random value generation
float hash(float3 p)
{
    p = frac(p * 0.3183099 + 0.1);
    p *= 17.0;
    return frac(p.x * p.y * p.z * (p.x + p.y + p.z));
}

// Standard 3D Value Noise
float noise(float3 p)
{
    float3 i = floor(p);
    float3 f = frac(p);
    f = f * f * (3.0 - 2.0 * f);

    return lerp(
        lerp(lerp(hash(i + float3(0, 0, 0)), hash(i + float3(1, 0, 0)), f.x),
             lerp(hash(i + float3(0, 1, 0)), hash(i + float3(1, 1, 0)), f.x), f.y),
        lerp(lerp(hash(i + float3(0, 0, 1)), hash(i + float3(1, 0, 1)), f.x),
             lerp(hash(i + float3(0, 1, 1)), hash(i + float3(1, 1, 1)), f.x), f.y), f.z);
}

// Fractal Brownian Motion for layered cloud details
float fbm(float3 p)
{
    float v = 0.0;
    float amp = 0.5;
    for (int i = 0; i < 5; i++)
    {
        v += noise(p) * amp;
        p *= 2.0;
        amp *= 0.5;
    }
    return v;
}