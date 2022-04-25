Texture2D g_MeshTexture;

SamplerState MeshTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_OUTPUT
{
	float4 Position   : SV_POSITION; // vertex position 
	float4 Normal   : NORMAL0; // vertex normal 

	float2 TextureUV  : TEXCOORD0;   // vertex texture coords 
};


float4 main(VS_OUTPUT input) : SV_Target{

	return input.Normal;//*0.5 + float4(0.2,0,0,1)*.5;//g_MeshTexture.Sample(MeshTextureSampler,input.TextureUV);
}