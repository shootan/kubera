#include "Light.fx"

cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//월드 변환 행렬을 위한 쉐이더 변수를 선언한다(슬롯 1을 사용). 
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
//시스템 생성 변수로 정점 쉐이더에 전달되는 객체 인스턴스의 ID를 픽셀 쉐이더로 전달한다.
//    float4 instanceID : INDEX;
};



VS_INSTANCED_COLOR_OUTPUT VSInstancedDiffusedColor(VS_INSTANCED_COLOR_INPUT input)
{
    VS_INSTANCED_COLOR_OUTPUT output = (VS_INSTANCED_COLOR_OUTPUT)0;
    //output.position = mul(input.position , gmtxWorld);
	
//정점의 위치 벡터에 인스턴스의 위치 벡터(월드 좌표)를 더한다.
    //output.position = output.position + input.instancePos;

    output.vPos = mul(float4(input.Pos, 1), input.mtxTransform).xyz;
    output.Pos = mul(float4(output.vPos, 1.0f), gmtxView);
    output.Pos = mul(output.Pos, gmtxProjection);

	output.Norm = mul(input.Norm, (float3x3)input.mtxTransform);
	output.Tex = input.Tex;

//인스턴스 ID를 픽셀 쉐이더로 전달한다.
   // output.instanceID = instanceID;
    return output;
}


// 픽셀-쉐이더
float4 PSInstancedDiffusedColor(VS_INSTANCED_COLOR_OUTPUT input) : SV_Target
{
	input.Norm = normalize(input.Norm);
	float4 cIllumination = Lighting(input.vPos, input.Norm);
	float4 vDiffuse = g_txDiffuse.Sample( g_samLinear, input.Tex ) * cIllumination;

	return vDiffuse;


}


