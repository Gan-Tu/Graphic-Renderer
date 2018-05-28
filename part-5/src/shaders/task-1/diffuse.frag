precision highp float;

uniform vec3 lPosition;
uniform vec3 lIntensity;

varying vec3 fPosition;
varying vec3 fNormal;

void main() {
    // TODO: Part 5.1
    vec3 illumination_from_source = lIntensity / dot(lPosition, lPosition);
    gl_FragColor = vec4(illumination_from_source * max(0.0, dot(fNormal, normalize(lIntensity))), 1.0);
}