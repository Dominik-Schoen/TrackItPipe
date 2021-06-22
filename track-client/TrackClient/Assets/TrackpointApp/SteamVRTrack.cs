using System;
using System.Collections;
using System.Collections.Generic;
using Newtonsoft.Json;
using UnityEngine;
using Valve.VR;

public class SteamVRTrack : TrackpointMesh
{
    public string filePath;

    private GameObject meshObject;
    private TrackpointMesh trackpointMesh;
    private SteamVR_TrackedObject tracking;
    
    // Start is called before the first frame update
    void Start()
    {
        meshObject = new GameObject();
        trackpointMesh = meshObject.AddComponent<TrackpointMesh>();
        trackpointMesh.transform.parent = this.transform;
        trackpointMesh.setup(filePath, TrackingSystem.SteamVRTrack);
        setupTrackpointTranslationAndRotation();
        tracking = gameObject.AddComponent<SteamVR_TrackedObject>();
        tracking.index = (SteamVR_TrackedObject.EIndex)1;
    }

    void setupTrackpointTranslationAndRotation()
    {
        string metadata = trackpointMesh.getMetaData(TrackingSystem.SteamVRTrack);
        trackpointMesh.transform.localPosition = new Vector3(0.0f, 0.0f, 3.0f);
    }
}
