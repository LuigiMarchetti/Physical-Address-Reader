#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

#define SECTOR_SIZE 512
#define BYTES_PER_LINE 8

// Window controls IDs
#define ID_SECTOR_INPUT 2
#define ID_READ_SECTOR 3
#define ID_BUTTON_PREV 4
#define ID_BUTTON_NEXT 5

#define ID_TOOLTIP_NEXT 1001
#define ID_TOOLTIP_PREV 1002


// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void ReadAndDisplaySector(HWND hList, LONGLONG sectorNumber);

void DisplaySectorData(HWND hList, BYTE *buffer, DWORD bytesRead, LONGLONG physicalAddress);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SectorReaderClass";
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "SectorReaderClass",
        "Disk C Sector Reader",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {0};
    // "Event listener" of application
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hList, hSectorInput, hDriveInfo, hButton, hButtonNext, hButtonPrev, hTooltip;
    RECT rect;

    switch (uMsg) {
        case WM_CREATE: {
    InitCommonControls();

    // Criação dos botões e outros controles
    hDriveInfo = CreateWindow("STATIC", "Drive C",
                              WS_VISIBLE | WS_CHILD,
                              10, 10, 300, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("STATIC", "Sector Number:",
                 WS_VISIBLE | WS_CHILD,
                 10, 40, 100, 20, hwnd, NULL, NULL, NULL);

    hSectorInput = CreateWindow("EDIT", "",
                                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                120, 40, 150, 20, hwnd, (HMENU)ID_SECTOR_INPUT, NULL, NULL);

    hButton = CreateWindow("BUTTON", "Read Sector",
                           WS_VISIBLE | WS_CHILD,
                           280, 40, 100, 30, hwnd, (HMENU)ID_READ_SECTOR, NULL, NULL);

    hButtonNext = CreateWindow("BUTTON", ">", WS_VISIBLE | WS_CHILD,
                               410, 40, 30, 30, hwnd, (HMENU)ID_BUTTON_NEXT, NULL, NULL);

    hButtonPrev = CreateWindow("BUTTON", "<", WS_VISIBLE | WS_CHILD,
                               380, 40, 30, 30, hwnd, (HMENU)ID_BUTTON_PREV, NULL, NULL);

    hList = CreateWindow(WC_LISTVIEW, "",
                         WS_VISIBLE | WS_CHILD | LVS_REPORT,
                         10, 80, 760, 500, hwnd, NULL, NULL, NULL);

    ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    // Configuração das colunas do ListView
    LVCOLUMN lvCol;
    lvCol.mask = LVCF_TEXT | LVCF_WIDTH;

    lvCol.pszText = "Physical Address";
    lvCol.cx = 150;
    ListView_InsertColumn(hList, 0, &lvCol);

    lvCol.pszText = "Hexadecimal";
    lvCol.cx = 250;
    ListView_InsertColumn(hList, 1, &lvCol);

    lvCol.pszText = "Binary";
    lvCol.cx = 200;
    ListView_InsertColumn(hList, 2, &lvCol);

    lvCol.pszText = "ASCII";
    lvCol.cx = 150;
    ListView_InsertColumn(hList, 3, &lvCol);

    // Configuração do tooltip
    hTooltip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL,
                              WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
                              CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                              hwnd, NULL, NULL, NULL);

    SetWindowPos(hTooltip, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    // Tooltip para o botão "Next"
    TOOLINFO tiNext = {0};
    tiNext.cbSize = sizeof(TOOLINFO);
    tiNext.uFlags = TTF_SUBCLASS;
    tiNext.hwnd = hButtonNext;
    tiNext.hinst = NULL;
    tiNext.uId = (UINT_PTR)hButtonNext;
    tiNext.lpszText = "Go to the next sector";
    GetClientRect(hButtonNext, &tiNext.rect);
    SendMessage(hTooltip, TTM_ADDTOOL, 0, (LPARAM)&tiNext);

    // Tooltip para o botão "Prev"
    TOOLINFO tiPrev = {0};
    tiPrev.cbSize = sizeof(TOOLINFO);
    tiPrev.uFlags = TTF_SUBCLASS;
    tiPrev.hwnd = hButtonPrev;
    tiPrev.hinst = NULL;
    tiPrev.uId = (UINT_PTR)hButtonPrev;
    tiPrev.lpszText = "Go to the previous sector";
    GetClientRect(hButtonPrev, &tiPrev.rect);
    SendMessage(hTooltip, TTM_ADDTOOL, 0, (LPARAM)&tiPrev);

    // Set initial sector to 0
    SetWindowText(hSectorInput, "0");

    // Read and display the initial sector (sector 0)
    ListView_DeleteAllItems(hList);
    ReadAndDisplaySector(hList, 0);

    break;
}


        case WM_SIZE: {
            // Obtenha o tamanho atual da janela
            GetClientRect(hwnd, &rect);

            // Ajuste os controles dinamicamente
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            MoveWindow(hDriveInfo, 10, 10, width - 20, 20, TRUE);
            MoveWindow(hSectorInput, 120, 40, 150, 20, TRUE);
            MoveWindow(hButton, 280, 40, 100, 30, TRUE);
            MoveWindow(hList, 10, 80, width - 20, height - 90, TRUE);

            break;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_READ_SECTOR) {
                char sectorStr[32];
                GetWindowText(hSectorInput, sectorStr, sizeof(sectorStr));
                LONGLONG sectorNumber = _atoi64(sectorStr);

                if (sectorNumber < 0) {
                    MessageBox(hwnd, "Please enter a valid sector number", "Error", MB_OK | MB_ICONERROR);
                    break;
                }

                ListView_DeleteAllItems(hList);
                ReadAndDisplaySector(hList, sectorNumber);
            } else if (LOWORD(wParam) == ID_BUTTON_PREV) {
                char sectorStr[32];
                GetWindowText(hSectorInput, sectorStr, sizeof(sectorStr));
                LONGLONG sectorNumber = _atoi64(sectorStr);

                if (sectorNumber > 0) {
                    sectorNumber--; // Decrementa o número do setor
                    snprintf(sectorStr, sizeof(sectorStr), "%lld", sectorNumber);
                    SetWindowText(hSectorInput, sectorStr);

                    ListView_DeleteAllItems(hList);
                    ReadAndDisplaySector(hList, sectorNumber);
                }
            } else if (LOWORD(wParam) == ID_BUTTON_NEXT) {
                char sectorStr[32];
                GetWindowText(hSectorInput, sectorStr, sizeof(sectorStr));
                LONGLONG sectorNumber = _atoi64(sectorStr);

                sectorNumber++; // Incrementa o número do setor
                snprintf(sectorStr, sizeof(sectorStr), "%lld", sectorNumber);
                SetWindowText(hSectorInput, sectorStr);

                ListView_DeleteAllItems(hList);
                ReadAndDisplaySector(hList, sectorNumber);
            }
            break;
        }


        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


void ReadAndDisplaySector(HWND hList, LONGLONG sectorNumber) {
    // Open volume C:
    HANDLE hDrive = CreateFile("\\\\.\\PhysicalDrive0",
                               GENERIC_READ,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);

    if (hDrive == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, "Error opening drive C:", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    LARGE_INTEGER distanceToMove;
    distanceToMove.QuadPart = sectorNumber * SECTOR_SIZE;

    LARGE_INTEGER newPosition;
    if (!SetFilePointerEx(hDrive, distanceToMove, &newPosition, FILE_BEGIN)) {
        MessageBox(NULL, "Error seeking to sector", "Error", MB_OK | MB_ICONERROR);
        CloseHandle(hDrive);
        return;
    }

    BYTE buffer[SECTOR_SIZE];
    DWORD bytesRead;
    if (!ReadFile(hDrive, buffer, SECTOR_SIZE, &bytesRead, NULL)) {
        MessageBox(NULL, "Error reading sector", "Error", MB_OK | MB_ICONERROR);
        CloseHandle(hDrive);
        return;
    }

    LONGLONG physicalAddress = sectorNumber * SECTOR_SIZE;
    DisplaySectorData(hList, buffer, bytesRead, physicalAddress);

    CloseHandle(hDrive);
}

void DisplaySectorData(HWND hList, BYTE *buffer, DWORD bytesRead, LONGLONG physicalAddress) {
    for (DWORD offset = 0; offset < bytesRead; offset += BYTES_PER_LINE) {
        char hexStr[BYTES_PER_LINE * 3 + 1] = "";
        char binStr[BYTES_PER_LINE * 9 + 1] = "";
        char asciiStr[BYTES_PER_LINE + 1] = "";
        char physAddr[32];

        // Format physical address
        snprintf(physAddr, sizeof(physAddr), "0x%08llX", physicalAddress + offset);

        // Convert bytes to hex, binary, and ASCII
        for (DWORD i = 0; i < BYTES_PER_LINE && (offset + i) < bytesRead; i++) {
            // Hex
            char byteHex[4];
            snprintf(byteHex, sizeof(byteHex), "%02X ", buffer[offset + i]);
            strcat(hexStr, byteHex);

            // Binary
            char byteBin[9] = "";
            for (int j = 7; j >= 0; j--) {
                strcat(byteBin, (buffer[offset + i] & (1 << j)) ? "1" : "0");
            }
            strcat(byteBin, " ");
            strcat(binStr, byteBin);

            // ASCII
            asciiStr[i] = (buffer[offset + i] >= 32 && buffer[offset + i] <= 126)
                              ? buffer[offset + i]
                              : '.';
            asciiStr[i + 1] = '\0';
        }

        // Add to ListView
        LVITEM lvItem;
        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = offset / BYTES_PER_LINE;
        lvItem.iSubItem = 0;
        lvItem.pszText = physAddr;
        ListView_InsertItem(hList, &lvItem);

        ListView_SetItemText(hList, lvItem.iItem, 1, hexStr);
        ListView_SetItemText(hList, lvItem.iItem, 2, binStr);
        ListView_SetItemText(hList, lvItem.iItem, 3, asciiStr);
    }
}