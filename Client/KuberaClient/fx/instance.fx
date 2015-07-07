#include "Light.fx"

cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//���� ��ȯ ����� ���� ���̴� ������ �����Ѵ�(���� 1�� ���). 
cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};

Texture2D	g_txDiffuse : register( t0 );
SamplerState g_samLinear : register( s0 );


struct VS_INSTANCED_COLOR_INPUT
{
    //float4 position : POSITION;
	//float3 normal		: NORMAL;
	//float2 texcoord		: TEXCOORD0;
	float3 Pos                      : POSITION;             //Position
	float3 Norm                     : NORMAL;               //Normal
	float2 Tex                      : TEXCOORD;             //Texture coordinate
	float3 Tan                      : TANGENT;              //Normalized Tangent vector
	float3 Binorm				   : BINORMAL;
	uint4 Bones                    : BONES;                //Bone indices
    float4 Weights                  : WEIGHTS;              //Bone weights
	row_major float4x4 mtxTransform : POSINSTANCE;
};

struct VS_INSTANCED_COLOR_OUTPUT
{
    //float4 position : SV_POSITION;
	//float3 normal		: NORMAL;
	//float2 texcoord		: TEXCOORD0;
	float4 Pos                      : SV_Position;          //Position
    float3 vPos                     : POSWORLD;             //world space Pos
    float3 Norm                     : NORMAL;               //Normal
    float2 Tex                      : TEXCOORD;             //Texture coordinate
    float3 Tangent                  : TANGENT;              //Normalized Tangent vector
//�ý��� ���� ������ ���� ���̴��� ���޵Ǵ� ��ü �ν��Ͻ��� ID�� �ȼ� ���̴��� �����Ѵ�.
//    float4 instanceID : INDEX;
};



VS_INSTANCED_COLOR_OUTPUT VSInstancedDiffusedColor(VS_INSTANCED_COLOR_INPUT input)
{
    VS_INSTANCED_COLOR_OUTPUT output = (VS_INSTANCED_COLOR_OUTPUT)0;
    //output.position = mul(input.position , gmtxWorld);
	
//������ ��ġ ���Ϳ� �ν��Ͻ��� ��ġ ����(���� ��ǥ)�� ���Ѵ�.
    //output.position = output.position + input.instancePos;

    output.vPos = mul(float4(input.Pos, 1), input.mtxTransform).xyz;
    output.Pos = mul(float4(output.vPos, 1.0f), gmtxView);
    output.Pos = mul(output.Pos, gmtxProjection);

	output.Norm = mul(input.Norm, (float3x3)input.mtxTransform);
	output.Tex = input.Tex;

//�ν��Ͻ� ID�� �ȼ� ���̴��� �����Ѵ�.
   // output.instanceID = instanceID;
    return output;
}


// �ȼ�-���̴�
float4 PSInstancedDiffusedColor(VS_INSTANCED_COLOR_OUTPUT input) : SV_Target
{
	input.Norm = normalize(input.Norm);
	float4 cIllumination = Lighting(input.vPos, input.Norm);
	float4 vDiffuse = g_txDiffuse.Sample( g_samLinear, input.Tex ) * cIllumination;

	return vDiffuse;


}


