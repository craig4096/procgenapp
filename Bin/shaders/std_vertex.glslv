
varying vec3 pos;
varying vec3 viewSpacePos;
varying vec3 normal;

void main() {
	pos = gl_Vertex.xyz;
	viewSpacePos = (gl_ModelViewMatrix * gl_Vertex).xyz;
	normal = gl_Normal;
	gl_TexCoord[0].st = gl_MultiTexCoord0.st;
	gl_Position = ftransform();
}
