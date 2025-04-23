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

uniform vec3 _CullPos;
uniform vec3 _CullNormal;

void main()
{
	float projection = dot(fs_surface.worldPos, _CullNormal) / (length(_CullNormal) *  length(_CullNormal));

	if(projection > 0)
	{
		//behind the projection 
		discard;
	}
	
	
	FragColor = vec4(texture(_MainTex, fs_surface.texcoord).rgb + _ColorOffset, 1.0); 
}