cbuffer SceneData : register(b0)
{
    float iTime;
    float2 iResolution;
    float iStepSize;

    float3 iCameraPos;
    float iCloudScale;

    float3 iCameraForward;
    float iCloudThreshold;

    float3 iCameraRight;
    float iAbsorption;

    float3 iCameraUp;
    float iFogDensity;

    float3 iSunDir;
    float padding1;

    float3 iSunColor;
    float padding2;

    float3 iFogColor;
    float padding3;
};