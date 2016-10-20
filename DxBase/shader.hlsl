Texture2D shaderTextures[25];
SamplerState SampleType;

struct Light
{
    float4 Ambient;
    float4 Diffuse;
    float3 LightDirection;
    //float3 ViewDirection;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Reflect;
};

cbuffer ConstantBuffer : register(b0)
{
	matrix skeleton[512];
    matrix WorldViewProjection;
    matrix World;
    matrix matView;
    matrix WorldInverseTranspose;
    Light gDirLight;
    Material gMaterial;
    float3 gEyeW;
};

struct VSInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
	int index1 : INDEX0;
	int index2 : INDEX1;
	int index3 : INDEX2;
	int index4 : INDEX3;
	float weight1 : WEIGHT0;
	float weight2 : WEIGHT1;
	float weight3 : WEIGHT2;
	float weight4 : WEIGHT3;
    float2 tex : TEXCOORD0;
};
struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldPos : POSITION;
    float2 tex : TEXCOORD0;
	float4 color: COLOR;
	//float3 normal : NORMAL;

    //float3 ViewDirection :   TEXCOORD1;
    //float3 LightDirection:   TEXCOORD2;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PSInput VS(VSInput input)
{
    PSInput output;

    float4 Pos = input.position;
    //output.position = mul(Pos, WorldViewProjection);
    output.worldPos = mul(Pos, World).xyz;

    matrix i1,i2,i3,i4;
    float w1,w2,w3,w4;
    float4 p1,p2,p3,p4;
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    i1 = skeleton[input.index1];
    i2 = skeleton[input.index2];
    i3 = skeleton[input.index3];
    i4 = skeleton[input.index4];
    w1=input.weight1;
    w2=input.weight2;
    w3 = input.weight3;
    w4= input.weight4;
    p1=mul(input.position,i1);
    p2=mul(input.position,i2);
    p3=mul(input.position,i3);
    p4 = mul(input.position,i4);
	//output.position = Pos;
	//output.position = p1;
	//output.position = w1 * p1 + (1 - w1) * p2;
    output.position= (w1*p1+w2*p2+w3*p3+w4*p4);
    output.position = mul(output.position, WorldViewProjection);
    //output.position = mul(output.position, matView);
    //output.position = mul(output.position, WorldViewProjection);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
	output.color.x = w1;
	output.color.y = w2;
	output.color.z = w3;
	output.color.w = w4;
    // Calculate the normal vector against the world matrix only.
    //output.normal = mul(input.normal, (float3x3)World);

    // Normalize the normal vector.
    //output.normal = normalize(output.normal);

    return output;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------


float4 PS(PSInput input) : SV_Target
{
    // Interpolating normal can unnormalize it, so normalize it.
    //input.normal = normalize(input.normal);

    // float3 toEyeW = normalize(gEyeW - input.worldPos);

    // return ComputeDirectionalLight(gMaterial, gDirLight, input.normal, toEyeW);
	//return input.color;
	return saturate(
        shaderTextures[0].Sample(SampleType, input.tex)
        );
}
