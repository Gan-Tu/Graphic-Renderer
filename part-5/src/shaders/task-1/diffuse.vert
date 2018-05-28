attribute vec3 position;
attribute vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

varying vec3 fPosition;
varying vec3 fNormal;

void main() {
	// TODO: Part 5.1
    fPosition = (modelMatrix * vec4(position, 1.0)).xyz;
    fNormal = (modelMatrix * vec4(normal, 1.0)).xyz;
    gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
}