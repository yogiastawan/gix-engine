cbuffer MVP : register(b0, space1)
{
    float4x4 vp;
};

struct VertexInput
{
    float3 color;
};

struct GridObj
{
    float point_v[2]; // 0 start,1 end
    uint increment;
};

StructuredBuffer<GridObj> line_data : register(t0, space0); // 0 for z line, 1 for x line

struct VertOutput
{
    float4 position : SV_POSITION;
    float4 color : TEXCOORD0;
};

float4x4 translate(float3 tr)
{
    return float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        tr.x, tr.y, tr.z, 1);
}

VertOutput main(VertexInput input, uint vert_id : SV_VertexID, uint instance_id : SV_InstanceID)
{
    VertOutput output;
    output.color = float4(input.color, 1.0f);

    uint blend_factor = (instance_id % 2);
    // if instance_id is even draw line from camera to forward
    float x_start_event = 0.0f;
    float x_end_event = 0.0f;
    float z_start_event = line_data[0].point_v[0];
    float z_end_event = line_data[0].point_v[1];

    // if instance_id is odd draw line from left to right
    float x_start_odd = line_data[1].point_v[0];
    float x_end_odd = line_data[1].point_v[1];
    float z_start_odd = 0.f;
    float z_end_odd = 0.f;

    float start_x = lerp(x_start_event, x_start_odd, (float)blend_factor);
    float end_x = lerp(x_end_event, x_end_odd, (float)blend_factor);
    float start_z = lerp(z_start_event, z_start_odd, (float)blend_factor);
    float end_z = lerp(z_end_event, z_end_odd, (float)blend_factor);

    // offset
    float offset = (float)instance_id * (float)line_data[instance_id].increment;

    float current_vertex_x = lerp(start_x, end_x, (float)vert_id % 2) + offset;
    float current_vertex_z = lerp(start_z, end_z, (float)vert_id % 2) + offset;

    float3 tr = lerp(float3(offset, 0, 0), float3(0, 0, offset), blend_factor);
    float4x4 model_matrix = translate(tr);

    float4 world_pos = mul(model_matrix, float4(current_vertex_x, 0, current_vertex_z, 1));
    // clip position
    output.position = mul(vp, world_pos);
    return output;
}