cbuffer CBuf : register(b2)
{
    float4 color;
};

float4 main() : SV_TARGET
{
    return color;
}