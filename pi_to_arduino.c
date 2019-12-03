
#include <wiringPi.h>
#include <wiringSerial.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sodium.h>

int main()
{
	int fd;
	int count;
	  
	const unsigned char reader_pk[32] =   {0x00, 0x08, 0x89, 0xb8, 0x8f, 0x2e, 0xea, 0x83, 0x49, 0x6c,
									 0x1c, 0xbd, 0x67, 0xd2, 0x2b, 0x44, 0xa3, 0x06, 0xe0, 0x51,
									 0x81, 0x58, 0x4a, 0x95, 0x71, 0x31, 0x18, 0x77, 0x8e, 0x1e,
									 0x92, 0x20};
				 
	const unsigned char reader_pubk[32] = {0x00, 0x00, 0x2f, 0xaa, 0x12, 0x7a, 0x28, 0xd4, 0x19, 0x79,
									 0x63, 0xee, 0x9f, 0x0e, 0xcf, 0x24, 0xb5, 0xed, 0x9c, 0xc5,
									 0x50, 0xca, 0x6a, 0x3f, 0x45, 0xc1, 0x48, 0x59, 0xe2, 0x6f,
									 0xca, 0x61};
									 
	const unsigned char tag_pk[32] = {0x00, 0x00, 0x00, 0x10, 0xea, 0x64, 0x42, 0x42, 0x80, 0xff, 0xdd, 
									  0xd3, 0x1c, 0x7b, 0xf1, 0x6a, 0x2d, 0x95, 0x2d, 0xd4, 0xbd, 0x3a, 
									  0xe8, 0x6a, 0xd4, 0x6c, 0x19, 0x97, 0x6e, 0xfe, 0x5a, 0x5c, 0xe2, 
									  0xd0, 0x40};
	const unsigned char tag_pubk[32] = "62a889617eac663c7819631e26e78e94cbeda24f3b522bb21568aacdda2a3c";

	unsigned int nextTime;
	if((fd =serialOpen("/dev/ttyS0", 115200)) < 0){
		printf("Unable able to open serial port\n");
		return 1;
	}
	nextTime = millis() + 300;
	for(count=0; count < 32; count++){
		serialPutchar(fd, reader_pubk[count]);
		nextTime += 300;
		
	}
	delay(3);
	unsigned char buf[32];
	count=0;
	while (serialDataAvail(fd)){
		if (count == 32)
			break;
		buf[count] = serialGetchar(fd);
		count++;
	}
	for(int i = 0; i < 32; i++)
		printf("%x", buf[i]);
		
	printf("\n");
}	
