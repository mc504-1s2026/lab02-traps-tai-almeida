#include <arch/timer.h>
#include <kernel/panic.h>
#include <arch/csr.h>

u64 user_alarm_sobrando = 0;
u64 uptime_segundos = 0;
static u64 next_tick = 0;

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
	u64 ticks_in_future = agr + (segundos * TIMER_FREQ);
    csr_write(CSR_STIMECMP, ticks_in_future);
}

void timer_setup(void)
{
    next_tick = timer_read() + TIMER_FREQ;
    csr_write(CSR_STIMECMP, next_tick);
}

void timer_irq()
{
    timer_set_alarm(1); 

    uptime_segundos++;

    if (user_alarm_sobrando > 0) {
        user_alarm_sobrando--;
        if (user_alarm_sobrando == 0) {
            printk(0, "alarm\r\n"); 
        }
    }
}
