/* Explicit live control smoke test. Restores WB attributes and weights;
 * never opens a sensor bin, starts an ISP, or changes a start-gain override. */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#include "../driver/include/tx_isp/tx_isp_tuning_abi.h"
_Static_assert(sizeof(void *)==4,"T41 32-bit userspace only");
static int call(int fd,unsigned int id,int get,void *p)
{
	struct tx_isp_tuning_t41_control r={0,get,id,(uintptr_t)p};
	return ioctl(fd,0xc0105435U,&r);
}
int main(void)
{
	uint32_t saved[19],attr[19],got[19],owner=0;
	unsigned char weights[225],check[225],zone[2700];
	struct timespec delay={0,200000000};
	int fd=open("/dev/isp-m0",O_RDWR|O_CLOEXEC),ret=1,restore=0;
	if (fd<0) { perror("isp-m0"); return 1; }
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"line %u: %s (errno=%d)\n",__LINE__,#x,errno); goto done; } } while(0)
	REQUIRE(!call(fd,TX_ISP_TUNING_CMD_OPEN_AWB_OWNER,1,&owner));
	REQUIRE(owner==TX_ISP_TUNING_AWB_OWNER_NATIVE);
	REQUIRE(!call(fd,TX_ISP_TUNING_CMD_T41_AWB_ATTR,1,saved));
	/* A start=1 restore would itself alter the day-bank override. */
	REQUIRE(saved[16]==0);
	REQUIRE(!call(fd,TX_ISP_TUNING_CMD_T41_AWB_WEIGHT,1,weights));
	restore=1;
	REQUIRE(!call(fd,TX_ISP_TUNING_CMD_T41_AWB_WEIGHT,0,weights));
	REQUIRE(!call(fd,TX_ISP_TUNING_CMD_T41_AWB_WEIGHT,1,check));
	REQUIRE(!memcmp(weights,check,sizeof(weights)));
	memset(zone,0xa5,sizeof(zone));
	REQUIRE(!call(fd,TX_ISP_TUNING_CMD_T41_AWB_STATS,1,zone));
	memcpy(attr,saved,sizeof(attr)); attr[0]=1; attr[1]=300; attr[2]=600;
	REQUIRE(!call(fd,TX_ISP_TUNING_CMD_T41_AWB_ATTR,0,attr));
	nanosleep(&delay,NULL);
	REQUIRE(!call(fd,TX_ISP_TUNING_CMD_T41_AWB_ATTR,1,got));
	REQUIRE(got[0]==1 && got[1]==300 && got[2]==600);
	attr[3]=1; attr[4]=4200;
	REQUIRE(!call(fd,TX_ISP_TUNING_CMD_T41_AWB_ATTR,0,attr));
	REQUIRE(!call(fd,TX_ISP_TUNING_CMD_T41_AWB_ATTR,1,got) && got[3]==1);
	attr[0]=10; errno=0;
	REQUIRE(call(fd,TX_ISP_TUNING_CMD_T41_AWB_ATTR,0,attr)==-1 && errno==EINVAL);
	REQUIRE(!call(fd,TX_ISP_TUNING_CMD_T41_AWB_ATTR,1,got) && got[0]==1 && got[3]==1);
	errno=0;
	REQUIRE(call(fd,TX_ISP_TUNING_CMD_T41_AWB_STATS,0,zone)==-1 && errno==EOPNOTSUPP);
	errno=0;
	REQUIRE(call(fd,TX_ISP_TUNING_CMD_T41_AWB_WEIGHT,1,(void *)1)==-1 && errno==EFAULT);
	ret=0;
done:
	if (restore) {
		if (call(fd,TX_ISP_TUNING_CMD_T41_AWB_ATTR,0,saved) ||
		    call(fd,TX_ISP_TUNING_CMD_T41_AWB_WEIGHT,0,weights)) { perror("restore"); ret=1; }
		else puts("Original AWB attributes and weights restored");
	}
	close(fd);
	if (!ret) puts("T41 live AWB: public mode/freeze/weight/zone and invalid-request checks PASS");
	return ret;
}
