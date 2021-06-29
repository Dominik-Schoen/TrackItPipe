using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Valve.VR;
using JsonConvert = Valve.Newtonsoft.Json.JsonConvert;
using TrackpointApp;

public class SteamVRTrack : MonoBehaviour
{
    public string filePath;
    public int rotationCorrection;
    private const int divisor = Configuration.divisor;

    private GameObject rotationObject;
    private GameObject meshObject;
    private List<ActionPoint> actionPoints = new List<ActionPoint>();
    private TrackpointMesh trackpointMesh;
    private SteamVR_TrackedObject tracking;
    
    // Start is called before the first frame update
    void Start()
    {
        rotationObject = new GameObject();
        rotationObject.name = "TrackingRotation";
        meshObject = new GameObject();
        meshObject.name = "TrackingMesh";
        rotationObject.transform.parent = this.transform;
        trackpointMesh = meshObject.AddComponent<TrackpointMesh>();
        trackpointMesh.transform.parent = rotationObject.transform;
        trackpointMesh.setup(filePath, TrackingSystem.SteamVRTrack);
        setupTrackpointTranslationAndRotation();
        setupActionPoints();
        tracking = gameObject.AddComponent<SteamVR_TrackedObject>();
        tracking.index = (SteamVR_TrackedObject.EIndex)1;
    }

    void setupTrackpointTranslationAndRotation()
    {
        string metadata = trackpointMesh.getMetaData(TrackingSystem.SteamVRTrack);
        List<trackpoints> metaObject = JsonConvert.DeserializeObject<List<trackpoints>>(metadata);
        if (metaObject.Count != 1)
        {
            Debug.Log("TrackpointApp Error: Only one tracker per element supported for SteamVR.");
            return;
        }
        float[] point = metaObject[0].point;
        float[] normal = metaObject[0].normal;
        trackpointMesh.transform.localPosition = new Vector3(-(point[0] / divisor), -(point[1] / divisor), -(point[2] / divisor));
        Vector3 unityNormal = new Vector3(normal[0], normal[2], normal[1]);
        Quaternion rotateObjectToTrackpoint = Quaternion.FromToRotation(Vector3.up, unityNormal); 
        Quaternion correction = Quaternion.AngleAxis(rotationCorrection, Vector3.back);
        Quaternion result = correction * rotateObjectToTrackpoint;
        rotationObject.transform.rotation = result;
    }

    void setupActionPoints()
    {
        string metadata = trackpointMesh.getActionPointMetaData();
        Dictionary<String, trackpoints> metaObject = JsonConvert.DeserializeObject<Dictionary<String, trackpoints>>(metadata);
        foreach (KeyValuePair<String, trackpoints> actionPoint in metaObject)
        {
            GameObject anchor = new GameObject();
            anchor.name = "ActionPoint " + actionPoint.Key;
            anchor.transform.parent = trackpointMesh.transform;
            float[] point = actionPoint.Value.point;
            float[] normal = actionPoint.Value.normal;
            anchor.transform.localPosition = new Vector3(point[0] / divisor, point[1] / divisor, point[2] / divisor);
            Vector3 unityNormal = new Vector3(normal[0], normal[2], normal[1]);
            anchor.transform.rotation = Quaternion.FromToRotation(Vector3.up, unityNormal);
            ActionPoint actionPointObject = anchor.AddComponent<ActionPoint>();
            actionPointObject.setup();
            actionPoints.Add(actionPointObject);
        }
    }
}

public class trackpoints
{
    public float[] point
    {
        get;
        set;
    }

    public float[] normal
    {
        get;
        set;
    }
}