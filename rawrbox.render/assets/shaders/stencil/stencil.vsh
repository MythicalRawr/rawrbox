struct VSInput {
 	half2 Pos : ATTRIB1; // 2 components
    half4 Color : ATTRIB2; // 4 components
    uint TextureID : ATTRIB3; // 1 component
    half4 UV : ATTRIB4; // 4 components
};

struct PSInput {
	float4 Pos : SV_POSITION;
	float4 UV : TEX_COORD;
	float4 Color : COLOR;

	uint TextureID : TEX_ARRAY_INDEX;
};

void main(in VSInput VSIn, out PSInput PSIn) {
	PSIn.Pos = float4(VSIn.Pos, 0.0, 1.0);
	PSIn.UV = VSIn.UV;
	PSIn.Color = VSIn.Color;
	PSIn.TextureID = VSIn.TextureID;
}
