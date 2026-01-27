/*
 * NoiseBaker.hlsl - CORRECTED & MATCHED TO SHADERTOY
 * Purpose: Generates a high-quality Perlin-Worley noise atlas.
 * Matches the multi-octave FBM logic of the reference ShaderToy code.
 * https://www.shadertoy.com/view/3sffzj
 */

RWTexture2D<float4> OutputAtlas : register(u0);

// --- Constants ---
#define SIZE 8.0f // Base frequency
static const float3 ATLAS_RES = float3(204.0f, 204.0f, 36.0f); // Width, Height, Depth (Slices)
static const float TILE_SIZE = 32.0f;
static const float TILE_ROWS = 6.0f;

// --- Helper Functions ---

float3 modulo(float3 m, float n)
{
    return (m % n + n) % n;
} // Correct positive modulo

float3 fade(float3 t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float remap(float x, float low1, float high1, float low2, float high2)
{
    return low2 + (x - low1) * (high2 - low2) / (high1 - low1);
}

float saturate(float x)
{
    return clamp(x, 0.0, 1.0);
}

// Standard Hash (Matched to Shadertoy)
float3 hash(float3 p3)
{
    p3 = modulo(p3, SIZE);
    p3 = frac(p3 * float3(0.1031, 0.1030, 0.0973));
    p3 += dot(p3, p3.yxz + 33.33);
    return 2.0 * frac((p3.xxy + p3.yxx) * p3.zyx) - 1.0;
}

// Gradient Noise (Perlin)
float gradientNoise(float3 p)
{
    float3 i = floor(p);
    float3 f = frac(p);
    float3 u = fade(f);

    return lerp(lerp(lerp(dot(hash(i + float3(0, 0, 0)), f - float3(0, 0, 0)),
                          dot(hash(i + float3(1, 0, 0)), f - float3(1, 0, 0)), u.x),
                     lerp(dot(hash(i + float3(0, 1, 0)), f - float3(0, 1, 0)),
                          dot(hash(i + float3(1, 1, 0)), f - float3(1, 1, 0)), u.x), u.y),
                lerp(lerp(dot(hash(i + float3(0, 0, 1)), f - float3(0, 0, 1)),
                          dot(hash(i + float3(1, 0, 1)), f - float3(1, 0, 1)), u.x),
                     lerp(dot(hash(i + float3(0, 1, 1)), f - float3(0, 1, 1)),
                          dot(hash(i + float3(1, 1, 1)), f - float3(1, 1, 1)), u.x), u.y), u.z);
}

// Multi-Octave Perlin Noise (FBM)
float getPerlinNoise(float3 pos, float frequency)
{
    float sum = 0.0;
    float weightSum = 0.0;
    float weight = 1.0;

    for (int oct = 0; oct < 3; oct++) // 3 Octaves
    {
        float3 p = pos * frequency;
        float val = 0.5 + 0.5 * gradientNoise(p);
        sum += val * weight;
        weightSum += weight;
        weight *= 0.5;
        frequency *= 2.0;
    }
    return saturate(sum / weightSum);
}

// Worley Noise (Cellular)
float worley(float3 pos, float numCells)
{
    float3 p = pos * numCells;
    float d = 1.0e10;
    
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            for (int z = -1; z <= 1; z++)
            {
                float3 tp = floor(p) + float3(x, y, z);
                // Tileable Worley logic
                float3 h = hash(modulo(tp, numCells));
                tp = p - tp - (0.5 + 0.5 * h);
                d = min(d, dot(tp, tp));
            }
        }
    }
    return 1.0 - saturate(d);
}

// Generates the composite noise value for a given point
// type: 0 = Perlin-Worley (Base), 1 = Worley (Detail) -- NOT USED HERE DIRECTLY
// We will bake specific combinations as per Shadertoy logic
float getCompositeNoise(float3 p, bool isPerlinWorley)
{
    float numCells = 2.0f; // Base cell density
    
    if (isPerlinWorley)
    {
        // 1. Base Perlin (Low Freq)
        float perlin = getPerlinNoise(p, SIZE);
        
        // 2. Worley FBM (High Freq details)
        float w0 = worley(p, numCells * 2.0);
        float w1 = worley(p, numCells * 8.0);
        float w2 = worley(p, numCells * 14.0);
        float worleyFBM = w0 * 0.625 + w1 * 0.25 + w2 * 0.125;
        
        // 3. Remap Perlin using Worley to create puffy shapes
        return remap(perlin, 0.0, 1.0, worleyFBM, 1.0);
    }
    else // Pure Worley FBM for Erosion
    {
        float w0 = worley(p, numCells);
        float w1 = worley(p, numCells * 2.0);
        float w2 = worley(p, numCells * 4.0);
        float w3 = worley(p, numCells * 8.0);
        
        float FBM0 = w0 * 0.625 + w1 * 0.25 + w2 * 0.125;
        float FBM1 = w1 * 0.625 + w2 * 0.25 + w3 * 0.125;
        float FBM2 = w2 * 0.75 + w3 * 0.25;
        
        return FBM0 * 0.625 + FBM1 * 0.25 + FBM2 * 0.125;
    }
}

// Coordinate Mapping (2D Atlas -> 3D Volume)
float3 get3Dfrom2D(float2 uv)
{
    // Reconstruct 3D coordinate from tiled UV
    // Logic must match exactly how Shadertoy samples the padding
    // For simplicity in baking, we calculate the exact 3D pos for this texel
    return float3(0, 0, 0); // Helper not strictly needed in compute main loop
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // 1. Calculate Tile Index and Local UV
    // Shadertoy atlas: 204x204, Tiles: 6x6, TileSize: 32+2(padding) = 34
    // We need to map the current pixel (DTid.xy) to a 3D coordinate (p)
    
    float2 pixel = (float2) DTid.xy;
    const float tileSize = 34.0f; // 32 + 2 padding
    const float coreSize = 32.0f;
    
    // Determine which tile we are in
    float2 tileIdx = floor(pixel / tileSize);
    
    // Check if we are in padding area
    float2 localPos = pixel - (tileIdx * tileSize); // 0 to 33
    
    // Handle Padding (Mirroring/Clamping logic from Shadertoy)
    // If inside padding (0 or 33), map to core edge
    bool isPadding = (localPos.x < 1.0 || localPos.x > 32.0 || localPos.y < 1.0 || localPos.y > 32.0);
    
    float2 coreUV = localPos - 1.0f; // Shift to 0..31 range
    
    // Wrap padding to opposite side for seamless tiling
    if (coreUV.x < 0)
        coreUV.x += coreSize;
    if (coreUV.y < 0)
        coreUV.y += coreSize;
    if (coreUV.x >= coreSize)
        coreUV.x -= coreSize;
    if (coreUV.y >= coreSize)
        coreUV.y -= coreSize;
    
    // Calculate normalized 3D position
    float zIndex = tileIdx.y * TILE_ROWS + tileIdx.x; // Current slice (Level L)
    
    // Current Slice (Level L)
    float3 p = float3(coreUV / coreSize, zIndex / 36.0f);
    
    // Next Slice (Level L+1) - Used for Green Channel interpolation
    float3 p_next = float3(coreUV / coreSize, (zIndex + 1.0f) / 36.0f);
    
    // --- Generate Noise ---
    
    // R Channel: Perlin-Worley at Level L
    // We combine Perlin-Worley and Worley into one value here as per 'col.r' logic in Shadertoy
    float pw_L = getCompositeNoise(p, true);
    float w_L = getCompositeNoise(p, false);
    float final_L = saturate(remap(pw_L, w_L, 1.0, 0.0, 1.0)); // The "col.r" logic
    
    // G Channel: Perlin-Worley at Level L+1 (for Z-interpolation)
    // NOTE: Shadertoy samples L+1 for the Green channel to allow single-texture lerp!
    float pw_Next = getCompositeNoise(p_next, true);
    float w_Next = getCompositeNoise(p_next, false);
    float final_Next = saturate(remap(pw_Next, w_Next, 1.0, 0.0, 1.0)); // The "col.g" logic
    
    // Write to Output
    // R: Current Slice Noise
    // G: Next Slice Noise (allows lerp(r, g, f) in pixel shader)
    // B: 0 (Unused)
    // A: 1
    OutputAtlas[DTid.xy] = float4(final_L, final_Next, 0, 1);
}