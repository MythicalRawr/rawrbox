
#include <cluster_uniforms.fxh>

#define WRITE_CLUSTERS
#include <cluster.fxh>

[numthreads(CLUSTERS_X_THREADS, CLUSTERS_Y_THREADS, CLUSTERS_Z_THREADS)]
void main(uint3 GIid: SV_DispatchThreadID) {
    // index calculation must match the inverse operation in the fragment shader (see getClusterIndex)
    uint clusterIndex = GIid.z * CLUSTERS_X_THREADS * CLUSTERS_Y_THREADS +
                        GIid.y * CLUSTERS_X_THREADS +
                        GIid.x;


    // calculate min (bottom left) and max (top right) xy in screen coordinates
    float4 minScreen = float4(GIid.xy                  * g_Cluster.clusterSize, 1.0, 1.0);
    float4 maxScreen = float4((GIid.xy + float2(1, 1)) * g_Cluster.clusterSize, 1.0, 1.0);

    // -> eye coordinates
    // z is the camera far plane (1 in screen coordinates)
    float3 minEye = screen2Eye(minScreen).xyz;
    float3 maxEye = screen2Eye(maxScreen).xyz;


    // calculate near and far depth edges of the cluster
    float clusterNear = g_Cluster.zNearFarVec.x * pow(abs(g_Cluster.zNearFarVec.y / g_Cluster.zNearFarVec.x),  GIid.z      / float(CLUSTERS_Z));
    float clusterFar  = g_Cluster.zNearFarVec.x * pow(abs(g_Cluster.zNearFarVec.y / g_Cluster.zNearFarVec.x), (GIid.z + 1) / float(CLUSTERS_Z));

    // this calculates the intersection between:
    // - a line from the camera (origin) to the eye point (at the camera's far plane)
    // - the cluster's z-planes (near + far)
    // we could divide by u_zFar as well
    float3 minNear = minEye * clusterNear / minEye.z;
    float3 minFar  = minEye * clusterFar  / minEye.z;
    float3 maxNear = maxEye * clusterNear / maxEye.z;
    float3 maxFar  = maxEye * clusterFar  / maxEye.z;

    // get extent of the cluster in all dimensions (axis-aligned bounding box)
    // there is some overlap here but it's easier to calculate intersections with AABB

    Cluster cl = g_Clusters[clusterIndex];
    cl.minBounds =  min(min(minNear, minFar), min(maxNear, maxFar));
    cl.maxBounds = max(max(minNear, minFar), max(maxNear, maxFar));

    g_Clusters[clusterIndex] = cl;
}
