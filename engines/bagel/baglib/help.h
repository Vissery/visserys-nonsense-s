
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

#ifndef BAGEL_BAGLIB_HELP_H
#define BAGEL_BAGLIB_HELP_H

#include "bagel/baglib/storage_dev_win.h"
#include "bagel/boflib/gui/text_box.h"
#include "bagel/boflib/gui/button.h"
#include "bagel/boflib/string.h"

namespace Bagel {

class CBagHelp : public CBofDialog {
public:
	CBagHelp();

#if BOF_DEBUG
	virtual ~CBagHelp();
#endif
	virtual ERROR_CODE Attach();
	virtual ERROR_CODE Detach();

#if 1
	virtual void OnInitDialog();
#endif

	ERROR_CODE SetHelpFile(const char *pszTextFile);

protected:
	virtual void OnPaint(CBofRect *pRect);
	virtual void OnBofButton(CBofObject *pObject, INT nState);
	virtual void OnKeyHit(uint32 lKey, uint32 lRepCount);

	CBofString m_cTextFile;

	CBofTextBox *m_pTextBox;
	CBofBmpButton *m_pOKButton;
	CBofBmpButton *m_pPageUp;
	CBofBmpButton *m_pPageDown;
	CBofPalette *m_pSavePalette;
};

} // namespace Bagel

#endif
