#include "test6.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define REGA 10
#define REGB 11
#define REGC 12
#define REGD 13

#define BIT(n) (0x01<<(n))

#define RTCIRQ 8
static int RTC_HOOK = 3;


int rtc_test_conf(void)
{
	disable();
	unsigned long data;
	sys_outb(RTC_ADDR_REG, REGA);
	sys_inb(RTC_DATA_REG, &data);
	printf("REGA: 0x%x     UIP: %d RS: %d  \n", data, (data & BIT(7)), (data & 0x0F));


	sys_outb(RTC_ADDR_REG, REGB);
	sys_inb(RTC_DATA_REG, &data);
	int SET = data & BIT(7);
	if(SET != 0)
		SET = 1;
	int PF = data & BIT(6);
	if(PF != 0)
		PF = 1;
	int AF = data & BIT(5);
	if(AF != 0)
		AF = 1;
	int UE = data & BIT(4);
	if(UE != 0)
		UE = 1;
	int SQWE = data & BIT(3);
	if(SQWE != 0)
		SQWE = 1;
	int DM = data & BIT(2);
	if(DM != 0)
		DM = 1;
	int TF = data & BIT(1);
	if(TF != 0)
		TF = 1;
	int DSE = data & BIT(0);
	if(DSE != 0)
		DSE = 1;
	printf("REGB: 0x%x     SET: %d PF: %d AF: %d UE: %d SQWE: %d DM: %d 24h: %d DSE: %d \n", data, SET, PF, AF, UE, SQWE, DM, TF, DM, DSE);


	sys_outb(RTC_ADDR_REG, REGC);
	sys_inb(RTC_DATA_REG, &data);
	int IRQF = data & BIT(7);
	if(IRQF != 0)
		IRQF = 1;
	PF = data & BIT(6);
	if(PF != 0)
		PF = 1;
	AF = data & BIT(5);
	if(AF != 0)
		AF = 1;
	UE = data & BIT(4);
	if(UE != 0)
		UE = 1;
	printf("REGC: 0x%x    IRQF: %d PF: %d AD: %d UE: %d \n", data, IRQF, PF, AF, UE);


	sys_outb(RTC_ADDR_REG, REGD);
	sys_inb(RTC_DATA_REG, &data);
	int VRT = data & BIT(7);
	if (VRT != 0)
		VRT = 1;
	printf("REGD: 0x%x    VRT: %d \n", data, VRT);

}

void disable()
{
	unsigned long regB;
	sys_outb(RTC_ADDR_REG, REGB);
	sys_inb(RTC_DATA_REG, &regB);

	regB = regB & 0xEF;

	sys_outb(RTC_ADDR_REG, REGB);
	sys_outb(RTC_DATA_REG, regB);
	/*unsigned long data;
	sys_outb(RTC_ADDR_REG, REGB);
	sys_inb(RTC_DATA_REG, &data);
	data = data | BIT(7);

	sys_outb(RTC_ADDR_REG, REGB);
	sys_outb(RTC_DATA_REG, data);*/
}

void enable()
{
	unsigned long regB;
	sys_outb(RTC_ADDR_REG, REGB);
	sys_inb(RTC_DATA_REG, &regB);

	regB = regB | BIT(4);

	sys_outb(RTC_ADDR_REG, REGB);
	sys_outb(RTC_DATA_REG, regB);

	/*unsigned long data;
	sys_outb(RTC_ADDR_REG, REGB);
	sys_inb(RTC_DATA_REG, &data);
	data = data & 0x7F;

	sys_outb(RTC_ADDR_REG, REGB);
	sys_outb(RTC_DATA_REG, data);*/
}

int rtc_test_date(void)
{
	unsigned long regA;
	unsigned long data;

	sys_outb(RTC_ADDR_REG, REGB);
	sys_inb(RTC_DATA_REG, &data);

	unsigned long year, month, day, hours, minutes, seconds, dotw;
	do
	{
		disable();
		sys_outb(RTC_ADDR_REG, REGA);
		sys_inb(RTC_DATA_REG, &regA);
		sys_outb(RTC_ADDR_REG, 0);
		sys_inb(RTC_DATA_REG, &seconds);
		sys_outb(RTC_ADDR_REG, 2);
		sys_inb(RTC_DATA_REG, &minutes);
		sys_outb(RTC_ADDR_REG, 4);
		sys_inb(RTC_DATA_REG, &hours);
		sys_outb(RTC_ADDR_REG, 6);
		sys_inb(RTC_DATA_REG, &dotw);
		sys_outb(RTC_ADDR_REG, 7);
		sys_inb(RTC_DATA_REG, &day);
		sys_outb(RTC_ADDR_REG, 8);
		sys_inb(RTC_DATA_REG, &month);
		sys_outb(RTC_ADDR_REG, 9);
		sys_inb(RTC_DATA_REG, &year);
		enable();
	}while(regA & BIT(7));


	if(data & BIT(2))
	{
		if(dotw == 0)
			printf("Sunday  ");
		else if (dotw == 1)
			printf("Monday  ");
		else if (dotw == 2)
			printf("Tuesday  ");
		else if (dotw == 3)
			printf("Wednesday  ");
		else if (dotw == 4)
			printf("Thursday  ");
		else if (dotw == 5)
			printf("Friday  ");
		else if (dotw == 6)
			printf("Saturday  ");
		printf("%d/%d/%d  %d:%d:%d \n", day, month, year, hours, minutes, seconds);
	}
	else
	{
		if(dotw == 0x0)
			printf("Sunday  ");
		else if (dotw == 0x1)
			printf("Monday  ");
		else if (dotw == 0x2)
			printf("Tuesday  ");
		else if (dotw == 0x3)
			printf("Wednesday  ");
		else if (dotw == 0x4)
			printf("Thursday  ");
		else if (dotw == 0x5)
			printf("Friday  ");
		else if (dotw == 0x6)
			printf("Saturday  ");
		printf("%x/%x/%x  %x:%x:%x \n", day, month, year, hours, minutes, seconds);
	}
}

int rtcSubscribe(void)
{
	int rh = RTC_HOOK;

	if (sys_irqsetpolicy(RTCIRQ, IRQ_REENABLE, &RTC_HOOK) == 0)
		if(sys_irqenable(&RTC_HOOK) == 0)
			return BIT(rh);
	return -1;
}

int rtcUnsubscribe()
{
	if (sys_irqdisable(&RTC_HOOK) == 0)
		if (sys_irqrmpolicy(&RTC_HOOK) == 0)
			return 0;
	return 1;
}

int rtc_test_int(/* to be defined in class */)
{//subscribe -> enable -> ler registo C -> ler normalmente -> disable
	int rtc_set = rtcSubscribe();


	enable();
	unsigned long regC;


	int c = 0;
	int ipc_status;
	message msg;
	int r;
	while(c < 10) /* You may want to use a different condition */
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status))
		{ /* received notification */
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE: /* hardware interrupt notification */
				if ((msg.NOTIFY_ARG & rtc_set))
				{ /* subscribed interrupt */
					sys_outb(RTC_ADDR_REG, REGC);
					sys_inb(RTC_DATA_REG, &regC);
					rtc_test_date();
					c++;
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
	}

	disable();
	rtcUnsubscribe();
}
