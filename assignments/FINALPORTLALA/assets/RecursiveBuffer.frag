#version 450
layout(location = 0) out vec4 FragColor;


in Surface 
{
	vec3 worldPos;
	vec3 worldNormal;
	vec2 texcoord;
	vec4 screenPos;
} fs_surface;

uniform sampler2D _MainTex;

void main()
{
	
	vec3 projectionCoords = fs_surface.screenPos.xyz / fs_surface.screenPos.w;
	projectionCoords = (projectionCoords * 0.5) + 0.5;
	FragColor = vec4(texture(_MainTex,projectionCoords.xy).rgb, 1.0); 
}