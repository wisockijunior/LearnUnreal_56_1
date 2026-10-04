using UnityEngine;

namespace LearnUnreal.Flappy
{
    /// <summary>
    /// Flappy Bird Immediate-Mode HUD for Unity.
    /// Uses OnGUI for zero-setup screen rendering.
    /// Direct equivalent to Unreal Engine's AFlappyHUD.
    /// </summary>
    public class FlappyHUD : MonoBehaviour
    {
        private GUIStyle titleStyle;
        private GUIStyle scoreStyle;
        private GUIStyle subStyle;
        private bool stylesInitialized = false;

        private void InitStyles()
        {
            if (stylesInitialized) return;

            titleStyle = new GUIStyle(GUI.skin.label)
            {
                fontSize = 38,
                fontStyle = FontStyle.Bold,
                alignment = TextAnchor.MiddleCenter
            };
            titleStyle.normal.textColor = Color.white;

            scoreStyle = new GUIStyle(GUI.skin.label)
            {
                fontSize = 44,
                fontStyle = FontStyle.Bold,
                alignment = TextAnchor.UpperCenter
            };
            scoreStyle.normal.textColor = Color.yellow;

            subStyle = new GUIStyle(GUI.skin.label)
            {
                fontSize = 20,
                alignment = TextAnchor.MiddleCenter
            };
            subStyle.normal.textColor = new Color(0.9f, 0.9f, 0.9f, 0.95f);

            stylesInitialized = true;
        }

        private void OnGUI()
        {
            InitStyles();

            FlappyGameManager gm = FlappyGameManager.Instance;
            if (gm == null) return;

            float screenW = Screen.width;
            float screenH = Screen.height;

            switch (gm.CurrentState)
            {
                case FlappyGameManager.GameState.Ready:
                    DrawShadowText(new Rect(0, screenH * 0.28f, screenW, 50), "FLAPPY BIRD", titleStyle);
                    DrawShadowText(new Rect(0, screenH * 0.40f, screenW, 35), "Press SPACE / CLICK / TAP to Flap", subStyle);
                    DrawShadowText(new Rect(0, screenH * 0.48f, screenW, 30), $"High Score: {gm.HighScore}", subStyle);
                    break;

                case FlappyGameManager.GameState.Playing:
                    DrawShadowText(new Rect(0, 30, screenW, 60), gm.CurrentScore.ToString(), scoreStyle);
                    break;

                case FlappyGameManager.GameState.GameOver:
                    DrawShadowText(new Rect(0, screenH * 0.25f, screenW, 50), "GAME OVER", titleStyle, Color.red);
                    DrawShadowText(new Rect(0, screenH * 0.36f, screenW, 40), $"Score: {gm.CurrentScore}", scoreStyle);
                    DrawShadowText(new Rect(0, screenH * 0.44f, screenW, 30), $"High Score: {gm.HighScore}", subStyle);
                    DrawShadowText(new Rect(0, screenH * 0.54f, screenW, 35), "Press R or SPACE to Restart", subStyle);
                    break;
            }
        }

        private void DrawShadowText(Rect rect, string text, GUIStyle style, Color? customTextColor = null)
        {
            Color originalColor = style.normal.textColor;
            Color textColor = customTextColor ?? originalColor;

            // Draw shadow offset
            Rect shadowRect = new Rect(rect.x + 2, rect.y + 2, rect.width, rect.height);
            style.normal.textColor = Color.black;
            GUI.Label(shadowRect, text, style);

            // Draw primary text
            style.normal.textColor = textColor;
            GUI.Label(rect, text, style);

            style.normal.textColor = originalColor;
        }
    }
}
