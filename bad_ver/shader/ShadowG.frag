#version 450 core

float near = 0.1;
float far = 1000.0;

//layout(location = 0) out vec4 fColor;
in float depth_GS_FS;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
	//gl_FragDepth = LinearizeDepth(gl_FragCoord.z) / far;
	//gl_FragDepth = LinearizeDepth(depth_GS_FS) / 200.f;
	//fColor = vec4(LinearizeDepth(gl_FragCoord.z) / far * 500);
	//fColor = vec4(gl_FragCoord.z);
}