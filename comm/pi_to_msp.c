
#include <wiringPi.h>
#include <wiringSerial.h>
#include <string.h>
#include <errno.h>
#include <sodium.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t dl;

static void pabort(const char *s)
{
	perror(s);
	abort();
}


static void transfer(int fd)
{
	int ret;
	uint8_t tx[] = {
        0x48, 0x45, 0x4C, 0x4C, 0x4F,
        0x20,
        0x57, 0x4F, 0x52, 0x4C, 0x44,
        0x0A
	};
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = dl,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

    /*
	for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
		if (!(ret % 6))
			puts("");
		printf("%.2X ", rx[ret]);
	}
	puts("");
    */
}

static void print_usage(const char *prog)
{
	printf("Usage: %s [-DsbdlHOLC3]\n", prog);
	puts("  -D --device   device to use (default /dev/spidev1.1)\n"
	     "  -s --speed    max speed (Hz)\n"
	     "  -d --dl    dl (usec)\n"
	     "  -b --bpw      bits per word \n"
	     "  -l --loop     loopback\n"
	     "  -H --cpha     clock phase\n"
	     "  -O --cpol     clock polarity\n"
	     "  -L --lsb      least significant bit first\n"
	     "  -C --cs-high  chip select active high\n"
	     "  -3 --3wire    SI/SO signals shared\n");
	exit(1);
}

static void parse_opts(int argc, char *argv[])
{
	while (1) {
		static const struct option lopts[] = {
			{ "device",  1, 0, 'D' },
			{ "speed",   1, 0, 's' },
			{ "dl",   1, 0, 'd' },
			{ "bpw",     1, 0, 'b' },
			{ "loop",    0, 0, 'l' },
			{ "cpha",    0, 0, 'H' },
			{ "cpol",    0, 0, 'O' },
			{ "lsb",     0, 0, 'L' },
			{ "cs-high", 0, 0, 'C' },
			{ "3wire",   0, 0, '3' },
			{ "no-cs",   0, 0, 'N' },
			{ "ready",   0, 0, 'R' },
			{ NULL, 0, 0, 0 },
		};
		int c;

		c = getopt_long(argc, argv, "D:s:d:b:lHOLC3NR", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'D':
			device = optarg;
			break;
		case 's':
			speed = atoi(optarg);
			break;
		case 'd':
			dl = atoi(optarg);
			break;
		case 'b':
			bits = atoi(optarg);
			break;
		case 'l':
			mode |= SPI_LOOP;
			break;
		case 'H':
			mode |= SPI_CPHA;
			break;
		case 'O':
			mode |= SPI_CPOL;
			break;
		case 'L':
			mode |= SPI_LSB_FIRST;
			break;
		case 'C':
			mode |= SPI_CS_HIGH;
			break;
		case '3':
			mode |= SPI_3WIRE;
			break;
		case 'N':
			mode |= SPI_NO_CS;
			break;
		case 'R':
			mode |= SPI_READY;
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;

	parse_opts(argc, argv);

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

	transfer(fd);

	close(fd);

	return ret;
}
//
// int main()
// {
// 	wiringPiSetup();
// 	int fd;
// 	int count;
//
// 	const unsigned char reader_pk[32] =   {0x00, 0x08, 0x89, 0xb8, 0x8f, 0x2e, 0xea, 0x83, 0x49, 0x6c,
// 									 0x1c, 0xbd, 0x67, 0xd2, 0x2b, 0x44, 0xa3, 0x06, 0xe0, 0x51,
// 									 0x81, 0x58, 0x4a, 0x95, 0x71, 0x31, 0x18, 0x77, 0x8e, 0x1e,
// 									 0x92, 0x20};
//
// 	const unsigned char reader_pubk[32] = {0x00, 0x00, 0x2f, 0xaa, 0x12, 0x7a, 0x28, 0xd4, 0x19, 0x79,
// 									 0x63, 0xee, 0x9f, 0x0e, 0xcf, 0x24, 0xb5, 0xed, 0x9c, 0xc5,
// 									 0x50, 0xca, 0x6a, 0x3f, 0x45, 0xc1, 0x48, 0x59, 0xe2, 0x6f,
// 									 0xca, 0x61};
//
// 	// const unsigned char tag_pk[32] = {0x00, 0x00, 0x00, 0x10, 0xea, 0x64, 0x42, 0x42, 0x80, 0xff, 0xdd,
// 									  // 0xd3, 0x1c, 0x7b, 0xf1, 0x6a, 0x2d, 0x95, 0x2d, 0xd4, 0xbd, 0x3a,
// 									  // 0xe8, 0x6a, 0xd4, 0x6c, 0x19, 0x97, 0x6e, 0xfe, 0x5a, 0x5c, 0xe2,
// 									  // 0xd0, 0x40};
// 	// const unsigned char tag_pubk[32] = "62a889617eac663c7819631e26e78e94cbeda24f3b522bb21568aacdda2a3c";
//
// 	// unsigned int nextTime;
// 	// if((fd =serialOpen("/dev/ttyS0", 115200)) < 0){
// 	// 	printf("Unable able to open serial port\n");
// 	// 	return 1;
// 	// }
// 	// nextTime = millis() + 300;
// 	// for(count=0; count < 32; count++){
// 	// 	serialPutchar(fd, reader_pubk[count]);
// 	// 	nextTime += 300;
// 	//
// 	// }
// 	// dl(3);
// 	// unsigned char buf[32];
// 	// count=0;
// 	// while (serialDataAvail(fd)){
// 	// 	if (count == 32)
// 	// 		break;
// 	// 	buf[count] = serialGetchar(fd);
// 	// 	count++;
// 	//
// 	// }
// 	// for(int i = 0; i < 32; i++)
// 	// 	printf("%x", buf[i]);
// 	// printf("\n");
// 	pinMode(4, INPUT);
// 	if(digitalRead(4) == 1){
//
// 	}
// }
