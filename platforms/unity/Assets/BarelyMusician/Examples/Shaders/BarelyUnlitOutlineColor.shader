Shader "BarelyMusician/Examples/Unlit/Outline Color" {
  Properties {
    _Color ("Color", Color) = (1,1,1)
    _OutlineColor ("Outline color", Color) = (0.0, 0.0, 0.0,1.0)
    _OutlineWidth ("Outline width", Float) = 1.0
  }

  SubShader {
    Tags {
      "RenderPipeline"="UniversalPipeline"
      "RenderType"="Opaque"
      "Queue"="Geometry"
    }

    Pass {
      Name "Forward"
      Tags { "LightMode"="UniversalForward" }

      HLSLPROGRAM
      #include "UnityCG.cginc"
      #pragma vertex vert
      #pragma fragment frag

      CBUFFER_START(UnityPerMaterial)
      float4 _Color;
      float4 _OutlineColor;
      float _OutlineWidth;
      CBUFFER_END

      struct appdata_t {
        float4 vertex : POSITION;
      };

      struct v2f {
        float4 pos : SV_POSITION;
      };

      v2f vert(appdata_t v) {
        v2f o;
        o.pos = UnityObjectToClipPos(v.vertex);
        return o;
      }

      half4 frag(v2f i) : SV_Target {
        return _Color;
      }
      ENDHLSL
    }

    Pass {
      Name "Outline"
      Tags { "LightMode"="SRPDefaultUnlit" }
      Cull Front
      ZWrite On

      HLSLPROGRAM
      #include "UnityCG.cginc"
      #pragma vertex vert
      #pragma fragment frag

      CBUFFER_START(UnityPerMaterial)
      float4 _Color;
      float4 _OutlineColor;
      float _OutlineWidth;
      CBUFFER_END

      struct appdata_t {
        float4 vertex : POSITION;
        float3 normal : NORMAL;
      };

      struct v2f {
        float4 pos : SV_POSITION;
      };

      v2f vert(appdata_t v) {
        v2f o;
        v.vertex.xyz += normalize(v.normal) * _OutlineWidth * 0.01;
        o.pos = UnityObjectToClipPos(v.vertex);
        return o;
      }

      half4 frag(v2f i) : SV_Target {
        return _OutlineColor;
      }
      ENDHLSL
    }
  }

  SubShader {
    Tags { "RenderType"="Opaque" }

    Lighting Off
    Fog { Mode Off }

    Color [_Color]
    Pass {}

    Pass {
	    Cull Front
	    ZWrite On

	    CGPROGRAM
      #include "UnityCG.cginc"
      #pragma fragmentoption ARB_precision_hint_fastest
      #pragma glsl_no_auto_normalization
      #pragma vertex vert
      #pragma fragment frag

	    struct appdata_t {
			float4 vertex : POSITION;
			float3 normal : NORMAL;
		  };

		  struct v2f {
			  float4 pos : SV_POSITION;
		  };

	    fixed _OutlineWidth;
	    v2f vert (appdata_t v) {
	      v2f o;
		    o.pos = v.vertex;
		    o.pos.xyz += normalize(v.normal.xyz) * _OutlineWidth * 0.01;
		    o.pos = UnityObjectToClipPos(o.pos);
		    return o;
	    }

	    fixed4 _OutlineColor;
	    fixed4 frag(v2f i) : COLOR {
	    	return _OutlineColor;
		  }
	    ENDCG
	  }
  }
  FallBack "Unlit/Color"
}
