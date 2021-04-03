#version 450

#define VISUALIZATION_FINAL 0
#define VISUALIZATION_SHADOWS 1
#define VISUALIZATION_AMBIENT_OCCLUSION 2
#define VISUALIZATION_REFLECTIONS 3
#define VISUALIZATION_GLOBAL_ILLUMINATION 4
#define VISUALIZATION_REFLECTIONS_TEMPORAL_VARIANCE 5

// ------------------------------------------------------------------------
// INPUTS -----------------------------------------------------------------
// ------------------------------------------------------------------------

layout(location = 0) in vec2 inUV;

// ------------------------------------------------------------------------
// OUTPUTS ----------------------------------------------------------------
// ------------------------------------------------------------------------

layout(location = 0) out vec4 FS_OUT_Color;

// ------------------------------------------------------------------------
// DESCRIPTOR SETS --------------------------------------------------------
// ------------------------------------------------------------------------

layout(set = 0, binding = 0) uniform sampler2D samplerColor;
layout(set = 1, binding = 0) uniform sampler2D samplerShadowAO;
layout(set = 2, binding = 0) uniform sampler2D samplerReflections;
layout(set = 3, binding = 0) uniform sampler2D samplerGlobalIllumination;

// ------------------------------------------------------------------------
// PUSH CONSTANTS ---------------------------------------------------------
// ------------------------------------------------------------------------

layout(push_constant) uniform PushConstants
{
    int   visualization;
    float exposure;
}
u_PushConstants;

// ------------------------------------------------------------------------
// FUNCTIONS --------------------------------------------------------------
// ------------------------------------------------------------------------

vec3 aces_film(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
}

// ------------------------------------------------------------------------
// MAIN -------------------------------------------------------------------
// ------------------------------------------------------------------------

void main()
{
    vec3 color = vec3(0.0f);

    if (u_PushConstants.visualization == VISUALIZATION_FINAL)
    {
        color = texture(samplerColor, inUV).rgb;

        // Apply exposure
        color *= u_PushConstants.exposure;

        // HDR tonemap and gamma correct
        color = aces_film(color);
        color = pow(color, vec3(1.0 / 2.2));
    }
    else if (u_PushConstants.visualization == VISUALIZATION_SHADOWS)
        color = texture(samplerShadowAO, inUV).rrr;
    else if (u_PushConstants.visualization == VISUALIZATION_AMBIENT_OCCLUSION)
        color = texture(samplerShadowAO, inUV).ggg;
    else if (u_PushConstants.visualization == VISUALIZATION_REFLECTIONS)
    {
        color = texture(samplerReflections, inUV).rgb;

        // Apply exposure
        color *= u_PushConstants.exposure;

        // HDR tonemap and gamma correct
        color = aces_film(color);
        color = pow(color, vec3(1.0 / 2.2));
    }
    else if (u_PushConstants.visualization == VISUALIZATION_GLOBAL_ILLUMINATION)
    {
        color = texture(samplerGlobalIllumination, inUV).rgb;

        // Apply exposure
        color *= u_PushConstants.exposure;

        // HDR tonemap and gamma correct
        color = aces_film(color);
        color = pow(color, vec3(1.0 / 2.2));
    }
    else if (u_PushConstants.visualization == VISUALIZATION_REFLECTIONS_TEMPORAL_VARIANCE)
        color = texture(samplerReflections, inUV).aaa;

    FS_OUT_Color = vec4(color, 1.0);
}

// ------------------------------------------------------------------------