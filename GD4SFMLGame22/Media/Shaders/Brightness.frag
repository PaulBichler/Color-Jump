uniform sampler2D source;

const float Threshold = 0.5;
const float Factor   = 2;

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Removed luminance
 */
void main()
{
	vec4 sourceFragment = texture2D(source, gl_TexCoord[0].xy);
	sourceFragment *= clamp(sourceFragment - Threshold, 0.0, 1.0) * Factor;
	gl_FragColor = sourceFragment;
}