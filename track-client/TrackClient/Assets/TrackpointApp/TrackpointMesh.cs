using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Lib3MF;

public enum TrackingSystem {
    OptiTrack = 0,
    EMFTrack = 1,
    SteamVRTrack = 2
};

public class TrackpointMesh : MonoBehaviour
{
    private const string META_NAMESPACE = "tk-ar-tracking";
    private const int divisor = 100;
    
    private CModel model;
    private CMeshObject fileMesh;

    public void setup(string filePath, TrackingSystem trackingSystem)
    {
        MeshRenderer meshRenderer = gameObject.AddComponent<MeshRenderer>();
        meshRenderer.sharedMaterial = new Material(Shader.Find("Standard"));

        MeshFilter meshFilter = gameObject.AddComponent<MeshFilter>();
        
        Mesh mesh = new Mesh();
        
        // Load trackpoint file
        model = Wrapper.CreateModel();
        CReader reader = model.QueryReader("3mf");
        reader.ReadFromFile(filePath);
        CMeshObjectIterator iterator = model.GetMeshObjects();
        fileMesh = getMesh(iterator, trackingSystem);

        if (fileMesh == null)
        {
            Debug.Log("TrackpointApp Error: No valid mesh found.");
            return;
        }

        sPosition[] fileVertices;
        sTriangle[] fileTriangles;
        fileMesh.GetVertices(out fileVertices);
        fileMesh.GetTriangleIndices(out fileTriangles);
        uint vertexCount = fileMesh.GetTriangleCount() * 3;
   
        Vector3[] vertices = new Vector3[vertexCount];
        int[] tris = new int[vertexCount];
        Vector3[] normals = new Vector3[vertexCount];

        for (int i = 0; i < fileMesh.GetTriangleCount(); i++)
        {
            sTriangle current = fileTriangles[i];

            sPosition one = fileVertices[current.Indices[0]];
            vertices[i * 3] = new Vector3(one.Coordinates[0] / divisor, one.Coordinates[1] / divisor, one.Coordinates[2] / divisor);
            sPosition two = fileVertices[current.Indices[1]];
            vertices[i * 3 + 1] = new Vector3(two.Coordinates[0] / divisor, two.Coordinates[1] / divisor, two.Coordinates[2] / divisor);
            sPosition three = fileVertices[current.Indices[2]];
            vertices[i * 3 + 2] = new Vector3(three.Coordinates[0] / divisor, three.Coordinates[1] / divisor, three.Coordinates[2] / divisor);

            tris[i * 3] = i * 3;
            tris[i * 3 + 1] = i * 3 + 1;
            tris[i * 3 + 2] = i * 3 + 2;

            Vector3 lineOne = vertices[i * 3 + 1] - vertices[i * 3];
            Vector3 lineTwo = vertices[i * 3 + 2] - vertices[i * 3];

            Vector3 normal = Vector3.Cross(lineOne, lineTwo).normalized;
            
            normals[i * 3] = normal;
            normals[i * 3 + 1] = normal;
            normals[i * 3 + 2] = normal;
        }
        
        mesh.vertices = vertices;
        mesh.triangles = tris;
        mesh.normals = normals;

        meshFilter.mesh = mesh;
    }

    public string getMetaData(TrackingSystem trackingSystem)
    {
        string key;
        switch (trackingSystem)
        {
            case TrackingSystem.OptiTrack:
                key = "trackpoints-optitrack";
                break;
            case TrackingSystem.EMFTrack:
                key = "trackpoints-emftrack";
                break;
            case TrackingSystem.SteamVRTrack:
                key = "trackpoints-steamvrtrack";
                break;
            default:
                key = "";
                Debug.Log("No tracking system specified. This error is fatal.");
                break;
        }
        CMetaDataGroup metaDataGroup = fileMesh.GetMetaDataGroup();
        CMetaData metaData = metaDataGroup.GetMetaDataByKey(META_NAMESPACE, key);
        return metaData.GetValue();
    }
    
    CMeshObject getMesh(CMeshObjectIterator iterator, TrackingSystem trackingSystem)
    {
        string comparator;
        switch (trackingSystem)
        {
            case TrackingSystem.OptiTrack:
                comparator = "optitrack";
                break;
            case TrackingSystem.EMFTrack:
                comparator = "emftrack";
                break;
            case TrackingSystem.SteamVRTrack:
                comparator = "steamvrtrack";
                break;
            default:
                comparator = "";
                Debug.Log("No tracking system specified. This error is fatal.");
                break;
        }
        
        ulong count = iterator.Count();
        CMeshObject fileMesh;
        for (ulong i = 0; i < count; i++)
        {
            iterator.MoveNext();
            fileMesh = iterator.GetCurrentMeshObject();
            if (fileMesh.GetName() == comparator)
            {
                return fileMesh;
            }
        }

        return null;
    }
}
