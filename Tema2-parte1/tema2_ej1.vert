#version 150 compatibility 

in vec3 aPosition;
in vec3 aColor;
out vec3 vColor;
uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 modelMat;

void main()
{
	gl_Position = projMat * viewMat * modelMat * vec4(aPosition,1);
	vColor = aColor;
}