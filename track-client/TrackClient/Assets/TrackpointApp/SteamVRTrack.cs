using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Lib3MF;

public class SteamVRTrack : MonoBehaviour
{
    public string filePath;
    
    // Start is called before the first frame update
    void Start()
    {
        MeshRenderer meshRenderer = gameObject.AddComponent<MeshRenderer>();
        meshRenderer.sharedMaterial = new Material(Shader.Find("Standard"));

        MeshFilter meshFilter = gameObject.AddComponent<MeshFilter>();

        Mesh mesh = new Mesh();
        
        // Load trackpoint file
        CModel model = Wrapper.CreateModel();
        CReader reader = model.QueryReader("3mf");
        reader.ReadFromFile(filePath);
        CMeshObjectIterator iterator = model.GetMeshObjects();
        CMeshObject fileMesh = getSteamVrMesh(iterator);
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
            vertices[i * 3] = new Vector3(one.Coordinates[0], one.Coordinates[1], one.Coordinates[2]);
            sPosition two = fileVertices[current.Indices[1]];
            vertices[i * 3 + 1] = new Vector3(two.Coordinates[0], two.Coordinates[1], two.Coordinates[2]);
            sPosition three = fileVertices[current.Indices[2]];
            vertices[i * 3 + 2] = new Vector3(three.Coordinates[0], three.Coordinates[1], three.Coordinates[2]);

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

    // Update is called once per frame
    void Update()
    {
        
    }

    CMeshObject getSteamVrMesh(CMeshObjectIterator iterator)
    {
        ulong count = iterator.Count();
        CMeshObject fileMesh;
        for (ulong i = 0; i < count; i++)
        {
            iterator.MoveNext();
            fileMesh = iterator.GetCurrentMeshObject();
            if (fileMesh.GetName() == "steamvrtrack")
            {
                return fileMesh;
            }
        }

        return null;
    }
}
