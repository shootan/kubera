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


struct VS_DIFFUSED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
	float4 color : COLOR;
};



VS_DIFFUSED_COLOR_OUTPUT VSDiffusedColor(float4 position : POSITION, float4 color : COLOR)
{
	 VS_DIFFUSED_COLOR_OUTPUT output = (VS_DIFFUSED_COLOR_OUTPUT)0;
    output.position = mul(position, gmtxWorld);
    output.position = mul(output.position, gmtxView);
    output.position = mul(output.position, gmtxProjection);
    output.color = color;
    return output;
}


float4 PSDiffusedColor(VS_DIFFUSED_COLOR_OUTPUT input) : SV_Target
{
/*�ȼ�-���̴��� �׻� ������ ��Ÿ���� 4���� �Ǽ�(float4)�� ��ȯ�ؾ� �Ѵ�. 
�Է� �Ķ���Ϳ� ������� ����� ������ ��ȯ�Ѵ�. 
�׷��Ƿ� �������� ����� �Ǵ� ��� �ȼ��� ������ ������� �ȴ�(����� = ������ + ���).*/
   
   // return float4(1.0f, 1.0f, 0.0f, 1.0f);    // R=1.0f, G=1.0f, B=0.0f, A=1.0f, 

	return input.color;
}

