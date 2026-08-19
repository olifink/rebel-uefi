#include <efi.h>
#include <efilib.h>

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    UINTN Index;
    EFI_INPUT_KEY Key;
    EFI_STATUS Status;

    InitializeLib(ImageHandle, SystemTable);

    // Clear screen for a clean terminal view
    uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

    Print(L"=========================================\n");
    Print(L"  Rebel Forth UEFI Kernel Initializing... \n");
    Print(L"=========================================\n\n");

    // --- FORTH ENGINE INITIALIZATION GOES HERE ---
    // e.g., rebel_init();

    Print(L"Interactive UEFI Shell ready. Type characters (ESC to exit):\n> ");

    // Reset keyboard buffer
    uefi_call_wrapper(SystemTable->ConIn->Reset, 2, SystemTable->ConIn, FALSE);

    while (1) {
        // Pause until an input event occurs on ConIn
        uefi_call_wrapper(SystemTable->BootServices->WaitForEvent, 3, 1, &SystemTable->ConIn->WaitForKey, &Index);
        Status = uefi_call_wrapper(SystemTable->ConIn->ReadKeyStroke, 2, SystemTable->ConIn, &Key);
        if (EFI_ERROR(Status)) {
            continue;
        }

        // Exit on ESC key
        if (Key.ScanCode == SCAN_ESC) {
            Print(L"\n\nExiting back to UEFI Firmware...\n");
            break;
        }

        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
            Print(L"\r\n> ");
        } else if (Key.UnicodeChar == CHAR_BACKSPACE) {
            Print(L"\b \b");
        } else if (Key.UnicodeChar != 0) {
            Print(L"%c", Key.UnicodeChar);
        }
    }

    return EFI_SUCCESS;
}
