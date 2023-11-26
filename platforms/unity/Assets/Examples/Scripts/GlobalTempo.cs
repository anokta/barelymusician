using UnityEngine;

namespace Barely {
  namespace Examples {
    [DisallowMultipleComponent]
    public class GlobalTempo : MonoBehaviour {
      /// Global tempo in beats.
      [Range(30.0f, 960.0f)]
      public double tempo = 120.0;

      private void Start() {
        Musician.Tempo = tempo;
      }

      private void Update() {
        Musician.Tempo = tempo;
      }
    }
  }  // namespace Examples
}  // namespace Barely
