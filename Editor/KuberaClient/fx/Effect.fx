
cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};


Texture2D	g_txDiffuse : register( t0 );
SamplerState g_samLinear : register( s0 );


struct VS_INPUT
{
    float4 position		: POSITION;
    float3 normal		: NORMAL;
	float2 texcoord		: TEXCOORD0;
};


struct VS_OUTPUT
{
    float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 texcoord		: TEXCOORD0;

};


VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = mul(input.position, gmtxWorld);
    output.position = mul(output.position, gmtxView);
    output.position = mul(output.position, gmtxProjection);

	output.normal = mul(input.normal, (float3x3)gmtxWorld);

	output.texcoord = input.texcoord;

    return output;
}

// 픽셀-쉐이더
float4 PS( VS_OUTPUT output) : SV_Target
{
    float4 vDiffuse = g_txDiffuse.Sample( g_samLinear, output.texcoord );

	return vDiffuse;
//입력되는 정점의 색상을 그대로 출력한다. 
}


