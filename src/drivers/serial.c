#include <kernel/serial.h>
#include <kernel/panic.h>

 
#define UART_BASE (0xffffffe000000000ULL + 0x10000000ULL)

#define RBR 0 
#define THR 0 
#define IER 1 
#define FCR 2 
#define LCR 3
#define LSR 5 
#define LSR_RX_READY (1 << 0) 
#define LSR_TX_VAZIO (1 << 5) 

#define uart_read_reg(reg) (*(volatile u8 *)(UART_BASE + (reg)))
#define uart_write_reg(reg, v) (*(volatile u8 *)(UART_BASE + (reg)) = (v))


static char rx_buffer[1024];

static size_t rx_head = 0;
static size_t rx_tail = 0;

void serial_init()
{
	uart_write_reg(IER, 0x00);
    uart_write_reg(LCR, 0x03);
    uart_write_reg(FCR, 0x07);
    serial_irq_enable();
}

void serial_irq_enable()
{
	u8 ier = uart_read_reg(IER);
    uart_write_reg(IER, ier | 0x01);
}

void serial_irq_disable()
{
	u8 ier = uart_read_reg(IER);
    uart_write_reg(IER, ier & ~0x01);
}

void serial_irq()
{
	while (uart_read_reg(LSR) & LSR_RX_READY) {
        char c = uart_read_reg(RBR);

        size_t next_head = (rx_head + 1) % 1024;

        if (next_head != rx_tail) {
            rx_buffer[rx_head] = c;
            rx_head = next_head;
        }
    }
}

size_t serial_read(char *buf)
{
	size_t bytes_lidos = 0;

    serial_irq_disable();

    while (rx_tail != rx_head) {
        buf[bytes_lidos] = rx_buffer[rx_tail];
        rx_tail = (rx_tail + 1) % 1024;
        bytes_lidos++;
    }
    serial_irq_enable();

    return bytes_lidos;
}

void serial_putc(char c)
{
	while (!(uart_read_reg(LSR) & LSR_TX_VAZIO)) {
		//fica esperando aq
    }
    uart_write_reg(THR, c);
}

void serial_puts(char *str)
{
	while (*str) {
        serial_putc(*str);
        str++;
    }
}


