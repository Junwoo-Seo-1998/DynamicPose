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
layout(location = 2) in vec4 boneIds; 
layout(location = 3) in vec4 weights;

uniform MatrixData Matrix;


const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];


out VS_OUT{ 
	vec3 FragPos;
	vec3 NormalVector;
} vs_out; 
		
void main()
{
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1.f) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition =vec4(aPos.x, aPos.y, aPos.z, 1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[int(boneIds[i])] * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(transpose(inverse(finalBonesMatrices[int(boneIds[i])]))) * aNormal;
        totalNormal += localNormal * weights[i];
   }
	gl_Position = Matrix.Projection*Matrix.View*Matrix.Model*totalPosition;

    vs_out.FragPos = vec3(Matrix.Model*totalPosition);
	vs_out.NormalVector=mat3(transpose(inverse(Matrix.Model))) * aNormal;
}
