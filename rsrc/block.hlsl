#pragma pack_matrix(row_major)



cbuffer core:register(b0){
	float4 cd;
	matrix cm;
	matrix pm;
};



cbuffer extra:register(b1){
	matrix tm;
	matrix rm;
	float4 c;
};



struct VS_OUT{
	float4 p:SV_POSITION;
	float3 n:NORMAL;
};



VS_OUT block_vs(float4 p:SV_POSITION,float3 n:NORMAL){
	VS_OUT o={
		mul(mul(mul(p,tm),cm),pm),
		mul(n,(float3x3)rm)
	};
	return o;
}



float4 block_ps(float4 p:SV_POSITION,float3 n:NORMAL):SV_Target{
	return c*saturate(dot(n,cd.xyz));
}
