#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 uv;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
	mat4 transform;
	mat4 normalMatrix;
	uint hasLighting;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT = 0.05;

void main() {
	gl_Position = push.transform * vec4(position, 1.0);

	if (push.hasLighting != 0) {
		vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

		float lightIntensity = AMBIENT + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);

		fragColor = lightIntensity * color;
	}
	else {
		fragColor = color;
	}
}