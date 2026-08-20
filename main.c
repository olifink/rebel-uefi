#include <efi.h>
#include <efilib.h>

void list_and_set_video_mode(EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    EFI_STATUS status;

    // Locate the Graphics Output Protocol
    status = uefi_call_wrapper(SystemTable->BootServices->LocateProtocol, 3,
                               &gopGuid, NULL, (void **)&gop);

    if (EFI_ERROR(status))
    {
        Print(L"GOP not supported!\n");
        return;
    }

    // Query current mode information
    UINT32 maxModes = gop->Mode->MaxMode;
    Print(L"Supported Screen Modes: %d\n", maxModes);

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN sizeOfInfo;

    for (UINT32 i = 0; i < maxModes; i++)
    {
        status = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &sizeOfInfo, &info);
        if (!EFI_ERROR(status))
        {
            Print(L"Mode %d: %dx%d\n", i, info->HorizontalResolution, info->VerticalResolution);
        }
    }

    // Example: Set to mode 0 or target resolution mode
    // uefi_call_wrapper(gop->SetMode, 2, gop, 0);
}

// Direct pixel drawing from Forth (e.g., 32-bit ARGB/XRGB)
void draw_pixel(EFI_SYSTEM_TABLE *SystemTable, uint32_t x, uint32_t y, uint32_t color)
{
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    EFI_STATUS status;

    // Locate the Graphics Output Protocol
    status = uefi_call_wrapper(SystemTable->BootServices->LocateProtocol, 3,
                               &gopGuid, NULL, (void **)&gop);

    if (EFI_ERROR(status))
    {
        Print(L"GOP not supported!\n");
        return;
    }

    // Accessing raw framebuffer memory in GOP
    uint32_t *framebuffer = (uint32_t *)gop->Mode->FrameBufferBase;
    uint32_t pitch = gop->Mode->Info->PixelsPerScanLine;

    framebuffer[y * pitch + x] = color;
}

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    UINTN Index;
    EFI_INPUT_KEY Key;
    EFI_STATUS Status;

    InitializeLib(ImageHandle, SystemTable);

    // Clear screen for a clean terminal view
    uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

    Print(L"=========================================\n");
    Print(L"  Rebel Forth UEFI Kernel Initializing... \n");
    Print(L"=========================================\n\n");

    list_and_set_video_mode(SystemTable);

    // --- FORTH ENGINE INITIALIZATION GOES HERE ---
    // e.g., rebel_init();

    Print(L"Interactive UEFI Shell ready. Type characters (ESC to exit):\n> ");

    // Reset keyboard buffer
    uefi_call_wrapper(SystemTable->ConIn->Reset, 2, SystemTable->ConIn, FALSE);

    int pos = 100;

    while (1)
    {
        draw_pixel(SystemTable, pos++, 100, 0xFF00FF00); // Draw a green pixel at (100, 100)

        // Pause until an input event occurs on ConIn
        uefi_call_wrapper(SystemTable->BootServices->WaitForEvent, 3, 1, &SystemTable->ConIn->WaitForKey, &Index);
        Status = uefi_call_wrapper(SystemTable->ConIn->ReadKeyStroke, 2, SystemTable->ConIn, &Key);
        if (EFI_ERROR(Status))
        {
            continue;
        }

        // Exit on ESC key
        if (Key.ScanCode == SCAN_ESC)
        {
            Print(L"\n\nExiting back to UEFI Firmware...\n");
            break;
        }

        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN)
        {
            Print(L"\r\n> ");
        }
        else if (Key.UnicodeChar == CHAR_BACKSPACE)
        {
            Print(L"\b \b");
        }
        else if (Key.UnicodeChar != 0)
        {
            Print(L"%c", Key.UnicodeChar);
        }
    }

    return EFI_SUCCESS;
}
