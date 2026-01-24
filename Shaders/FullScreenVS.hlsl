struct VS_INPUT
{
    float3 pos : POSITION;
    float4 col : COLOR;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = float4(input.pos, 1.0);
    output.uv = input.pos.xy * 0.5 + 0.5;
    output.uv.y = 1.0 - output.uv.y; // Flip Y for DirectX
    return output;
}