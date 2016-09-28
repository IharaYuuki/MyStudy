float4x4 mWVP;// : WORLDVIEWPROJECTION;
float4x4 mWorld;// : WORLD;

float4 matDiffuse;// : MATERIALDIFFUSE;
float4 matAmbient;// : MATERIALAMBIENT;

float3 lightDir0;// : LIGHTDIRECTION;
float3 lightCol0;// : LIGHTCOLOR;
float3 lightDir1;// : LIGHTDIRECTION;
float3 lightCol1;// : LIGHTCOLOR;
float3 lightDir2;// : LIGHTDIRECTION;
float3 lightCol2;// : LIGHTCOLOR;

texture gTex0;// : TEXTURE0;
sampler2D tex0 = sampler_state
{
	Texture = (gTex0);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float4 Col : COLOR0;
	float2 TexUV : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float4 Col : COLOR0;
	float2 TexUV : TEXCOORD0;
};

float CalcLightingDot(float4 nor, float3 lightDir)
{
	// conversion this normal to it in World Coordinates
	float3 N = mul(nor, mWorld).xyz;
	float3 baseN = mul(float4(0, 0, 0, 1), mWorld).xyz;
	N = normalize(N - baseN);
	return max( 0, dot(-lightDir, N) ); // if dot(-lightDir0, N) is less than 0 , max() makes it 0.
}

VS_OUTPUT VS_main(VS_INPUT In)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	
	Out.Pos = mul(In.Pos, mWVP);
	Out.TexUV = In.TexUV;

	float4 ReflectionColor;
	ReflectionColor.r = matDiffuse.r;//In.Col.r * matDiffuse.r;
	ReflectionColor.g = matDiffuse.g;//In.Col.g * matDiffuse.g;
	ReflectionColor.b = matDiffuse.b;//In.Col.b * matDiffuse.b;
	                   
	float4 VisibleColor;
	VisibleColor.r = ReflectionColor.r * 
					(lightCol0.r * CalcLightingDot(In.Normal, lightDir0) +
					lightCol1.r * CalcLightingDot(In.Normal, lightDir1) +
					lightCol2.r * CalcLightingDot(In.Normal, lightDir2));
	VisibleColor.g = ReflectionColor.g * 
					(lightCol0.g * CalcLightingDot(In.Normal, lightDir0) +
					lightCol1.g * CalcLightingDot(In.Normal, lightDir1) +
					lightCol2.g * CalcLightingDot(In.Normal, lightDir2));
	VisibleColor.b = ReflectionColor.b * 
					(lightCol0.b * CalcLightingDot(In.Normal, lightDir0) +
					lightCol1.b * CalcLightingDot(In.Normal, lightDir1) +
					lightCol2.b * CalcLightingDot(In.Normal, lightDir2));
					
	//VisibleColor.r = clamp( VisibleColor.r, 0.0f, matDiffuse.r );
	//VisibleColor.g = clamp( VisibleColor.g, 0.0f, matDiffuse.g );
	//VisibleColor.b = clamp( VisibleColor.b, 0.0f, matDiffuse.b );
	
	Out.Col.rgb = VisibleColor.rgb;// + matAmbient.rgb * 0.5f;
	Out.Col.a = In.Col.a;
	
	return Out;
}

float4 PS_main(VS_OUTPUT In) : COLOR0
{
	return In.Col; // * tex2D(tex0, In.TexUV);
}


technique MainShader
{
	pass P0
	{
		//AlphaBlendEnable = true;
		//SrcBlend = SrcAlpha;
		//DestBlend = InvSrcAlpha;
		CullMode = None;
		VertexShader = compile vs_2_0 VS_main();
		PixelShader = compile ps_2_0 PS_main();
	}
}