#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>  // Para Shell_NotifyIcon e estruturas relacionadas
#include <vector>
#include <string>

// abaixo define se é debug ou não, apenas descomentar para debug.
#define ENABLE_DEBUG_CONSOLE

#ifdef ENABLE_DEBUG_CONSOLE
#include <iostream>
#endif

// Variáveis globais para as telas
RECT g_screen1Rect = { 0 };
RECT g_screen2Rect = { 0 };
HMONITOR g_screen1Hmonitor = NULL;
HMONITOR g_screen2Hmonitor = NULL;
bool g_screen1Found = false;
bool g_screen2Found = false;

// armazenar posições do cursor
POINT g_lastPositionBeforeAlt1 = { 0, 0 };
POINT g_lastPositionBeforeAlt2 = { 0, 0 };
bool g_wasAlt1Last = false;  // Indica se o último comando foi Alt+1
bool g_wasAlt2Last = false;  // Indica se o último comando foi Alt+2

// Registra os hotkeys para Alt+1 e Alt+2
const int ID_HOTKEY_ALT_1 = 1;
const int ID_HOTKEY_ALT_2 = 2;
const int ID_HOTKEY_ALT_DEL = 3;


BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    MONITORINFOEX mi;
    mi.cbSize = sizeof(MONITORINFOEX);
    if (GetMonitorInfo(hMonitor, (LPMONITORINFO)&mi)) {
        if (mi.dwFlags & MONITORINFOF_PRIMARY) {
            if (!g_screen1Found) {
                g_screen1Rect = mi.rcMonitor;
                g_screen1Hmonitor = hMonitor;
                g_screen1Found = true;
#ifdef ENABLE_DEBUG_CONSOLE
                std::cout << "Tela 1 (Primaria) encontrada: "
                    << "Rect: " << mi.rcMonitor.left << "," << mi.rcMonitor.top << " - "
                    << mi.rcMonitor.right << "," << mi.rcMonitor.bottom << std::endl;
#endif
            }
        }
        else {
            if (!g_screen2Found) {
                g_screen2Rect = mi.rcMonitor;
                g_screen2Hmonitor = hMonitor;
                g_screen2Found = true;
#ifdef ENABLE_DEBUG_CONSOLE
                std::cout << "Tela 2 (Secundaria) encontrada: "
                    << "Rect: " << mi.rcMonitor.left << "," << mi.rcMonitor.top << " - "
                    << mi.rcMonitor.right << "," << mi.rcMonitor.bottom << std::endl;
#endif
            }
        }
    }
    return TRUE; // Continua a enumeração para encontrar todos os monitores
}

void SetupScreens() {
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

    if (!g_screen1Found) {
        HMONITOR hMon = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi;
        mi.cbSize = sizeof(MONITORINFO);
        if (GetMonitorInfo(hMon, &mi)) {
            g_screen1Rect = mi.rcMonitor;
            g_screen1Hmonitor = hMon;
            g_screen1Found = true;
#ifdef ENABLE_DEBUG_CONSOLE
            std::cout << "Tela 1 (Primaria) (fallback): "
                << "Rect: " << mi.rcMonitor.left << "," << mi.rcMonitor.top << " - "
                << mi.rcMonitor.right << "," << mi.rcMonitor.bottom << std::endl;
#endif
        }
    }

    // Se não encontrou uma segunda tela, usa a primeira como segunda
    if (!g_screen2Found && g_screen1Found) {
        g_screen2Rect = g_screen1Rect;
        g_screen2Hmonitor = g_screen1Hmonitor;
        g_screen2Found = true;
#ifdef ENABLE_DEBUG_CONSOLE
        std::cout << "Apenas um monitor encontrado. Tela 2 logica usara as coordenadas da Tela 1." << std::endl;
#endif
    }
}

// Função para centralizar o cursor em uma tela específica
void CenterCursorOnScreen(int screenNumber) {
    POINT currentPos;
    GetCursorPos(&currentPos);

    // Salva a posição atual antes de mover
    if (screenNumber == 1) {
        // Verifica se a última ação foi Alt+1 (centralizar na tela 1)
        if (g_wasAlt1Last) {
            // Volta para a posição anterior
            SetCursorPos(g_lastPositionBeforeAlt1.x, g_lastPositionBeforeAlt1.y);
            g_wasAlt1Last = false;
#ifdef ENABLE_DEBUG_CONSOLE
            std::cout << "Voltou para posição anterior: ("
                << g_lastPositionBeforeAlt1.x << "," << g_lastPositionBeforeAlt1.y << ")" << std::endl;
#endif
        }
        else {
            // Salva posição atual e centraliza na tela 1
            g_lastPositionBeforeAlt1 = currentPos;
            int centerX = g_screen1Rect.left + (g_screen1Rect.right - g_screen1Rect.left) / 2;
            int centerY = g_screen1Rect.top + (g_screen1Rect.bottom - g_screen1Rect.top) / 2;
            SetCursorPos(centerX, centerY);
            g_wasAlt1Last = true;
            g_wasAlt2Last = false;
#ifdef ENABLE_DEBUG_CONSOLE
            std::cout << "Cursor centralizado na Tela 1: (" << centerX << "," << centerY << ")" << std::endl;
#endif
        }
    }
    //aqui centraliza na tela 2
    else if (screenNumber == 2) {
        // Verifica se a última ação foi Alt+2 (centralizar na tela 2)
        if (g_wasAlt2Last) {
            // Volta para a posição anterior
            SetCursorPos(g_lastPositionBeforeAlt2.x, g_lastPositionBeforeAlt2.y);
            g_wasAlt2Last = false;
#ifdef ENABLE_DEBUG_CONSOLE
            std::cout << "Voltou para posição anterior: ("
                << g_lastPositionBeforeAlt2.x << "," << g_lastPositionBeforeAlt2.y << ")" << std::endl;
#endif
        }
        else {
            // Salva posição atual e centraliza na tela 2
            g_lastPositionBeforeAlt2 = currentPos;
            int centerX = g_screen2Rect.left + (g_screen2Rect.right - g_screen2Rect.left) / 2;
            int centerY = g_screen2Rect.top + (g_screen2Rect.bottom - g_screen2Rect.top) / 2;
            SetCursorPos(centerX, centerY);
            g_wasAlt2Last = true;
            g_wasAlt1Last = false;
#ifdef ENABLE_DEBUG_CONSOLE
            std::cout << "Cursor centralizado na Tela 2: (" << centerX << "," << centerY << ")" << std::endl;
#endif
        }
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        // Registra os hotkeys Alt+1 e Alt+2 e Alt+Del
        if (!RegisterHotKey(hwnd, ID_HOTKEY_ALT_1, MOD_ALT, '1')) {
#ifdef ENABLE_DEBUG_CONSOLE
            std::cerr << "Falha ao registrar hotkey Alt+1. Erro: " << GetLastError() << std::endl;
#endif
        }
        if (!RegisterHotKey(hwnd, ID_HOTKEY_ALT_2, MOD_ALT, '2')) {
#ifdef ENABLE_DEBUG_CONSOLE
            std::cerr << "Falha ao registrar hotkey Alt+2. Erro: " << GetLastError() << std::endl;
#endif

            if (!RegisterHotKey(hwnd, ID_HOTKEY_ALT_DEL, MOD_ALT, '0')) {
#ifdef ENABLE_DEBUG_CONSOLE
                std::cerr << "Falha ao registrar hotkey Alt+Del. Erro: " << GetLastError() << std::endl;
#endif
            }

        }
        break;

    case WM_HOTKEY:
        // Processa os hotkeys
        if (wParam == ID_HOTKEY_ALT_1) {
            CenterCursorOnScreen(1);
        }

        else if (wParam == ID_HOTKEY_ALT_DEL) {
#ifdef ENABLE_DEBUG_CONSOLE
            std::cout << "Alt+Del pressionado. Encerrando o programa." << std::endl;
#endif
            PostQuitMessage(0);
        }


        else if (wParam == ID_HOTKEY_ALT_2) {
            CenterCursorOnScreen(2);
        }
        break;

    case WM_DESTROY:
        // Desregistra os hotkeys antes de fechar
        UnregisterHotKey(hwnd, ID_HOTKEY_ALT_1);
        UnregisterHotKey(hwnd, ID_HOTKEY_ALT_2);
        UnregisterHotKey(hwnd, ID_HOTKEY_ALT_DEL);

        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Exibe um ícone na área de notificação (system tray)
void AddTrayIcon(HWND hwnd) {
    NOTIFYICONDATA nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    lstrcpy(nid.szTip, L"Cursor Repositioner - Alt+1/Alt+2");
    Shell_NotifyIcon(NIM_ADD, &nid);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#ifdef ENABLE_DEBUG_CONSOLE
    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONIN$", "r", stdin);
    freopen_s(&dummy, "CONERR$", "w", stderr);
    std::cout << "Console de depuracao inicializado." << std::endl;
#endif

    SetupScreens();
    if (!g_screen1Found) {
#ifdef ENABLE_DEBUG_CONSOLE
        std::cerr << "Nao foi possivel encontrar a tela primaria. Saindo." << std::endl;
#endif
        MessageBox(NULL, L"Nao foi possivel encontrar a tela primaria.", L"Erro", MB_OK | MB_ICONERROR);
        return 1;
    }

    const wchar_t CLASS_NAME[] = L"CursorRepositionerClass";

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassEx(&wc)) {
#ifdef ENABLE_DEBUG_CONSOLE
        std::cerr << "Registro da classe de janela falhou. Erro: " << GetLastError() << std::endl;
#endif
        MessageBox(NULL, L"Registro da Classe de Janela Falhou!", L"Erro", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"Cursor Repositioner", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 200,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
#ifdef ENABLE_DEBUG_CONSOLE
        std::cerr << "Criacao da janela falhou. Erro: " << GetLastError() << std::endl;
#endif
        MessageBox(NULL, L"Criacao da Janela Falhou!", L"Erro", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Minimiza a janela e adiciona ícone na área de notificação
    ShowWindow(hwnd, SW_HIDE);
    AddTrayIcon(hwnd);

#ifdef ENABLE_DEBUG_CONSOLE
    std::cout << "Aplicacao inicializada. Use Alt+1 para centralizar na Tela 1 ou Alt+2 para centralizar na Tela 2." << std::endl;
    std::cout << "Pressionar o mesmo atalho novamente retornará à posição anterior." << std::endl;
#endif

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

#ifdef ENABLE_DEBUG_CONSOLE
    if (dummy) fclose(dummy);
    FreeConsole();
#endif

    return (int)msg.wParam;
}