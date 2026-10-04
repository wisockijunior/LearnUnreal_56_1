using UnityEngine;

namespace LearnUnreal.Flappy
{
    /// <summary>
    /// Flappy Bird Controller for Unity3D.
    /// Handles flap impulse, gravitational descent, pitch tilt rotation, and collision detection.
    /// Direct equivalent to Unreal Engine's AFlappyBirdPawn.
    /// </summary>
    [RequireComponent(typeof(SphereCollider))]
    public class BirdController : MonoBehaviour
    {
        [Header("Physics Settings")]
        [Tooltip("Upward impulse added to velocity when flapping (in m/s).")]
        [SerializeField] private float flapStrength = 8.5f;

        [Tooltip("Downward gravitational acceleration (in m/s^2).")]
        [SerializeField] private float gravity = 24.0f;

        [Header("Bounds")]
        [Tooltip("Floor Y position where the bird crashes.")]
        [SerializeField] private float floorY = -4.5f;

        [Tooltip("Ceiling Y position limiting maximum altitude.")]
        [SerializeField] private float ceilingY = 5.0f;

        [Header("Tilt / Visuals")]
        [Tooltip("Maximum upward tilt angle when flapping (degrees).")]
        [SerializeField] private float maxUpPitch = 25.0f;

        [Tooltip("Maximum downward dive angle when falling (degrees).")]
        [SerializeField] private float maxDownPitch = -75.0f;

        [Tooltip("Smoothing speed for pitch tilt transitions.")]
        [SerializeField] private float pitchSpeed = 8.0f;

        private float verticalVelocity = 0.0f;
        private bool isDead = false;
        private Vector3 startPosition;
        private Quaternion startRotation;

        public bool IsDead => isDead;
        public float VerticalVelocity => verticalVelocity;

        private void Awake()
        {
            startPosition = transform.position;
            startRotation = transform.rotation;
        }

        private void Update()
        {
            if (FlappyGameManager.Instance != null && FlappyGameManager.Instance.CurrentState == FlappyGameManager.GameState.Ready)
            {
                // Idle gentle hovering bobbing motion before game starts
                float hoverOffset = Mathf.Sin(Time.time * 5.0f) * 0.15f;
                transform.position = startPosition + new Vector3(0.0f, hoverOffset, 0.0f);
                transform.rotation = Quaternion.identity;

                if (Input.GetKeyDown(KeyCode.Space) || Input.GetMouseButtonDown(0) || Input.GetKeyDown(KeyCode.UpArrow))
                {
                    FlappyGameManager.Instance.StartGame();
                    Flap();
                }
                return;
            }

            if (isDead)
            {
                // After death, bird falls to ground if still in air
                if (transform.position.y > floorY)
                {
                    verticalVelocity -= gravity * Time.deltaTime;
                    transform.position += new Vector3(0.0f, verticalVelocity * Time.deltaTime, 0.0f);
                    transform.rotation = Quaternion.Euler(0.0f, 0.0f, maxDownPitch);
                }
                else
                {
                    transform.position = new Vector3(transform.position.x, floorY, transform.position.z);
                }

                if (Input.GetKeyDown(KeyCode.R) || Input.GetKeyDown(KeyCode.Space))
                {
                    FlappyGameManager.Instance?.RestartGame();
                }
                return;
            }

            // --- Active Gameplay Physics Simulation ---
            if (Input.GetKeyDown(KeyCode.Space) || Input.GetMouseButtonDown(0) || Input.GetKeyDown(KeyCode.UpArrow))
            {
                Flap();
            }

            // Apply gravity
            verticalVelocity -= gravity * Time.deltaTime;

            // Update Y position
            Vector3 pos = transform.position;
            pos.y += verticalVelocity * Time.deltaTime;

            // Check Ceiling clamp
            if (pos.y > ceilingY)
            {
                pos.y = ceilingY;
                verticalVelocity = 0.0f;
            }

            // Check Ground collision
            if (pos.y <= floorY)
            {
                pos.y = floorY;
                transform.position = pos;
                Die();
                return;
            }

            transform.position = pos;

            // Pitch tilt simulation (Z rotation in Unity 2D/side-view 3D)
            float targetPitch = verticalVelocity > 0.0f
                ? Mathf.Lerp(0.0f, maxUpPitch, verticalVelocity / flapStrength)
                : Mathf.Lerp(0.0f, maxDownPitch, -verticalVelocity / (flapStrength * 1.5f));

            Quaternion targetRotation = Quaternion.Euler(0.0f, 0.0f, targetPitch);
            transform.rotation = Quaternion.Slerp(transform.rotation, targetRotation, pitchSpeed * Time.deltaTime);
        }

        public void Flap()
        {
            if (isDead) return;
            verticalVelocity = flapStrength;
        }

        public void Die()
        {
            if (isDead) return;
            isDead = true;
            FlappyGameManager.Instance?.OnBirdDied();
        }

        public void ResetBird()
        {
            transform.position = startPosition;
            transform.rotation = startRotation;
            verticalVelocity = 0.0f;
            isDead = false;
        }

        private void OnTriggerEnter(Collider other)
        {
            if (isDead) return;

            if (other.CompareTag("Pipe") || other.CompareTag("Ground"))
            {
                Die();
            }
        }

        private void OnCollisionEnter(Collision collision)
        {
            if (isDead) return;

            if (collision.gameObject.CompareTag("Pipe") || collision.gameObject.CompareTag("Ground"))
            {
                Die();
            }
        }
    }
}
