using UnityEngine;

namespace TrackpointApp
{
    public class ActionPoint : MonoBehaviour
    {
        private GameObject cylinder;
        private const int divisor = Configuration.divisor;
        
        public void setup()
        {
            cylinder = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            cylinder.transform.parent = this.transform;
            size(2.5f / divisor, 2.0f / divisor);
            cylinder.GetComponent<Renderer>().material.color = Color.blue;
        }

        private void size(float radius, float length)
        {
            float actualLength = cylinder.GetComponent<Renderer>().bounds.size.y;
            float actualDiameter = cylinder.GetComponent<Renderer>().bounds.size.x;
            Vector3 rescale = cylinder.transform.localScale;
            rescale.y = length * rescale.y / actualLength;
            float xzFactor = radius * 2 * rescale.x / actualDiameter;
            rescale.x = xzFactor;
            rescale.z = xzFactor;
            cylinder.transform.localScale = rescale;
        }
    }
}