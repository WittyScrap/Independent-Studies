Shader "Hidden/PSOBackground"
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
            #define m 0.5f

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
                const float4 lowEnd = float4(m, m, 1, 1);
                const float4 midEnd = float4(m, 1, m, 1);
                const float4 highEnd = float4(1, m, m, 1);

                float x = (i.uv.x * 2 - 1) * 3;
                float y = (i.uv.y * 2 - 1) * 3;

                float fn = saturate(cos(x * y) + sin(x * y));

                float4 lowLerp = lerp(lowEnd, midEnd, saturate(fn * 2));
                float4 highLerp = lerp(midEnd, highEnd, saturate(fn / 2 + .5f));

                return lerp(lowLerp, highLerp, fn) + tex2D(_MainTex, i.uv) - (((i.uv.x * 5000) % 200 < 10) | ((i.uv.y * 5000) % 200 < 10)) * .05f;
            }
            ENDCG
        }
    }
}
