precision highp float;

uniform vec3 cameraPosition;
uniform vec3 lPosition;
uniform vec3 lIntensity;

varying vec3 fPosition;
varying vec3 fNormal;

void main() {
    // TODO: Part 5.2

    vec3 illumination_from_source = lIntensity / dot(lPosition, lPosition);

    vec4 ambient = vec4(0.5 * vec4(192.0/256.0, 192.0/256.0, 192.0/256.0, 1.0));

    vec4 diffuse = vec4(0.5 * illumination_from_source * max(0.0, dot(fNormal, normalize(lIntensity))), 1.0);

    vec3 h = normalize(lPosition + cameraPosition);

    vec4 specular = vec4(0.5 * illumination_from_source * pow(max(0.0, dot(fNormal, h)), 25.0), 1.0);

    gl_FragColor = ambient + diffuse + specular;
}