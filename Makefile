ARCH            = x86_64
OBJS            = main.o
TARGET          = REBEL.EFI

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
	gcc $(CFLAGS) -c $< -o $@

rebel.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ -lgnuefi -lefi

$(TARGET): rebel.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc -j .rodata \
		--target=efi-app-$(ARCH) rebel.so $@

clean:
	rm -f *.o *.so $(TARGET)

