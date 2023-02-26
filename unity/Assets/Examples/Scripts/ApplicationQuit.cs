using UnityEngine;

public class ApplicationQuit : MonoBehaviour {
  private void Update() {
    if (Input.GetKeyDown(KeyCode.Escape)) {
      Application.Quit();
    }
  }
}
