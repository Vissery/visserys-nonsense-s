
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

#ifndef BAGEL_BAGLIB_SOUND_OBJECT_H
#define BAGEL_BAGLIB_SOUND_OBJECT_H

#include "bagel/baglib/object.h"
#include "bagel/baglib/bagel.h"
#include "bagel/boflib/sound.h"

namespace Bagel {

/**
 * CBagSoundObject is an object that can be place within the slide window.
 */
class CBagSoundObject : public CBagObject {
private:
	static CBofSound *m_pMidiSound; // There is only one allowed at a time
	CBofSound *m_pSound;

	uint16 m_wFlags;
	int m_nLoops;
	byte m_nVol;

protected:
	byte m_bWait;

public:
	CBagSoundObject();
	virtual ~CBagSoundObject();
	static void initialize();

	void KillSound();
	ErrorCode NewSound(CBofWindow *pWin);

	// Return true if the Object had members that are properly initialized/de-initialized
	ErrorCode attach() {
		return attach((CBofWindow *)CBagel::getBagApp()->getMasterWnd());
	}
	ErrorCode attach(CBofWindow *pWnd);
	bool isAttached() {
		return m_pSound != nullptr;
	}
	ErrorCode detach();

	CBofSound *GetLastMidi() {
		return m_pMidiSound;
	}

	CBofSound *GetSound() {
		return m_pSound;
	}

	void SetWave() {
		m_wFlags = SOUND_WAVE;
	}
	void SetMidi() {
		m_wFlags = (SOUND_MIDI | SOUND_LOOP);
	}

	// Gives ability to sound over certain sounds
	void SetSoundOver() {
		m_wFlags |= SOUND_OVEROK;
	}

	void SetSync(bool b = true);
	void SetASync(bool b = true) {
		SetSync(!b);
	}
	bool IsSync() {
		return m_wFlags & SOUND_ASYNCH;
	}

	void SetMix() {
		m_wFlags = SOUND_MIX;
	}
	void SetQueue(bool b = true);

	virtual bool runObject();

	void SetVolume(int nVol);
	int GetVolume();

	void SetNumOfLoops(int n);

	int getProperty(const CBofString &sProp);
	void setProperty(const CBofString &sProp, int nVal);

	/**
	 * Takes in info and then removes the relative information and returns
	 * the info without the relevant info.
	 */
	PARSE_CODES setInfo(bof_ifstream &istr);

	//  Added properties to sound object
	bool IsPlaying();
	bool IsQueued();

	void SetPlaying(bool bVal = true);
};

} // namespace Bagel

#endif
