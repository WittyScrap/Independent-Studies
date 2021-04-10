Shader "Hidden/BeesBackground"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
    }
    SubShader
    {
        // No culling or depth
        Cull Off ZWrite Off ZTest Always

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"
            #define PointRadius .05f
            #define BaseColor float4(1, 1, 1, 1)
            #define CircleColor float4(1, 0.75f, 0.75f, 1)

            float4 _Options[5];
            int _TotalOptions;

            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                float4 vertex : SV_POSITION;
            };

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                return o;
            }

            sampler2D _MainTex;

            fixed4 frag (v2f i) : SV_Target
            {
                int inCircle = 0;

                for (int x = 0; x < _TotalOptions; x += 1)
                {
                    inCircle |= length(i.uv - _Options[x].xy) < PointRadius;
                }

                return lerp(BaseColor, CircleColor, inCircle) - tex2D(_MainTex, i.uv);
            }
            ENDCG
        }
    }
}
