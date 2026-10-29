# Rebel Forth UEFI Kernel

A standalone x86_64 UEFI application and kernel testbed written in C using `gnu-efi`.

---

## Prerequisites & Dependencies

The application targets the **x86_64 (amd64)** UEFI architecture. It can be built natively on x86_64 Linux or cross-compiled from an ARM64 (aarch64) host.

### Debian / Ubuntu (ARM64 / aarch64 Host)

If you are developing on an ARM-based Linux system (such as Debian on ARM Chromebooks/Googlebooks), enable the `amd64` multiarch foreign architecture and install the cross-compilation toolchain and runtime utilities:

```bash
# Enable amd64 foreign architecture
sudo dpkg --add-architecture amd64
sudo apt-get update

# Cross-compiler, binutils, and amd64 EFI libraries
sudo apt-get install -y gcc-x86-64-linux-gnu binutils-x86-64-linux-gnu gnu-efi:amd64

# QEMU emulator, OVMF firmware, and mtools (for creating FAT disk images)
sudo apt-get install -y qemu-system-x86 ovmf mtools
```

### Debian / Ubuntu (Native x86_64 Host)

If building natively on an x86_64 Linux machine:

```bash
sudo apt-get update
sudo apt-get install -y build-essential gnu-efi qemu-system-x86 ovmf mtools
```

---

## Building

To build the UEFI application:

```bash
make
```

This compiles `main.c` and outputs the PE32+ UEFI executable:
- `REBEL.EFI`

To clean build artifacts:

```bash
make clean
```

---

## Running in QEMU

A helper script `run.sh` is provided. It compiles the program, creates a virtual FAT32 disk image (`disk.img`), places the binary at `/EFI/BOOT/BOOTX64.EFI`, and launches QEMU with OVMF firmware:

```bash
./run.sh
```

- When the application boots, it queries the Graphics Output Protocol (GOP) display modes, draws sample pixels, and starts an interactive shell.
- Press **ESC** inside the virtual machine to exit back to the UEFI firmware shell.

---

## Testing on Real Hardware (Bootable USB)

To prepare files for a physical USB flash drive:

1. Generate the USB directory structure:
   ```bash
   make usb
   ```
   This creates a `usb/` folder structured as required by the UEFI specification:
   ```text
   usb/
   └── EFI/
       └── BOOT/
           ├── BOOTX64.EFI
           └── REBEL.EFI
   ```

2. Format a USB flash drive as **FAT32** (or FAT16).

3. Copy the `EFI` directory from `usb/` directly to the root of your USB drive:
   ```bash
   cp -r usb/EFI /media/<your-username>/<USB-DRIVE>/
   ```

4. Insert the USB drive into your target x86_64 PC, enter the BIOS/UEFI boot selection menu (commonly F11, F12, or Esc at boot), and select your USB drive in UEFI mode.
