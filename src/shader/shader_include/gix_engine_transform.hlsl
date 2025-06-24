namespace GixEngineTransform
{
    // translate
    float4 translate(float4 obj, float4 trsnl)
    {
        return mul(float4x4(
                       1, 0, 0, trsnl.x,
                       0, 1, 0, trsnl.y,
                       0, 0, 1, trsnl.z,
                       0, 0, 0, 1),
                   obj);
    }

    // scale
    float4 scale(float4 obj, float4 scl)
    {
        return mul(float4x4(
                       scl.x, 0, 0, 0,
                       0, scl.y, 0, 0,
                       0, 0, scl.z, 0,
                       0, 0, 0, 1),
                   obj);
    }

    // rotate
    float4 rotate_axis_x(float4 obj, float angle)
    {
        float s = sin(angle);
        float c = cos(angle);

        float4x4 rotate_x = float4x4(1, 0, 0, 0,
                                     0, c, -s, 0,
                                     0, s, c, 0,
                                     0, 0, 0, 1);

        return mul(rotate_x, obj);
    }

    float4 rotate_axis_y(float4 obj, float angle)
    {
        float s = sin(angle);
        float c = cos(angle);

        float4x4 rotate_y = float4x4(c, 0, s, 0,
                                     0, 1, 0, 0,
                                     -s, 0, c, 0,
                                     0, 0, 0, 1);

        return mul(rotate_y, obj);
    }

    float4 rotate_axis_z(float4 obj, float angle)
    {
        float s = sin(angle);
        float c = cos(angle);

        float4x4 rotate_z = float4x4(c, -s, 0, 0,
                                     s, c, 0, 0,
                                     0, 0, 1, 0,
                                     0, 0, 0, 1);

        return mul(rotate_z, obj);
    }

    float4 rotate(float4 obj, float angle, float3 axis)
    {
        axis = normalize(axis);
        float s = sin(angle);
        float c = cos(angle);
        float t = 1.0 - c;

        float x = axis.x;
        float y = axis.y;
        float z = axis.z;

        float4x4 rot = float4x4(t * x * x + c, t * x * y - s * z, t * x * z + s * y, 0,
                                t * x * y + s * z, t * y * y + c, t * y * z - s * x, 0,
                                t * x * z - s * y, t * y * z + s * x, t * z * z + c, 0,
                                0, 0, 0, 1);
        return mul(rot, obj);
    }
}