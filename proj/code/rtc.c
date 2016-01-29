#include "rtc.h"

int rtc_subscribe()
{
	int rh = RTC_HOOK;

	if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &RTC_HOOK) == 0)
		if(sys_irqenable(&RTC_HOOK) == 0)
			return BIT(rh);
	return -1;
}

int rtc_unsubscribe()
{
	if (sys_irqdisable(&RTC_HOOK) == 0)
		if (sys_irqrmpolicy(&RTC_HOOK) == 0)
			return 0;
	return -1;
}

void enable()
{
	unsigned long regB;
	sys_outb(RTC_ADDR_REG, REGB);
	sys_inb(RTC_DATA_REG, &regB);

	regB = regB | BIT(4);

	sys_outb(RTC_ADDR_REG, REGB);
	sys_outb(RTC_DATA_REG, regB);
}

void disable()
{
	unsigned long regB;
	sys_outb(RTC_ADDR_REG, REGB);
	sys_inb(RTC_DATA_REG, &regB);

	regB = regB & 0xEF;

	sys_outb(RTC_ADDR_REG, REGB);
	sys_outb(RTC_DATA_REG, regB);
}

void getDate(Date* d)
{
	unsigned long regA, regC;
	unsigned long data;

	sys_outb(RTC_ADDR_REG, REGB);
	sys_inb(RTC_DATA_REG, &data);

	sys_outb(RTC_ADDR_REG, REGC);
	sys_inb(RTC_DATA_REG, &regC);

	unsigned long year, month, day, hours, minutes, seconds, dotw;
	do
	{
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
	}while(regA & BIT(7));


	if(data & BIT(2))
	{
		d->year = year;
		d->month = month;
		d->day = day;
		d->hours = hours;
		d->minutes = minutes;
		d->seconds = seconds;
		d->dotw = dotw;
	}
	else
	{
		year = ((year & 0xF0) >> 4) * 10 + (year & 0x0F);
		month = ((month & 0xF0) >> 4) * 10 + (month & 0x0F);
		day = ((day & 0xF0) >> 4) * 10 + (day & 0x0F);
		hours = ((hours & 0xF0) >> 4) * 10 + (hours & 0x0F);
		minutes = ((minutes & 0xF0) >> 4) * 10 + (minutes & 0x0F);
		seconds = ((seconds & 0xF0) >> 4) * 10 + (seconds & 0x0F);
		dotw = ((dotw & 0xF0) >> 4) * 10 + (dotw & 0x0F);

		d->year = year;
		d->month = month;
		d->day = day;
		d->hours = hours;
		d->minutes = minutes;
		d->seconds = seconds;
		d->dotw = dotw;
	}
}
