#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
uniform vec4 fogcolor;
uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords) + fogcolor;
}