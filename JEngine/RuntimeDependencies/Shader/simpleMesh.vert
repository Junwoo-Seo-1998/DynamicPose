#version 460 core

struct MatrixData
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
    mat4 Normal;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform MatrixData Matrix;

out VS_OUT{ 
	vec3 FragPos;
	vec3 NormalVector;
} vs_out; 
		
void main()
{
    vec4 pos=Matrix.Model*vec4(aPos,1.f);
	gl_Position = Matrix.Projection*Matrix.View*pos;

    vs_out.FragPos = vec3(pos);
	vs_out.NormalVector = mat3(transpose(inverse(Matrix.Model))) * aNormal;
}
