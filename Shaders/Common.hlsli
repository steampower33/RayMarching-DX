cbuffer SceneData : register(b0)
{
    // Group 1: Time and Resolution
    float Time; // Total elapsed time
    float2 Resolution; // Screen dimensions
    float StepSize; // Raymarching step delta

    // Group 2: Camera State (Packed)
    float3 CameraPos; // World space camera position
    float CloudScale; // Global noise scale for clouds

    float3 CameraForward; // Normalized view forward vector
    float CloudThreshold; // Cloud density cutoff (Coverage)

    float3 CameraRight; // Normalized view right vector
    float Absorption; // Light absorption coefficient

    float3 CameraUp; // Normalized view up vector
    float FogDensity; // Atmospheric fog intensity

    // Group 3: Lighting and Fog (Packed)
    float3 SunDir; // Normalized direction to the sun
    float Padding0; // Explicit padding for 16-byte alignment

    float3 SunColor; // Color and intensity of sunlight
    float Padding1; // Explicit padding

    float3 FogColor; // Distant atmospheric color
    float Padding2; // Explicit padding
};