
#version 450
// Tells shaderc that this is a fragment shader, so the type can be infered.
#pragma shader_stage(fragment)

layout(set = 0, binding = 0) uniform GlobalData
{
    mat4 cameraView;
    mat4 cameraProjection;
    vec4 cameraPosition;

    vec4 lightSourcePosition;
    vec4 lightSourceColor;

    vec4 phongParameters;
} globalData;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragPosition;

layout(location = 0) out vec4 outColor;

void main()
{
    bool usePhongReflectionModel = bool(globalData.phongParameters.x);
    float phongAmbient = globalData.phongParameters.y;
    float phongShininess = globalData.phongParameters.z;

    if (usePhongReflectionModel == true)
    {
        // Phong reflection model.

        // Vector from a fragment to the light source.
        vec3 lightDirection = normalize(globalData.lightSourcePosition.xyz - fragPosition);

        // Non-standard adjustment.
        // Transparent triangle's normal will always be flipped towards the side with the light.
        // This ensures that even if we look through a transparent sphere at the light source
        // for example, the triangles at both sides will be lit.
        vec3 adjustedNormal = fragNormal;
        // Even if 1.0 is used everywhere, the value here might not be exactly 1.0. 
        if (fragColor.a < 0.9999f && dot(fragNormal, lightDirection) < 0.0)
        {
            adjustedNormal = -fragNormal;
        }

        // Vector from a fragment to the camera position.
        vec3 viewDirection = normalize(globalData.cameraPosition.xyz - fragPosition);
        
        // Vector from a fragment to a reflected light position along a normal.
        // reflect() output vector "starts" where input vector "ends". So the input vector must be flipped.
        vec3 reflectionDirection = reflect(-lightDirection, adjustedNormal);

        // Diffuse component.
        // Represented by a cosine of the angle between light direction and fragment normal.
        // If the angle is bigger than 90 degrees, there is no diffused light.
        float diffuse = max(dot(normalize(adjustedNormal), lightDirection), 0.0);

        // Specular component.
        // Can only happen if the camera is on the same side as the triangle normal. Specular calculation itself does not gurantee this.
        // Represented by a cosine of the angle between view direction and light reflection.
        // This is then amplified exponentialy based on the surface's shininess.
        float specular = diffuse == 0.0 ? 0.0 : pow(max(dot(viewDirection, reflectionDirection), 0.0), phongShininess);

        // Ambient component.
        float ambient = phongAmbient;
        
        // Light color is multiplied by all the components added together.
        // This determines the total ilumination of a specific fragment.
        vec3 lighting = globalData.lightSourceColor.rgb*(ambient + diffuse + specular);

        // Final mixing with the color of the fragment.
        outColor = vec4(fragColor.rgb*lighting, fragColor.a);
    }
    else
    {
        outColor = fragColor;
    }
}
