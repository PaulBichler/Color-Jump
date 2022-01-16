uniform sampler2D 	source;
uniform vec2 		offsetFactor;

vec4 getColor(float pos, float multiplyer, vec2 textureCoordinates)
{
	return texture2D(source, textureCoordinates + pos * offsetFactor) * multiplyer;
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Added a method to simplify things
 */
void main()
{
	vec2 textureCoordinates = gl_TexCoord[0].xy;
	vec4 color = vec4(0.0);
	color += getColor(-4.0, 0.0162162162, textureCoordinates);
	color += getColor(-3.0, 0.0540540541, textureCoordinates);
	color += getColor(-2.0, 0.1216216216, textureCoordinates);
	color += getColor(-1.0, 0.1945945946, textureCoordinates);
	color += getColor(0.0, 0.2270270270, textureCoordinates);
	color += getColor(1.0, 0.1945945946, textureCoordinates);
	color += getColor(2.0, 0.1216216216, textureCoordinates);
	color += getColor(3.0, 0.0540540541, textureCoordinates);
	color += getColor(4.0, 0.0162162162, textureCoordinates);
	gl_FragColor = color;
}