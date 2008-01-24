#include "CUIDialog.h"

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

CUIDialog::CUIDialog(void)
{
}

CUIDialog::CUIDialog(CUIWindow* parentWindow):
CUIWindow(parentWindow, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	CW_USEDEFAULT)
{
}

CUIDialog::CUIDialog(HINSTANCE hInstance, HWND hParentWindow):
CUIWindow(hParentWindow, hInstance, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	CW_USEDEFAULT)
{
}

CUIDialog::~CUIDialog(void)
{
}

void CUIDialog::dealloc(void)
{
	CUIWindow::dealloc();
}

INT_PTR CUIDialog::privateDialogProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	m_curHWnd = hDlg;
	m_curMessage = message;
	m_curWParam = wParam;
	m_curLParam = lParam;
	return dialogProc(hDlg, message, wParam, lParam);
}

BOOL CUIDialog::doInitDialog(HWND /*hKbControl*/)
{
	return TRUE;
}

INT_PTR CUIDialog::dialogProc(
	HWND /*hDlg*/,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)doInitDialog((HWND)wParam);
		break;

	case WM_COMMAND:
		if (doCommand(HIWORD(wParam), LOWORD(wParam), (HWND)lParam) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_DESTROY:
		if (doDestroy() == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_VSCROLL:
		if (doVScroll((int)(short)LOWORD(wParam), (int)(short)HIWORD(wParam),
			(HWND)lParam) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_HSCROLL:
		if (doHScroll((int)(short)LOWORD(wParam), (int)(short)HIWORD(wParam),
			(HWND)lParam) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_NOTIFY:
		if (doNotify((int)(short)LOWORD(wParam), (LPNMHDR)lParam) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

LRESULT CUIDialog::doVScroll(
	int /*scrollCode*/,
	int /*position*/,
	HWND /*hScrollBar*/)
{
	return 1;
}

LRESULT CUIDialog::doHScroll(
	int /*scrollCode*/,
	int /*position*/,
	HWND /*hScrollBar*/)
{
	return 1;
}

void CUIDialog::doOK(void)
{
	EndDialog(hWindow, (INT_PTR)IDOK);
}

void CUIDialog::doCancel(void)
{
	EndDialog(hWindow, (INT_PTR)IDCANCEL);
}

LRESULT CUIDialog::doTextFieldChange(int /*controlId*/, HWND /*control*/)
{
	return 1;
}

LRESULT CUIDialog::doCommand(
	int notifyCode,
	int commandId,
	HWND control)
{
	if (commandId == IDOK)
	{
		doOK();
		return 0;
	}
	else if (commandId == IDCANCEL)
	{
		doCancel();
		return 0;
	}
	else if (notifyCode == EN_CHANGE)
	{
		return doTextFieldChange(commandId, control);
	}
	return CUIWindow::doCommand(notifyCode, commandId, control);
}

INT_PTR CUIDialog::doModal(UINT dialogId, HWND hWndParent /*= NULL*/)
{
	return doModal(MAKEINTRESOURCEA(dialogId), hWndParent);
}

INT_PTR CUIDialog::doModal(LPCTSTR dialogName, HWND hWndParent /*= NULL*/)
{
	return ::DialogBoxParam(hInstance, dialogName, hWndParent, staticDialogProc,
		(LPARAM)this);
}

INT_PTR CALLBACK CUIDialog::staticDialogProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	CUIDialog *dialog;

	if (message == WM_INITDIALOG)
	{
		dialog = (CUIDialog *)lParam;
		dialog->hWindow = hDlg;
		::SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)dialog);
	}
	else
	{
		dialog = (CUIDialog *)::GetWindowLongPtr(hDlg, GWLP_USERDATA);
	}
	if (dialog)
	{
		return dialog->privateDialogProc(hDlg, message, wParam, lParam);
	}
	return (INT_PTR)FALSE;
	//return DefDlgProc(hDlg, message, wParam, lParam);
}

bool CUIDialog::checkGet(int buttonId)
{
	return SendDlgItemMessage(hWindow, buttonId, BM_GETCHECK, 0, 0) != 0;
}

void CUIDialog::checkSet(int buttonId, bool value)
{
	SendDlgItemMessage(hWindow, buttonId, BM_SETCHECK, value, 0);
}

void CUIDialog::sliderSetup(
	int controlId,
	short min,
	short max,
	WORD frequency,
	int value)
{
	CUIWindow::setupDialogSlider(hWindow, controlId, min, max, frequency,
		value);
}

void CUIDialog::sliderSetTic(int controlId, int position)
{
	SendDlgItemMessage(hWindow, controlId, TBM_SETTIC, 0, position);
}

void CUIDialog::sliderSetValue(int controlId, int value)
{
	SendDlgItemMessage(hWindow, controlId, TBM_SETPOS, 1, value);
}

int CUIDialog::sliderGetValue(int controlId)
{
	return SendDlgItemMessage(hWindow, controlId, TBM_GETPOS, 0, 0);
}

void CUIDialog::spinSetup(
	int controlId,
	short min,
	short max,
	int value,
	UDACCEL *accels	/*= NULL*/,
	int numAccels	/* = 0*/)
{

	SendDlgItemMessage(hWindow, controlId, UDM_SETRANGE, 0, MAKELONG((short)max,
		(short)min)); 
	spinSetValue(controlId, value);
	if (accels && numAccels > 0)
	{
		SendDlgItemMessage(hWindow, controlId, UDM_SETACCEL,
			(WPARAM)numAccels, (long)accels);
	}
}

void CUIDialog::spinSetValue(
	int controlId,
	int value)
{
	SendDlgItemMessage(hWindow, controlId, UDM_SETPOS, 0, value);
}

void CUIDialog::comboAddString(int controlId, CUCSTR string)
{
	sendDlgItemMessageUC(hWindow, controlId, CB_ADDSTRING, 0, (LPARAM)string);
}

LRESULT CUIDialog::comboSelectItem(int controlId, int index)
{
	return SendDlgItemMessage(hWindow, controlId, CB_SETCURSEL,
		(WPARAM)index, 0);
}

int CUIDialog::comboGetSelectedItem(int controlId)
{
	return (int)SendDlgItemMessage(hWindow, controlId, CB_GETCURSEL, 0, 0);
}

void CUIDialog::windowGetText(int controlId, ucstring &text)
{
	text.resize(sendDlgItemMessageUC(hWindow, controlId, WM_GETTEXTLENGTH, 0,
		0));
	sendDlgItemMessageUC(hWindow, controlId, WM_GETTEXT,
		(WPARAM)text.size() + 1, (LPARAM)&text[0]);
}

void CUIDialog::windowSetText(int controlId, const ucstring &text)
{
	sendDlgItemMessageUC(hWindow, controlId, WM_SETTEXT, 0,
		(LPARAM)text.c_str());
}

#ifndef TC_NO_UNICODE

void CUIDialog::windowGetText(int controlId, std::string &text)
{
	text.resize(SendDlgItemMessageA(hWindow, controlId, WM_GETTEXTLENGTH, 0,
		0));
	SendDlgItemMessageA(hWindow, controlId, WM_GETTEXT,
		(WPARAM)text.size() + 1, (LPARAM)&text[0]);
}

void CUIDialog::windowSetText(int controlId, const std::string &text)
{
	SendDlgItemMessageA(hWindow, controlId, WM_SETTEXT, 0,
		(LPARAM)text.c_str());
}

#endif // TC_NO_UNICODE

ucstring CUIDialog::windowGetText(int controlId)
{
	ucstring retValue;

	windowGetText(controlId, retValue);
	return retValue;
}

void CUIDialog::textFieldSetLimitText(int controlId, int value)
{
	SendDlgItemMessage(hWindow, controlId, EM_SETLIMITTEXT, (WPARAM)value, 0);
}

void CUIDialog::textFieldGetSelection(int controlId, int &start, int &end)
{
	SendDlgItemMessage(hWindow, controlId, EM_GETSEL, (WPARAM)&start,
		(LPARAM)&end);
}

void CUIDialog::textFieldSetSelection(int controlId, int start, int end)
{
	SendDlgItemMessage(hWindow, controlId, EM_SETSEL, (WPARAM)start,
		(LPARAM)end);
}