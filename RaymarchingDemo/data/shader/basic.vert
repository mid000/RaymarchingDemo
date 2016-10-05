#version 330

uniform vec2 Resolution;
in vec3 Position;
out vec2 FragPosition;

void main( void )
{
	FragPosition = ( Position.xy * Resolution.xy ) / min( Resolution.x, Resolution.y );
	gl_Position = vec4( Position.xyz, 1.0 );
};
