uniform sampler2D 	source;
uniform vec2 		sourceSize;

vec4 getColor(vec2 textureCoordinates, vec2 pos, vec2 pixelSize)
{
	return texture2D(source, textureCoordinates + pos * pixelSize);
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Added a method to simplify things
 *	only take blue and red colors to do bloom
 */
void main()
{
	vec2 pixelSize = vec2(1.0 / sourceSize.x, 1.0 / sourceSize.y);
	vec2 textureCoordinates = gl_TexCoord[0].xy;
	vec4 color = texture2D(source, textureCoordinates);
	color     += getColor(textureCoordinates, vec2( 1.0,  0.0), pixelSize);
	color     += getColor(textureCoordinates, vec2(-1.0,  0.0), pixelSize);
	color     += getColor(textureCoordinates, vec2( 0.0,  1.0), pixelSize);
	color     += getColor(textureCoordinates, vec2( 0.0, -1.0), pixelSize);
	color     += getColor(textureCoordinates, vec2( 1.0,  1.0), pixelSize);
	color     += getColor(textureCoordinates, vec2( 1.0, -1.0), pixelSize);
	color     += getColor(textureCoordinates, vec2(-1.0,  1.0), pixelSize);
	color     += getColor(textureCoordinates, vec2(-1.0, -1.0), pixelSize);

	vec4 redBlue = vec4(color.r, 0, color.b, color.a);

    redBlue.r = color.r - color.b - color.g;
	redBlue.b = color.b - color.r - color.g;
	redBlue.g = 0.0;

	gl_FragColor = redBlue / 2.0;
}