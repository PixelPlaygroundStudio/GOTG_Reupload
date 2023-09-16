attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec4 a_color;

uniform mat4 u_mw;
uniform mat4 u_mvp;

varying vec3 v_world_normal;
varying vec4 v_vertex_color;

void PassUv() {
}

mat4 GetWorldMatrix() { return u_mw; }

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
}