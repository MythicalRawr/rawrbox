vec3 a_position         : POSITION;
vec2 a_texcoord0        : TEXCOORD0;
vec4 a_color0           : COLOR0;
vec3 a_normal           : NORMAL;
vec3 a_tangent          : TANGENT;
vec3 a_bitangent        : BITANGENT;
ivec4 a_indices         : BLENDINDICES0;
vec4 a_weight           : BLENDWEIGHT0;

vec2  v_texcoord0       : TEXCOORD0     = vec2(0.0, 0.0);
vec3  v_view            : TEXCOORD1     = vec3(0.0, 0.0, 0.0);
vec3  v_normal          : NORMAL        = vec3(0.0, 0.0, 1.0);
vec3  v_tangent         : TANGENT       = vec3(1.0, 0.0, 0.0);
vec3  v_bitangent       : BINORMAL      = vec3(0.0, 1.0, 0.0);
vec4  v_color0    	    : COLOR0        = vec4(1.0, 0.0, 0.0, 0.0);

