void printfoo()
{
	printf("foo");
}


__kernel void getXY(/*__global char* buff*/)
{
//as this is only a test program I am hard coding the width
//   int width = 16 * get_global_id(1);
//    buff[0] = (char*)get_global_id(0);
//	buff[1] = ',';
//  buff[2] = width;
//  buff[3] = '\n';
//	printf("%d\n",69);
	printf("X:%d\n",get_global_id(0));
	printf("Y:%d\n",get_global_id(1));
//	printfoo();
//	printf("B:%d,%d\n",get_local_id(0),get_local_id(1));
}