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

#include "common/file.h"
#include "bagel/baglib/sound_object.h"
#include "bagel/baglib/storage_dev_win.h"

namespace Bagel {

// There is only one allowed at a time
CBofSound *CBagSoundObject::m_pMidiSound;

void CBagSoundObject::initStatics() {
	m_pMidiSound = nullptr;
}

CBagSoundObject::CBagSoundObject() {
	m_xObjType = SOUNDOBJ;
	m_pSound = nullptr;

	// Assume MIX if not specified
	m_wFlags = SOUND_MIX; //(SOUND_WAVE | SOUND_ASYNCH);

	m_nVol = VOLUME_INDEX_DEFAULT;
	SetState(0);
	m_bWait = FALSE;

	m_nLoops = 1;

	SetVisible(FALSE);
	SetOverCursor(3);
}

CBagSoundObject::~CBagSoundObject() {
	Detach();
}

ERROR_CODE CBagSoundObject::Attach(CBofWindow *pWnd) {
	NewSound(pWnd);

	return CBagObject::Attach();
}

ERROR_CODE CBagSoundObject::NewSound(CBofWindow *pWin) {
	ERROR_CODE errCode;

	// assume no error
	errCode = ERR_NONE;

	KillSound();

	if ((m_pSound = new CBofSound(pWin, GetFileName(), m_wFlags, m_nLoops)) != nullptr) {
		m_pSound->SetVolume(m_nVol);
		m_pSound->SetQSlot(GetState());

	} else {
		errCode = ERR_MEMORY;
	}

	return errCode;
}

VOID CBagSoundObject::KillSound(VOID) {
	if (m_pSound != nullptr) {
		delete m_pSound;
		m_pSound = nullptr;
	}
}

ERROR_CODE CBagSoundObject::Detach() {
	KillSound();
	return CBagObject::Detach();
}

BOOL CBagSoundObject::RunObject() {
	if (((m_wFlags & SOUND_MIDI) && CBagMasterWin::GetMidi()) || (((m_wFlags & SOUND_WAVE) || (m_wFlags & SOUND_MIX)) && CBagMasterWin::GetDigitalAudio())) {

		if (m_pSound && m_pMidiSound != m_pSound) {

			m_pSound->SetQSlot(GetState());
			m_pSound->Play();

			// If waiting until this sound finishes
			//
			if (m_bWait) {

				// Show busy cursor
				CBagMasterWin::SetActiveCursor(6);

				// Reset check for escape
				IsKeyDown(BKEY_ESC);

				while (m_pSound->IsPlaying()) {

					CBofSound::AudioTask();

					// Update the screen
					CBagMasterWin::ForcePaintScreen(TRUE);

					// Let user escape out of synch sounds
					//
					if (IsKeyDown(BKEY_ESC)) {
						m_pSound->Stop();
						break;
					}

#ifdef _DEBUG
					// Prevent infinite loop when DebugAudio is 0
					//
					BOOL bDebugAudio;
					CBagel *pApp;

					if ((pApp = CBagel::GetBagApp()) != nullptr) {
						pApp->GetOption("UserOptions", "DebugAudio", &bDebugAudio, TRUE);
						if (!bDebugAudio) {
							break;
						}
					}
#endif
				}
			}

			if (m_wFlags & SOUND_MIDI)
				m_pMidiSound = m_pSound;
		}
#ifndef BOF_FINAL_RELEASE
		//
		// This would be much cooler if it were a cast to another object type and
		// then a run.  But this is a quicker fix.
		//
		else { /* if no sound */

			if (!(m_wFlags & SOUND_MIDI)) {

				int nExt = GetFileName().GetLength() - 4; // ".EXT"

				if (nExt <= 0) {
					LogError("Sound does not have a file name or proper extention.  Please write better scripts.");
					return FALSE;
				}

				CBofString sBaseStr = GetFileName().Left(nExt) + ".TXT";

				Common::File f;
				if (FileExists(sBaseStr) && f.open(sBaseStr.GetBuffer())) {
					Common::String line = f.readLine();

					BofMessageBox(line.c_str(), "Using .TXT for missing .WAV!");
					f.close();
					return TRUE;
				} else {
					LogError(BuildString("Sound TEXT file could not be read: %s.  Why? because we like you ...", GetFileName()));
					return FALSE;
				}
			}
		}
#endif
	}

	return CBagObject::RunObject();
}

PARSE_CODES CBagSoundObject::SetInfo(bof_ifstream &istr) {
	INT nChanged;
	BOOL nObjectUpdated = FALSE;
	CHAR ch;

	while (!istr.eof()) {
		nChanged = 0;
		istr.EatWhite(); // Eat any white space between script elements MDM 7/16
		switch (ch = (char)istr.peek()) {

		// VOLUME
		//
		case 'V': {
			CHAR szLocalStr[256];
			szLocalStr[0] = 0;
			CBofString sStr(szLocalStr, 256);
			INT n;

			GetAlphaNumFromStream(istr, sStr);

			if (!sStr.Find("VOLUME")) {
				istr.EatWhite();
				GetIntFromStream(istr, n);
				SetVolume(n);
				nObjectUpdated = TRUE;
				nChanged++;
			} else {
				PutbackStringOnStream(istr, sStr);
			}
			break;
		}

		//
		//  AS [WAVE|MIDI]  - how to run the link
		//
		case 'A': {
			CHAR szLocalStr[256];
			szLocalStr[0] = 0;
			CBofString sStr(szLocalStr, 256); // jwl 08.28.96 performance improvement
			GetAlphaNumFromStream(istr, sStr);

			if (!sStr.Find("AS")) {

				istr.EatWhite();
				GetAlphaNumFromStream(istr, sStr);

				if (!sStr.Find("WAVE")) {
					// m_xSndType  = WAVE;
					SetWave();
					nChanged++;
					nObjectUpdated = TRUE;

				} else if (!sStr.Find("MIDI")) {

					// m_xSndType  = MIDI;
					SetMidi();
					nChanged++;
					nObjectUpdated = TRUE;

				} else if (!sStr.Find("SYNC")) {
					SetSync();
					nChanged++;
					nObjectUpdated = TRUE;

				} else if (!sStr.Find("ASYNC")) {
					SetASync();
					nChanged++;
					nObjectUpdated = TRUE;

					// BCW - 11/27/96 06:39 pm
					// Added Mix and Wait
					//
				} else if (!sStr.Find("WAITMIX")) {

					SetMix();
					m_bWait = TRUE;
					nChanged++;
					nObjectUpdated = TRUE;

					// BCW - 11/27/96 06:39 pm
					// Added Queue and Wait
					//
				} else if (!sStr.Find("WAITQUEUE")) {

					SetQueue();
					m_bWait = TRUE;
					nChanged++;
					nObjectUpdated = TRUE;

				} else if (!sStr.Find("QUEUE")) {

					SetQueue();
					nChanged++;
					nObjectUpdated = TRUE;

					// added Wavemix sounds 8/4/96 barb
				} else if (!sStr.Find("MIX")) {
					SetMix();
					nChanged++;
					nObjectUpdated = TRUE;

				} else {
					PutbackStringOnStream(istr, sStr);
					PutbackStringOnStream(istr, "AS ");
				}

			} else {
				PutbackStringOnStream(istr, sStr);
			}
		}
		break;

		// MDM 7/17 - ADDED LOOP PROPERTY TO SOUNDS
		case 'L': {
			CHAR szLocalStr[256];
			szLocalStr[0] = 0;
			CBofString sStr(szLocalStr, 256); // jwl 08.28.96 performance improvement

			GetAlphaNumFromStream(istr, sStr);

			if (!sStr.Find("LOOP")) {
				istr.EatWhite();
				GetIntFromStream(istr, m_nLoops);
				nObjectUpdated = TRUE;
				nChanged++;
			} else {
				PutbackStringOnStream(istr, sStr);
			}
		}
		break;
		// jwl 12.02.96 added oversound attribute to sound object
		case 'S': {
			CHAR szLocalStr[256];
			szLocalStr[0] = 0;
			CBofString sStr(szLocalStr, 256); // jwl 08.28.96 performance improvement

			GetAlphaNumFromStream(istr, sStr);

			if (!sStr.Find("SOUNDOVEROK")) {
				SetSoundOver();
				nObjectUpdated = TRUE;
				nChanged++;
			} else {
				PutbackStringOnStream(istr, sStr);
			}
		}
		break;
		//
		//  no match return from funtion
		//
		default: {
			PARSE_CODES rc;
			if ((rc = CBagObject::SetInfo(istr)) == PARSING_DONE) {
				return PARSING_DONE;
			} else if (rc == UPDATED_OBJECT) {
				nObjectUpdated = TRUE;
			} else if (!nChanged) { // rc==UNKNOWN_TOKEN
				if (nObjectUpdated)
					return UPDATED_OBJECT;
				else
					return UNKNOWN_TOKEN;
			}
		}
		break;
		} // end switch

	} // end while

	return PARSING_DONE;
}

VOID CBagSoundObject::SetQueue(BOOL b) {
	if (b) {
		m_wFlags = SOUND_MIX | SOUND_QUEUE;

	} else {
		m_wFlags &= ~SOUND_QUEUE;
	}
}

INT CBagSoundObject::GetVolume() {
	return m_nVol;
}

VOID CBagSoundObject::SetVolume(INT nVol) {
	m_nVol = (UBYTE)nVol;
	if (IsAttached()) {

		if (m_pSound != nullptr) {
			m_pSound->SetVolume(m_nVol);
		}
	}
}

BOOL CBagSoundObject::IsPlaying() {
	BOOL bPlaying;

	bPlaying = FALSE;
	if (m_pSound != nullptr) {
		bPlaying = m_pSound->Playing();
	}

	return bPlaying;
}

BOOL CBagSoundObject::IsQueued() {
	BOOL bQueued;

	bQueued = FALSE;
	if (m_pSound != nullptr) {
		bQueued = m_pSound->IsQueued();
	}

	return bQueued;
}

VOID CBagSoundObject::SetPlaying(BOOL bVal) {
	if (((m_wFlags & SOUND_MIDI) && CBagMasterWin::GetMidi()) || (((m_wFlags & SOUND_WAVE) || (m_wFlags & SOUND_MIX)) && CBagMasterWin::GetDigitalAudio())) {

		if (bVal) {

			if (m_pSound && m_pMidiSound != m_pSound) {

				m_pSound->SetQSlot(GetState());
				m_pSound->Play();

				// If we are supposed to wait until this audio finishes
				//
				if (m_bWait) {

					// Show busy cursor
					CBagMasterWin::SetActiveCursor(6);

					// Reset check for escape
					IsKeyDown(BKEY_ESC);

					while (m_pSound->IsPlaying()) {

						CBofSound::AudioTask();

						// Update the screen
						CBagMasterWin::ForcePaintScreen(TRUE);

						// Let user escape out of synch sounds
						//
						if (IsKeyDown(BKEY_ESC)) {
							m_pSound->Stop();
							break;
						}

#ifdef _DEBUG
						// Prevent infinite loop when DebugAudio is 0
						//
						BOOL bDebugAudio;
						CBagel *pApp;

						if ((pApp = CBagel::GetBagApp()) != nullptr) {
							pApp->GetOption("UserOptions", "DebugAudio", &bDebugAudio, TRUE);
							if (!bDebugAudio) {
								break;
							}
						}
#endif
					}
				}
				if (m_wFlags & SOUND_MIDI)
					m_pMidiSound = m_pSound;
			}
		} else {

			if (m_pSound) {
				m_pSound->Stop();
				if (m_wFlags & SOUND_MIDI)
					m_pMidiSound = nullptr;
			}
		}
	}
}

VOID CBagSoundObject::SetNumOfLoops(INT n) {
	m_nLoops = n; // Only have ability to set at creation of BofSound
}

INT CBagSoundObject::GetProperty(const CBofString &sProp) {
	if (!sProp.Find("VOLUME")) {
		return GetVolume();

	} else if (!sProp.Find("QUEUED")) {
		return IsQueued();

	} else if (!sProp.Find("PLAYING")) {
		return IsPlaying();

	} else if (!sProp.Find("LOOP")) {
		return m_nLoops;
	} else {
		return CBagObject::GetProperty(sProp);
	}
}

VOID CBagSoundObject::SetProperty(const CBofString &sProp, int nVal) {
	if (!sProp.Find("VOLUME")) {
		SetVolume(nVal);

	} else if (!sProp.Find("PLAYING")) {

		if (nVal == 1)
			SetPlaying();
		else
			SetPlaying(FALSE);

	} else if (!sProp.Find("LOOP")) {
		SetNumOfLoops(nVal);
	} else {
		CBagObject::SetProperty(sProp, nVal);
	}
}

VOID CBagSoundObject::SetSync(BOOL b) {
	m_wFlags = SOUND_WAVE;
	if (!b)
		m_wFlags |= SOUND_ASYNCH;
}

} // namespace Bagel
