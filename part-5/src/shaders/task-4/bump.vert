attribute vec3 position;
attribute vec3 normal;
attribute vec3 tangent;
attribute vec2 uv;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelMatrix;
uniform sampler2D textureDisplacement;
uniform vec2 textureDimension;

varying vec3 fPosition;
varying vec3 fNormal;

void main() {
    vec3 offset = position;
    vec3 b = cross(normal, tangent);

    
    mat3 tbh = mat3(
        tangent.x, tangent.y, tangent.z,
        b.x, b.y, b.z,
        normal.x, normal.y, normal.z
    );

    float heightScaling = 0.8;
    float normalScaling = 1.0;

    float w = textureDimension.y;
    float h = textureDimension.x;
    float u = uv.x;
    float v = uv.y;

    vec4 huv = texture2D(textureDisplacement, uv);
    vec4 duh = (texture2D(textureDisplacement, vec2(u + 1.0/w, v)) - huv) * heightScaling * normalScaling;
    vec4 dvh = (texture2D(textureDisplacement, vec2(u, v + 1.0/h)) - huv) * heightScaling * normalScaling;

    float du = duh.r;
    float dv = dvh.r;
    vec3  n0 = vec3(-du, -dv, 1.0);

    fPosition = (modelMatrix * vec4(position, 1.0)).xyz;
    fNormal = (modelMatrix * vec4(tbh * n0, 1.0)).xyz;

    gl_Position = projectionMatrix * modelViewMatrix * vec4(offset, 1.0);

}