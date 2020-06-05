//#undef UNICODE
//#undef _UNICODE


/*************************************************************************************
CreateDirectory
DeleteFile
FindClose
FindFirstFile
FindNextFile
GetDiskFreeSpace
GetDriveType
GetFileAttributes
GetFileSize
GetFileTime
GetLogicalDrives
GetLogicalDriveStrings
RemoveDirectory
*************************************************************************************/

#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <io.h>
#include <tchar.h>
#include <stdlib.h>
#include <strsafe.h>
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
		TCHAR szFullPath[MAX_PATH];

		hList = CreateWindowEx(0, WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL, 0, 0, 0, 0, hWnd, NULL, NULL, NULL);
		ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT);
		hPath = CreateWindowEx(0, WC_EDIT, NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_READONLY, 0, 0, 0, 0, hWnd, NULL, NULL, NULL);

		LVCOLUMN lvColumn;

		lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		lvColumn.fmt = LVCFMT_LEFT;

		lvColumn.cx = 100;
		lvColumn.pszText = TEXT("Имя");
		ListView_InsertColumn(hList, 0, &lvColumn);

		lvColumn.cx = 70;
		lvColumn.pszText = TEXT("Тип");
		ListView_InsertColumn(hList, 1, &lvColumn);

		lvColumn.cx = 80;
		lvColumn.fmt = LVCFMT_RIGHT;
		lvColumn.pszText = TEXT("Размер");
		ListView_InsertColumn(hList, 2, &lvColumn);

		// получение и сохранение текущего пути
		_tfullpath(szFullPath, TEXT("*.*"), MAX_PATH);
		Edit_SetText(hPath, szFullPath);

		SendMessage(hWnd, WM_UPDATE, 0, 0);

		break;
	}
	case WM_SIZE:
	{
		int nWidth = LOWORD(lParam);
		int nHeight = HIWORD(lParam);

		MoveWindow(hList, 0, 20, nWidth, nHeight - 20, TRUE);
		ListView_SetColumnWidth(hList, 0, nWidth - ListView_GetColumnWidth(hList, 1) - ListView_GetColumnWidth(hList, 2) - 20);
		MoveWindow(hPath, 0, 0, nWidth, 20, TRUE);

		break;
	}
	case WM_NOTIFY:
	{
		if (((LPNMITEMACTIVATE)lParam)->hdr.hwndFrom == hList)
			if (((LPNMITEMACTIVATE)lParam)->hdr.code == NM_DBLCLK)
			{
				TCHAR szPath[MAX_PATH];
				TCHAR szName[MAX_PATH];
				TCHAR szExt[MAX_PATH];
				TCHAR szFullName[MAX_PATH];
				HANDLE hFindFile;
				WIN32_FIND_DATA  fileinfo;

				ListView_GetItemText(hList, ((LPNMITEMACTIVATE)lParam)->iItem, 0, szName, MAX_PATH);
				ListView_GetItemText(hList, ((LPNMITEMACTIVATE)lParam)->iItem, 1, szExt, MAX_PATH);
				_tmakepath(szFullName, NULL, NULL, szName, szExt);

				Edit_GetText(hPath, szPath, MAX_PATH);
				SetCurrentDirectory(szPath);

				if (!lstrcmp(szFullName, TEXT("..")))
				{
					SetCurrentDirectory(szFullName);
					_tfullpath(szFullName, TEXT("*.*"), MAX_PATH);
					Edit_SetText(hPath, szFullName);
					SendMessage(hWnd, WM_UPDATE, 0, 0);
				}
				else
					if ((hFindFile = FindFirstFile(szFullName, &fileinfo)) != INVALID_HANDLE_VALUE)
					{
						// FILE_ATTRIBUTE_REPARSE_POINT вроде для ссылок
						if (fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							SetCurrentDirectory(szFullName);
							_tfullpath(szFullName, TEXT("*.*"), MAX_PATH);
							Edit_SetText(hPath, szFullName);
							SendMessage(hWnd, WM_UPDATE, 0, 0);
						}
						//else
						//	MessageBox(hWnd, TEXT("Не каталог"), 0, MB_ICONHAND | MB_OK);
						FindClose(hFindFile);
					}
					else
						MessageBox(hWnd, TEXT("Файл не найден"), 0, MB_ICONHAND | MB_OK);
			}
		break;
	}
	case WM_UPDATE:
	{
		int i = 0;
		HANDLE hFindFile;
		ULONGLONG nFileSize;
		TCHAR szPath[MAX_PATH];
		TCHAR szFileSize[MAX_PATH];
		TCHAR szFormatSize[MAX_PATH];
		WIN32_FIND_DATA  fileinfo;
		NUMBERFMT format;
		TCHAR szFileName[MAX_PATH];
		TCHAR szFileExt[MAX_PATH];
		LVITEM lvItem;

		memset(&lvItem, 0, sizeof(lvItem));
		lvItem.mask = LVIF_TEXT;
		ListView_DeleteAllItems(hList);

		Edit_GetText(hPath, szPath, MAX_PATH);
		SetCurrentDirectory(szPath);

		// добавление каталогов
		if ((hFindFile = FindFirstFile(TEXT("*.*"), &fileinfo)) != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && lstrcmp(fileinfo.cFileName, TEXT(".")))
				{
					lvItem.iItem = i++;
					lvItem.iSubItem = 0;
					lvItem.pszText = fileinfo.cFileName;
					ListView_InsertItem(hList, &lvItem);

					lvItem.iSubItem = 2;
					lvItem.pszText = TEXT("<Папка>");
					ListView_SetItem(hList, &lvItem);
				}
			} while (FindNextFile(hFindFile, &fileinfo));
			FindClose(hFindFile);
		}
		// добавление файлов
		if ((hFindFile = FindFirstFile(TEXT("*.*"), &fileinfo)) != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!(fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					lstrcpy(szFileExt, TEXT("."));
					_tsplitpath(fileinfo.cFileName, NULL, NULL, szFileName, szFileExt);
					if (lstrlen(szFileName) == 0)
					{
						lstrcpy(szFileName, fileinfo.cFileName);
						lstrcpy(szFileExt, TEXT("."));
					}
					lvItem.iItem = i++;
					lvItem.iSubItem = 0;
					lvItem.pszText = szFileName;
					ListView_InsertItem(hList, &lvItem);

					lvItem.iSubItem = 1;
					lvItem.pszText = &szFileExt[1];
					ListView_SetItem(hList, &lvItem);

					nFileSize = ((ULONGLONG)fileinfo.nFileSizeHigh * ((ULONGLONG)MAXDWORD + (ULONGLONG)1)) + (ULONGLONG)fileinfo.nFileSizeLow;
					_ui64tot_s(nFileSize, szFileSize, MAX_PATH, 10);
					format.NumDigits = 0;
					format.LeadingZero = 1;
					format.Grouping = 3;
					format.lpDecimalSep = TEXT(".");
					format.lpThousandSep = TEXT(" ");
					format.NegativeOrder = 0;
					GetNumberFormat((LCID)LOCALE_NAME_USER_DEFAULT, 0, szFileSize, &format, szFormatSize, MAX_PATH);

					lvItem.iSubItem = 2;
					lvItem.pszText = szFormatSize;
					ListView_SetItem(hList, &lvItem);
				}
			} while (FindNextFile(hFindFile, &fileinfo));
			FindClose(hFindFile);
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
