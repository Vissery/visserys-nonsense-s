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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "fullpipe/fullpipe.h"

#include "fullpipe/objectnames.h"
#include "fullpipe/constants.h"

#include "fullpipe/gameloader.h"
#include "fullpipe/motion.h"
#include "fullpipe/scenes.h"
#include "fullpipe/statics.h"

#include "fullpipe/interaction.h"
#include "fullpipe/behavior.h"


namespace Fullpipe {

void scene26_initScene(Scene *sc) {
	g_vars->scene26_chhi = sc->getStaticANIObject1ById(ANI_CHHI, -1);
	g_vars->scene26_drop = sc->getStaticANIObject1ById(ANI_DROP_26, -1);
	g_vars->scene26_sockPic = sc->getPictureObjectById(PIC_SC26_SOCK, 0);
	g_vars->scene26_sock = sc->getStaticANIObject1ById(ANI_SOCK_26, -1);
	g_vars->scene26_var01 = 200;
	g_vars->scene26_var02 = 200;
	g_vars->scene26_var03 = 300;
	g_vars->scene26_var04 = 300;

	if (g_fp->getObjectState(sO_Hatch_26) == g_fp->getObjectEnumState(sO_Hatch_26, sO_WithSock)) {
		g_fp->setObjectState(sO_Hatch_26, g_fp->getObjectEnumState(sO_Hatch_26, sO_Closed));
		g_fp->setObjectState(sO_Sock_26, g_fp->getObjectEnumState(sO_Sock_26, sO_HangsOnPipe));
	}

	Interaction *inter = getGameLoaderInteractionController()->getInteractionByObjectIds(ANI_LUK26, ANI_MAN, ANI_INV_SOCK);

	if (getGameLoaderInventory()->getCountItemsWithId(ANI_INV_VENT) == 0)
		inter->_flags &= 0xFFFDFFFF;
	else
		inter->_flags |= 0x20000;

	if (g_fp->getObjectState(sO_Sock_26) == g_fp->getObjectEnumState(sO_Sock_26, sO_HangsOnPipe))
		g_vars->scene26_sockPic->_flags |= 4;
	else
		g_vars->scene26_sockPic->_flags &= 0xFFFB;

	if (g_fp->getObjectState(sO_Valve1_26) == g_fp->getObjectEnumState(sO_Valve1_26, sO_Opened))
		g_fp->playSound(SND_26_018, 1);
}

int scene26_updateCursor() {
	g_fp->updateCursorCommon();

	if (g_fp->_objectIdAtCursor != ANI_VENT || g_fp->_cursorId != PIC_CSR_DEFAULT) {
		if (g_fp->_cursorId == PIC_CSR_ITN && g_fp->_objectIdAtCursor == PIC_SC26_LTRUBA)
			g_fp->_cursorId = PIC_CSR_GOL;
	} else {
		g_fp->_cursorId = PIC_CSR_ITN;
	}

  return g_fp->_cursorId;
}

void sceneHandler26_updateDrop() {
	warning("STUB: sceneHandler26_updateDrop()");
}

void sceneHandler26_showChi() {
	warning("STUB: sceneHandler26_showChi()");
}

void sceneHandler26_updatePool() {
	warning("STUB: sceneHandler26_updatePool()");
}

void sceneHandler26_hideChi() {
	warning("STUB: sceneHandler26_hideChi()");
}

void sceneHandler26_testVent() {
	warning("STUB: sceneHandler26_testVent()");
}

void sceneHandler26_showVent() {
	warning("STUB: sceneHandler26_showVent()");
}

void sceneHandler26_hideVent() {
	warning("STUB: sceneHandler26_hideVent()");
}

void sceneHandler26_clickVent(StaticANIObject *ani, ExCommand *cmd) {
	warning("STUB: sceneHandler26_clickVent(ani, cmd)");
}

void sceneHandler26_sub01() {
	warning("STUB: sceneHandler26_sub01()");
}

int sceneHandler26(ExCommand *cmd) {
	if (cmd->_messageKind != 17)
		return 0;

	switch (cmd->_messageNum) {
	case MSG_SC26_UPDATEDROP:
		sceneHandler26_updateDrop();
		break;

	case MSG_SC26_SHOWCHI:
		sceneHandler26_showChi();
		break;

	case MSG_SC26_UPDATEPOOL:
		sceneHandler26_updatePool();
		break;

	case MSG_SC26_HIDECHI:
		sceneHandler26_hideChi();
		break;

	case MSG_SC26_TESTVENT:
		sceneHandler26_testVent();
		break;

	case MSG_SC26_SHOWVENT:
		sceneHandler26_showVent();
		break;

	case MSG_SC26_CLICKVENT:
		{
			StaticANIObject *ani = g_fp->_currentScene->getStaticANIObject1ById(ANI_VENT, cmd->_keyCode);

			if (ani && ani->_id == ANI_VENT)
				sceneHandler26_clickVent(ani, cmd);

			break;
		}

	case MSG_SC26_HIDEVENT:
		sceneHandler26_hideVent();
		break;

	case 29:
		{
			StaticANIObject *ani = g_fp->_currentScene->getStaticANIObjectAtPos(cmd->_sceneClickX, cmd->_sceneClickY);

			if (ani && ani->_id == ANI_VENT)
				sceneHandler26_clickVent(ani, cmd);

			break;
		}

	case 33:
		if (g_fp->_aniMan2) {
			int x = g_fp->_aniMan2->_ox;

			if (x < g_fp->_sceneRect.left + g_vars->scene26_var01)
				g_fp->_currentScene->_x = x - g_vars->scene26_var03 - g_fp->_sceneRect.left;

			if (x > g_fp->_sceneRect.right - g_vars->scene26_var01)
				g_fp->_currentScene->_x = x + g_vars->scene26_var03 - g_fp->_sceneRect.right;
		}

		g_fp->_behaviorManager->updateBehaviors();

		g_fp->startSceneTrack();

		break;
	}

	return 0;
}

} // End of namespace Fullpipe
