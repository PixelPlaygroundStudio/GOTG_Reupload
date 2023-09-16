#version 330 core

layout(location = 0) in vec2 a_uv;
layout(location = 1) in vec3 a_position;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec4 a_color;

out vec2 v_vertex_uv;
out vec3 v_world_normal;
out vec4 v_vertex_color;

out vec3 Normal;
out vec3 LocalPos;
out vec4 LightSpacePos;

uniform mat4 u_mvp;
uniform mat4 u_mw;

uniform mat4 u_LightMVP;

void PassUv() {
  v_vertex_uv = a_uv;
}

mat4 GetWorldMatrix() {return u_mw;}

void main() {
	mat4 world_matrix = GetWorldMatrix();
	
	vec4 vertex = vec4(a_position.xyz, 1.);
	
	gl_Position = u_mvp * world_matrix * vertex;
	
	mat3 cross_matrix = mat3(
		cross(world_matrix[1].xyz, world_matrix[2].xyz),
		cross(world_matrix[2].xyz, world_matrix[0].xyz),
		cross(world_matrix[0].xyz, world_matrix[1].xyz));
		
	float invdet = 1.0 / dot(cross_matrix[2], world_matrix[2].xyz);
	mat3 normal_matrix = cross_matrix * invdet;
	
	v_world_normal = normal_matrix * a_normal;
	
	v_vertex_color = a_color;
	
	PassUv();
	
	Normal =(world_matrix * vec4(a_normal, 0)).xyz;
	
	LocalPos = vec3(world_matrix * vec4(a_position, 1.0f));

	LightSpacePos = u_LightMVP * vec4(LocalPos, 1.0);
}