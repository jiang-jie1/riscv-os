.PHONY: kernel clean

CFLAGS=-Wall -O0 \
	-fno-omit-frame-pointer -ggdb \
	-MD -mcmodel=medany \
	-ffreestanding -fno-common -nostdlib \
	-mno-relax \
	-fno-stack-protector -fno-pie -no-pie \
	-z max-page-size=4096
	
kernel: 
	riscv64-unknown-elf-gcc $(CFLAGS) \
		kernel/entry.S \
		kernel/kernelvec.S \
		kernel/trap.c \
		kernel/start.c\
		kernel/uart.c \
		kernel/main.c \
		kernel/console.c \
		kernel/printf.c \
		kernel/pmm.c \
		kernel/vm.c \
		kernel/vm_boot.c \
		-T kernel/kernel.ld \
		-o kernel.elf

QEMU=qemu-system-riscv64 -machine virt -bios none \
	-kernel kernel.elf \
	-m 128 \
	-smp 1 \
	-nographic

qemu: kernel
	$(QEMU)

qemu-gdb: kernel
	$(QEMU) -s -S
	
gdb: kernel
	$(QEMU) -s -S & \
	gdb-multiarch -q -x gdbinit
clean:
	rm -rf kernel.elf kernel/*.o kernel.d
