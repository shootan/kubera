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
/*픽셀-쉐이더는 항상 색상을 나타내는 4개의 실수(float4)를 반환해야 한다. 
입력 파라메터에 관계없이 노란색 색상을 반환한다. 
그러므로 렌더링의 대상이 되는 모든 픽셀의 색상은 노란색이 된다(노란색 = 빨간색 + 녹색).*/
   
   // return float4(1.0f, 1.0f, 0.0f, 1.0f);    // R=1.0f, G=1.0f, B=0.0f, A=1.0f, 

	return input.color;
}

