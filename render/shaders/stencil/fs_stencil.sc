$input v_color0, v_texcoord

#include <bgfx_shader.sh>

SAMPLER2DARRAY(s_texColor, 0);

void main() {
	vec4 texColor = texture2DArray(s_texColor, vec3(v_texcoord.xy, v_texcoord.z)) * v_color0;
	if (texColor.a <= 0.0) discard;

	gl_FragColor = texColor;
}
