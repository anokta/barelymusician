Shader "BarelyMusician/Examples/Unlit/Color Transparent" {
  Properties {
    _Color ("Color", Color) = (1.0, 1.0, 1.0, 1.0)
  }

  SubShader {
    Tags {
    "Queue"="Transparent"
    "RenderType"="Transparent"
    }

    Cull back
    ZWrite Off
    Blend SrcAlpha OneMinusSrcAlpha
    Lighting Off
    Fog { Mode Off }

    Color [_Color]
    Pass {}
  }
  Fallback "Unlit/Color"
}
