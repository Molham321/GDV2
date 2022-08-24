// Quelle: https://www.shadertoy.com/view/tsV3Dc

// -----------------------------------------------------------------------------
// Define the constant buffers.
// -----------------------------------------------------------------------------

cbuffer VSBuffer : register(b0)      // Register the vertex constant buffer on slot 0
{
    float4x4 g_ViewProjectionMatrix;
    float4x4 g_WorldMatrix;
}

cbuffer PSBuffer : register(b0)     // Register the pixel constant buffer on slot 0
{
    float     g_PSLoopZoom;
    float     g_PSMeasureBulk;
}

// -----------------------------------------------------------------------------
// Define input and output data of the vertex shader.
// -----------------------------------------------------------------------------

struct VSInput
{
    float3 m_Position       : POSITION;
    float2 m_PositionNormed : TEXCOORD0;
};

struct PSInput
{
    float4 m_Position       : SV_POSITION;
    float2 m_PositionNormed : TEXCOORD0;
};

// -----------------------------------------------------------------------------
// Vertex Shader
// -----------------------------------------------------------------------------

PSInput VSMain(VSInput _Input)
{
    float4 WSPosition;
    float4 CSPosition;

    PSInput Output = (PSInput)0;

    // -------------------------------------------------------------------------------
    // Get the world space position.
    // -------------------------------------------------------------------------------
    WSPosition = mul(float4(_Input.m_Position, 1.0f), g_WorldMatrix);

    // -------------------------------------------------------------------------------
    // Get the clip space position.
    // -------------------------------------------------------------------------------
    CSPosition = mul(WSPosition, g_ViewProjectionMatrix);

    Output.m_Position = CSPosition;
    Output.m_PositionNormed = _Input.m_PositionNormed;
    return Output;
}

float TexXY(int _X, int _Y) 
{
    return ( (_X >= 0 && _Y >= 0) && (!(_X & _Y)) ) ? 1.0 : 0.0f;
}

float TexY(float _X, int _Y) {
    _X -= 0.5f * float(_Y);
    float texX1 = TexXY(int(floor(_X )), _Y);
    float texX2 = TexXY(int(floor(_X)) + 1, _Y);
    return lerp(texX1, texX2, frac(_X));
}

float Tex(float2 _Pos) {
    float TexY1 = TexY(_Pos.x, int(_Pos.y));
    float TexY2 = TexY(_Pos.x, int(_Pos.y) + 1);
    return lerp(TexY1, TexY2, frac(_Pos.y));
}

float4 MainImage(float2 _Pos, float _ITime, float _MeasureBulk)
{
    float2 m_fragCoord = _Pos * _MeasureBulk;
    float m_t = frac(_ITime * 0.35 / 80.0f);

    float m_zoom = pow(2.0f, m_t);
    float m_distortion = sqrt(3) / 2;

    m_fragCoord   *= m_distortion / m_zoom;
    m_fragCoord.x *= m_distortion;

    float m_texM = lerp(Tex(m_fragCoord), Tex(m_fragCoord * 2.0f), m_zoom - 1.0f);
    
    return float4(m_texM, m_texM, m_texM, 1.0f);
}

// -----------------------------------------------------------------------------
// Pixel Shader
// -----------------------------------------------------------------------------
float4 PSMain(PSInput _Input) : SV_TARGET
{
    return MainImage(_Input.m_PositionNormed, g_PSLoopZoom, g_PSMeasureBulk );
}
