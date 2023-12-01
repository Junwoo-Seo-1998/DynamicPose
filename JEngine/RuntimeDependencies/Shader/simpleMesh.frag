#version 460 core

out vec4 FragColor;
uniform vec4 Color;
uniform vec3 CamPos;

vec3 ComputeReflection(vec3 normalVector, vec3 lightVector)
{
    return 2*(dot(normalVector,lightVector))*normalVector-lightVector;
}

in VS_OUT{ 
	vec3 FragPos;
	vec3 NormalVector;
} fs_in; 

void main()
{
	vec3 NormalVector=normalize(fs_in.NormalVector);
	vec3 ViewVector=CamPos-fs_in.FragPos;
	float ViewDistance=length(ViewVector);
    ViewVector=ViewVector/ViewDistance; //normalize
	
	vec3 lightPos=vec3(150.f, 150.f, 150.f);

	vec3 lightVector=lightPos-fs_in.FragPos;
    float lightDistance=length(lightVector);
	lightVector=lightVector/lightDistance;//normalize(lightVector)

	vec3 reflection=ComputeReflection(NormalVector,lightVector);
	//
    //vec3 ambient = material.Ambient*light.Ambient;
	vec3 diffuse = vec3(Color)*max(dot(NormalVector,lightVector),0.f);
	vec3 specular = vec3(0.8f, 0.8f, 0.8f)*pow(max(dot(reflection,ViewVector),0.000001), 32);

	FragColor = vec4(diffuse+specular, 1.f);
	//ambient+
} 