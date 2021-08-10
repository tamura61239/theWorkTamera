//uintÅ®float4
float4 ColorCompressionConversion(uint inColor)
{
    float4 outColor = (float4) 0;
    outColor.r = ((inColor >> 24) & 0x000000FF) / 255.0f;
    outColor.g = ((inColor >> 16) & 0x000000FF) / 255.0f;
    outColor.b = ((inColor >> 8) & 0x000000FF) / 255.0f;
    outColor.a = ((inColor >> 0) & 0x000000FF) / 255.0f;

    return outColor;
}
//float4Å®uint
uint ColorReleaseConversion(float4 inColor)
{
    uint outColor = (uint) 0;
    outColor |= ((uint) (inColor.r * 255) & 0x00FFFFFF) << 24;
    outColor |= ((uint) (inColor.g * 255) & 0x00FFFFFF) << 16;
    outColor |= ((uint) (inColor.b * 255) & 0x00FFFFFF) << 8;
    outColor |= ((uint) (inColor.a * 255) & 0x00FFFFFF) << 0;

    return outColor;
}