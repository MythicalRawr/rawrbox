#include <vertex_bindless_uniforms.fxh>
#include <camera.fxh>

#define TRANSFORM_DISPLACEMENT
#define TRANSFORM_PSX
#define TRANSFORM_BILLBOARD
#ifdef SKINNED
    #define TRANSFORM_BONES
#endif

Texture2DArray g_Textures[];
SamplerState   g_Sampler;

#include <model_transforms.fxh>

struct VSInput {
    float3 Pos     : ATTRIB0;

    float4 UV      : ATTRIB1;
    float4 Color   : ATTRIB2;

    float3 Normal  : ATTRIB3;
    float3 Tangent : ATTRIB4;

    #ifdef SKINNED
        uint4 BoneIndex   : ATTRIB5;
        float4 BoneWeight : ATTRIB6;

        #ifdef INSTANCED
            // Instance attributes
            float4 MtrxRow0      : ATTRIB7;
            float4 MtrxRow1      : ATTRIB8;
            float4 MtrxRow2      : ATTRIB9;
            float4 MtrxRow3      : ATTRIB10;
            float4 ColorOverride : ATTRIB11;
            float4 Extra         : ATTRIB12;
        #endif
    #else
        #ifdef INSTANCED
            // Instance attributes
            float4 MtrxRow0      : ATTRIB5;
            float4 MtrxRow1      : ATTRIB6;
            float4 MtrxRow2      : ATTRIB7;
            float4 MtrxRow3      : ATTRIB8;
            float4 ColorOverride : ATTRIB9;
            float4 Extra         : ATTRIB10;
        #endif
    #endif
};

struct PSInput {
    float4 Pos                      : SV_POSITION;
    float4 WorldPos                 : POSITION1;

    float4 Normal                   : NORMAL;
    float4 Tangent                  : TANGENT;

    float2 UV                       : TEX_COORD;
    float4 Color                    : COLOR;

    nointerpolation uint   TexIndex : TEX_ARRAY_INDEX;
};


void main(in VSInput VSIn, out PSInput PSIn) {
    #ifdef SKINNED
        float4 pos = boneTransform(VSIn.BoneIndex, VSIn.BoneWeight, VSIn.Pos);
    #else
        float4 pos = float4(VSIn.Pos, 1.);
    #endif

    #ifdef INSTANCED
        float4x4 InstanceMatr = MatrixFromRows(VSIn.MtrxRow0, VSIn.MtrxRow1, VSIn.MtrxRow2, VSIn.MtrxRow3);
        TransformedData transform = applyPosTransforms(mul(pos, InstanceMatr), VSIn.UV.xy);
    #else
        TransformedData transform = applyPosTransforms(pos, VSIn.UV.xy);
    #endif

    PSIn.Normal   = mul(float4(VSIn.Normal, 0.0), Camera.world);
    PSIn.Tangent  = mul(float4(VSIn.Tangent, 0.0), Camera.world);

    PSIn.Pos      = transform.final;
    PSIn.WorldPos = mul(transform.pos, Camera.world);
    PSIn.UV       = VSIn.UV.xy; //applyUVTransform(VSIn.UV.xy);

    #ifdef INSTANCED
        PSIn.Color    = VSIn.Color * VSIn.ColorOverride * Constants.colorOverride;
        PSIn.TexIndex = VSIn.Extra.x;
    #else
        PSIn.Color    = VSIn.Color * Constants.colorOverride;
        PSIn.TexIndex = VSIn.UV.z;
    #endif
}
