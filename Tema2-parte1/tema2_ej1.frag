#version 150 compatibility

in vec4 vColor;
out vec4 fFragColor;

void main()
{
    fFragColor = /*vec4(0.0, 0.0, 0.0, 1.0);*/ vColor * 0.5f;
}