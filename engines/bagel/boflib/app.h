
/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef BAGEL_BOFLIB_APP_H
#define BAGEL_BOFLIB_APP_H

#include "bagel/boflib/boffo.h"
#include "bagel/boflib/gui/window.h"
#include "bagel/boflib/gfx/cursor.h"
#include "bagel/boflib/error.h"
#include "bagel/boflib/list.h"

namespace Bagel {

#define MAX_APPNAME 128
#define DEFAULT_MAINLOOPS   1
#define kReallyFastPPC 50
#define kReallySlowPPC 200

class CBofApp : public CBofError {
private:
	CBofWindow *m_pWindow = nullptr;
	CBofWindow *_captureControl = nullptr;
	CBofWindow *_focusControl = nullptr;

protected:
	void StartupCode();
	void ShutDownCode();

	char m_szAppName[MAX_APPNAME] = { 0 };
	CBofList<CBofCursor> m_cCursorList;
	CBofCursor m_cDefaultCursor;

	CBofWindow *m_pMainWnd = nullptr;
	CBofPalette *m_pPalette = nullptr;
	CBofPalette *m_pDefPalette = nullptr;
	INT m_nScreenDX = 0;
	INT m_nScreenDY = 0;
	INT m_nColorDepth = 0;

	INT m_nIterations = DEFAULT_MAINLOOPS;

	static CBofApp *m_pBofApp;

	virtual bool shouldQuit() const = 0;

public:
	CBofApp();
	CBofApp(const char *pszAppName);
	virtual ~CBofApp();

	ERROR_CODE PreInit();
	ERROR_CODE PreShutDown();
	ERROR_CODE PostShutDown();

	// These functions can be overridden by the child class
	virtual ERROR_CODE initialize();
	virtual ERROR_CODE RunApp();
	virtual ERROR_CODE shutdown();

	void SetAppName(const char *pszNewAppName) {
		Common::strcpy_s(m_szAppName, pszNewAppName);
	}

	const char *GetAppName() const {
		return (const char *)m_szAppName;
	}

	void SetMainWindow(CBofWindow *pWnd) {
		m_pMainWnd = pWnd;
	}
	CBofWindow *GetMainWindow() const {
		return m_pMainWnd;
	}

	CBofWindow *GetActualWindow() const {
		return m_pWindow;
	}

	void SetPalette(CBofPalette *pPalette);

	CBofPalette *GetPalette() const {
		return m_pPalette;
	}

	INT ScreenWidth() const {
		return m_nScreenDX;
	}
	INT ScreenHeight() const {
		return m_nScreenDY;
	}
	INT ScreenDepth() const {
		return m_nColorDepth;
	}

	CBofCursor GetDefaultCursor() const {
		return m_cDefaultCursor;
	}
	void SetDefaultCursor(CBofCursor &cCursor) {
		m_cDefaultCursor = cCursor;
	}

	void AddCursor(CBofCursor &cCursor);
	void DelCursor(INT nIndex);

	CBofCursor GetCursor(INT nIndex) {
		return m_cCursorList[nIndex];
	}
	INT GetNumberOfCursors() const {
		return m_cCursorList.GetCount();
	}

	void setCaptureControl(CBofWindow *ctl) {
		_captureControl = ctl;
	}
	CBofWindow *getCaptureControl() const {
		return _captureControl;
	}
	void setFocusControl(CBofWindow *ctl) {
		_focusControl = ctl;
	}
	CBofWindow *getFocusControl() const {
		return _focusControl;
	}

	static uint32 GetMachineSpeed() {
		return kReallyFastPPC;
	}

	static CBofApp *GetApp() {
		return m_pBofApp;
	}

#if BOF_WINDOWS
	HDIGDRIVER GetDriver() {
		return m_hDriver;
	}
	HMDIDRIVER GetMidiDriver() {
		return m_hMidiDriver;
	}

	static void SetInstanceHandle(HINSTANCE hInstance) {
		m_hInstance = hInstance;
	}

	static HINSTANCE GetInstanceHandle() {
		return m_hInstance;
	}
#elif BOF_MAC

#endif
};

// Global routines
//
void BofPostMessage(CBofWindow *pWindow, uint32 lMessage, uint32 lParam1, uint32 lParam2);
void BofMessageBox(const char *pszTitle, const char *pszMessage);

#define Quit() g_engine->quitGame()

void SetMousePos(CBofPoint &cPoint);
CBofPoint GetMousePos();

} // namespace Bagel

#endif
