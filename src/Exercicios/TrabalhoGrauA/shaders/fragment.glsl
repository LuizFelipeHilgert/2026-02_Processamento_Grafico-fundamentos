#version 330 core

in vec2 texCoord;

uniform sampler2D texture1;
uniform vec4 inputColor;
uniform bool useTexture;

out vec4 color;

void main()
{
    if (useTexture)
    {
        color = texture(texture1, texCoord);
    }
    else
    {
        color = inputColor;
    }
}