#include <arch/timer.h>
#include <kernel/panic.h>
#include <arch/csr.h>

u64 user_alarm_sobrando = 0;
u64 uptime_segundos = 0;

u64 timer_read()
{
	return csr_read(CSR_TIME);
}

void timer_irq_enable()
{
	csr_set(CSR_SIE, CSR_SIE_STIE);
}

void timer_irq_disable()
{
	csr_clear(CSR_SIE, CSR_SIE_STIE);
}

void timer_set_alarm(u64 segundos)
{
	u64 agr = timer_read();
    u64 ticks_in_future = agr + (segundos * 10000000);
    csr_write(CSR_STIMECMP, ticks_in_future);
}

void timer_irq()
{
	uptime_segundos++;

    if (user_alarm_sobrando > 0) {
        user_alarm_sobrando--;
        if (user_alarm_sobrando == 0) {
            printk(0, "%s", "\nalarm\n> ");
        }
    }

    
    timer_set_alarm(1);
}
