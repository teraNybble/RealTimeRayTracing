__kernel void makeItRed(__write_only image2d_t image)
{
	//printf("is it not doing a thing\n");
	int2 pos = (int2)(get_global_id(0),get_global_id(1));
	float4 colour = (float4)(1.0f,0.0f,0.0f,1.0f);
 	write_imagef( image , pos ,  colour  );
 	//printf("are we making it red\n");
}