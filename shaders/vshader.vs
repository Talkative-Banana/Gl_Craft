#version 330 core

layout(location = 0) in uint vVertex;

uniform float side;
uniform vec3 chunkpos;
uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

// uniform vec4 vColor;

out vec2 tile;
out float isoscale;
out vec3 NormalDir;
out vec2 TexCoord; 

vec3 Center(vec3 pos, uint centeroff){
	vec3 center = pos;

	if((centeroff & 1u) == 1u){
		center.z += side / 2;
	} else{
		center.z -= side / 2;
	}

	if((centeroff & 2u) == 2u){
		center.y += side / 2;
	} else{
		center.y -= side / 2;
	}

	if((centeroff & 4u) == 4u){
		center.x += side / 2;
	} else{
		center.x -= side / 2;
	}

	return center;
}

ivec3 Normal(uint normaldir){
	if(normaldir == 0u){
		return ivec3(0, 0, -1);
	}

	if(normaldir == 1u){
		return ivec3(0, 0, 1);
	}

	if(normaldir == 2u){
		return ivec3(-1, 0, 0);
	}

	if(normaldir == 3u){
		return ivec3(1, 0, 0);
	}

	if(normaldir == 4u){
		return ivec3(0, 1, 0);
	}

	if(normaldir == 5u){
		return ivec3(0, -1, 0);
	}
}

void setup_textures(ivec3 _NormalDir, uint vVertex, uint centeroff){
	if (_NormalDir.z != 0) {
		// Z face → use (x,y)
		tile.x = 3;
		tile.y = 0;
		TexCoord = vec2((centeroff & 4u) >> 2,   // X bit
                    (centeroff & 2u) >> 1);  // Y bit
	}
	else if (_NormalDir.y != 0) {
		// Y face → use (x,z)
		tile.x = _NormalDir.y < 0 ? 2 : 0;
		tile.y = 0;
		TexCoord = vec2((centeroff & 4u) >> 2,   // X bit
                    (centeroff & 1u));       // Z bit
	}
	else {
		// X face → use (z,y)
		tile.x = 3;
		tile.y = 0;
		TexCoord = vec2((centeroff & 1u),        // Z bit
                    (centeroff & 2u) >> 1);  // Y bit
	}
}

void main() {
	uint positionX = (vVertex)        & 63u;
	uint positionY = (vVertex >> 6u)  & 63u;
	uint positionZ = (vVertex >> 12u) & 63u;
	uint centeroff = (vVertex >> 18u) & 7u;
	uint normaldir = (vVertex >> 21u) & 7u;
	
	vec3 pos = vec3(side * positionX, side * positionY, side * positionZ);
	vec3 centercord = Center(pos, centeroff);
	gl_Position = vProjection * vView * vModel * vec4(pos + chunkpos, 1.0);

	vec3 texcoord = pos - centercord; 
	ivec3 _NormalDir = Normal(normaldir);

	NormalDir = _NormalDir;

	setup_textures(_NormalDir, vVertex, centeroff);
	// figure out which 2D coords to use based on face
	if (_NormalDir.z != 0) {
		isoscale = 0.980;
	}
	else if (_NormalDir.y != 0) {
		isoscale = 0.800;
	}
	else {
		isoscale = 0.608;
	}
}
