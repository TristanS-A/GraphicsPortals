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
uniform vec2 resolution;
uniform float _Time;

//noise -> https://discourse.threejs.org/t/help-with-portal-shader-border-from-shadertoy/56448

float snoise(vec3 uv, float res)
{
//maybe precalculate this?
	const vec3 s = vec3(10, 1 * pow(10,2), 1 * pow(10,3));
	
	uv *= res;
	
	vec3 uv0 = floor(mod(uv, res))*s;
	vec3 uv1 = floor(mod(uv+vec3(1.), res))*s;
	
	vec3 f = fract(uv); f = f*f*(3.0-2.0*f);

	vec4 v = vec4(uv0.x+uv0.y+uv0.z, uv1.x+uv0.y+uv0.z,
		      	  uv0.x+uv1.y+uv0.z, uv1.x+uv1.y+uv0.z);

	vec4 r = fract(sin(v*1e-1)*1e3);
	float r0 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	r = fract(sin((v + uv1.z - uv0.z)*1e-1)*1e3);
	float r1 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	return mix(r0, r1, f.z)*2.-1.;
	return 0.0;
}
void main()
{
	//Perspective devide -> normalized device coords
	vec3 projectionCoords = fs_surface.screenPos.xyz / fs_surface.screenPos.w;

	//Map to 0-1
	projectionCoords = (projectionCoords * 0.5) + 0.5;

	//portal effect code --> https://discourse.threejs.org/t/help-with-portal-shader-border-from-shadertoy/56448

	//changes the UV points --> these points are not in the correct scale
	vec2 p = -0.42 + 0.84 * fs_surface.texcoord;

	float color = 3.0 - (3.0 * length(2.*p));

	vec3 coord = vec3(atan(p.x,p.y)/6.2832 + 0.5, length(p) * .4, 0.5); 

	coord = 1.0 - coord; //converts range;

	for(int i = 0; i <= 2; i++)
	{
		float power = pow(2.0, float(i));
		color += (0.4 / power) * snoise(coord + vec3(0.,-_Time*.05, _Time*.01), power*16.);
	}

	//invert color
	color = 1 - color;
	color *= 2.7; 
	color *= smoothstep(0.43, 0.4, length(p));

	float pct = distance(fs_surface.texcoord, vec2(0.5));

	float y = smoothstep(0.16,0.525, pct);

	//FragColor = vec4(texture(_MainTex, projectionCoords.xy).rgb + vec3(0.2, 0, 0), 1.0); 

	FragColor = vec4(pow(max(color,0.),3.)*0.15, pow(max(color,0.),2.)*0.4, color, 0);

}