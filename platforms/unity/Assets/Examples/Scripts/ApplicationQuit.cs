using UnityEngine;

namespace Barely {
  namespace Examples {
    public class ApplicationQuit : MonoBehaviour {
      private void Update() {
        if (Input.GetKeyDown(KeyCode.Escape)) {
          Application.Quit();
        }
      }
    }
  }  // namespace Examples
}  // namespace Barely
