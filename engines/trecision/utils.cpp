/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "3d.h"
#include "common/system.h"

#include "trecision/scheduler.h"
#include "trecision/text.h"
#include "trecision/actor.h"
#include "trecision/defines.h"
#include "trecision/graphics.h"
#include "trecision/logic.h"
#include "trecision/trecision.h"
#include "trecision/nl/struct.h"

namespace Trecision {

char *TrecisionEngine::getNextSentence() {
	while (*_textPtr) {
		*_textPtr = ~(*_textPtr);
		_textPtr++;
	}

	_textPtr++;
	return _textPtr;
}

void TrecisionEngine::setRoom(uint16 r, bool b) {
	_logicMgr->setupAltRoom(r, b);
}

/*-------------------------------------------------
 * Compute string length from character 0 to num
 *-------------------------------------------------*/
uint16 TrecisionEngine::textLength(Common::String text, uint16 num) {
	if (text.empty())
		return 0;

	uint16 len = (num == 0) ? text.size() : num;

	uint16 retVal = 0;
	for (uint16 c = 0; c < len; c++)
		retVal += _font[(uint8)text[c] * 3 + 2];

	return retVal;
}

char TrecisionEngine::getKey() {
	Common::KeyCode key = _curKey;
	uint16 ascii = _curAscii;
	_curKey = Common::KEYCODE_INVALID;
	_curAscii = 0;

	switch (key) {
	case Common::KEYCODE_SPACE:
	case Common::KEYCODE_ESCAPE:
	case Common::KEYCODE_RETURN:
	case Common::KEYCODE_CLEAR:
	case Common::KEYCODE_BACKSPACE:
		return key;
	case Common::KEYCODE_F1:
	case Common::KEYCODE_F2:
	case Common::KEYCODE_F3:
	case Common::KEYCODE_F4:
	case Common::KEYCODE_F5:
	case Common::KEYCODE_F6:
		return 0x3B + key - Common::KEYCODE_F1;
	default:
		if (ascii) {
			return ascii;
		}

		return 0;
	}
}

char TrecisionEngine::waitKey() {
	_graphicsMgr->hideCursor();
	while (_curKey == Common::KEYCODE_INVALID)
		checkSystem();
	_graphicsMgr->showCursor();

	Common::KeyCode t = _curKey;
	_curKey = Common::KEYCODE_INVALID;

	return t;
}

void TrecisionEngine::waitDelay(uint32 val) {
	uint32 sv = readTime();

	while (sv + val > readTime())
		checkSystem();
}

void TrecisionEngine::freeKey() {
	_curKey = Common::KEYCODE_INVALID;
}

uint32 TrecisionEngine::readTime() {
	return (g_system->getMillis() * 3) / 50;
}

bool TrecisionEngine::checkMask(Common::Point pos) {
	for (int8 a = MAXOBJINROOM - 1; a >= 0; a--) {
		uint16 checkedObj = _room[_curRoom]._object[a];
		Common::Rect lim = _obj[checkedObj]._lim;
		lim.translate(0, TOP);
		// Trecision includes the bottom and right coordinates
		lim.right++;
		lim.bottom++;

		if (checkedObj && isObjectVisible(checkedObj)) {
			if (lim.contains(pos)) {

				if ((_obj[checkedObj]._mode & OBJMODE_FULL) || (_obj[checkedObj]._mode & OBJMODE_LIM)) {
					_curObj = checkedObj;
					return true;
				}

				if (_obj[checkedObj]._mode & OBJMODE_MASK) {
					uint8 *mask = _maskPointers[a];
					int16 d = _obj[checkedObj]._rect.left;
					uint16 max = _obj[checkedObj]._rect.bottom;

					for (uint16 b = _obj[checkedObj]._rect.top; b < max; b++) {
						bool insideObj = false;
						int16 e = 0;
						while (e < _obj[checkedObj]._rect.width()) {
							if (!insideObj) { // not inside an object
								if (b + TOP == pos.y) {
									if ((pos.x >= d + e) && (pos.x < d + e + *mask)) {
										_curObj = 0;
									}
								}

								e += *mask;
								mask++;
								insideObj = true;
							} else { // inside an object
								if (b + TOP == pos.y) {
									if ((pos.x >= d + e) && (pos.x < d + e + *mask)) {
										_curObj = checkedObj;
										return true;
									}
								}

								e += *mask;
								mask++;
								insideObj = false;
							}
						}
					}
				}
			}
		}
	}
	_curObj = 0;
	return false;
}

Graphics::Surface *TrecisionEngine::convertScummVMThumbnail(Graphics::Surface *thumbnail) {
	Graphics::Surface *thumbnailConverted = thumbnail->convertTo(g_system->getScreenFormat());
	Graphics::Surface *result = thumbnailConverted->scale(ICONDX, ICONDY);

	thumbnailConverted->free();
	delete thumbnailConverted;
	thumbnail->free();
	delete thumbnail;

	return result;
}

void TrecisionEngine::openSys() {
	// head
	int32 idx = 0;
	FTexture[idx]._dx = 300 / 2;
	FTexture[idx]._dy = 208 / 2;
	FTexture[idx]._angle = 0;
	FTexture[idx]._texture = _textureArea;
	FTexture[idx]._flag = TEXTUREACTIVE + TEXTURECYLIND;

	// body
	idx = 1;
	FTexture[idx]._dx = 300;
	FTexture[idx]._dy = 300;
	FTexture[idx]._angle = 0;
	FTexture[idx]._texture = FTexture[0]._texture + (300 * 208) / 4;
	FTexture[idx]._flag = TEXTUREACTIVE + TEXTURECYLIND;

	// arms
	idx = 2;
	FTexture[idx]._dx = 300;
	FTexture[idx]._dy = 150;
	FTexture[idx]._angle = 0;
	FTexture[idx]._texture = FTexture[1]._texture + 300 * 300;
	FTexture[idx]._flag = TEXTUREACTIVE + TEXTURECYLIND;

	delete _actor;
	_actor = new Actor(this);
	_actor->readModel("jm.om");

	_actor->_light = (SLight *)&VLight;
	_actor->_camera = (SCamera *)&FCamera;
	_actor->_texture = (STexture *)&FTexture[0];

	TextArea = new char[MAXTEXTAREA];

	_graphicsMgr->clearScreen();

	_graphicsMgr->hideCursor();
}

float TrecisionEngine::sinCosAngle(float sinus, float cosinus) {
	if (sinus == 0 && cosinus == 0)
		return 0;

	float t = (float)sqrt((double)(sinus * sinus) + (double)(cosinus * cosinus));
	cosinus /= t;
	sinus /= t;

	// 1e3 & 2e4 quad
	if (sinus >= 0)
		// 1 & 2 quad
		return (float)acos(cosinus);

	// 3 quad
	return PI2 - (float)acos(cosinus);
}

void TrecisionEngine::ProcessTime() {
	static uint8 OldRegInvSI = 0xFF;
	static uint8 OldRegInvSL = 0xFF;
	static uint8 OldLightIcon = 0xFF;

	_curTime = readTime();
	_animMgr->refreshAllAnimations();

	if (_curTime >= _nextRefresh) {
		_textMgr->drawTexts();

		if (_inventoryStatus == INV_PAINT || _inventoryStatus == INV_DEPAINT)
			rollInventory(_inventoryStatus);

		if (_inventoryStatus != INV_OFF && (OldRegInvSI != _inventoryRefreshStartIcon || OldRegInvSL != _inventoryRefreshStartLine || OldLightIcon != _lightIcon)) {
			refreshInventory(_inventoryRefreshStartIcon, _inventoryRefreshStartLine);
			OldRegInvSI = _inventoryRefreshStartIcon;
			OldRegInvSL = _inventoryRefreshStartLine;
			OldLightIcon = _lightIcon;
		}

		_renderer->paintScreen(false);
		_textMgr->clearTextStack();

		uint32 paintTime = readTime();
		if (paintTime - _curTime >= 5)
			_nextRefresh = paintTime + 1;
		else
			_nextRefresh = _curTime + 5;
	}
}

void TrecisionEngine::ProcessMouse() {
	static bool MaskMouse;
	static Common::Point oldMousePos;
	static bool lastMouseOn = true;
	int16 mx = _mousePos.x;
	int16 my = _mousePos.y;

	if (lastMouseOn && !_graphicsMgr->isCursorVisible()) {
		oldMousePos = Common::Point(0, 0); // Switch off
	} else if (!lastMouseOn && _graphicsMgr->isCursorVisible()) {
		oldMousePos = Common::Point(0, 0); // Switch on
	}

	lastMouseOn = _graphicsMgr->isCursorVisible();
	checkSystem();

	if (!_graphicsMgr->isCursorVisible())
		return;

	if (_mouseLeftBtn || _mouseRightBtn) {
		if (!MaskMouse) {
			_scheduler->doEvent(MC_MOUSE, _mouseRightBtn ? ME_MRIGHT : ME_MLEFT, MP_DEFAULT, mx, my, 0, 0);
			MaskMouse = true;
		}
	} else {
		MaskMouse = false;

		if (!_flagscriptactive && (mx != oldMousePos.x || my != oldMousePos.y)) {
			_scheduler->doEvent(MC_MOUSE, ME_MMOVE, MP_DEFAULT, mx, my, 0, 0);
			oldMousePos = Common::Point(mx, my);
		}
	}
}

/*------------------------------------------------
		Fake distance between two 2D points
--------------------------------------------------*/
float TrecisionEngine::dist2D(float x1, float y1, float x2, float y2) {
	double d1 = fabs((double)(x1 - x2));
	double d2 = fabs((double)(y1 - y2));

	return (float)sqrt(d1 * d1 + d2 * d2);
}

/*------------------------------------------------
			Distance between two 3D points
--------------------------------------------------*/
float TrecisionEngine::dist3D(float x1, float y1, float z1, float x2, float y2, float z2) {
	double d1 = fabs((double)(x1 - x2));
	double d2 = fabs((double)(y1 - y2));
	double d3 = fabs((double)(z1 - z2));
	return (float)sqrt(d1 * d1 + d2 * d2 + d3 * d3);
}

bool TrecisionEngine::isBetween(int a, int x, int b) {
	return x >= a && x <= b;
}

bool TrecisionEngine::isGameArea(Common::Point pos) {
	return isBetween(TOP, pos.y, TOP + AREA - 1);
}

bool TrecisionEngine::isInventoryArea(Common::Point pos) {
	return pos.y >= TOP + AREA;
}

bool TrecisionEngine::isIconArea(Common::Point pos) {
	return pos.y >= TOP + AREA && pos.y < MAXY && pos.x >= ICONMARGSX && pos.x <= MAXX - ICONMARGDX;
}

int TrecisionEngine::getRoomObjectIndex(uint16 objectId) {
	for (uint16 index = 0; index < MAXOBJINROOM; index++) {
		const uint16 curObjId = _room[g_vm->_curRoom]._object[index];
		if (curObjId == 0)
			return -1;
		if (curObjId == objectId)
			return index;
	}

	return -1;
}

} // End of namespace Trecision
