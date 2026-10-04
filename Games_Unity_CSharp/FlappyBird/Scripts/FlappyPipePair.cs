using UnityEngine;

namespace LearnUnreal.Flappy
{
    /// <summary>
    /// Flappy Pipe Pair in Unity3D.
    /// Manages the top and bottom obstacle pipes, score trigger, and procedural positioning.
    /// Direct equivalent to Unreal Engine's AFlappyPipePair.
    /// </summary>
    public class FlappyPipePair : MonoBehaviour
    {
        [Header("Movement")]
        [SerializeField] private float moveSpeed = 3.5f;
        [SerializeField] private float destroyXThreshold = -12.0f;

        [Header("Geometry & Pivot Alignment")]
        [Tooltip("Total vertical length of each pipe cylinder/cube.")]
        [SerializeField] private float pipeLength = 8.0f;

        [Tooltip("Pipe diameter / width.")]
        [SerializeField] private float pipeDiameter = 1.2f;

        [Header("References")]
        [SerializeField] private Transform topPipeTransform;
        [SerializeField] private Transform bottomPipeTransform;
        [SerializeField] private BoxCollider scoreTriggerCollider;

        private bool scoreAwarded = false;

        public void SetupPipes(float gapCenterY, float gapSize, float speed)
        {
            moveSpeed = speed;
            scoreAwarded = false;

            // -------------------------------------------------------------------------
            // CRITICAL UNITY Y-COORDINATE & PIVOT ALIGNMENT CHECK:
            // Standard Unity 3D primitives (Cylinder, Cube) have their pivot in the EXACT CENTER (0, 0, 0).
            // A primitive of height H spans from -H/2 to +H/2 along its local Y axis.
            // 
            // 1. Top pipe:
            //    The bottom rim of the top pipe must be at: gapCenterY + (gapSize / 2).
            //    Since the pivot is centered, the top pipe's center must be offset upward by:
            //    Y = gapCenterY + (gapSize / 2) + (pipeLength / 2).
            //
            // 2. Bottom pipe:
            //    The top rim of the bottom pipe must be at: gapCenterY - (gapSize / 2).
            //    Since the pivot is centered, the bottom pipe's center must be offset downward by:
            //    Y = gapCenterY - (gapSize / 2) - (pipeLength / 2).
            // -------------------------------------------------------------------------

            float halfGap = gapSize * 0.5f;
            float halfPipe = pipeLength * 0.5f;

            if (topPipeTransform != null)
            {
                topPipeTransform.localScale = new Vector3(pipeDiameter, pipeLength * 0.5f, pipeDiameter); // Cylinder base height is 2 in Unity
                topPipeTransform.localPosition = new Vector3(0.0f, gapCenterY + halfGap + halfPipe, 0.0f);
            }

            if (bottomPipeTransform != null)
            {
                bottomPipeTransform.localScale = new Vector3(pipeDiameter, pipeLength * 0.5f, pipeDiameter);
                bottomPipeTransform.localPosition = new Vector3(0.0f, gapCenterY - halfGap - halfPipe, 0.0f);
            }

            if (scoreTriggerCollider != null)
            {
                scoreTriggerCollider.transform.localPosition = new Vector3(0.0f, gapCenterY, 0.0f);
                scoreTriggerCollider.size = new Vector3(0.5f, gapSize, 2.0f);
                scoreTriggerCollider.isTrigger = true;
            }
        }

        private void Update()
        {
            if (FlappyGameManager.Instance != null && FlappyGameManager.Instance.CurrentState != FlappyGameManager.GameState.Playing)
            {
                return;
            }

            // Move left along X axis (in Unity 2D/3D side-view, movement is -X)
            transform.position += Vector3.left * (moveSpeed * Time.deltaTime);

            // Destroy once off-screen
            if (transform.position.x < destroyXThreshold)
            {
                Destroy(gameObject);
            }
        }

        private void OnTriggerEnter(Collider other)
        {
            // Score trigger check
            if (!scoreAwarded && other.GetComponent<BirdController>() != null)
            {
                scoreAwarded = true;
                FlappyGameManager.Instance?.AddScore(1);
            }
        }
    }
}
