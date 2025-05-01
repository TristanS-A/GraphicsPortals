#version 450
layout(location = 0) out vec4 FragColor;


in Surface 
{
	vec3 worldPos;
	vec3 worldNormal;
	vec2 texcoord;
} fs_surface;

uniform sampler2D _MainTex;
uniform vec3 _ColorOffset;

void main()
{
	FragColor = vec4(texture(_MainTex, fs_surface.texcoord).rgb + _ColorOffset, 1.0); 
}