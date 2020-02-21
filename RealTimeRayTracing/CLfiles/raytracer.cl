#define BACKGROUND_COLOUR (float4)(0,0,0,1)

int raySphereIntersect(float3 point, float3 direction,float* t, float3* q, float3 spherePos, float sphereRadius)
{
	//float3 spherePos = (float3)(640,360,15);//640,360,15
	//float sphereRadius = 1;

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

float4 calculatePixelColour(float3 cameraPos, float screenDist, __global float* sphereData, int numSpheres)
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
	for(int i = 0; i < numSpheres; i++){
		float3 spherePos = (float3)(
			sphereData[(i*7)+0],
			sphereData[(i*7)+1],
			sphereData[(i*7)+2]
		);

		float3 temp = (float3)(69,420,8008);

		//printf("sphere pos %4.0v3hlf\n", spherePos);
		if(raySphereIntersect(cameraPos, direction, &t, &q, spherePos, sphereData[(i*7)+3])){
			float4 sphereColour = (float4)(
				sphereData[(i*7)+4],
				sphereData[(i*7)+5],
				sphereData[(i*7)+6],
				1
			);
			return sphereColour;
		}
	}



	return BACKGROUND_COLOUR;
}

__kernel void raytracer(__write_only image2d_t image, float3 cameraPos, float screenDist, __global float* sphereData, int numSpheres)
{
	int2 pos = (int2)(get_global_id(0),get_global_id(1));
	//float4 colour = (float4)(inColour,1);
	//printf("numSpheres %d\n", numSpheres);
	float4 colour = calculatePixelColour(cameraPos, screenDist, sphereData, numSpheres);
	write_imagef(image,pos,colour);
}