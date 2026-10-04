using System.Collections.Generic;
using UnityEngine;

namespace LearnUnreal.Flappy
{
    /// <summary>
    /// Spawner for Flappy Pipe pairs.
    /// Manages spawn timer, gap randomization, and active pipe pooling/tracking.
    /// Direct equivalent to pipe spawning logic in Unreal Engine's AFlappyGameMode.
    /// </summary>
    public class FlappySpawner : MonoBehaviour
    {
        [Header("Spawn Settings")]
        [Tooltip("Prefab containing FlappyPipePair script.")]
        [SerializeField] private GameObject pipePairPrefab;

        [Tooltip("Interval between pipe spawns (in seconds).")]
        [SerializeField] private float spawnInterval = 1.75f;

        [Tooltip("X coordinate at which new pipes appear off-screen to the right.")]
        [SerializeField] private float spawnX = 10.0f;

        [Header("Gap Settings")]
        [Tooltip("Minimum gap center Y coordinate.")]
        [SerializeField] private float minGapY = -1.5f;

        [Tooltip("Maximum gap center Y coordinate.")]
        [SerializeField] private float maxGapY = 2.5f;

        [Tooltip("Vertical opening size of the gap for the bird to fly through.")]
        [SerializeField] private float gapSize = 3.2f;

        [Tooltip("Horizontal speed of spawned pipes.")]
        [SerializeField] private float pipeSpeed = 3.5f;

        private float spawnTimer = 0.0f;
        private readonly List<GameObject> activePipes = new List<GameObject>();

        private void Update()
        {
            if (FlappyGameManager.Instance == null || FlappyGameManager.Instance.CurrentState != FlappyGameManager.GameState.Playing)
            {
                return;
            }

            spawnTimer += Time.deltaTime;
            if (spawnTimer >= spawnInterval)
            {
                spawnTimer = 0.0f;
                SpawnPipe();
            }
        }

        private void SpawnPipe()
        {
            float gapCenterY = Random.Range(minGapY, maxGapY);
            Vector3 spawnPos = new Vector3(spawnX, 0.0f, 0.0f);

            GameObject pipeObj;
            if (pipePairPrefab != null)
            {
                pipeObj = Instantiate(pipePairPrefab, spawnPos, Quaternion.identity);
            }
            else
            {
                // Procedural fallback if no prefab is assigned
                pipeObj = CreateProceduralPipePair(spawnPos);
            }

            FlappyPipePair pipePair = pipeObj.GetComponent<FlappyPipePair>();
            if (pipePair != null)
            {
                pipePair.SetupPipes(gapCenterY, gapSize, pipeSpeed);
            }

            activePipes.Add(pipeObj);
        }

        /// <summary>
        /// Creates a procedural pipe pair with Unity primitives if no prefab was pre-assigned.
        /// Accounts for Unity center pivots automatically.
        /// </summary>
        private GameObject CreateProceduralPipePair(Vector3 position)
        {
            GameObject root = new GameObject("PipePair_Procedural");
            root.transform.position = position;

            // Top pipe cylinder
            GameObject top = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            top.name = "TopPipe";
            top.tag = "Pipe";
            top.transform.SetParent(root.transform);
            Renderer topRend = top.GetComponent<Renderer>();
            if (topRend != null) topRend.material.color = new Color(0.18f, 0.78f, 0.22f);

            // Bottom pipe cylinder
            GameObject bottom = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            bottom.name = "BottomPipe";
            bottom.tag = "Pipe";
            bottom.transform.SetParent(root.transform);
            Renderer botRend = bottom.GetComponent<Renderer>();
            if (botRend != null) botRend.material.color = new Color(0.18f, 0.78f, 0.22f);

            // Trigger box
            GameObject trigger = new GameObject("ScoreTrigger");
            trigger.transform.SetParent(root.transform);
            BoxCollider box = trigger.AddComponent<BoxCollider>();
            box.isTrigger = true;

            FlappyPipePair pipePair = root.AddComponent<FlappyPipePair>();
            // Use reflection or serialized fields via helper
            typeof(FlappyPipePair).GetField("topPipeTransform", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance)?.SetValue(pipePair, top.transform);
            typeof(FlappyPipePair).GetField("bottomPipeTransform", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance)?.SetValue(pipePair, bottom.transform);
            typeof(FlappyPipePair).GetField("scoreTriggerCollider", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance)?.SetValue(pipePair, box);

            return root;
        }

        public void ClearAllPipes()
        {
            foreach (GameObject pipe in activePipes)
            {
                if (pipe != null)
                {
                    Destroy(pipe);
                }
            }
            activePipes.Clear();
            spawnTimer = 0.0f;
        }
    }
}
