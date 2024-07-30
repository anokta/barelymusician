using System;
using System.Runtime.InteropServices;

namespace Barely {
  /// Control definition.
  [StructLayout(LayoutKind.Sequential)]
  public struct ControlDefinition {
    /// Identifier.
    public Int32 controlId;

    /// Default value.
    public double defaultValue;

    /// Minimum value.
    public double minValue;

    /// Maximum value.
    public double maxValue;
  }
}  // namespace Barely
