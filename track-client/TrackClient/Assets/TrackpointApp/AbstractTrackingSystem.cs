using System;
using System.Collections.Generic;
using System.Linq;
using Newtonsoft.Json;
using UnityEngine;

namespace TrackpointApp
{
    public class AbstractTrackingSystem : MonoBehaviour
    {
        public string filePath;
        private const int divisor = Configuration.divisor;
        
        private GameObject rotationObject;
        private GameObject meshObject;
        private List<ActionPoint> actionPoints = new List<ActionPoint>();
        protected TrackpointMesh trackpointMesh;

        protected void generalSetup()
        {
            rotationObject = new GameObject();
            rotationObject.name = "TrackingRotation";
            meshObject = new GameObject();
            meshObject.name = "TrackingMesh";
            rotationObject.transform.parent = this.transform;
            trackpointMesh = meshObject.AddComponent<TrackpointMesh>();
            trackpointMesh.transform.parent = rotationObject.transform;
        }

        protected void setupTrackpointTranslation(TrackpointData trackpoint)
        {
            float[] point = trackpoint.point;
            trackpointMesh.transform.localPosition = new Vector3(-(point[0] / divisor), -(point[1] / divisor), -(point[2] / divisor));
        }

        protected void setupTrackpointRotation(TrackpointData trackpoint, int rotationCorrection)
        {
            float[] normal = trackpoint.normal;
            Vector3 unityNormal = new Vector3(normal[0], normal[2], normal[1]);
            Quaternion rotateObjectToTrackpoint = Quaternion.FromToRotation(Vector3.up, unityNormal);
            Quaternion correction = Quaternion.AngleAxis(rotationCorrection, Vector3.back);
            Quaternion result = correction * rotateObjectToTrackpoint;
            rotationObject.transform.rotation = result;
        }
        
        protected void setupActionPoints()
        {
            string metadata = trackpointMesh.getActionPointMetaData();
            if (metadata != "")
            {
                Dictionary<String, TrackpointData> metaObject =
                    JsonConvert.DeserializeObject<Dictionary<String, TrackpointData>>(metadata);
                if (metaObject.Count() <= 0) return;
                foreach (KeyValuePair<String, TrackpointData> actionPoint in metaObject)
                {
                    GameObject anchor = new GameObject();
                    anchor.name = "ActionPoint " + actionPoint.Key;
                    anchor.transform.parent = trackpointMesh.transform;
                    float[] point = actionPoint.Value.point;
                    float[] normal = actionPoint.Value.normal;
                    anchor.transform.localPosition =
                        new Vector3(point[0] / divisor, point[1] / divisor, point[2] / divisor);
                    Vector3 unityNormal = new Vector3(normal[0], normal[1], normal[2]);
                    anchor.transform.localRotation = Quaternion.FromToRotation(Vector3.up, unityNormal);
                    ActionPoint actionPointObject = anchor.AddComponent<ActionPoint>();
                    actionPointObject.setup();
                    actionPoints.Add(actionPointObject);
                }
            }
        }
    }
}