#define BACKGROUND_COLOUR (float4)(0,0,0,1)

int raySphereIntersect(float3 point, float3 direction,float* t, float3* q)
{
	float3 spherePos = (float3)(640,360,15);
	float sphereRadius = 1;

	float3 m = point - spherePos;
	float b = dot(m,direction);
	float c = dot(m,m) - sphereRadius*sphereRadius;

	if(c > 0.0f && b > 0.0f)
		return 0;
	float discr = b*b - c;

	if(discr < 0.0f)
		return 0;

	*t = -b - sqrt(discr);

	if(*t < 0.0f) *t = 0.0f;
	*q = point + *t * direction;

	return 1;
}

float magnitude(float3 vec)
{
	return sqrt((vec.x*vec.x)+(vec.y*vec.y)+(vec.z*vec.z));
}

float4 calculatePixelColour(float3 cameraPos, float screenDist)
{
	float3 screenPos;
	//ray code goes here
	screenPos.x = (get_global_id(0));
	screenPos.y = (get_global_id(1));
	screenPos.z = screenDist;
	//screenPos.z = 650;//hard coding the screen distance
	float3 direction = screenPos - cameraPos;
	direction = (direction)/magnitude(direction);
	float t;
	float3 q;
	if(raySphereIntersect(cameraPos, direction, &t, &q)){
		return (float4)(1,0,0,1);
	}



	return BACKGROUND_COLOUR;
}

__kernel void raytracer(__write_only image2d_t image, float3 cameraPos, float screenDist)
{
	int2 pos = (int2)(get_global_id(0),get_global_id(1));
	//float4 colour = (float4)(inColour,1);
	float4 colour = calculatePixelColour(cameraPos, screenDist);
	write_imagef(image,pos,colour);
}