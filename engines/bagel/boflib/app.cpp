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

#include "common/system.h"

#include "bagel/boflib/boffo.h"

#include "bagel/boflib/debug.h"
#include "bagel/boflib/misc.h"
#include "bagel/boflib/app.h"
#include "bagel/boflib/timer.h"
#include "bagel/boflib/gfx/text.h"
#include "bagel/boflib/sound.h"

namespace Bagel {

#define DEBUG_LOG "DEBUG.LOG"
#define DEBUG_INI "BOFFO.INI"

#define BOFDISP 0

#if BOF_MAC && USEDRAWSPROCKET
CGrafPtr gBackBuffer;
#endif

CBofApp *CBofApp::m_pBofApp;

#if BOF_WINDOWS
HINSTANCE CBofApp::m_hInstance = nullptr;

extern "C" HINSTANCE g_hInst = nullptr;
#endif

CHAR g_szCopyright[] = "Copyright(C) 1996 Boffo Games, Inc.  All rights reserved.";

// Local functions
//
BOOL FirstInstance();
#if BOF_MAC && USEDRAWSPROCKET
void InitDSpContextAttributes(DSpContextAttributes *inAttributes);
#endif

CBofApp::CBofApp() {
	StartupCode();
}


CBofApp::CBofApp(const CHAR *pszAppName) {
	StartupCode();

	SetAppName(pszAppName);
}

CBofApp::~CBofApp() {
	ShutDownCode();

	m_szAppName[0] = '\0';
	m_pMainWnd = nullptr;
	m_pPalette = nullptr;
	m_pBofApp = nullptr;
}


VOID CBofApp::StartupCode() {
	BOOL bRand;

	// Open the Boffo debug options file (BOFFO.INI)
	bRand = TRUE;
	if ((g_pDebugOptions = new CBofDebugOptions(DEBUG_INI)) != nullptr) {
		g_pDebugOptions->ReadSetting("DebugOptions", "MainLoops", &m_nIterations, DEFAULT_MAINLOOPS);
		bRand = g_pDebugOptions->m_bRandomOn;
	}

	// Initialize the logging file (DEBUG.LOG)
	g_pDebugLog = new CBofLog(DEBUG_LOG);

#if BOF_DEBUG
	// Mark all currently allocated memory blocks so that a call
	// to VerifyAllBlocksDeleted() in the destructor will tell us about any
	// new blocks that have not been freed.
	MarkMemBlocks();
#endif

	//
	// Initialize the boffo libraries
	//

	// Init the Window library
	CBofWindow::Initialize();

	// Init the text library
	CBofText::Initialize();
}


VOID CBofApp::ShutDownCode() {
	// Un-initialize the text library
	CBofText::ShutDown();

	// Shut down the Window library
	CBofWindow::ShutDown();

	// Kill any shared palette
	CBofPalette::SetSharedPalette(nullptr);

#if BOF_DEBUG
	// Make sure that all memory allocated by our game has been deleted
	VerifyAllBlocksDeleted();
#endif

	if (g_pDebugLog != nullptr) {
		delete g_pDebugLog;
		g_pDebugLog = nullptr;
	}
	if (g_pDebugOptions != nullptr) {
		delete g_pDebugOptions;
		g_pDebugOptions = nullptr;
	}
}


ERROR_CODE CBofApp::PreInit() {
#if BOF_MAC || BOF_WINMAC
	HideMenuBar();
#endif

	if ((m_pPalette == nullptr) && (m_pDefPalette == nullptr)) {

		if ((m_pDefPalette = new CBofPalette) != nullptr) {
			m_pDefPalette->CreateDefault();
			SetPalette(m_pDefPalette);
		}
	}

#if BOFDISP
	CBofWindow *pWnd;

	// Allocate a default palette
	//
	if ((m_pDefPalette = new CBofPalette) != nullptr) {
		m_pDefPalette->LoadPalette("ANIMOIDS.BMP");
		//m_pDefPalette->CreateDefault(PAL_DEFAULT);

		if (m_pPalette == nullptr)
			m_pPalette = m_pDefPalette;

	} else {
		ReportError(ERR_MEMORY);
	}

	if ((pWnd = new CBofAppWindow()) != nullptr) {
		CBofRect cRect(0, 0, m_nScreenDX - 1, m_nScreenDY - 1);

#ifdef _DEBUG
		cRect.SetRect(0, 0, 640 - 1, 480 - 1);
#endif

		pWnd->Create(m_szAppName, &cRect, nullptr);
		pWnd->Show();

		m_pWindow = pWnd;
	} else {
		ReportError(ERR_MEMORY);
	}

	CBofDisplayObject::Initialize();
#endif

	LogInfo(BuildString("CBofApp::PreInit - Available Physical Memory: %ld", GetFreePhysMem()));

	LogInfo(BuildString("Initializing %s...", m_szAppName));

	return (m_errCode);
}


ERROR_CODE CBofApp::Initialize() {
	//Assert(IsValidObject(this));

	return (m_errCode);
}


ERROR_CODE CBofApp::RunApp() {
	CBofWindow *pWindow;
	INT i, nCount;

	// jwl 08.08.96 add code for profiling.
#if BOF_MAC && __profile__ && PROFILE_GAME
	OSErr oserr = ::ProfilerInit(collectSummary, bestTimeBase, 300, 20);
	Assert(oserr == noErr);
	::ProfilerSetStatus(true);
#endif

	LogInfo(BuildString("CBofApp::RunApp - Available Physical Memory: %ld", GetFreePhysMem()));

#if BOF_WINDOWS
	MSG msg;

#elif BOF_MAC
	CBofMessage *pMessage;

	EventRecord event;
	Rect theRect;

#endif

#if BOFDISP
	// You should have registered a main window by this point
	Assert(m_pWindow != nullptr);
#endif

	nCount = m_nIterations;

	// Acquire and dispatch messages until a BM_QUIT message is received,
	// or until there are too many errors
	//

	while (CBofError::GetErrorCount() < MAX_ERRORS) {

#if BOF_WINDOWS

		// if there is a message for a window, then process it
		//
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {

#if BOF_DEBUG
			if ((g_pDebugOptions != nullptr) && g_pDebugOptions->m_bShowMessages) {
				LogInfo(BuildString("HWND1: %08lx, MSG: %08lx (%08lx, %08lx)", msg.hwnd, msg.message, msg.wParam, msg.lParam));
			}
#endif

			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// This 2nd Message hook is needed for Wave Mixing, because when
		// we play a wave mix sound, that 1st message hook only gets MM_WOM_DONE
		// messages, and the app looks halted until well after the sound stops.
		// I put in this 2nd hook so that we can get other messages.
		//
		// If there is a message for the main window (or any of it's children),
		// then process it.
		//

#elif BOF_MAC

		const ULONG wneInterval = 15; // ticks between WNE calls

		::OSEventAvail(0, &event);

		if (event.where.h != m_prevMouse.h || event.where.v != m_prevMouse.v) {

			//  synthesize a mouse moved event to take advantage of the existing
			//  architecture for HandleMacEvent.
			event.what = osEvt;
			event.message = ((ULONG) mouseMovedMessage) << 24;

			if (CBofWindow::HandleMacEvent(&event))
				break;

			m_prevMouse = event.where;
		}

		if (event.when >= m_nextWNETime) {

			// if there is a message for a window, then process it

//	        if (::WaitNextEvent(everyEvent, &event, 0xFFFFFFFF, hCursorRgn) != 0) {
			if (::WaitNextEvent(everyEvent, &event, 0, nil) != 0) {

				// Convert the Mac event into a message our CBofWindows can handle.
				// HandleMacEvent returns TRUE when QUIT message is received.
				//
				if (CBofWindow::HandleMacEvent(&event))
					break;
			}

			m_nextWNETime = event.when + wneInterval;
		}

		CBofWindow::HandleMacTimers();
#endif
		CBofSound::AudioTask();

#if BOFDISP
		CBofDisplayWindow::HandleMessages();
#endif

		CBofTimer::HandleTimers();

		if (nCount < 0)  {
			nCount++;
			if (nCount == 0)
				nCount = 1;

		} else {

			for (i = 0; i < nCount; i++) {

				// give each window it's own main loop (sort-of)
				//
				pWindow = CBofWindow::GetWindowList();
				while (pWindow != nullptr) {

					if (pWindow->IsCreated()) {
#if BOF_MAC
						STBofPort stSavePort(pWindow->GetMacWindow());
#if PALETTESHIFTFIX
						CBofWindow::CheckPaletteShiftList();
#endif
#endif
						pWindow->OnMainLoop();
					}

					pWindow = (CBofWindow *)pWindow->GetNext();
				}
			}

			nCount = m_nIterations;
		}
	}
#if BOF_MAC

#if __profile__ && PROFILE_GAME
	::ProfilerSetStatus(false);
#if __POWERPC__
	::ProfilerDump("\pMacintosh HD:spacebar_ppc.prof");
#else
	::ProfilerDump("\pMacintosh HD:spacebar_68k.prof");
#endif
	::ProfilerTerm();
#endif
#endif

	return (m_errCode);
}


ERROR_CODE CBofApp::ShutDown() {
	//Assert(IsValidObject(this));

	return (m_errCode);
}


ERROR_CODE CBofApp::PreShutDown() {
#if BOF_MAC || BOF_WINMAC
	ShowMenuBar();
#endif
#if USESOUNDMUSICSYS
	MacQT::Finish();
#endif

#if BOF_MAC && USEDRAWSPROCKET
	DSpShutdown();
#endif

#if BOF_MAC || BOF_WINMAC
#define SCG_HACK 1
#if SCG_HACK
	::ExitToShell();
#endif
#endif

	return (m_errCode);
}


ERROR_CODE CBofApp::PostShutDown() {
	LogInfo(BuildString("Shutting down %s...", m_szAppName));

#if BOFDISP
	CBofDisplayObject::CleanUp();
#endif

	if (m_pWindow != nullptr) {
		delete m_pWindow;
		m_pWindow = nullptr;
	}

	// No more palettes
	//
	m_pPalette = nullptr;
	if (m_pDefPalette != nullptr) {
		delete m_pDefPalette;
		m_pDefPalette = nullptr;
	}

	LogInfo(BuildString("CBofApp::PostShutDown - Available Physical Memory: %ld", GetFreePhysMem()));

	return (m_errCode);
}

VOID CBofApp::SetPalette(CBofPalette *pPalette) {
	m_pPalette = pPalette;

	if (pPalette != nullptr) {

#if BOF_WINDOWS
		HDC hDC;

		if ((hDC = ::GetDC(nullptr)) != nullptr) {

			::SelectPalette(hDC, (HPALETTE)pPalette->GetPalette(), FALSE);

			::RealizePalette(hDC);

			::ReleaseDC(nullptr, hDC);
		}
#endif

#if BOF_MAC
		// jwl 09.24.96 make this our current palette...
		Assert(pPalette != nullptr);
		PaletteHandle thePH = pPalette->GetPalette();

		// jwl 08.08.96 have seen newPH nullptr, if it is, the default 256 colors
		// of the game are used (see 'pltt' resource)
		if (thePH != nullptr) {
			Assert((*thePH)->pmEntries == 256);
			Assert(GetHandleSize((Handle) thePH) != 0);

			// jwl 09.24.96 palette shift fix... take all the calls that cause palette shifts
			// and move them as close to the onscreen rendering code as possible, this will
			// minimize the shift... but not eliminate it.

			CBofWindow *pWnd = CBofApp::GetApp()->GetMainWindow();

			if (pWnd != nullptr) {
#if PALETTESHIFTFIX
				PaletteShiftItem        psi;

				psi.m_nItemOfInterest = (LONG) thePH;

				psi.m_nAssociatedItem = (LONG) pWnd->GetMacWindow();
				psi.m_eItemID = SETPALETTE;

				if (CBofWindow::m_pPaletteShiftList == nullptr) {
					CBofWindow::m_pPaletteShiftList = new CBofList<PaletteShiftItem>;
				}

				CBofWindow::m_pPaletteShiftList->AddToTail(psi);
#else
				::SetPalette(pWnd->GetMacWindow(), thePH, false);
#endif
			}
		}
#endif

	} else {
		// Use default palette
		m_pPalette = m_pDefPalette;
	}
}


#if !BOF_WIN16
VOID CBofApp::AddCursor(CBofCursor &cCursor) {
	//Assert(IsValidObject(this));

	m_cCursorList.AddToTail(cCursor);
}


VOID CBofApp::DelCursor(INT nIndex) {
	//Assert(IsValidObject(this));

	m_cCursorList.Remove(nIndex);
}
#endif


#if BOF_MAC

VOID CBofApp::InitMacToolBox() {
	THz             az;
	unsigned long   limit;
	short           omm;

	//Assert(IsValidObject(this));

	InitGraf(&qd.thePort);
	InitFonts();
	FlushEvents(everyEvent, 0);
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(0l);
	InitCursor();

	//  jwl 07.03.96 get more master pointers

	//  scg 01.13.97 This is the 68K way of doing things.  For PPC, the
	//  stack size is set in the development environment
#if 0
	limit = (unsigned long)LMGetCurStackBase() - 64 * 1024;
	if ((unsigned long)GetApplLimit() > limit)
		SetApplLimit((Ptr)limit);
#endif

	MaxApplZone();

	//  scg 01.13.97 Rather than interacting with the zone itself, I
	//  suggest we do this the way it is most often done.
#if 1
	MoreMasters();
	MoreMasters();
	MoreMasters();
#else
	az = ApplicZone();
	omm = az->moreMast;
	az->moreMast = omm * 3;
	MoreMasters();
	az->moreMast = omm;
#endif
}
#endif

#if BOF_WINDOWS
#if BOF_WINMAC
extern "C" INT WINAPI
#else
extern "C" INT PASCAL
#endif
WinMain(HINSTANCE hInst, HINSTANCE /*hPrevInst*/, LPSTR /*lpCmdLine*/, INT /*nCmdShow*/)
#else
INT main(INT argc, CHAR *argv[])
#endif // !BOF_WINDOWS
{
	CBofApp *pApp;

	pApp = CBofApp::GetApp();

	// The game-object must be instantiated by now
	Assert(pApp != nullptr);

	// Don't allow the game to be in memory more than once (BCW 10/08/96 03:03 pm)
	//
#if BOF_WINDOWS
	HANDLE hMutex;

	hMutex = ::CreateMutex(nullptr, TRUE, "BoffoMutex");

	if ((GetLastError() != ERROR_ALREADY_EXISTS) && FirstInstance()) {
#endif

#if BOF_WINDOWS

		CBofApp::SetInstanceHandle(hInst);
		g_hInst = hInst;

#endif
		pApp->PreInit();

		pApp->Initialize();

		if (!pApp->ErrorOccurred())
			pApp->RunApp();

		pApp->PreShutDown(); // scg 01.13.97

		pApp->ShutDown();

		pApp->PostShutDown();

#if BOF_WINDOWS
	}

	// Delete the Mutex we created (to stop multiple instances)
	//
	if (hMutex != nullptr) {
		::ReleaseMutex(hMutex);
	}

#endif

#if BOF_MAC
	::ExitToShell();
#endif
	return ((INT)pApp->GetErrorCode());
}


BOOL FirstInstance() {
	BOOL bFirstTime;

	// assume this is the first instance
	bFirstTime = TRUE;

#if BOF_WINDOWS
	HWND hWnd;

	if ((hWnd = ::FindWindow("BofWindowParent", nullptr)) != nullptr) {
		bFirstTime = FALSE;
	}
#endif

	return (bFirstTime);
}


#if BOF_MAC && USEDRAWSPROCKET
VOID CBofApp::InitDrawSprocket() {
	DSpStartup();

	DSpContextAttributes theDesiredAttributes;

	InitDSpContextAttributes(&theDesiredAttributes);

	theDesiredAttributes.displayWidth           = 640;
	theDesiredAttributes.displayHeight          = 480;
	theDesiredAttributes.colorNeeds             = kDSpColorNeeds_Require;
	theDesiredAttributes.backBufferDepthMask    = kDSpDepthMask_8;
	theDesiredAttributes.displayDepthMask       = kDSpDepthMask_8;
	theDesiredAttributes.backBufferBestDepth    = 8;
	theDesiredAttributes.displayBestDepth       = 8;
	theDesiredAttributes.pageCount              = 1;

	DSpContextReference theContext;

	if (DebuggerIsRunning() == FALSE) {
		OSStatus theError = DSpFindBestContext(&theDesiredAttributes, &theContext);

		theError = DSpContext_Reserve(theContext, &theDesiredAttributes);
		Assert(theError == noErr);
		theError = DSpContext_FadeGammaOut(nullptr, nullptr);
		Assert(theError == noErr);
		theError = DSpContext_SetState(theContext, kDSpContextState_Active);
		Assert(theError == noErr);
		//  <call DSpContext_SetCLUTEntries as needed>
		theError = DSpContext_FadeGammaIn(nullptr, nullptr);
		Assert(theError == noErr);
		theError = DSpContext_GetBackBuffer(theContext, kDSpBufferKind_Normal, &gBackBuffer);
		Assert(theError == noErr);
	}
}


void InitDSpContextAttributes(
    DSpContextAttributes *inAttributes      /* attr structure to init */
) {
	Assert(inAttributes != nullptr);

	inAttributes->frequency                 = 0;
	inAttributes->displayWidth              = 0;
	inAttributes->displayHeight             = 0;
	inAttributes->reserved1                 = 0;
	inAttributes->reserved2                 = 0;
	inAttributes->colorNeeds                = 0;
	inAttributes->colorTable                = nullptr;
	inAttributes->contextOptions            = 0;
	inAttributes->backBufferDepthMask       = 0;
	inAttributes->displayDepthMask          = 0;
	inAttributes->backBufferBestDepth       = 0;
	inAttributes->displayBestDepth          = 0;
	inAttributes->pageCount                 = 0;
	inAttributes->gameMustConfirmSwitch     = false;
	inAttributes->reserved3[0]              = 0;
	inAttributes->reserved3[1]              = 0;
	inAttributes->reserved3[2]              = 0;
	inAttributes->reserved3[3]              = 0;
}
#endif


VOID BofPostMessage(CBofWindow *pWindow, ULONG lMessage, ULONG lParam1, ULONG lParam2) {
#if BOF_WINDOWS
	HWND hWnd;

	hWnd = nullptr;
	if (pWindow != nullptr)
		hWnd = pWindow->GetHandle();

	::PostMessage(hWnd, lMessage, (WPARAM)lParam1, (LPARAM)lParam2);

#elif BOF_MAC
	CBofMessage *pMessage;

	// Create a user defined message.
	// NOTE: This message will be deleted by HandleMacEvent()
	//
	if ((pMessage = new CBofMessage) != nullptr) {

		pMessage->m_pWindow = pWindow;
		pMessage->m_nMessage = lMessage;
		pMessage->m_lParam1 = lParam1;
		pMessage->m_lParam2 = lParam2;

		PostEvent(app3Evt, (LONG)pMessage);
	}
#endif
}


///////////////////////////////////////////////////////////////////////////
// Global routines
///////////////////////////////////////////////////////////////////////////

VOID SetMousePos(CBofPoint &cPoint) {
	g_system->warpMouse(cPoint.x, cPoint.y);
}


CBofPoint GetMousePos() {
	CBofPoint cPoint;

#if BOF_WINDOWS
	POINT stPoint;

	GetCursorPos(&stPoint);
	cPoint.x = stPoint.x;
	cPoint.y = stPoint.y;

#elif BOF_MAC
	Point stPoint;

	::GetMouse(&stPoint);
	::LocalToGlobal(&stPoint);

	cPoint.x = stPoint.h;
	cPoint.y = stPoint.v;
#endif
	return (cPoint);
}


#if BOFDISP
VOID CBofAppWindow::OnPaint(CBofRect *pRect) {
	Assert(IsValidObject(this));
	Assert(pRect != nullptr);

	CBofBitmap cBmp(pRect->Width(), pRect->Height(), nullptr);

	cBmp.FillRect(nullptr, COLOR_BLACK);

	cBmp.Paint(this, pRect);

	// Must now repaint all display objects that have been invalidated
	//
	CBofDisplayObject::AddToDirtyRect(pRect);
	CBofDisplayObject::UpdateDirtyRect();
}


VOID CBofAppWindow::OnMouseMove(UINT nFlags, CBofPoint *pPoint, void *) {
	Assert(IsValidObject(this));
	Assert(pPoint != nullptr);

	CBofDisplayWindow::HandleMouseMove(nFlags, *pPoint);
}


VOID CBofAppWindow::OnLButtonDown(UINT nFlags, CBofPoint *pPoint, void *) {
	Assert(IsValidObject(this));
	Assert(pPoint != nullptr);

	CBofDisplayWindow::HandleLButtonDown(nFlags, *pPoint);
}

VOID CBofAppWindow::OnLButtonUp(UINT nFlags, CBofPoint *pPoint, void *) {
	Assert(IsValidObject(this));
	Assert(pPoint != nullptr);

	CBofDisplayWindow::HandleLButtonUp(nFlags, *pPoint);
}

VOID CBofAppWindow::OnLButtonDblClk(UINT nFlags, CBofPoint *pPoint) {
	Assert(IsValidObject(this));
	Assert(pPoint != nullptr);

	CBofDisplayWindow::HandleLButtonDblClk(nFlags, *pPoint);
}

VOID CBofAppWindow::OnRButtonDown(UINT nFlags, CBofPoint *pPoint) {
	Assert(IsValidObject(this));
	Assert(pPoint != nullptr);

	CBofDisplayWindow::HandleRButtonDown(nFlags, *pPoint);
}

VOID CBofAppWindow::OnRButtonUp(UINT nFlags, CBofPoint *pPoint) {
	Assert(IsValidObject(this));
	Assert(pPoint != nullptr);

	CBofDisplayWindow::HandleRButtonUp(nFlags, *pPoint);
}

VOID CBofAppWindow::OnRButtonDblClk(UINT nFlags, CBofPoint *pPoint) {
	Assert(IsValidObject(this));
	Assert(pPoint != nullptr);

	CBofDisplayWindow::HandleRButtonDblClk(nFlags, *pPoint);
}


VOID CBofAppWindow::OnKeyHit(ULONG lKey, ULONG lRepCount) {
	Assert(IsValidObject(this));

	CBofDisplayWindow::HandleKeyHit(lKey, lRepCount);

#if !BOF_MAC
#if _DEBUG
	switch (lKey) {

	case BKEY_ALT_F4:
	case BKEY_ALT_Q:
	case BKEY_ALT_q:
		Quit();
		break;

	default:
		break;
	}
#endif
#endif
}

#endif // DISPOBJ

VOID BofMessageBox(const CHAR *pszTitle, const CHAR *pszMessage) {
	CBofCursor::Show();

#if BOF_WINDOWS
	CBofApp *pApp;
	HWND hWnd;

	hWnd = nullptr;
	if ((pApp = CBofApp::GetApp()) != nullptr) {
		CBofWindow *pWnd;
		if ((pWnd = pApp->GetMainWindow()) != nullptr) {
			hWnd = pWnd->GetHandle();
		}
	}

	::MessageBox(hWnd, pszTitle, pszMessage, MB_OK);

#elif BOF_MAC
	MacMessageBox(pszTitle, pszMessage);
#else
#endif

	CBofCursor::Hide();
}

} // namespace Bagel
