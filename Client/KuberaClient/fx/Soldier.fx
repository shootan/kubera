//--------------------------------------------------------------------------------------
// File: Soldier.fx
//
// The effect file for the Skinning10 sample.  
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// defines
//--------------------------------------------------------------------------------------
#define MAX_BONE_MATRICES 70 
#define FT_CONSTANTBUFFER 0
#define FT_TEXTUREBUFFER 1
#define FT_TEXTURE 2
#define FT_BUFFER 3




//--------------------------------------------------------------------------------------
// Input/Output structures
//--------------------------------------------------------------------------------------
struct VSSkinnedIn
{
    float3 Pos                      : POSITION;             //Position
	float3 Norm                     : NORMAL;               //Normal
	float2 Tex                      : TEXCOORD;             //Texture coordinate
	float3 Tan                      : TANGENT;              //Normalized Tangent vector
	float3 Binorm				   : BINORMAL;
	uint4 Bones                    : BONES;                //Bone indices
    float4 Weights                  : WEIGHTS;              //Bone weights
};

struct PSSkinnedIn
{
    float4 Pos                      : SV_Position;          //Position
    float3 vPos                     : POSWORLD;             //world space Pos
    float3 Norm                     : NORMAL;               //Normal
    float2 Tex                      : TEXCOORD;             //Texture coordinate
    float3 Tangent                  : TANGENT;              //Normalized Tangent vector
};

//--------------------------------------------------------------------------------------
// Constant buffers
//--------------------------------------------------------------------------------------
cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};


// Constant buffer for bone matrices
cbuffer cbAnimMatrices : register (b2)
{
    matrix m_matConstBoneWorld[MAX_BONE_MATRICES];
};


//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------
Texture2D g_txDiffuse : register( t0 );
SamplerState g_samLinear : register( s0 );
Texture2D g_txNormal;
// Texture for bone matrices
Texture1D g_txTexBoneWorld;



//--------------------------------------------------------------------------------------
// Helper struct for passing back skinned vertex information
//--------------------------------------------------------------------------------------
struct SkinnedInfo
{
    float4 Pos;
    float3 Norm;
    float3 Tan;
};

//--------------------------------------------------------------------------------------
// FetchBoneTransform fetches a bone transformation using one of several methods
//
//        FT_CONSTANTBUFFER:
//            With this approach, the bone matrices are stored in a constant buffer.
//            The shader will index into the constant buffer to grab the correct
//            transformation matrices for each vertex.
//--------------------------------------------------------------------------------------
matrix FetchBoneTransform( uint iBone )
{
    matrix mret;
       mret = m_matConstBoneWorld[ iBone ];    
    return mret;
}

//--------------------------------------------------------------------------------------
// SkinVert skins a single vertex
//--------------------------------------------------------------------------------------
SkinnedInfo SkinVert( VSSkinnedIn Input)
{
    SkinnedInfo Output = (SkinnedInfo)0;
    
    float4 Pos = float4(Input.Pos, 1);
    float3 Norm = Input.Norm;
    float3 Tan = Input.Tan;
    Matrix m = 
        FetchBoneTransform (Input.Bones.x) * Input.Weights.x
        + FetchBoneTransform (Input.Bones.y) * Input.Weights.y
        + FetchBoneTransform (Input.Bones.z) * Input.Weights.z
        + FetchBoneTransform (Input.Bones.w) * Input.Weights.w;
    Output.Pos = mul (Pos,m);
    Output.Norm = mul (Norm,(float3x3)m);
    Output.Tan = mul (Tan,(float3x3)m);
    
    return Output;
}


//--------------------------------------------------------------------------------------
// Vertex shader used for skinning the mesh for immediate render
//--------------------------------------------------------------------------------------
PSSkinnedIn VSSkinnedmain(VSSkinnedIn input)
{
    PSSkinnedIn output;
    SkinnedInfo vSkinned = SkinVert( input);
    //output.Pos = mul( vSkinned.Pos, m_matWorldViewProj );
    //output.vPos = mul( vSkinned.Pos, m_matWorld );


	output.Pos = mul( vSkinned.Pos, gmtxWorld );
	output.Pos = mul( output.Pos, gmtxView );
	output.Pos = mul( output.Pos, gmtxProjection );

    output.Norm = normalize( mul( vSkinned.Norm, (float3x3)gmtxWorld ) );
    output.Tangent = normalize( mul( vSkinned.Tan, (float3x3)gmtxWorld ) );
    output.Tex = input.Tex;
  
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel shader that performs bump mapping on the final vertex
//--------------------------------------------------------------------------------------
float4 PSSkinnedmain(PSSkinnedIn input) : SV_Target
{    
	float4 vDiffuse = g_txDiffuse.Sample( g_samLinear, input.Tex );
	
	return vDiffuse;
}




