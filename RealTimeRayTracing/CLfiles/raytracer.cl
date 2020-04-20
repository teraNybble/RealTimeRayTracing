#define BACKGROUND_COLOUR (float4)(0,0,0,1)

bool _combFloat4(float4 f4a,float4 f4b)
{
	return (f4a.w == f4b.w && f4a.x == f4b.x && f4a.y == f4b.y && f4a.z == f4b.z);
}

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

//https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code
int raySphereIntersect(float3 point, float3 direction,float* t, float3* q, float3 spherePos, float sphereRadius)
{
	//float3 spherePos = (float3)(640,360,15);//640,360,15
	//float sphereRadius = 1;

	float3 m = point - spherePos;
	float b = dot(m,direction);
	float c = dot(m,m) - sphereRadius*sphereRadius;

	if(c > 0.0f && b > 0.0f){
		return 0;
		}
	float discr = b*b - c;

	if(discr < 0.0f)
		return 0;

	*t = -b - sqrt(discr);
	float tmax = -b + sqrt(discr);

	if (tmax < *t && tmax > 0.0)
		*t = tmax;

	//if(*t < 0.0f) *t = 0.0f;
	if (*t < 0.0f)
		return 0;

	*q = point + *t * direction;

	return 1;
}

#define SPHERE_DATA_SIZE 17

void dataStreamToFloats(__global float* sphereData, int i, float4* sphereColour, float4* lightAmbient, float4* lightSpecular, float4* materialAmbient, float4* materialDiffuse, float4* materialSpecular)
{
	*sphereColour = (float4)(
		sphereData[(i*SPHERE_DATA_SIZE)+4],
		sphereData[(i*SPHERE_DATA_SIZE)+5],
		sphereData[(i*SPHERE_DATA_SIZE)+6],
		1
	);/*
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
	);*/
	*materialAmbient = (float4)(
		sphereData[(i*SPHERE_DATA_SIZE)+7],
		sphereData[(i*SPHERE_DATA_SIZE)+8],
		sphereData[(i*SPHERE_DATA_SIZE)+9],
		1
	);
	*materialDiffuse = (float4)(
		sphereData[(i*SPHERE_DATA_SIZE)+10],
		sphereData[(i*SPHERE_DATA_SIZE)+11],
		sphereData[(i*SPHERE_DATA_SIZE)+12],
		1
	);
	*materialSpecular = (float4)(
		sphereData[(i*SPHERE_DATA_SIZE)+13],
		sphereData[(i*SPHERE_DATA_SIZE)+14],
		sphereData[(i*SPHERE_DATA_SIZE)+15],
		1
	);
}

void findClosestSphere(__global float* sphereData, int numSpheres, float3 startPos, float3 direction, int* closestSphere,float3* closestIntesect, float* closestT, int skipID)
{
	*closestSphere = -1;
	//float3 closestIntesect;
	//float closestT;

	for(int i = 0; i < numSpheres; i++){
		float t;
		float3 q;

		float3 spherePos = (float3)(
			sphereData[(i*SPHERE_DATA_SIZE)+0],
			sphereData[(i*SPHERE_DATA_SIZE)+1],
			sphereData[(i*SPHERE_DATA_SIZE)+2]
		);
		//printf("sphere pos %4.0v3hlf\n", spherePos);
		if(raySphereIntersect(startPos, direction, &t, &q, spherePos, sphereData[(i*SPHERE_DATA_SIZE)+3])){
			if(i == skipID) continue;
			if(*closestSphere != -1){
				//need to switch to t value
				//if(q.z < closestIntesect->z){
				if(t < *closestT){
					*closestSphere = i;
					*closestIntesect = q;
					*closestT = t;
				}
			}else{
				*closestSphere = i;
				*closestIntesect = q;
				//printf("old t %d new t %d\n", closestT, t);
				//printf(" new t %d\n", t);
				*closestT = t;
			}
		}
	}
}

float4 reflectColour(float3 intersectPoint, float3 cameraPos, float3 normalVec, float3 spherePos, int sphereId,
	__global float* sphereData, int numSpheres)
{
	float3 reflectVec = reflect(/*normalise*/(intersectPoint-cameraPos), normalVec);
	//float3 reflectVec = reflect(normalise(cameraPos-intersectPoint), normalVec);
	reflectVec = -normalise(reflectVec);
	int closestSphere = -1;
    float3 closestIntesect;

	for(int j = 0; j < numSpheres; j++){
		if(j == sphereId) continue; // no point reflecting the sphere with its self
		float3 rSpherePos= (float3)(
			sphereData[(j*SPHERE_DATA_SIZE)+0],
			sphereData[(j*SPHERE_DATA_SIZE)+1],
			sphereData[(j*SPHERE_DATA_SIZE)+2]
		);
		float4 rSphereColour,rLightAmbient,rLightSpecular,rMaterialAmbient,rMaterialDiffuse,rMaterialSpecular;

		float rt;
		float3 rq;
		float closestT;

		dataStreamToFloats(sphereData, j,&rSphereColour,&rLightAmbient,&rLightSpecular,&rMaterialAmbient,&rMaterialDiffuse,&rMaterialSpecular);
		if(raySphereIntersect(intersectPoint/*+cameraPos*/, reflectVec, &rt, &rq, spherePos/*-cameraPos*/, sphereData[(j*SPHERE_DATA_SIZE)+3])){
			if(closestSphere != -1){
				//if(rq.z < closestIntesect.z){
				if(magnitude(intersectPoint-rq) < magnitude(closestIntesect)) {
				//if(rt < closestT){
					//printf("%d: Closest ID: %d\n",sphereId,j);
					closestSphere = j;
					closestIntesect = rq;
					closestT = rt;
				}
			}else{
				//printf("%d: Closest ID: %d\n",sphereId,j);
				closestSphere = j;
				closestIntesect = rq;
				closestT = rt;
			}
		}
	}

	if(closestSphere != -1){
		//printf("%d: the sphere ID used is %d\n",sphereId,closestSphere);
		return (float4)(
    		sphereData[(closestSphere*SPHERE_DATA_SIZE)+4],
    		sphereData[(closestSphere*SPHERE_DATA_SIZE)+5],
    		sphereData[(closestSphere*SPHERE_DATA_SIZE)+6],
    		1
    	);
	}
	return (float4)(
			sphereData[(sphereId*SPHERE_DATA_SIZE)+4],
			sphereData[(sphereId*SPHERE_DATA_SIZE)+5],
			sphereData[(sphereId*SPHERE_DATA_SIZE)+6],
			1
		);
}

float4 calculatePixelColour(float3 cameraPos, float screenDist, __global float* sphereData, int numSpheres, __global float* lighting)
{
	float3 screenPos;
	//ray code goes here
	screenPos.x = (get_global_id(0));
	screenPos.y = (get_global_id(1));
	screenPos.z = screenDist;
	//screenPos.z = 650;//hard coding the screen distance
	float3 direction = screenPos - cameraPos;
	direction = (direction)/magnitude(direction);


	int closestSphere = -1;
	float3 closestIntesect;

	for(int i = 0; i < numSpheres; i++){
		float t;
		float3 q;

		float3 spherePos = (float3)(
			sphereData[(i*SPHERE_DATA_SIZE)+0],
			sphereData[(i*SPHERE_DATA_SIZE)+1],
			sphereData[(i*SPHERE_DATA_SIZE)+2]
		);
		//printf("sphere pos %4.0v3hlf\n", spherePos);
		if(raySphereIntersect(cameraPos, direction, &t, &q, spherePos, sphereData[(i*SPHERE_DATA_SIZE)+3])){
			if(closestSphere != -1){
				if(q.z < closestIntesect.z){
					closestSphere = i;
					closestIntesect = q;
				}
			}else{
				closestSphere = i;
				closestIntesect = q;
			}
		}
	}


	if(closestSphere != -1){
		float3 spherePos = (float3)(
			sphereData[(closestSphere*SPHERE_DATA_SIZE)+0],
			sphereData[(closestSphere*SPHERE_DATA_SIZE)+1],
			sphereData[(closestSphere*SPHERE_DATA_SIZE)+2]
		);
		//printf("spherePos %4.9v3hlf\n", spherePos);
		if(spherePos.x == 650 && spherePos.y == 360 && spherePos.z == 30){
			//printf("the sphere is here\n");
		}
		//get all the sphere data
		float4 sphereColour,lightAmbient,lightSpecular,materialAmbient,materialDiffuse,materialSpecular;
			dataStreamToFloats(sphereData, closestSphere,&sphereColour,&lightAmbient,&lightSpecular,&materialAmbient,&materialDiffuse,&materialSpecular);

		lightAmbient = (float4)(
			lighting[0],lighting[1],lighting[2],1);
		lightSpecular = (float4)(
			lighting[3],lighting[4],lighting[5],1);

		//get the light direction vector
		float3 lightPos = (float3)(640,1000,60);
		float3 lightDirection = lightPos - closestIntesect;
		float3 normalVec = closestIntesect - spherePos;
		normalVec = normalise(normalVec);

		//printf("closestIntersect %4.0v3hlf\n", closestIntesect);
		//using the green sphere as a reflective sphere
		if(_combFloat4(sphereColour, (float4)(0,1,0,1))){
			sphereColour = reflectColour(closestIntesect, cameraPos, normalVec, spherePos, closestSphere, sphereData, numSpheres);
		}


		return calculateLighting(sphereColour,normalVec,closestIntesect - cameraPos,
			lightDirection,lightAmbient,lightSpecular,//light //direction ambiant specular
			materialAmbient,materialDiffuse,materialSpecular,sphereData[(closestSphere*SPHERE_DATA_SIZE)+16]);//material //ambient diffuse specular shininess
	}

	return BACKGROUND_COLOUR;
}

__kernel void raytracer(__write_only image2d_t image, float3 cameraPos, float screenDist, __global float* sphereData, int numSpheres, __global float* lighting)
{
	int2 pos = (int2)(get_global_id(0),get_global_id(1));
	//float4 colour = (float4)(inColour,1);
	//printf("numSpheres %d\n", numSpheres);
	//printf("sphere pos %d,%d,%d\n", sphereData[0],sphereData[1],sphereData[2]);
	float4 colour = calculatePixelColour(cameraPos, screenDist, sphereData, numSpheres, lighting);
	write_imagef(image,pos,colour);
}
