using System;
using System.Collections.Generic;
using Newtonsoft.Json;

namespace TrackpointApp
{
    public class OptiTrack : AbstractTrackingSystem
    {
        private void Start()
        {
            generalSetup();
            trackpointMesh.setup(filePath, TrackingSystem.OptiTrack);
            TrackpointData data = loadMetaData();
            setupTrackpointTranslation(data);
            setupTrackpointRotation(data, 0);
            setupActionPoints();
        }

        TrackpointData loadMetaData()
        {
            string metadata = trackpointMesh.getMetaData(TrackingSystem.SteamVRTrack);
            List<TrackpointData> metaObject = JsonConvert.DeserializeObject<List<TrackpointData>>(metadata);
            TrackpointData result = new TrackpointData();

            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            int c = 0;
            
            foreach (TrackpointData data in metaObject)
            {
                x += data.point[0];
                y += data.point[1];
                z += data.point[2];
                c++;
            }
            
            result.point[0] = x / c;
            result.point[1] = y / c;
            result.point[2] = z / c;

            return result;
        }
    }
}