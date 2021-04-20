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
            #include "BeeVisualisation.cginc"

            #define BaseColor float4(1, 1, 1, 1)
            #define MinimumBrightness 0.25f
            #define MaximumBrightness 0.5f
            #define ParticleBrightness 0.5f


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
            sampler2D _PSO;
            float4 _Options[5];
            float4 _Colors[5];
            float _Fade;
            int _TotalOptions;
            int _OutputSize;

            #include "SolutionSpace.cginc"
            #define m 0.5f

            fixed4 frag (v2f i) : SV_Target
            {
                float circleRadius = (float)OptionRadius / (_OutputSize * 2);

                float fn = saturate(fnSolutionSpace(i.uv, 1));
                float4 circleColors = lerp(MinimumBrightness, MaximumBrightness, fn);

                for (int p = 0; p < _TotalOptions; p += 1)
                {
                    circleColors += (length(i.uv - _Options[p].xy) < circleRadius) * _Colors[p];
                }
                
                float4 particles = tex2D(_MainTex, i.uv);
                float grid = (((i.uv.x * 5000) % 200 < 10) | ((i.uv.y * 5000) % 200 < 10)) * .025f;
                
                const float4 lowEnd = float4(m, m, 1, 1);
                const float4 midEnd = float4(m, 1, m, 1);
                const float4 highEnd = float4(1, m, m, 1);

                float4 lowLerp = lerp(lowEnd, midEnd, saturate(fn * 2));
                float4 highLerp = lerp(midEnd, highEnd, saturate(fn / 2 + .5f));
                float4 colorful = lerp(lowLerp, highLerp, fn);

                float4 baseColor = lerp(colorful, circleColors, saturate(_Fade));

                return baseColor + particles * ParticleBrightness - grid;
            }
            ENDCG
        }
    }
}
