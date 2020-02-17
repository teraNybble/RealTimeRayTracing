/*
__kernel void makeItRed(__write_only image2d_t image)
{
	//printf("is it not doing a thing\n");
	int2 pos = (int2)(get_global_id(0),get_global_id(1));
	float4 colour = (float4)(1.0f,0.0f,0.0f,1.0f);//rgba
 	write_imagef( image , pos ,  colour  );
 	//printf("are we making it red\n");
}
*/

#define BACKGROUND_COLOUR (float4)(0,0,0,1)

float4 calculatePixelColour()
{
	//ray code goes here
	return BACKGROUND_COLOUR;
}

__kernel void makeItAnyColour(__write_only image2d_t image, float3 inColour)
{
	int2 pos = (int2)(get_global_id(0),get_global_id(1));
	float4 colour = (float4)(inColour,1);
	//colour = calculatePixelColour();
	write_imagef(image,pos,colour);
}