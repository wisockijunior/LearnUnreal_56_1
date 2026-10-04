using UnityEngine;

namespace LearnUnreal.Tetris
{
    /// <summary>
    /// Tetris HUD in Unity3D.
    /// Direct equivalent to Unreal Engine's ATetrisHUD.
    /// </summary>
    public class TetrisHUD : MonoBehaviour
    {
        [SerializeField] private TetrisBoard board;

        private GUIStyle titleStyle;
        private GUIStyle headerStyle;
        private GUIStyle valueStyle;
        private GUIStyle smallStyle;
        private bool stylesInitialized = false;

        private void Awake()
        {
            if (board == null)
            {
                board = FindObjectOfType<TetrisBoard>();
            }
        }

        private void InitStyles()
        {
            if (stylesInitialized) return;

            titleStyle = new GUIStyle(GUI.skin.label)
            {
                fontSize = 28,
                fontStyle = FontStyle.Bold,
                alignment = TextAnchor.UpperLeft
            };
            titleStyle.normal.textColor = Color.yellow;

            headerStyle = new GUIStyle(GUI.skin.label)
            {
                fontSize = 18,
                fontStyle = FontStyle.Bold,
                alignment = TextAnchor.UpperLeft
            };
            headerStyle.normal.textColor = new Color(0.7f, 0.85f, 1.0f);

            valueStyle = new GUIStyle(GUI.skin.label)
            {
                fontSize = 24,
                fontStyle = FontStyle.Bold,
                alignment = TextAnchor.UpperLeft
            };
            valueStyle.normal.textColor = Color.white;

            smallStyle = new GUIStyle(GUI.skin.label)
            {
                fontSize = 14,
                alignment = TextAnchor.UpperLeft
            };
            smallStyle.normal.textColor = new Color(0.8f, 0.8f, 0.8f);

            stylesInitialized = true;
        }

        private void OnGUI()
        {
            InitStyles();
            if (board == null) return;

            float panelX = 40;
            float panelY = 40;

            // Header Title
            GUI.Label(new Rect(panelX, panelY, 200, 35), "TETRIS", titleStyle);
            panelY += 45;

            // Score
            GUI.Label(new Rect(panelX, panelY, 200, 25), "SCORE", headerStyle);
            GUI.Label(new Rect(panelX, panelY + 22, 200, 30), board.Score.ToString(), valueStyle);
            panelY += 60;

            // Lines Cleared
            GUI.Label(new Rect(panelX, panelY, 200, 25), "LINES", headerStyle);
            GUI.Label(new Rect(panelX, panelY + 22, 200, 30), board.LinesCleared.ToString(), valueStyle);
            panelY += 60;

            // Level
            GUI.Label(new Rect(panelX, panelY, 200, 25), "LEVEL", headerStyle);
            GUI.Label(new Rect(panelX, panelY + 22, 200, 30), board.Level.ToString(), valueStyle);
            panelY += 75;

            // Controls Box
            GUI.Box(new Rect(panelX, panelY, 240, 145), "");
            GUI.Label(new Rect(panelX + 10, panelY + 8, 220, 25), "CONTROLS", headerStyle);
            GUI.Label(new Rect(panelX + 10, panelY + 32, 220, 20), "A / D / Arrows : Move", smallStyle);
            GUI.Label(new Rect(panelX + 10, panelY + 52, 220, 20), "W / Up Arrow : Rotate", smallStyle);
            GUI.Label(new Rect(panelX + 10, panelY + 72, 220, 20), "S / Down Arrow : Soft Drop", smallStyle);
            GUI.Label(new Rect(panelX + 10, panelY + 92, 220, 20), "Space / Enter : Hard Drop", smallStyle);
            GUI.Label(new Rect(panelX + 10, panelY + 112, 220, 20), "R : Restart", smallStyle);

            // Game Over Banner
            if (board.IsGameOver)
            {
                float midX = Screen.width * 0.5f;
                float midY = Screen.height * 0.5f;

                GUIStyle goStyle = new GUIStyle(titleStyle)
                {
                    fontSize = 42,
                    alignment = TextAnchor.MiddleCenter
                };
                goStyle.normal.textColor = Color.red;

                GUIStyle goSub = new GUIStyle(smallStyle)
                {
                    fontSize = 20,
                    alignment = TextAnchor.MiddleCenter
                };

                GUI.Label(new Rect(midX - 200, midY - 60, 400, 50), "GAME OVER", goStyle);
                GUI.Label(new Rect(midX - 200, midY, 400, 35), "Press R to Play Again", goSub);
            }
        }
    }
}
