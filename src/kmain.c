#include <kernel/printf.h>
#include <kernel/mm.h>
#include <arch/timer.h>
#include <kernel/trap.h>
#include <kernel/serial.h>
#include <kernel/string.h>

static char line_buffer[256];
static int pos = 0;

extern int _hartid[];
extern u64 uptime_segundos;
extern u64 user_alarm_sobrando;

extern void plic_init(void);
void kmain()
{
	printk_set_level(LOG_DEBUG);
	info("entered S-mode\n");
	info("booting on hart %d\n", _hartid[0]);
	info("setting up virtual memory...\n");
	vm_init();


	info("enabling traps...\n");
	trap_setup();

	plic_init();  

	info("enabling timer...\n");
	timer_setup();
	timer_irq_enable();
	info("enabling serial...\n");
	serial_init();
	// serial_irq_enable();

	/* implement your shell here */
    hart_irq_enable();

    printk(0, "> ");

    char c;
    while (1) {
        if (serial_read(&c, 1) > 0) {
            if (c == '\r' || c == '\n') {
                line_buffer[pos] = '\0';
                printk(0, "\n"); 

                if (strncmp(line_buffer, "echo ", 5) == 0) {
                    printk(0, "%s\n", line_buffer + 5);
                } 
                else if (strcmp(line_buffer, "uptime") == 0) {
                    printk(0, "%ds\n", (int)uptime_segundos);
                } 
                else if (strncmp(line_buffer, "alarm ", 6) == 0) {
                    u64 t = 0;
                    int i = 6; // comeca dps do alarm
                    
                    while (line_buffer[i] >= '0' && line_buffer[i] <= '9') {
                        t = t * 10 + (u64)(line_buffer[i] - '0');
                        i++;
                    }
                    u64 flags = hart_irq_save();
					user_alarm_sobrando = t;
					hart_irq_restore(flags);
                    // timer_set_alarm(1);
				}

                pos = 0; 
                printk(0, "> "); 
				
            } 
            else if (pos < 255) {
                line_buffer[pos++] = c;
                serial_putc(c); 
            }
        }
    }
}
