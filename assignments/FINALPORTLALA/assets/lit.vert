#version 450

layout (location = 0) in vec3 v_In_Pos;
layout (location = 1) in vec3 v_In_Normal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 in_texcoord;

out vec3 out_Normal;

out Surface 
{
	vec3 worldPos;
	vec3 worldNormal;
	vec2 texcoord;
	mat3 TBN_Mat;
} vs_surface;

uniform mat4 camera_viewProj;
uniform mat4 _Model;

void main()
{
	vs_surface.worldPos = vec3(_Model * vec4(v_In_Pos, 1.0));
	vs_surface.worldNormal = transpose(inverse(mat3(_Model))) * v_In_Normal;

	//TBN matrix creation
	vec3 tangent = normalize(vec3(_Model * vec4(aTangent, 0.0)));
	vec3 normal = normalize(vec3(_Model * vec4(v_In_Normal, 0.0)));
	vec3 bitangent = normalize(cross(normal, tangent));
	vs_surface.TBN_Mat = mat3(tangent, bitangent, normal);

	vs_surface.texcoord = in_texcoord;
	gl_Position = camera_viewProj * _Model * vec4(v_In_Pos, 1.0);
}