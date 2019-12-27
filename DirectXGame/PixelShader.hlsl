struct PS_INPUT
{
	float4 position: SV_POSITION;
	float3 color: COLOR;
	float3 color1: COLOR1;
};

cbuffer constant: register(b0)
{
	unsigned int m_time;
};


float4 psmain(PS_INPUT input) : SV_TARGET
{
	float3 color = lerp(input.color, input.color1, (sin(m_time / 1000.0f) + 1.0f) / 2.0f);
	return float4(color, 1.0f);
}
