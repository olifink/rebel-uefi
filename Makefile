ARCH            ?= x86_64
HOST_ARCH       := $(shell uname -m)

ifneq ($(HOST_ARCH),x86_64)
CROSS_COMPILE   ?= x86_64-linux-gnu-
endif

CC              = $(CROSS_COMPILE)gcc
LD              = $(CROSS_COMPILE)ld
OBJCOPY         = $(CROSS_COMPILE)objcopy

OBJS            = main.o
TARGET          = REBEL.EFI
USB_DIR         = usb

EFIINC          = /usr/include/efi
EFIINCS         = -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol
LIB             = /usr/lib
EFILIB          = /usr/lib
EFI_CRT0        = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFILIB)/elf_$(ARCH)_efi.lds

CFLAGS          = $(EFIINCS) -fno-stack-protector -fpic \
                  -fshort-wchar -mno-red-zone -Wall -ffreestanding
LDFLAGS         = -shared -Bsymbolic -L$(EFILIB) -L$(LIB) \
                  -T $(EFI_LDS) $(EFI_CRT0)

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

rebel.so: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ -lgnuefi -lefi

$(TARGET): rebel.so
	$(OBJCOPY) -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc -j .rodata \
		--target=efi-app-$(ARCH) rebel.so $@

usb: $(TARGET)
	mkdir -p $(USB_DIR)/EFI/BOOT
	cp $(TARGET) $(USB_DIR)/EFI/BOOT/BOOTX64.EFI
	cp $(TARGET) $(USB_DIR)/EFI/BOOT/

clean:
	rm -rf *.o *.so $(TARGET) $(USB_DIR)

.PHONY: all clean usb

