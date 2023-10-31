#include <windows.h>
#include <iostream>

HWND hwndDuration, hwndButton, hwndStart, hwndStop;
int clickDuration = 3;  // Default click duration in seconds
POINT clickPosition = { -1, -1 };   // Initialize to an invalid position
bool clicking = false;  // Indicates if auto-clicking is active

// Timer ID
#define TIMER_ID 1

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Create the "Duration" label
            CreateWindow("STATIC", "Duration:", WS_CHILD | WS_VISIBLE, 20, 20, 80, 20, hwnd, nullptr, nullptr, nullptr);

            // Create the "Time Duration" text field
            hwndDuration = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
                                        100, 20, 80, 20, hwnd, (HMENU)1, nullptr, nullptr);

            // Create the "Record Position" button
            hwndButton = CreateWindow("BUTTON", "Record Position", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                                      20, 60, 150, 30, hwnd, (HMENU)2, nullptr, nullptr);

            // Create the "Start Auto-Clicking" button
            hwndStart = CreateWindow("BUTTON", "Start Auto-Clicking", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                                     20, 100, 150, 30, hwnd, (HMENU)3, nullptr, nullptr);
            EnableWindow(hwndStart, FALSE);

            // Create the "Stop Auto-Clicking" button
            hwndStop = CreateWindow("BUTTON", "Stop Auto-Clicking", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                                    180, 100, 150, 30, hwnd, (HMENU)4, nullptr, nullptr);
            EnableWindow(hwndStop, FALSE);
            break;
        }

        case WM_COMMAND: {
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == 2) { // "Record Position" button clicked
                    SetCapture(hwnd); // Capture mouse input
                }
                else if (LOWORD(wParam) == 3) { // "Start Auto-Clicking" button clicked
                    clickDuration = GetDlgItemInt(hwnd, 1, nullptr, FALSE); // Retrieve the click duration from the text field

                    if (clickDuration > 0) {
                        clicking = true;
                        SetTimer(hwnd, TIMER_ID, clickDuration * 1000, nullptr); // Set the timer to trigger after the specified duration
                        MessageBox(hwnd, "Auto-Clicking started!", "Info", MB_OK | MB_ICONINFORMATION);
                        EnableWindow(hwndButton, FALSE); // Disable recording while clicking
                        EnableWindow(hwndStart, FALSE); // Disable the "Start Auto-Clicking" button
                        EnableWindow(hwndStop, TRUE); // Enable the "Stop Auto-Clicking" button
                    }
                    else {
                        MessageBox(hwnd, "Please enter a valid click duration.", "Error", MB_OK | MB_ICONERROR);
                    }
                }
                else if (LOWORD(wParam) == 4) { // "Stop Auto-Clicking" button clicked
                    clicking = false;
                    KillTimer(hwnd, TIMER_ID); // Stop the timer
                    MessageBox(hwnd, "Auto-Clicking stopped!", "Info", MB_OK | MB_ICONINFORMATION);
                    EnableWindow(hwndButton, TRUE); // Enable recording again
                    EnableWindow(hwndStart, TRUE); // Enable the "Start Auto-Clicking" button
                    EnableWindow(hwndStop, FALSE); // Disable the "Stop Auto-Clicking" button
                }
            }
            break;
        }

        case WM_TIMER: {
            if (wParam == TIMER_ID && clicking) {
                if (clickPosition.x != -1 && clickPosition.y != -1) {
                    // Move the cursor to the recorded position
                    SetCursorPos(clickPosition.x, clickPosition.y);

                    // Simulate a left mouse button click
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                }
            }
            break;
        }

        case WM_LBUTTONDOWN: {
            if (GetCapture() == hwnd) {
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                clickPosition = cursorPos;
                char posStr[50];
                snprintf(posStr, sizeof(posStr), "Click position recorded! X:%d Y:%d", clickPosition.x, clickPosition.y);
                MessageBox(hwnd, posStr, "Info", MB_OK | MB_ICONINFORMATION);
                EnableWindow(hwndStart, TRUE);
            } else {
                SetCapture(hwnd); // Capture mouse input
            }
            break;
        }


        case WM_CLOSE:
            DestroyWindow(hwnd);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, "AutoClicker", nullptr};
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindow("AutoClicker", "Auto Clicker", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 200, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
