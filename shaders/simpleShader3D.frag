#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D image;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
	uint hasLighting;
	uint hasTexture;
} push;

void main() {
	if (push.hasTexture != 0) {
		vec3 imageColor = texture(image, fragUV).rgb;
		outColor = vec4(fragColor * imageColor, 1.0);
	}
	else {
		outColor = vec4(fragColor, 1.0);
	}
}