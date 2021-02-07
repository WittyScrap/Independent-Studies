// Constant buffer representation
cbuffer ConstantBuffer : register(b0)
{
	float4 scale;
    float4 color;
};

/**
 *  Vertex input structure.
 *  This must match the input vertex layout structure.
 *
 */
struct vin
{
	float2 position : POSITION;
    float4 color    : COLOR;
    float2 texcoord : TEXCOORD1;
	float1 globalVs : TEXCOORD2;
};

/**
 *  Vertex output/pixel input
 *  structure.
 *
 */
struct vout
{
	float4 position : SV_POSITION;
    float4 color    : COLOR;
};

/**
 *  Vertex function/shader. This function must match
 *  the signature given in the compilation phase.
 *
 */
vout vert(vin i)
{
	vout o;

	o.position = float4((i.position / scale.xy) * 2 - 1, 0, 1);
    o.color = i.color;

	return o;
}

/**
 *  Pixel function/shader. This function must match
 *  the signature given in the compilation phase.
 *
 */
float4 pixel(vout i) : SV_Target
{
	return float4(abs(i.color.rgb), 1);
}