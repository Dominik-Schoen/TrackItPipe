using System.Collections.Generic;
using Newtonsoft.Json;
using UnityEngine;
using Valve.VR;
using TrackpointApp;

public class SteamVRTrack : AbstractTrackingSystem
{
    public int rotationCorrection;
    
    private SteamVR_TrackedObject tracking;

    // Start is called before the first frame update
    void Start()
    {
        generalSetup();
        trackpointMesh.setup(filePath, TrackingSystem.SteamVRTrack);
        TrackpointData data = loadMetaData();
        if (data == null)
        {
            return;
        }
        setupTrackpointTranslation(data);
        setupTrackpointRotation(data, rotationCorrection);
        setupActionPoints();
        tracking = gameObject.AddComponent<SteamVR_TrackedObject>();
        tracking.index = (SteamVR_TrackedObject.EIndex)1;
    }

    TrackpointData loadMetaData()
    {
        string metadata = trackpointMesh.getMetaData(TrackingSystem.SteamVRTrack);
        List<TrackpointData> metaObject = JsonConvert.DeserializeObject<List<TrackpointData>>(metadata);
        if (metaObject.Count != 1)
        {
            Debug.Log("TrackpointApp Error: Only one tracker per element supported for SteamVR.");
            return null;
        }

        return metaObject[0];
    }
}
