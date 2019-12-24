#include "Window.h"

Window::Window()
{

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_CREATE:
		{
			// Event fired when the window is created
			break;
		}
		case WM_DESTROY:
		{
			// Event fired when the window is destroyed
			::PostQuitMessage(0);
			break;
		}
		default:
		{
			return ::DefWindowProc(hwnd, msg, wparam, lparam);
		}
	}

	return NULL;
}

bool Window::init()
{
	LPCSTR className = "MyWindowClass";
	WNDCLASSEX wc;
	wc.cbClsExtra = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = NULL;
	wc.lpszClassName = className;
	wc.lpszMenuName = "";
	wc.style = NULL;
	wc.lpfnWndProc = &WndProc;

	if (!::RegisterClassEx(&wc)) // If the registration of class fails, the function returns false
	{
		return false;
	}

	m_hwnd = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, className, "DirectX Application", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
		NULL, NULL, NULL, NULL);

	if (!m_hwnd)
	{
		return false;
	}

	// Show up the window
	::ShowWindow(m_hwnd, SW_SHOW);
	::UpdateWindow(m_hwnd);

	return true;
}

bool Window::release()
{
	// Destroy the window
	if (!::DestroyWindow(m_hwnd))
	{
		return false;
	}
	return true;
}

Window::~Window()
{

}
