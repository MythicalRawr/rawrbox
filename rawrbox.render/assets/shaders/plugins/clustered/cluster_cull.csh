// Based off https://github.com/simco50/D3D12_Research <3

#include <camera.fxh>
#include <light_uniforms.fxh>

#define DECAL_CONSTANTS
#include <decal_uniforms.fxh>

#define READ_CLUSTERS
#define READ_LIGHTS
#define READ_DECALS
#define WRITE_CLUSTER_DATA_GRID
#include <cluster.fxh>

struct Sphere {
	float3 Position;
	float Radius;
};

bool BoxInAABB(float4x4 localPos, ClusterAABB aabb) {
    float4 dPos = mul(aabb.Center, localPos);
    float3 decalTexCoord = dPos.xyz * float3(0.5f, -0.5f, 0.5f) + 0.5f;

	return all(decalTexCoord >= 0.0) && all(decalTexCoord <= 1.0);
}

bool SphereInAABB(Sphere sphere, ClusterAABB aabb) {
    float3 d = max(0, abs(aabb.Center.xyz - sphere.Position) - aabb.Extents.xyz);
	float distanceSq = dot(d, d);

	return distanceSq <= sphere.Radius * sphere.Radius;
}

bool ConeInSphere(float3 conePosition, float3 coneDirection, float coneRange, float2 coneAngleSinCos, Sphere sphere) {
	float3 v = sphere.Position - conePosition;

	float lenSq = dot(v, v);
	float v1Len = dot(v, coneDirection);

	float distanceClosestPoint = coneAngleSinCos.y * sqrt(lenSq - v1Len * v1Len) - v1Len * coneAngleSinCos.x;

	bool angleCull = distanceClosestPoint > sphere.Radius;
	bool frontCull = v1Len > sphere.Radius + coneRange;
	bool backCull = v1Len < -sphere.Radius;

	return !(angleCull || frontCull || backCull);
}

[numthreads(CLUSTERS_X_THREADS, CLUSTERS_Y_THREADS, CLUSTERS_Z_THREADS)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
    uint3 clusterIndex3D = dispatchThreadId;
    if(any(clusterIndex3D >= GROUP_SIZE))
		return;

	uint clusterIndex = Flatten3D(clusterIndex3D, float2(CLUSTERS_X, CLUSTERS_Y));
    ClusterAABB cluster = g_Clusters[clusterIndex];

	float clusterRadius = sqrt(dot(cluster.Extents.xyz, cluster.Extents.xyz));

	uint lightIndex = 0;
	uint decalIndex = 0;

    float3 lightDir = mul(float4(0, 0, 0, 1.0), g_view).xyz;

    [loop]
    for(uint bucketIndex = 0; bucketIndex < CLUSTERED_NUM_BUCKETS && (lightIndex < TOTAL_LIGHTS || decalIndex < TOTAL_DECALS); ++bucketIndex) {
		uint lightMask = 0;
        uint decalMask = 0;

        [loop]
        for(uint i = 0; i < CLUSTERS_Z && lightIndex < TOTAL_LIGHTS; ++i) {
            Light light = g_Lights[lightIndex];
            ++lightIndex;

            if(light.type == LIGHT_POINT) {
                Sphere sphere;
                sphere.Radius = light.radius;
                sphere.Position = mul(light.position, g_view).xyz;

                if(SphereInAABB(sphere, cluster)) {
                    lightMask |= 1u << i;
                }
            } else if(light.type == LIGHT_SPOT) {
                float3 viewSpacePos = mul(light.position, g_view).xyz;
                float3 viewSpaceDir = mul(light.direction, g_view).xyz;
                viewSpaceDir = normalize(viewSpaceDir - lightDir);

                float2 coneAngleSinCos = float2(sin(light.umbra), cos(light.umbra)) + 0.3; // Add a bit of offset to the cull, so when you move the camera you don't notice the delay

                Sphere sphere;
				sphere.Radius = clusterRadius;
				sphere.Position = cluster.Center.xyz;

                if(ConeInSphere(viewSpacePos, viewSpaceDir, light.radius, coneAngleSinCos, sphere)) {
                    lightMask |= 1u << i;
                }
            } else {
                lightMask |= 1u << i; // Unknown / directional, don't calculate cull
            }
        }

        [loop]
        for(uint o = 0; o < CLUSTERS_Z && decalIndex < TOTAL_DECALS; ++o) {
            Decal decal = g_Decals[decalIndex];
            ++decalIndex;

            if(decal.data.y == 1) { //test
                if(BoxInAABB(decal.worldToLocal, cluster)) {
                    decalMask |= 1u << o;
                }
            } else {
                decalMask |= 1u << o;
            }
        }

	    g_ClusterDataGrid[clusterIndex * CLUSTERED_NUM_BUCKETS + bucketIndex] = uint2(lightMask, decalMask);
    }
}
