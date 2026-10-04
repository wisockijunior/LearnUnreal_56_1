using UnityEngine;

namespace LearnUnreal.Flappy
{
    /// <summary>
    /// Flappy Bird Game Manager in Unity3D.
    /// Manages game states (Ready, Playing, GameOver), score, high scores (PlayerPrefs), and restart flow.
    /// Direct equivalent to Unreal Engine's AFlappyGameMode.
    /// </summary>
    public class FlappyGameManager : MonoBehaviour
    {
        public enum GameState
        {
            Ready,
            Playing,
            GameOver
        }

        public static FlappyGameManager Instance { get; private set; }

        [Header("State")]
        [SerializeField] private GameState currentState = GameState.Ready;

        [Header("Scores")]
        [SerializeField] private int currentScore = 0;
        [SerializeField] private int highScore = 0;

        [Header("References")]
        [SerializeField] private BirdController bird;
        [SerializeField] private FlappySpawner spawner;

        public GameState CurrentState => currentState;
        public int CurrentScore => currentScore;
        public int HighScore => highScore;

        private const string HIGH_SCORE_KEY = "FlappyBird_HighScore";

        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(gameObject);
                return;
            }
            Instance = this;

            highScore = PlayerPrefs.GetInt(HIGH_SCORE_KEY, 0);

            if (bird == null) bird = FindObjectOfType<BirdController>();
            if (spawner == null) spawner = FindObjectOfType<FlappySpawner>();
        }

        private void Start()
        {
            ResetGame();
        }

        public void StartGame()
        {
            if (currentState == GameState.Ready)
            {
                currentState = GameState.Playing;
            }
        }

        public void AddScore(int amount = 1)
        {
            if (currentState != GameState.Playing) return;

            currentScore += amount;
            if (currentScore > highScore)
            {
                highScore = currentScore;
                PlayerPrefs.SetInt(HIGH_SCORE_KEY, highScore);
                PlayerPrefs.Save();
            }
        }

        public void OnBirdDied()
        {
            if (currentState == GameState.Playing)
            {
                currentState = GameState.GameOver;
            }
        }

        public void RestartGame()
        {
            ResetGame();
        }

        private void ResetGame()
        {
            currentState = GameState.Ready;
            currentScore = 0;

            if (bird != null)
            {
                bird.ResetBird();
            }

            if (spawner != null)
            {
                spawner.ClearAllPipes();
            }
        }
    }
}
