#version 450
layout(location = 0) out vec4 FragColor;


in Surface 
{
	vec3 worldPos;
	vec4 screenPos;
	vec3 worldNormal;
	vec2 texcoord;
} fs_surface;

uniform sampler2D _MainTex;

void main()
{
	//Perspective devide -> normalized device coords
	vec3 projectionCoords = fs_surface.screenPos.xyz / fs_surface.screenPos.w;

	//Map to 0-1
	projectionCoords = (projectionCoords * 0.5) + 0.5;

	FragColor = vec4(texture(_MainTex, projectionCoords.xy).rgb , 1.0); 
}