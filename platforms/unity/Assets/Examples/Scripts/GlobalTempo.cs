using UnityEngine;

namespace Barely {
  namespace Examples {
    [DisallowMultipleComponent]
    public class GlobalTempo : MonoBehaviour {
      /// Global tempo in beats.
      [Range(30, 960)]
      public int tempo = 120;

      private void Start() {
        Musician.Tempo = tempo;
      }

      private void Update() {
        Musician.Tempo = tempo;
      }
    }
  }  // namespace Examples
}  // namespace Barely
