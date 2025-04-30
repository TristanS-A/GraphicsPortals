#version 450
layout(location = 0) out vec4 FragColor;


in Surface 
{
	vec3 worldPos;
	vec3 worldNormal;
	vec2 texcoord;
} fs_surface;

uniform sampler2D _MainTex;
uniform sampler2D zatoon;

struct Pallet
{
	vec3 highlight;
	vec3 shadow;
};

uniform Pallet _Pallet;


uniform vec3 _LightDirection = vec3(0.0, 1.0,0.0);
uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);

vec3 toonLighting(vec3 normal, vec2 frag_pos, vec3 light_direction)
{
	float diff = (dot(normal, light_direction) + 1) * 0.5;
	vec3 light_color = vec3(1.0) * diff;

	float stepping = texture(zatoon, vec2(diff)).r;

	light_color = mix(_Pallet.shadow, _Pallet.highlight, stepping);
	return light_color * stepping;
}

void main()
{
	vec3 normal = normalize(fs_surface.worldNormal);
	vec3 lightColor = toonLighting(normal, fs_surface.texcoord, _LightDirection); 
	vec3 objectColor = texture(_MainTex, fs_surface.texcoord).rgb;

	FragColor = vec4(objectColor * lightColor, 1.0); 
}