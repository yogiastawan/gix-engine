namespace GixEngineTransform
{
    /// translate
    float4 translate(float4 o, float4 trsnl)
    {
        return mul(float4x4(
                       1, 0, 0, trsnl.x,
                       0, 1, 0, trsnl.y,
                       0, 0, 1, trsnl.z,
                       0, 0, 0, 1),
                   o);
    }

    // scale
    float4 scale(float4 o, float4 scl)
    {
        return mul(float4x4(
                       scl.x, 0, 0, 0,
                       0, scl.y, 0, 0,
                       0, 0, scl.z, 0,
                       0, 0, 0, 1),
                   o);
    }

    // rotate
    float4 rotate(float angle)
    {
        return float4(0, 0, 0, 0);
    }
}