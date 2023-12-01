#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
uniform mat4 ViewProjection;
uniform mat4 Model;
void main()
{
	gl_Position = ViewProjection*Model*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
