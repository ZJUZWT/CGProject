#version 450 core

layout(vertices = 4)out;

in vec3 fragPos[];
out vec3 fragPos_CTS_ETS[];

void main() {
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	fragPos_CTS_ETS[gl_InvocationID] = fragPos[gl_InvocationID];

	// ��������ϸ������̶�
	gl_TessLevelInner[0] = 40.0;     // �ڲ�����3����ֱ���򣬼��ڲ�����2�ж���
	gl_TessLevelInner[1] = 40.0;     // �ڲ�����4��ˮƽ���򣬼��ڲ�����3�ж���

	gl_TessLevelOuter[0] = 50.0;     // ���2���߶�
	gl_TessLevelOuter[1] = 50.0;     // �±�3���߶�
	gl_TessLevelOuter[2] = 50.0;     // �ұ�4���߶�
	gl_TessLevelOuter[3] = 50.0;     // �ϱ�5���߶�
}