using UnityEngine;
using UnityEditor;

[CreateAssetMenu(fileName = "NewAudioPlayer", menuName = "Audio/Audio Player")]
public class AudioPlayer : ScriptableObject
{
    [Header("Audio Clip")]
    public AudioClip audioClip;

    [Range(0.0f, 1.0f)]
    public float volume = 1.0f;

    private AudioSource previewAudioSource;

    public void PlayClip(float pitch = 1.0f)
    {
        if (audioClip == null)
        {
            Debug.LogWarning("No AudioClip assigned to play.");
            return;
        }

        if (previewAudioSource == null)
        {
            GameObject previewObject = new GameObject("AudioClipPreview");
            previewObject.hideFlags = HideFlags.HideAndDontSave;
            previewAudioSource = previewObject.AddComponent<AudioSource>();
        }

        previewAudioSource.clip = audioClip;
        previewAudioSource.volume = volume;
        previewAudioSource.pitch = pitch;
        previewAudioSource.Play();
    }

    public void StopClip()
    {
        if (previewAudioSource != null && previewAudioSource.isPlaying)
        {
            previewAudioSource.Stop();
        }
    }

    private void OnDisable()
    {
        StopClip(); // Ensure the clip stops when entering Play mode or exiting Edit mode
    }
}

#if UNITY_EDITOR
[CustomEditor(typeof(AudioPlayer))]
public class AudioPlayerEditor : Editor
{
    private bool[] keyStates = new bool[13]; // One octave + root note (C to C)
    private readonly string[] keyLabels = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C" };
    private int lastPlayedKey = -1;

    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        AudioPlayer player = (AudioPlayer)target;

        GUILayout.Space(10);

        if (GUILayout.Button("Play"))
        {
            player.PlayClip();
        }

        if (GUILayout.Button("Stop"))
        {
            player.StopClip();
        }

        GUILayout.Space(20);
        DrawGraphicalPianoRoll(player);
    }

    private void DrawGraphicalPianoRoll(AudioPlayer player)
    {
        GUILayout.Label("Piano Roll (One Octave + Root Note)", EditorStyles.boldLabel);

        Rect pianoRect = GUILayoutUtility.GetRect(0, 120, GUILayout.ExpandWidth(true));
        float whiteKeyWidth = pianoRect.width / 8; // 8 white keys (C to C)
        float whiteKeyHeight = 100;
        float blackKeyHeight = whiteKeyHeight * 0.6f;
        float blackKeyWidth = whiteKeyWidth * 0.6f;

        // Draw white keys
        for (int i = 0; i < 8; i++) // 8 white keys
        {
            int noteIndex = i * 2 - (i > 2 ? 1 : 0); // Map to white keys only
            Rect keyRect = new Rect(pianoRect.x + i * whiteKeyWidth, pianoRect.y, whiteKeyWidth, whiteKeyHeight);

            DrawKey(player, keyRect, noteIndex, Color.white);
        }

        // Draw black keys
        for (int i = 0; i < 7; i++) // 7 black keys
        {
            if (i == 2 || i == 6) continue; // Skip E and B (no black keys above)

            int noteIndex = i * 2 + 1; // Map to black keys
            Rect keyRect = new Rect(
                pianoRect.x + (i + 1) * whiteKeyWidth - blackKeyWidth / 2,
                pianoRect.y,
                blackKeyWidth,
                blackKeyHeight
            );

            DrawKey(player, keyRect, noteIndex, Color.black);
        }

        // Stop all notes when the mouse is released
        if (Event.current.type == EventType.MouseUp)
        {
            for (int i = 0; i < keyStates.Length; i++)
            {
                keyStates[i] = false;
            }
            player.StopClip();
            lastPlayedKey = -1;
        }
    }

    private void DrawKey(AudioPlayer player, Rect keyRect, int noteIndex, Color keyColor)
    {
        Color originalColor = GUI.color;

        // Highlight pressed key
        if (keyStates[noteIndex])
        {
            GUI.color = Color.green;
        }
        else
        {
            GUI.color = keyColor;
        }

        // Draw key
        if (GUI.Button(keyRect, GUIContent.none))
        {
            PlayNoteIfNeeded(player, noteIndex);
        }

        GUI.color = originalColor;

        // Draw key border
        Handles.DrawSolidRectangleWithOutline(keyRect, Color.clear, Color.black);

        // Handle dragging between keys
        if (Event.current.type == EventType.MouseDrag && keyRect.Contains(Event.current.mousePosition))
        {
            PlayNoteIfNeeded(player, noteIndex);
        }
    }

    private void PlayNoteIfNeeded(AudioPlayer player, int noteIndex)
    {
        if (lastPlayedKey != noteIndex)
        {
            keyStates[noteIndex] = true;
            float pitch = Mathf.Pow(2.0f, (noteIndex - 12) / 12.0f); // Calculate pitch relative to middle C
            player.PlayClip(pitch);
            lastPlayedKey = noteIndex;
        }
    }

    private void OnDisable()
    {
        AudioPlayer player = (AudioPlayer)target;
        player.StopClip(); // Ensure the clip stops when entering Play mode or exiting Edit mode
    }
}
#endif
