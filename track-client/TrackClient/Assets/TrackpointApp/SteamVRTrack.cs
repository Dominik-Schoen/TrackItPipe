using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Lib3MF;

public class SteamVRTrack : MonoBehaviour
{
    public string filePath;
    public float width = 1;
    public float height = 1;
    
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
            return;
        }
        

        Vector3[] vertices = new Vector3[4]
        {
            new Vector3(0, 0, 0),
            new Vector3(width, 0, 0),
            new Vector3(0, height, 0),
            new Vector3(width, height, 0)
        };
        mesh.vertices = vertices;

        int[] tris = new int[6]
        {
            // lower left triangle
            0, 2, 1,
            // upper right triangle
            2, 3, 1
        };
        mesh.triangles = tris;

        Vector3[] normals = new Vector3[4]
        {
            -Vector3.forward,
            -Vector3.forward,
            -Vector3.forward,
            -Vector3.forward
        };
        mesh.normals = normals;

        Vector2[] uv = new Vector2[4]
        {
            new Vector2(0, 0),
            new Vector2(1, 0),
            new Vector2(0, 1),
            new Vector2(1, 1)
        };
        mesh.uv = uv;

        meshFilter.mesh = mesh;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    CMeshObject getSteamVrMesh(CMeshObjectIterator iterator)
    {
        iterator.MoveNext();
        bool found = false;
        ulong count = iterator.Count();
        CMeshObject fileMesh = iterator.GetCurrentMeshObject();
        for (ulong i = 1; i < count; i++)
        {
            if (fileMesh.GetName() == "steamvrtrack")
            {
                return fileMesh;
            }
            iterator.MoveNext();
            fileMesh = iterator.GetCurrentMeshObject();
        }

        return null;
    }
}
