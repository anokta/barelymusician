using UnityEngine;

namespace Barely.Examples {
  public class ApplicationQuit : MonoBehaviour {
    private void Update() {
      if (Input.GetKeyDown(KeyCode.Escape)) {
        Application.Quit();
      }
    }
  }
}  // namespace Barely.Examples
