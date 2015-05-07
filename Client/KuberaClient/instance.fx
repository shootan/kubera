
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

cbuffer cbClickTarget : register (b2)
{
    float4 m_vSelected ;
};

Texture2D	g_txDiffuse : register( t0 );
SamplerState g_samLinear : register( s0 );


struct VS_INSTANCED_COLOR_INPUT
{
    float4 position : POSITION;
	float3 normal		: NORMAL;
	float2 texcoord		: TEXCOORD0;
    //float4 color : COLOR0;
//���� ���̴��� ���޵Ǵ� ��ü �ν��Ͻ��� ��ġ �����̴�.
    //float4 instancePos : POSINSTANCE;
	row_major float4x4 mtxTransform : POSINSTANCE;
};

struct VS_INSTANCED_COLOR_OUTPUT
{
    float4 position : SV_POSITION;
	float3 normal		: NORMAL;
	float2 texcoord		: TEXCOORD0;
   // float4 color : COLOR0;
//�ý��� ���� ������ ���� ���̴��� ���޵Ǵ� ��ü �ν��Ͻ��� ID�� �ȼ� ���̴��� �����Ѵ�.
//    float4 instanceID : INDEX;
};



VS_INSTANCED_COLOR_OUTPUT VSInstancedDiffusedColor(VS_INSTANCED_COLOR_INPUT input)
{
    VS_INSTANCED_COLOR_OUTPUT output = (VS_INSTANCED_COLOR_OUTPUT)0;
    //output.position = mul(input.position , gmtxWorld);
	
//������ ��ġ ���Ϳ� �ν��Ͻ��� ��ġ ����(���� ��ǥ)�� ���Ѵ�.
    //output.position = output.position + input.instancePos;
    output.position = mul(input.position, input.mtxTransform);
    output.position = mul(output.position, gmtxView);
    output.position = mul(output.position, gmtxProjection);

	output.normal = mul(input.normal, (float3x3)input.mtxTransform);
	output.texcoord = input.texcoord;

    //output.color = input.color;
//�ν��Ͻ� ID�� �ȼ� ���̴��� �����Ѵ�.
   // output.instanceID = instanceID;
    return output;
}


// �ȼ�-���̴�
float4 PSInstancedDiffusedColor(VS_INSTANCED_COLOR_OUTPUT input) : SV_Target
{
	 float4 vDiffuse = g_txDiffuse.Sample( g_samLinear, input.texcoord );

	 return vDiffuse;

	// return input.color;
}


