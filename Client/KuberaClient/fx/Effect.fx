#include "Light.fx"



cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};


//¿œπ›
struct VS_INPUT
{
    float3 position		: POSITION;
	float4 color			: COLOR;
};


struct VS_OUTPUT
{
    float4 position		: SV_POSITION;
	float4 color			: COLOR;

};


VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = mul(float4(input.position,1), gmtxWorld);
    output.position = mul(output.position, gmtxView);
    output.position = mul(output.position, gmtxProjection);

	output.color = input.color;

	//output.normal = mul(input.normal, (float3x3)gmtxWorld);
	//output.texcoord = input.texcoord;

    return output;
}

// «»ºø-Ω¶¿Ã¥ı
float4 PS( VS_OUTPUT output) : SV_Target
{
	return output.color;
}


