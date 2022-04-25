struct VS_OUTPUT
{
	float4 Position   : SV_POSITION; // vertex position 
	float4 Normal   : NORMAL0; // vertex normal 
	float2 TextureUV  : TEXCOORD0;   // vertex texture coords 
};

cbuffer CBuf
{
	matrix Camera;
};

VS_OUTPUT main(float3 pos : Position, float3 nor : Normal, float2 texco : TextureCoordinate)
{
	VS_OUTPUT vso;
	vso.Position = mul(Camera,float4(pos.x , pos.y, pos.z, 1.0f));
	vso.Normal = abs(float4(nor.x , nor.y, nor.z, 1.0f));
	vso.TextureUV = texco;
	return vso;
}