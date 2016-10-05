#version 330

uniform vec3 CameraPosition;
uniform vec3 CameraTarget;
uniform float CameraZoom;
uniform int RaymarchingType;

in vec2 FragPosition;
out vec4 OutColor;


float distanceFunction_Sphere( vec3 pos )
{
	return length( pos ) - 1.0f;
}

float distanceFunction_Mandelbulb( vec3 pos )
{
	vec3 zn		= vec3( pos.xyz );
	float rad	= 0.0;
	float dist	= 0.0;
	float d		= 1.0;
	for( int i = 0; i < 10; ++i ) {
		rad = length( zn );
		
		if( rad > 2.0 ) {
			dist = 0.5 * log( rad) * rad / d;
		}
		else {
			float th	= atan( length( zn.xy ), zn.z );
			float phi	= atan( zn.y, zn.x );		
			float rado	= pow( rad, 8.0 );
			d			= pow( rad, 7.0 ) * 7.0 * d + 1.0;
			float sint	= sin( th * 8.0 );
			
			zn.x	 = rado * sint * cos( phi * 8.0 );
			zn.y	 = rado * sint * sin( phi * 8.0 );
			zn.z	 = rado * cos( th * 8.0 ) ;
			zn		+= pos;
		}
	}
	return dist;
}

float distanceFunction_Box( vec3 pos )
{
	vec3 d = abs( pos ) - vec3( 1.0 );
	return min( max( d.x, max( d.y, d.z ) ), 0.0 ) + length( max( d, 0.0 ) );
}

float distanceFunction_MengerSponge( vec3 pos )
{
	float d = distanceFunction_Box( pos );
	float s = 1.0;
	vec3 a, r;
	float da, db, dc, c;
	for( int i = 0; i < 4; ++i ) {
		a	 = mod( pos * s, 2.0 ) - 1.0;
		s	*= 3.0;
		r	 = abs( 1.0 - 3.0 * abs( a ) );
		da	 = max( r.x, r.y );
		db	 = max( r.y, r.z );
		dc	 = max( r.z, r.x );
		c	 = ( min( da, min( db, dc ) ) - 1.0 ) / s;
		d	 = max( d, c );
	}
	return d;
}

const vec3 va = vec3(  0.0,	 0.57735,	 0.0 );
const vec3 vb = vec3(  0.0,	-1.0,		 1.15470 );
const vec3 vc = vec3(  1.0,	-1.0,		-0.57735 );
const vec3 vd = vec3( -1.0,	-1.0,		-0.57735 );
float distanceFunction_SierpinskiGasket( vec3 pos )
{
	vec3 zn	= vec3( pos.xyz );
	float r	= 1.0;
	float dm;
	vec3 v;
	float d;
	for( int i = 0; i < 8; ++i ) {
		d	 = dot( zn - va, zn - va );				   v = va; dm = d;
		d	 = dot( zn - vb, zn - vb );	if( d < dm ) { v = vb; dm = d; }
		d	 = dot( zn - vc, zn - vc );	if( d < dm ) { v = vc; dm = d; }
		d	 = dot( zn - vd, zn - vd );	if( d < dm ) { v = vd; dm = d; }
		zn	 = v + 2.0 * ( zn - v );
		r	*= 2.0;
	}
	return ( sqrt( dm ) - 1.0 ) / r;
}

float distanceFunction( vec3 pos )
{
	if( RaymarchingType == 1 )		return distanceFunction_Mandelbulb( pos );
	else if( RaymarchingType == 2 )	return distanceFunction_MengerSponge( pos );
	else if( RaymarchingType == 3 )	return distanceFunction_SierpinskiGasket( pos );
	
	return distanceFunction_Sphere( pos );
}

vec3 getNormal( vec3 pos )
{
	float d = 0.002;
	return normalize( vec3( 
		distanceFunction( pos + vec3(   d, 0.0, 0.0 ) ) - distanceFunction( pos + vec3(  -d, 0.0, 0.0 ) ),
		distanceFunction( pos + vec3( 0.0,   d, 0.0 ) ) - distanceFunction( pos + vec3( 0.0,  -d, 0.0 ) ),
		distanceFunction( pos + vec3( 0.0, 0.0,   d ) ) - distanceFunction( pos + vec3( 0.0, 0.0,  -d ) )
	));
}

vec3 getBackgroundColor( void )
{
#if 1
	// 画面中心から離れるほど暗くする
	vec3 retColor = vec3( 0.30, 0.30, 0.60 ) - 0.15 * ( FragPosition.x * FragPosition.x + FragPosition.y * FragPosition.y );
#else
	// 画面上から下に行くほど明るくする
	float rate = clamp( FragPosition.y + 1.3, 0.0, 1.0 );
	vec3 retColor = mix( vec3( 1.0, 1.0, 1.0 ), vec3( 0.15, 0.15, 0.45 ), rate );
#endif
	return retColor;
}

void main( void )
{
	vec2 pos = FragPosition;
	
	vec3 camDir			= normalize( CameraTarget - CameraPosition );
	vec3 camSide		= normalize( cross( camDir, vec3( 0.0, 1.0,  0.0 ) ) );
	vec3 camUp			= normalize( cross( camSide, camDir ) );
	float targetDepth	= 2.5 + CameraZoom;
	
	vec3 ray = normalize( camSide * pos.x + camUp * pos.y + camDir * targetDepth );
	
	float distance	= 0.0;
	float rayLen	= 0.0;
	vec3  rayPos	= CameraPosition;
	for( int i = 0; i < 128; ++i ) {
		distance	 = distanceFunction( rayPos );
		rayLen		+= distance;
		rayPos		 = CameraPosition + ray * rayLen;
		
		if( abs( distance ) < 0.001 ) {
			break;
		}
	}
	
	OutColor = vec4( getBackgroundColor(), 1.0 );	// 背景色
	if( abs( distance ) < 0.001 ) {
		vec3 lightDir	 = vec3( -0.577, 0.577, 0.577 );
		vec3 normal		 = getNormal( rayPos );
		float diff		 = clamp( dot( lightDir, normal ), 0.0, 1.0 );
		vec3 ambi		 = vec3( 0.2, 0.2, 0.2 );
		vec3 color		 = diff * vec3( 0.5, 0.5 + 0.5 * normal.y, 0.2 );
		color			+= ambi;
		OutColor		 = vec4( color, 1.0 );
	}
}
