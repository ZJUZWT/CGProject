#version 450 core

layout(location = 0) in vec4 position;

out vec3 worldPos;
out vec3 normal;

uniform mat4 V;
uniform mat4 P;
uniform float time;

vec3 CalculateWavePosition(float q, float a, float w, vec3 dir, vec3 meshVert, float ph, float t) {
	float qa = q * a;
	float theta = dot(w * dir.xz, meshVert.xz) + ph * t ;
	float cosTh = cos(theta);
	float sinTh = sin(theta);
	float x = qa * dir.x * cosTh;
	float z = qa * dir.z * cosTh;
	float y = a * sinTh;
	return vec3(x, y, z);
}

vec3 CalculateWaveNormal(float q, float a, float w, vec3 dir, vec3 waveVert, float ph, float t) {
	float wa = w * a;
	float theta = dot(w * dir, waveVert) + ph * t;
	float cosTh = cos(theta);
	float sinTh = sin(theta);
	float x = dir.x * wa * cosTh;
	float z = dir.z * wa * cosTh;
	float y = q * wa * sinTh;
	return vec3(x, y, z);
}

float getLinearDepthOfViewCoord(vec3 viewCoord) {
	vec4 p = vec4(viewCoord, 1.0);
	p = P * p;
	p /= p.w;
	return p.z;//linearizeDepth(p.z) / far;
}
out float depth;

void main() {
	float l0 = 31.25, a0 = 0.16, s0 = 2.56;
	vec3 dir0 = vec3(0.58, 0.0, 0.42);
	float w0 = 2.0 / l0, ph0 = s0 * w0, q0 = 1.28;

	float l1 = 25.0, a1 = 0.22, s1 = 5.12;
	vec3 dir1 = vec3(0.31, 0.0, 0.69);
	float w1 = 2.0 / l1, ph1 = s1 * w1, q1 = 2.56;

	float l2 = 25.6, a2 = 0.22, s2 = 1.28;
	vec3 dir2 = vec3(0.33, 0.0, 0.67);
	float w2 = 2.0 / l2, ph2 = s2 * w2, q2 = 2.56;

	float l3 = 52.5, a3 = 0.34, s3 = 0.64;
	vec3 dir3 = vec3(0.26, 0.0, 0.74);
	float w3 = 2.0 / l3, ph3 = s3 * w3, q3 = 5.12;

	float l4 = 25.6, a4 = 0.34, s4 = 2.56;
	vec3 dir4 = vec3(0.3, 0.0, -0.7);
	float w4 = 2.0 / l4, ph4 = s4 * w4, q4 = 5.12;

	float l5 = 42.5, a5 = 0.46, s5 = 5.12;
	vec3 dir5 = vec3(0.4, 0.0, -0.6);
	float w5 = 2.0 / l5, ph5 = s5 * w5, q5 = 2.56;

	float l6 = 25.0, a6 = 0.22, s6 = 1.28;
	vec3 dir6 = vec3(0.1, 0.0, -0.9);
	float w6 = 2.0 / l6, ph6 = s6 * w6, q6 = 2.56;

	float l7 = 31.25, a7 = 0.16, s7 = 0.64;
	vec3 dir7 = vec3(0.43, 0.0, -0.57);
	float w7 = 2.0 / l7, ph7 = s7 * w7, q7 = 1.28;

	worldPos = position.xyz;
	float heightCof = 1.7;
	float wCof = 1.4;
	vec3 pos0 = CalculateWavePosition(q0, a0 * heightCof, w0 * wCof, dir0, worldPos, ph0, time);
	vec3 pos1 = CalculateWavePosition(q1, a1 * heightCof, w1 * wCof, dir1, worldPos, ph1, time);
	vec3 pos2 = CalculateWavePosition(q2, a2 * heightCof, w2 * wCof, dir2, worldPos, ph2, time);
	vec3 pos3 = CalculateWavePosition(q3, a3 * heightCof, w3 * wCof, dir3, worldPos, ph3, time);
	vec3 pos4 = CalculateWavePosition(q4, a4 * heightCof, w4 * wCof, dir4, worldPos, ph4, time);
	vec3 pos5 = CalculateWavePosition(q5, a5 * heightCof, w5 * wCof, dir5, worldPos, ph5, time);
	vec3 pos6 = CalculateWavePosition(q6, a6 * heightCof, w6 * wCof, dir6, worldPos, ph6, time);
	vec3 pos7 = CalculateWavePosition(q7, a7 * heightCof, w7 * wCof, dir7, worldPos, ph7, time);

	vec3 new_position = pos0 + pos1 + pos2 + pos3 + pos4 + pos5 + pos6 + pos7;
	new_position.xyz += worldPos.xyz;

	vec3 nor0 = CalculateWaveNormal(q0, a0 * heightCof, w0 * wCof, dir0, new_position, ph0, time);
	vec3 nor1 = CalculateWaveNormal(q1, a1 * heightCof, w1 * wCof, dir1, new_position, ph1, time);
	vec3 nor2 = CalculateWaveNormal(q2, a2 * heightCof, w2 * wCof, dir2, new_position, ph2, time);
	vec3 nor3 = CalculateWaveNormal(q3, a3 * heightCof, w3 * wCof, dir3, new_position, ph3, time);
	vec3 nor4 = CalculateWaveNormal(q4, a4 * heightCof, w4 * wCof, dir4, new_position, ph4, time);
	vec3 nor5 = CalculateWaveNormal(q5, a5 * heightCof, w5 * wCof, dir5, new_position, ph5, time);
	vec3 nor6 = CalculateWaveNormal(q6, a6 * heightCof, w6 * wCof, dir6, new_position, ph6, time);
	vec3 nor7 = CalculateWaveNormal(q7, a7 * heightCof, w7 * wCof, dir7, new_position, ph7, time);

	normal = nor0 + nor1 + nor2 + nor3 + nor4 + nor5 + nor6 + nor7;
	normal = normalize(vec3(0.0, 1.0, 0.0) - normal);
	//lightSpacePos = lightSpaceMatrix * new_position;
	//normal = (normalize(normal)+1)/2;
	worldPos = new_position.xyz;

	//debug
	//normal = vec3(0.0f, 1.0f, 0.0f);
	//gl_Position = P * V * vec4(worldPos, 1.0f);

	gl_Position = P * V * vec4(new_position, 1.0f);

	depth = getLinearDepthOfViewCoord((V * vec4(worldPos, 1.0f)).xyz);
}