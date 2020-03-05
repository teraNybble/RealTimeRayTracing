#define BACKGROUND_COLOUR (float4)(0,0,0,1)

//useful vector functions
float magnitude(float3 vec)
{
	return sqrt((vec.x*vec.x)+(vec.y*vec.y)+(vec.z*vec.z));
}

float3 reflect(float3 I, float3 N)
{
	return (I - 2.0f * dot(N,I) * N);
}

float3 refract(float3 I, float3 N, float ior)
{
	float cosi = clamp(-1.0f,1.0f,(float)dot(I,N));
	float etai = 1, etat = ior;
	float3 n = N;
	if(cosi < 0) { cosi = -cosi; } else { swap(etai, etat); n = -N; }
	float eta = etai/etat;
	float k = 1 - eta * eta * (1- cosi * cosi);

	return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
}

#define normalise(x) ({x/magnitude(x);})
//!useful vector functions
float4 calculateLighting(
	float4 inColour, float3 inNormal,
	float3 eyePos, float3 lightDirection, float4 lightAmbient, float4 lightSpecular,
	float4 materialAmbient, float4 materialDiffuse, float4 materialSpecular , float materialShininess)
{
	float3 n = normalise(inNormal);
	float3 L = normalise(lightDirection);

	float3 v = normalise(-eyePos);
	float3 r = normalise(-reflect(L,n));

	float RdotV = max(0.0f,(float)dot(r,v));
	float NdotL = max((float)dot(n,L),0.0f);

	float4  colour = lightAmbient * materialAmbient;

	//printf("calculated colour %1.2v3hlf\n", colour);
	//printf("NdotL %1.2v8hlf\n", (float8)(n,0.69,0.42,L));
	//printf("NdotL %d\n", NdotL);

	if(NdotL > 0.0)
	{
		colour += (lightAmbient * materialDiffuse * NdotL);
		colour += materialSpecular * lightSpecular * pow(RdotV, materialShininess);
	}



	return colour * inColour;
}

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

#define SPHERE_DATA_SIZE 23

void dataStreamToFloats(__global float* sphereData, int i, float4* sphereColour, float4* lightAmbient, float4* lightSpecular, float4* materialAmbient, float4* materialDiffuse, float4* materialSpecular)
{
	*sphereColour = (float4)(
		sphereData[(i*SPHERE_DATA_SIZE)+4],
		sphereData[(i*SPHERE_DATA_SIZE)+5],
		sphereData[(i*SPHERE_DATA_SIZE)+6],
		1
	);
	*lightAmbient = (float4)(
		sphereData[(i*SPHERE_DATA_SIZE)+7],
		sphereData[(i*SPHERE_DATA_SIZE)+8],
		sphereData[(i*SPHERE_DATA_SIZE)+9],
		1
		);
	*lightSpecular = (float4)(
		sphereData[(i*SPHERE_DATA_SIZE)+10],
		sphereData[(i*SPHERE_DATA_SIZE)+11],
		sphereData[(i*SPHERE_DATA_SIZE)+12],
		1
	);
	*materialAmbient = (float4)(
		sphereData[(i*SPHERE_DATA_SIZE)+13],
		sphereData[(i*SPHERE_DATA_SIZE)+14],
		sphereData[(i*SPHERE_DATA_SIZE)+15],
		1
	);
	*materialDiffuse = (float4)(
		sphereData[(i*SPHERE_DATA_SIZE)+16],
		sphereData[(i*SPHERE_DATA_SIZE)+17],
		sphereData[(i*SPHERE_DATA_SIZE)+18],
		1
	);
	*materialSpecular = (float4)(
		sphereData[(i*SPHERE_DATA_SIZE)+19],
		sphereData[(i*SPHERE_DATA_SIZE)+20],
		sphereData[(i*SPHERE_DATA_SIZE)+21],
		1
	);
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
			sphereData[(i*SPHERE_DATA_SIZE)+0],
			sphereData[(i*SPHERE_DATA_SIZE)+1],
			sphereData[(i*SPHERE_DATA_SIZE)+2]
		);

		//printf("sphere pos %4.0v3hlf\n", spherePos);
		if(raySphereIntersect(cameraPos, direction, &t, &q, spherePos, sphereData[(i*SPHERE_DATA_SIZE)+3])){
			/*
			float4 sphereColour = (float4)(
				sphereData[(i*SPHERE_DATA_SIZE)+4],
				sphereData[(i*SPHERE_DATA_SIZE)+5],
				sphereData[(i*SPHERE_DATA_SIZE)+6],
				1
			);*/
			//get the light direction vector
			float3 lightPos = (float3)(640,1000,60);
			float3 lightDirection = lightPos - q;
			float3 normalVec = q - spherePos;
			/*
			float4 lightAmbient = (float4)(
				sphereData[(i*SPHERE_DATA_SIZE)+7],
				sphereData[(i*SPHERE_DATA_SIZE)+8],
				sphereData[(i*SPHERE_DATA_SIZE)+9],
				1
				);
			float4 lightSpecular = (float4)(
				sphereData[(i*SPHERE_DATA_SIZE)+10],
            	sphereData[(i*SPHERE_DATA_SIZE)+11],
            	sphereData[(i*SPHERE_DATA_SIZE)+12],
            	1
			);
			float4 materialAmbient = (float4)(
            	sphereData[(i*SPHERE_DATA_SIZE)+13],
                sphereData[(i*SPHERE_DATA_SIZE)+14],
                sphereData[(i*SPHERE_DATA_SIZE)+15],
                1
            );
            float4 materialDiffuse = (float4)(
				sphereData[(i*SPHERE_DATA_SIZE)+16],
				sphereData[(i*SPHERE_DATA_SIZE)+17],
				sphereData[(i*SPHERE_DATA_SIZE)+18],
				1
            );
            float4 materialSpecular = (float4)(
				sphereData[(i*SPHERE_DATA_SIZE)+19],
				sphereData[(i*SPHERE_DATA_SIZE)+20],
				sphereData[(i*SPHERE_DATA_SIZE)+21],
				1
			);
			*/
			float4 sphereColour,lightAmbient,lightSpecular,materialAmbient,materialDiffuse,materialSpecular;
			dataStreamToFloats(sphereData, i,&sphereColour,&lightAmbient,&lightSpecular,&materialAmbient,&materialDiffuse,&materialSpecular);
			//return sphereColour;
			return calculateLighting(sphereColour,normalVec,q - cameraPos,
				lightDirection,lightAmbient,lightSpecular,//light //direction ambiant specular
				materialAmbient,materialDiffuse,materialSpecular,sphereData[(i*SPHERE_DATA_SIZE)+22]);//material //ambient diffuse specular shininess
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