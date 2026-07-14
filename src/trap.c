#include <kernel/trap.h>
#include <kernel/panic.h>
#include <arch/csr.h>


/* defined in src/trap_entry.S */
extern void trap_entry();

void timer_irq(void);
void plic_irq_handler(void);

void handle_irq(u64 code)
{
    if (code == 5) {
        timer_irq(); 
    } else if (code == 9) {
        plic_irq_handler(); 
    } else {
        panic("interrupcao desconhecida: %d\n", code);
    }
}

void handle_exception(u64 code)
{
    u64 stval = csr_read(CSR_STVAL);
    u64 sepc = csr_read(CSR_SEPC);
    
    printk(0, "deu excecao. scause: %d, stval: 0x%lx, sepc: 0x%lx\n", code, stval, sepc);
    panic("kernel crashou com a excecao\n");
}

void trap_setup()
{
	/* not implemented */

    csr_write(CSR_STVEC, (u64)trap_entry);

}

void handle_trap(struct registers *regs)
{
    u64 scause = csr_read(CSR_SCAUSE);

    if (scause & TRAP_IRQ_BIT) {
        handle_irq(scause & ~TRAP_IRQ_BIT); 
    } else {
        handle_exception(scause);
    }
}

void hart_irq_enable()
{
    csr_set(CSR_SSTATUS, CSR_SSTATUS_SIE);
}

void hart_irq_disable()
{
    csr_clear(CSR_SSTATUS, CSR_SSTATUS_SIE);
}

u64 hart_irq_save()
{
    u64 sstatus = csr_read(CSR_SSTATUS);
    csr_clear(CSR_SSTATUS, CSR_SSTATUS_SIE);
    return sstatus & CSR_SSTATUS_SIE;
}

void hart_irq_restore(u64 flags)
{
    if (flags & CSR_SSTATUS_SIE) {
        csr_set(CSR_SSTATUS, CSR_SSTATUS_SIE);
    } else {
        csr_clear(CSR_SSTATUS, CSR_SSTATUS_SIE);
    }
}