#include "vector.h"
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@float2�̊֐�
/*****************************************************/
/******************��v�Z������*************************/

float ToDistVec2(const VECTOR2F v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y));
};
/****************************���K��**************************/

VECTOR2F NormalizeVec2(const VECTOR2F v)
{
	float dist = ToDistVec2(v);


	return VECTOR2F(v.x / dist,
		v.y / dist);
};;
/***************************(float�Ƃ�)�O�ς��v�Z����******************************/

VECTOR2F ConvertVec2(const float conv, const VECTOR2F v)
{
	const VECTOR2F normalize_vec2 = NormalizeVec2(v);
	return normalize_vec2 * conv;
};
/***************************(�x�N�g���Ƃ�)�O�ς��v�Z����******************************/

float CrossVec2(const VECTOR2F v1, const VECTOR2F v2)
{	//�x�N�g���̊O��
	return v1.x * v2.y - v1.y * v2.x;
};
/***********************���ς��v�Z����************************/

float DotVec2(const VECTOR2F v1, const VECTOR2F v2)
{	//�x�N�g���̓���
	return v1.x * v2.x + v1.y * v2.y;
};
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@float3�̊֐�
/*****************************************************/

/******************��v�Z������*************************/
float ToDistVec3(const VECTOR3F v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}
/****************************���K��**************************/
VECTOR3F NormalizeVec3(const VECTOR3F v)
{
	float dist = ToDistVec3(v);


	return VECTOR3F(v.x / dist,
		v.y / dist,
		v.z / dist);
}
/***************************(float�Ƃ�)�O�ς��v�Z����******************************/

VECTOR3F ConvertVec3(const float conv, VECTOR3F v)
{
	const VECTOR3F normalize_vec3 = NormalizeVec3(v);
	return normalize_vec3 * conv;
}
/***************************(�x�N�g���Ƃ�)�O�ς��v�Z����******************************/
VECTOR3F CrossVec3(const VECTOR3F v1, const VECTOR3F v2)
{
	return VECTOR3F(v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}
/***********************���ς��v�Z����************************/
float DotVec3(const VECTOR3F v1, const VECTOR3F v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}