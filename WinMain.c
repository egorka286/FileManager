//#undef UNICODE
//#undef _UNICODE
#include <Windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <io.h>
#include <tchar.h>
#include "resource.h"

LRESULT CALLBACK MainProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK PanelProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

HWND hPrimary;
HWND hSecondary;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, INT nCmdShow)
{
	TCHAR szMainClassName[] = TEXT("Main");
	WNDCLASSEX wclMain;
	wclMain.cbSize = sizeof(wclMain);
	wclMain.style = CS_HREDRAW | CS_VREDRAW;
	wclMain.lpfnWndProc = MainProc;
	wclMain.cbClsExtra = 0;
	wclMain.cbWndExtra = 0;
	wclMain.hInstance = hInstance;
	wclMain.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wclMain.hCursor = LoadCursor(NULL, IDC_ARROW);
	wclMain.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wclMain.lpszMenuName = (const TCHAR*)IDR_MENU;
	wclMain.lpszClassName = szMainClassName;
	wclMain.hIconSm = NULL;

	TCHAR szPanelClassName[] = TEXT("Panel");
	WNDCLASSEX wclPanel;
	wclPanel.cbSize = sizeof(wclPanel);
	wclPanel.style = CS_HREDRAW | CS_VREDRAW;
	wclPanel.lpfnWndProc = PanelProc;
	wclPanel.cbClsExtra = 0;
	wclPanel.cbWndExtra = 0;
	wclPanel.hInstance = hInstance;
	wclPanel.hIcon = NULL;
	wclPanel.hCursor = NULL;
	wclPanel.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wclPanel.lpszMenuName = NULL;
	wclPanel.lpszClassName = szPanelClassName;
	wclPanel.hIconSm = NULL;

	if (!RegisterClassEx(&wclMain) || !RegisterClassEx(&wclPanel))
		return 0;

	HWND hMainWnd = CreateWindowEx(0, szMainClassName, TEXT("Файловый менеджер"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	HACCEL hAccel = LoadAccelerators(hInstance, (const TCHAR*)IDR_ACCELERATOR);

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);

	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hMainWnd, hAccel, &Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
	}
	return Msg.wParam;
}

LRESULT CALLBACK MainProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;
	static HWND hPanelWndLeft;
	static HWND hPanelWndRight;
	static HWND hButtonCopy;
	static HWND hButtonRename;
	static HWND hButtonMkdir;
	static HWND hButtonRemove;
	static HWND hButtonExit;
	switch (uMessage)
	{
	case WM_CREATE:
	{
		hInstance = GetModuleHandle(0);

		hPanelWndLeft = CreateWindowEx(0, TEXT("Panel"), NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
		hPanelWndRight = CreateWindowEx(0, TEXT("Panel"), NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);

		hButtonCopy = CreateWindowEx(0, TEXT("Button"), TEXT("F5 Копировать"), WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
		hButtonRename = CreateWindowEx(0, TEXT("Button"), TEXT("F6 Переименовать"), WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
		hButtonMkdir = CreateWindowEx(0, TEXT("Button"), TEXT("F7 Создать папку"), WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
		hButtonRemove = CreateWindowEx(0, TEXT("Button"), TEXT("F8 Удалить"), WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
		hButtonExit = CreateWindowEx(0, TEXT("Button"), TEXT("F10 Выход"), WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);

		hPrimary = hPanelWndLeft;
		hSecondary = hPanelWndRight;

		SetFocus(hPrimary);

		break;
	}
	case WM_SIZE:
	{
		int nWidth = LOWORD(lParam);
		int nHeight = HIWORD(lParam);

		MoveWindow(hPanelWndLeft, 2, 2, nWidth / 2 - 4, nHeight - 27, TRUE);
		MoveWindow(hPanelWndRight, nWidth / 2 + 2, 2, nWidth / 2 - 4, nHeight - 27, TRUE);

		MoveWindow(hButtonCopy, 2, nHeight - 25, nWidth / 5, 25, TRUE);
		MoveWindow(hButtonRename, 2 + nWidth / 5, nHeight - 25, nWidth / 5, 25, TRUE);
		MoveWindow(hButtonMkdir, 2 + nWidth / 5 * 2, nHeight - 25, nWidth / 5, 25, TRUE);
		MoveWindow(hButtonRemove, 2 + nWidth / 5 * 3, nHeight - 25, nWidth / 5, 25, TRUE);
		MoveWindow(hButtonExit, 2 + nWidth / 5 * 4, nHeight - 25, nWidth - nWidth / 5 * 4, 25, TRUE);

		break;
	}
	case WM_COMMAND:
	{
		if ((HWND)lParam == hButtonExit && HIWORD(wParam) == BN_CLICKED)
		{
			SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_EXIT, 0), (LPARAM)NULL);
			break;
		}

		if ((HWND)lParam == hButtonMkdir && HIWORD(wParam) == BN_CLICKED)
		{
			SendMessage(hPanelWndLeft, WM_CD, 0, (LPARAM)TEXT("C:\\Windows"));
			break;
		}

		switch (LOWORD(wParam))
		{
		case ID_EXIT:
		{
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			break;
		}
		}
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	default:
	{
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	}
	}
	return 0;
}

LRESULT CALLBACK PanelProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	HWND hList = FindWindowEx(hWnd, NULL, TEXT("SysListView32"), NULL);
	HWND hPath = FindWindowEx(hWnd, NULL, TEXT("Edit"), NULL);

	switch (uMessage)
	{
	case WM_CREATE:
	{
		hList = CreateWindowEx(0, TEXT("SysListView32"), NULL, WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL | LVS_NOLABELWRAP | LVS_ALIGNLEFT | WS_TABSTOP | WS_BORDER, 0, 0, 0, 0, hWnd, NULL, NULL, NULL);
		hPath = CreateWindowEx(0, TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, NULL, NULL, NULL);

		LVCOLUMN lvCol;
		memset(&lvCol, 0, sizeof(lvCol));

		lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		lvCol.fmt = LVCFMT_LEFT;

		lvCol.cx = 100;
		lvCol.pszText = TEXT("Имя");
		SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&lvCol);

		lvCol.cx = 70;
		lvCol.pszText = TEXT("Тип");
		SendMessage(hList, LVM_INSERTCOLUMN, 1, (LPARAM)&lvCol);

		lvCol.cx = 80;
		lvCol.fmt = LVCFMT_RIGHT;
		lvCol.pszText = TEXT("Размер");
		SendMessage(hList, LVM_INSERTCOLUMN, 2, (LPARAM)&lvCol);

		TCHAR szCurPath[255];
		GetCurrentDirectory(sizeof(szCurPath), szCurPath);
		SendMessage(hWnd, WM_CD, 0, (LPARAM)szCurPath);

		break;
	}
	case WM_SIZE:
	{
		int nWidth = LOWORD(lParam);
		int nHeight = HIWORD(lParam);

		MoveWindow(hList, 0, 20, nWidth, nHeight - 20, TRUE);
		SendMessage(hList, LVM_SETCOLUMNWIDTH, 0, nWidth - 70 - 80 - 20);
		MoveWindow(hPath, 0, 0, nWidth, 20, TRUE);

		break;
	}
	case WM_CD:
	{
		SetWindowText(hPath, (const TCHAR*)lParam);
		SetCurrentDirectory((const TCHAR*)lParam);
		SendMessage(hWnd, WM_DIR, 0, 0);

		break;
	}
	case WM_DIR:
	{
		int i = 0;
		struct _tfinddata_t fileinfo;
		LVITEM lvItem;
		memset(&lvItem, 0, sizeof(lvItem));
		intptr_t hFile;
		SendMessage(hList, LVM_DELETEALLITEMS, 0, 0);

		if ((hFile = _tfindfirst(TEXT("*.*"), &fileinfo)) != -1)
		{
			do
			{
				if (fileinfo.attrib & _A_SUBDIR && lstrcmp(fileinfo.name, TEXT(".")))
				{
					lvItem.mask = LVIF_TEXT;

					lvItem.iItem = i++;
					lvItem.iSubItem = 0;
					lvItem.pszText = fileinfo.name;
					SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&lvItem);

					lvItem.iSubItem = 2;
					lvItem.pszText = TEXT("<Папка>");
					SendMessage(hList, LVM_SETITEM, 0, (LPARAM)&lvItem);
				}
			} while (_tfindnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
		
		if ((hFile = _tfindfirst(TEXT("*.*"), &fileinfo)) != -1)
		{
			do
			{
				if (!(fileinfo.attrib & _A_SUBDIR))
				{
					lvItem.mask = LVIF_TEXT;
					lvItem.iItem = i++;
					lvItem.iSubItem = 0;
					lvItem.pszText = fileinfo.name;
					SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&lvItem);

					long nSize = fileinfo.size;
					TCHAR szSize[20] = { 0 };
					TCHAR szFormatSize[20] = { 0 };
					_stprintf_s(szSize, sizeof(szSize) / sizeof(TCHAR), TEXT("%ld"), nSize);
					NUMBERFMT format;
					format.NumDigits = 0;
					format.LeadingZero = 1;
					format.Grouping = 3;
					format.lpDecimalSep = TEXT(".");
					format.lpThousandSep = TEXT(" ");
					format.NegativeOrder = 0;
					GetNumberFormat((LCID)LOCALE_NAME_USER_DEFAULT, 0, szSize, &format, szFormatSize, sizeof(szFormatSize) / sizeof(TCHAR));
					
					lvItem.iSubItem = 2;
					lvItem.pszText = szFormatSize;
					SendMessage(hList, LVM_SETITEM, 0, (LPARAM)&lvItem);
				}
			} while (_tfindnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}

		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);

		break;
	}
	default:
	{
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	}
	}
	return 0;
}
