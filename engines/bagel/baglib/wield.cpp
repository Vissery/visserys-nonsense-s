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

#include "bagel/baglib/wield.h"
#include "bagel/baglib/menu_dlg.h"
#include "bagel/baglib/master_win.h"
#include "bagel/baglib/sprite_object.h"
#include "bagel/baglib/bagel.h"

namespace Bagel {

int CBagWield::m_nWieldCursor = -1;

CBagWield::CBagWield(CBofWindow *pParent, const CBofRect &xRect) :
	CBagStorageDevBmp(pParent, xRect) {
	_xSDevType = SDEV_WIELD;
	m_nObjects = 0;         // This should be changed on the attach
	m_pCurrObj = nullptr;
}


CBagWield::~CBagWield() {
}

ErrorCode CBagWield::loadFile(const CBofString &sFile) {
	ErrorCode error = CBagStorageDev::loadFile(sFile);

	return error;
}

ErrorCode CBagWield::attach() {
	CBagStorageDevBmp::attach();

	// Take care of objects being held
	int nObjects = 0;
	m_nObjects = 0;

	for (int i = 0; i < GetObjectCount(); ++i) {
		CBagObject *pObj = GetObjectByPos(i);

		if (pObj->IsActive() && (pObj->GetType() == BOFSPRITEOBJ || pObj->GetType() == BOFBMPOBJ)) {
			if (nObjects == 0) {
				nObjects++;
				activateLocalObject(pObj);

			} else {
				pObj->detach();
			}
		}
	}

	return _errCode;
}

ErrorCode CBagWield::detach() {
	CBagStorageDev::detach();
	return CBagBmpObject::detach();
}

bool CBagWield::onObjInteraction(CBagObject *pObj, CBagStorageDev *pSDev) {
	CBofString sObjName = pObj->GetRefName();

	if (sObjName.isEmpty())
		return false;

	// Find the storage device
	if (CBagStorageDev::activateLocalObject(sObjName) != ERR_NONE)
		return false;

	if (pSDev->deactivateLocalObject(sObjName) != ERR_NONE) {
		CBagStorageDev::deactivateLocalObject(sObjName);
		return false;
	}

	return true;
}

ErrorCode CBagWield::activateLocalObject(CBagObject *pObj) {
	ErrorCode errCode = ERR_UNKNOWN;

	if (pObj != nullptr) {
		// Since we can only hold one object at a time, put the object that
		// the user is currently holding into their stash (Inventory), and then
		// put the new object that they are trying to pick up into their wield.
		if (m_nObjects == 1) {
			CBagObject *pPrevObj = getCurrObj();

			if (pPrevObj != nullptr) {
				// Move current object to stash
				SDEV_MANAGER->MoveObject("INV_WLD", GetName(), pPrevObj->GetRefName());

			} else {
				reportError(ERR_UNKNOWN, "Wielded Object has been lost");
			}

			// There are no objects in wield
			m_nObjects = 0;
		}

		// Add to wield
		if (pObj->getMenuPtr() != nullptr) {
			CBagMenu::SetUniversalObjectList(pObj->getMenuPtr()->GetObjectList());
		}

		if (pObj->GetType() == SPRITEOBJ) {
			setCurrObj(pObj);
			m_nObjects++;
			CBagSpriteObject *pSpObj = (CBagSpriteObject *)pObj;
			SetWieldCursor(pSpObj->GetWieldCursor());
		}

		// Fix for sometimes getting the wield menu instead of thud's order
		// menu.  Do this by causing the wielded item to not have a location
		// on the You icon. (It's rectangle will appear empty)
		CBofPoint cPos(2056, 2056);
		pObj->setPosition(cPos);
		pObj->SetFloating(false);

		errCode = CBagStorageDev::activateLocalObject(pObj);
	}

	return errCode;
}

ErrorCode CBagWield::deactivateLocalObject(CBagObject *pObj) {
	if (m_nObjects == 1) {

		if (pObj->GetType() == SPRITEOBJ) {
			SetWieldCursor(-1);
			m_nObjects--;
		}

		CBagMenu::SetUniversalObjectList(nullptr);
		CBagStorageDev::deactivateLocalObject(pObj);
		setCurrObj(nullptr);
	}

	return _errCode;
}


ErrorCode CBagWield::update(CBofBitmap *pBmp, CBofPoint /*cPoint*/, CBofRect * /*pSrcRect*/, int /*nMaskColor*/) {
	assert(pBmp != nullptr);

	// If this object is visible
	if (isVisible() && isAttached()) {

		CBofBitmap *pYouIcon = getBackground();
		if (pYouIcon != nullptr) {
			pYouIcon->paint(pBmp, getPosition().x, getPosition().y, nullptr, DEFAULT_CHROMA_COLOR);
			SetDirty(false);
		}
	}

	// Set the firstpaint flag and attach objects to allow for immediate run objects to run
	if (_bFirstPaint) {
		_bFirstPaint = false;
		AttachActiveObjects();
	}

	return ERR_NONE;
}

} // namespace Bagel
