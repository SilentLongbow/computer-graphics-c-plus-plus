#version 330

uniform sampler2D textureDots;

in vec2 TexCoord;
in float diffTerm; // Diffuse term
out vec4 outputColor;

void main()
{
	vec4 textureColour = texture(textureDots, TexCoord);

	if (textureColour.g > 0.9) discard;

	outputColor = diffTerm * textureColour;
}
