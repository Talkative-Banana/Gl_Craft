#version 330 core

layout(location = 0) in uint vVertex;

uniform float side;
uniform vec3 worldpos;
uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

// uniform vec4 vColor;

out vec3 TexCoord;

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

void main() {
	uint positionX = (vVertex)        & 63u;
	uint positionY = (vVertex >> 6u)  & 63u;
	uint positionZ = (vVertex >> 12u) & 63u;
	uint centeroff = (vVertex >> 18u) & 7u;
	
	vec3 pos = vec3(side * positionX, side * positionY, side * positionZ);
	vec3 centercord = Center(pos, centeroff);
	gl_Position = vProjection * vView * vModel * vec4(pos + worldpos, 1.0);

	TexCoord = pos - centercord; //Interpolate color
}
