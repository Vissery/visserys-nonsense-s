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
 * aint32 with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * Based on the original sources
 *   Faery Tale II -- The Halls of the Dead
 *   (c) 1993-1996 The Wyrmkeep Entertainment Co.
 */

#define FORBIDDEN_SYMBOL_ALLOW_ALL // FIXME: Remove

#include "saga2/std.h"
#include "saga2/tilemode.h"
#include "saga2/tile.h"
#include "saga2/setup.h"
#include "saga2/objects.h"
#include "saga2/grabinfo.h"
#include "saga2/mouseimg.h"
#include "saga2/motion.h"
#include "saga2/task.h"
#include "saga2/transit.h"
#include "saga2/magic.h"
#include "saga2/sensor.h"
#include "saga2/timers.h"
#include "saga2/intrface.h"
#include "saga2/dispnode.h"
#include "saga2/uidialog.h"
#include "saga2/contain.h"
#include "saga2/savefile.h"

namespace Saga2 {

#define TEST1           1           //  enable test code
#define TEST2           1
#define TEST3           1
#if DEBUG
#define CHEATMOVE       1           // For moving with keypad in 8 directions
#else
#define CHEATMOVE       0
#endif

/* ===================================================================== *
   gStickyDragControl class: a gGenericControl with a sticky mouse
 * ===================================================================== */

class gStickyDragControl : public gGenericControl {
	bool    sticky;

public:
	gStickyDragControl(gPanelList &, const Rect16 &, uint16, AppFunc *cmd = NULL);

	void setSticky(bool s) {
		sticky = s;
	}
	bool isSticky(void) {
		return sticky;
	}

private:
	void deactivate(void);

//	void pointerMove( gPanelMessage &msg );
	bool pointerHit(gPanelMessage &msg);
	void pointerRelease(gPanelMessage &msg);
};

/* ===================================================================== *
   Globals
 * ===================================================================== */

//void startVideo( char *fileName,int x, int y );
//int16 OptionsDialog( bool disableSaveResume=false );

extern int16        speechButtonCount;      // count of speech buttons
extern void         abortSpeech(void);

extern const uint32 imageGroupID = MKTAG('I', 'M', 'A', 'G');

extern hResContext          *tileRes;       // tile resource handle
extern CycleHandle          cycleList;      // list of tile cycling info
extern int16                    cycleCount;
extern int32                lastUpdateTime;         // time of last display update

//Prototypes For Tile Mode GameMode Object Init
void TileModeHandleTask(void);
void TileModeHandleKey(int16 key, int16 qual);

void initTileBanks(void);
void freeAllTileBanks(void);

void navigateDirect(TilePoint pick, bool runFlag);
void navigatePath(TilePoint pick);

void moveActors(int32 deltaTime);

void drawMainDisplay(void);
void updateMainDisplay(void);

#if DEBUG
void showObjectTerrain(GameObject *obj);
void soundTest1(void);
void soundTest2(void);
void voiceTest1(void);
void voiceTest2(void);
#endif

void toggleMusic(void);

void RShowMem(void);

#if CHEATMOVE
void cheatMove(int16 key);
#endif

void incrementActiveFaction(Actor *a);

//  dispatch functions
static APPFUNC(cmdClickTileMap);                 // appFunc for map display
TilePoint           tilePickPos,            // mouse coord over tilemap (floor)
                    tilePickExactPos,       // mouse coord of click on tilemap
                    objPickPos,             // coord of mouse picked object
                    walkToPos;              // navigation target location

ObjectID            pickedObject;           // which object picked by mouse
ActiveItemPtr       pickedTAI;              // which active item instance

ObjectID            lastPickedObject = Nothing; // ID of last picked object
Alarm               dispObjNameAlarm;       // Alarm used for time delay
// in displaying mouse object's
// name

Alarm               containerObjTextAlarm;  // time delay for container view object text

#if DEBUG && TEST1
ObjectID            pickedActor;
#endif
#if CHEATMOVE
ObjectID            selectedObject = Nothing;
bool                nudge = false;
#endif

extern ObjectID     viewCenterObject;

static struct _delayedNavigation {
	TilePoint   walkToPos;
	bool        pathFindFlag;
	Alarm       delay;

	//  Visual C++ complains about this not having a constructor.
	_delayedNavigation(void) {;}

} delayedNavigation;
static bool navigationDelayed = false;

extern Rect16       tileRect;

//Tile Mode GameMode Object

GameMode            TileMode = {
	NULL,                                   // no previous mode
	false,                                  // mode is not nestable
	TileModeSetup,
	TileModeCleanup,
	TileModeHandleTask,
	TileModeHandleKey,
	drawMainDisplay,
};

//  Duration, in timer ticks, that a character must walk before
//  they can begin to run.

const int           runThreshhold = 32;

extern gToolBase    G_BASE;
extern Alarm        frameAlarm;             // 10 fps frame rate
Alarm               updateAlarm,            // max coord update rate
                    pathFindAlarm;          // mouse click rate for path find
bool                tileLockFlag;           // true if tile mode is locked

GameObject          *mouseObject = NULL;    // object being dragged
Point32             lastMousePos;           // Last mouse position over map
static bool         mousePressed,           // State of mouse button
       clickActionDone = true; // Flag indication wether current
// mouse click action is done
static bool         runFlag = false;        // Reflexs wether the mouse is
// the run zone

static bool         uiKeysEnabled = true;

static char         lastUnusedKey = '\0';


//  The control that covers the scrolling tile display
gStickyDragControl  *tileMapControl;
extern gPanelList   *tileControls,          // panelList of play controls
       *playControls;

extern BackWindow   *mainWindow;

extern uint32 frames;
//  Resource handle for UI imagery

extern hResContext          *imageRes;              // image resource handle

//  Combat related data
static bool         aggressiveActFlag = false;  //  Indicates wether or not
//  there has been an
//  aggressive act
static CalenderTime timeOfLastAggressiveAct;    //  Used to determine the
//  mode state

static bool         inCombat,
       combatPaused;


// This is a correction required by MSVC's inability to provided
// precompiled header services if data is assigned during declaration
// inside a header.  GT 09/11/95

WindowDecoration mainWindowDecorations[] = {
	WindowDecoration(Rect16(0,  0, 640, 20),    MWTopBorder),       // top border
	WindowDecoration(Rect16(0, 440, 640, 40), MWBottomBorder),   // bottom border
	WindowDecoration(Rect16(0, 20, 20, 420),    MWLeftBorder),      // left border
	WindowDecoration(Rect16(460, 20, 180, 142),  MWRightBorder1), // right border #1
	WindowDecoration(Rect16(460, 162, 180, 151),  MWRightBorder2),// right border #2
	WindowDecoration(Rect16(460, 313, 180, 127),  MWRightBorder3),// right border #3
};

/* ===================================================================== *
   TileMode utility functions
 * ===================================================================== */

bool InCombatPauseKludge(void) {
	return (inCombat && combatPaused);
}


//-----------------------------------------------------------------------
//	Function to enable/disable user interface keys

bool enableUIKeys(bool enabled) {
	bool        oldVal = uiKeysEnabled;

	uiKeysEnabled = enabled;
	return oldVal;
}

char luckyKey(char *choices) {
	return lastUnusedKey;

}

//-----------------------------------------------------------------------
//	This function performs all combat pausing tasks

static void pauseCombat(void) {
	pauseCalender();
	pauseBackgroundSimulation();
	pauseInterruptableMotions();
	pauseObjectStates();
	pauseActorStates();
	pauseActorTasks();

	setCenterActorIndicator(true);
}

//-----------------------------------------------------------------------
//	This function performs all combat un-pausing tasks

static void resumeCombat(void) {
	setCenterActorIndicator(false);

	resumeActorTasks();
	resumeActorStates();
	resumeObjectStates();
	resumeInterruptableMotions();
	resumeBackgroundSimulation();
	resumeCalender();
}

//-----------------------------------------------------------------------
//	This function performs all combat initialization tasks

static void startCombat(void) {
	if (g_vm->_autoAggression)
		autoAdjustAggression();

	setCombatBehavior(true);
	combatPaused = false;
}

//-----------------------------------------------------------------------
//	This function performs all combat cleanup tasks

static void endCombat(void) {
	if (combatPaused) {
		combatPaused = false;
		resumeCombat();
	}
	setCombatBehavior(false);

	handleEndOfCombat();
}

//-----------------------------------------------------------------------

void toggleAutoAggression(void) {
	g_vm->_autoAggression = !g_vm->_autoAggression;
	updateAutoAggressionButton(g_vm->_autoAggression);
}

//-----------------------------------------------------------------------

bool isAutoAggressionSet(void) {
	return g_vm->_autoAggression;
}

//-----------------------------------------------------------------------

void toggleAutoWeapon(void) {
	g_vm->_autoWeapon = !g_vm->_autoWeapon;
	updateAutoWeaponButton(g_vm->_autoWeapon);
}

//-----------------------------------------------------------------------

bool isAutoWeaponSet(void) {
	return g_vm->_autoWeapon;
}

//-----------------------------------------------------------------------
//	Called to notify this module of an aggressive act

void logAggressiveAct(ObjectID attackerID, ObjectID attackeeID) {
	if (isPlayerActor(attackerID) || isPlayerActor(attackeeID)) {
		PlayerActorID       playerID;

		if (actorIDToPlayerID(attackeeID, playerID))
			handlePlayerActorAttacked(playerID);

		aggressiveActFlag = true;
		timeOfLastAggressiveAct = calender;
	}
}

//-----------------------------------------------------------------------
//	Determine how much time has elapsed since the last aggressive act
//	involving a player actor

uint16 timeSinceLastAggressiveAct(void) {
	return aggressiveActFlag ? calender - timeOfLastAggressiveAct : maxuint16;
}

//-----------------------------------------------------------------------
//	Determine if there are any enemies within the active regions.

bool areThereActiveEnemies(void) {
	ActiveRegionObjectIterator  iter;
	GameObject                  *obj;

	for (iter.first(&obj); obj != NULL; iter.next(&obj)) {
		if (isActor(obj)
		        &&  !((Actor *)obj)->isDead()
		        && ((Actor *)obj)->disposition == dispositionEnemy)
			return true;
	}

	return false;
}

void CheckCombatMood(void) {
	GameObject          *obj;
	TilePoint           centerLoc;
	ActiveRegion        *ar;
	GameWorld           *world;
	static bool         wasHostile = false;

	ar = getActiveRegion(getCenterActorPlayerID());
	if (ar == NULL) return;

	world = ar->getWorld();
	if (world == NULL || !isWorld(world)) return;

	//  Search for hostile monsters.

	//  If hostile monsters were found last time we searched, then expand the
	//  search radius slightly to provide a little bit of hysterisis to prevent
	//  indecision in music selection.
	CircularObjectIterator  iter8(world,
	                              getCenterActor()->getLocation(),
	                              wasHostile ? 220 : 180);

	bool                agress = isCenterActorAggressive();

	wasHostile = false;
	clearActiveFactions();
	for (iter8.first(&obj); obj != NULL; iter8.next(&obj)) {
		if (isActor(obj)
		        &&  !((Actor *)obj)->isDead()
		        && ((Actor *)obj)->disposition == dispositionEnemy) {
			if (agress || !(((Actor *)obj)->flags & Actor::afraid)) {
				incrementActiveFaction((Actor *) obj);
				wasHostile = true;
			}
		}
	}
	useActiveFactions();
}

//-----------------------------------------------------------------------
//	This function evaluates the mouse state in the standard manner

static void evalMouseState(void) {
	GameObject      *obj = GameObject::objectAddress(pickedObject);
	Actor           *a = getCenterActor();
	bool            interruptable = a->isInterruptable();

	mouseInfo.setDoable(interruptable);

	if (mouseInfo.getObject() != NULL) {
		GameObject  *mObj = mouseInfo.getObject();

		//  If the mouse pointer has an object and the intention
		//  is set to use, modify the doable setting depending
		//  on wether the mouse is pointing at another object
		//  and if so, wether the other object is within the
		//  use range of the center actor
		if (mouseInfo.getIntent() == GrabInfo::Use) {
			assert(obj != NULL);

			if (mObj->containmentSet() & (ProtoObj::isSkill | ProtoObj::isSpell)) {
				GameObject  *tob = pickedObject != Nothing ? obj : NULL;
				// If it's a spell we need to do more complex testing
				//   to see if the current target is valid
				mouseInfo.setDoable(
				    interruptable
				    &&  validTarget(
				        a,
				        tob,
				        pickedTAI,
				        (SkillProto *)GameObject::protoAddress(
				            mObj->thisID())));
			} else {
				mouseInfo.setDoable(
				    interruptable
				    && (pickedObject == Nothing
				        || (a->inUseRange(
				                obj->getLocation(),
				                mObj)
				            && (a->inRange(obj->getLocation(), 8)
				                ||  lineOfSight(a, obj, terrainTransparent)))));
			}
		}
	} else {
		//  Determine if the mouse is being dragged
		if (mousePressed) {
			//  Adjust the intention and doable settings based
			//  factors such as center actor aggression, wether
			//  the mouse is pointing at an object, etc...
			//  Determine if the center actor is aggressive
			if (isCenterActorAggressive()) {
				//  Determine if the mouse is pointing at an
				//  object
				if (pickedObject != Nothing
				        &&  !isPlayerActor(pickedObject)) {
					//  If in attack range, set the intention
					//  to attack, else set the intention to walk
					//  to the picked object
					if (a->inAttackRange(obj->getLocation())
					        && (a->inRange(obj->getLocation(), 8)
					            ||  lineOfSight(a, obj, terrainTransparent)))
						mouseInfo.setIntent(GrabInfo::Attack);
					else {
						mouseInfo.setIntent(GrabInfo::WalkTo);
						walkToPos = obj->getLocation();
					}
				} else
					//  The mouse is not pointing at an object
				{
					//  Since there is no picked object,
					//  determine wether the center actor has
					//  finished can initiate a new action, if so, set
					//  the intention to walk to the mouse pointer
					if (interruptable) {
						mouseInfo.setIntent(GrabInfo::WalkTo);
						if (tileMapControl->isSticky())
							setMouseImage(kMouseAutoWalkImage, -8, -8);
						walkToPos = tilePickPos;
					}
				}
			} else
				//  The center actor is not aggressive
			{
				//  Set the intention to walk to the mouse
				//  pointer
				mouseInfo.setIntent(GrabInfo::WalkTo);
				if (tileMapControl->isSticky())
					setMouseImage(kMouseAutoWalkImage, -8, -8);
				walkToPos = tilePickPos;
			}
		} else
			//  The mouse is not being dragged
		{
			//  Determine if mouse is pointing at an object
			if (pickedObject != Nothing) {
				//  Determine if the center actor is aggressive
				if (isCenterActorAggressive()
				        &&  !isPlayerActor(pickedObject)) {
					//  If center actor is in attack range of
					//  the picked object, set the intention to
					//  attack, else set the intention to walk
					//  to the object
					if (a->inAttackRange(obj->getLocation())
					        && (a->inRange(obj->getLocation(), 8)
					            ||  lineOfSight(a, obj, terrainTransparent))) {
						mouseInfo.setIntent(GrabInfo::Attack);
						mouseInfo.setDoable(true);
					} else {
						mouseInfo.setIntent(GrabInfo::WalkTo);
						walkToPos = obj->getLocation();
					}
				} else
					//  Center actor is not aggressive
				{
					//  If pointing at an actor, set the
					//  intention to walk to the actor, else
					//  set the intention to pick up the object
					if (isActor(pickedObject)) {
						a = (Actor *)obj;

						mouseInfo.setIntent(
						    !a->isDead()
						    ?   GrabInfo::WalkTo
						    :   GrabInfo::Open);
						walkToPos = obj->getLocation();
					} else {
						mouseInfo.setIntent(obj->isCarryable()
						                    ? GrabInfo::PickUp
						                    : GrabInfo::Open);
						mouseInfo.setDoable(
						    interruptable
						    &&  a->inReach(obj->getLocation())
						    && (a->inRange(obj->getLocation(), 8)
						        ||  lineOfSight(a, obj, terrainTransparent)));
					}
				}
			} else
				//  The mouse is not pointing at an object
			{
				//  Simply set the intention to walk to the mouse
				//  pointer
				mouseInfo.setIntent(GrabInfo::WalkTo);
				if (tileMapControl->isSticky())
					setMouseImage(kMouseAutoWalkImage, -8, -8);
				walkToPos = tilePickPos;
			}
		}
	}

	if (mousePressed
	        &&  !clickActionDone
	        &&  mouseInfo.getObject() == NULL) {
		a = getCenterActor();

		//  Since the mouse is being dragged, initiate
		//  the effects of the mouse drag

		if (mouseInfo.getIntent() == GrabInfo::WalkTo) {
			if (mouseInfo.getDoable()
			        &&  !navigationDelayed) {
				MotionTask  *mt = a->moveTask;

				if (mt == NULL || !mt->isWalk()) {
					navigateDirect(walkToPos, runFlag);
				} else if (updateAlarm.check()) {
					mt->changeDirectTarget(
					    walkToPos,
					    runFlag);
					updateAlarm.set(ticksPerSecond / 2);
				}
			}
		} else if (mouseInfo.getIntent() == GrabInfo::Attack) {
			if (mouseInfo.getDoable())
				a->attack(GameObject::objectAddress(pickedObject));
		}
	}
}

//-----------------------------------------------------------------------
//	Initialize the tile mode state

void initTileModeState(void) {
	assert(uiKeysEnabled);

	aggressiveActFlag = false;
	inCombat = false;
	combatPaused = false;
}

//-----------------------------------------------------------------------
//	Save the tile mode state to a save file

void saveTileModeState(SaveFileConstructor &saveGame) {
	int32       size = 0;

	assert(uiKeysEnabled);

	//  Compute the number of bytes needed
	size +=     sizeof(aggressiveActFlag)
	            +   sizeof(inCombat)
	            +   sizeof(combatPaused);
	if (aggressiveActFlag)
		size += sizeof(timeOfLastAggressiveAct);

	//  Create the chunk and write the data
	saveGame.newChunk(MakeID('T', 'M', 'S', 'T'), size);
	saveGame.write(&aggressiveActFlag, sizeof(aggressiveActFlag));
	saveGame.write(&inCombat, sizeof(inCombat));
	saveGame.write(&combatPaused, sizeof(combatPaused));
	if (aggressiveActFlag) {
		saveGame.write(
		    &timeOfLastAggressiveAct,
		    sizeof(timeOfLastAggressiveAct));
	}
}

//-----------------------------------------------------------------------
//	Load the tile mode state from a save file

void loadTileModeState(SaveFileReader &saveGame) {
	assert(uiKeysEnabled);

	//  Simply read in the data
	saveGame.read(&aggressiveActFlag, sizeof(aggressiveActFlag));
	saveGame.read(&inCombat, sizeof(inCombat));
	saveGame.read(&combatPaused, sizeof(combatPaused));
	if (aggressiveActFlag) {
		saveGame.read(
		    &timeOfLastAggressiveAct,
		    sizeof(timeOfLastAggressiveAct));
	}
	tileLockFlag = false;
}

/* ===================================================================== *
   TileMode management functions
 * ===================================================================== */

//-----------------------------------------------------------------------
//	Initialize the Tile mode

void TileModeSetup(void) {
	//  Load in decorative panels for the main window (for this mode)
	mainWindow->setDecorations(mainWindowDecorations, elementsof(mainWindowDecorations), imageRes);

	//  Test to draw borders.
	//  REM: We should actually have a routine to refresh the window...
	mainWindow->draw();

	//  Create a control covering the map area.
	tileMapControl = new gStickyDragControl(*playControls, tileRect, 0, cmdClickTileMap);

	//Enable Tile Mode Specific Controls
	tileControls->enable(true);

	initTileBanks();

	lastUpdateTime = gameTime;

#if DEBUG*0
	debugMap.size.x = 64;
	debugMap.size.y = 200;

	debugMap.data = (uint8 *)RNewPtr(debugMap.bytes(), NULL);
	if (debugMap.data == NULL) {
		fatal("Unable to allocate memory for debug map\n");
	}
#endif

	setCurrentWorld(WorldBaseID);
	setCurrentMap(currentWorld->mapNum);
}

//-----------------------------------------------------------------------
//	Cleanup function for Tile mode

void TileModeCleanup(void) {
	//Disable Tile Mode Specific Controls
	tileControls->enable(false);

	freeAllTileBanks();

//	freePalette();

//	if (tileRes) resFile->disposeContext( tileRes );
//	tileRes = NULL;

	delete tileMapControl;

//	This Fixes the mousePanel That's not set up
	G_BASE.mousePanel = NULL;

	mainWindow->removeDecorations();
}

//-----------------------------------------------------------------------
//	This code handles most of the periodic repetitive tasks in
//	the main game mode.
static int postDisplayFrame = 3;

//  We need to test if UI is locked, so as to not pause combat
extern int          lockUINest;

void TileModeHandleTask(void) {
	bool taskChek = false;
	//  Run any SAGA scripts which are waiting to run.
	dispatchScripts();


	// update day and night
	//mm("daytime transition update loop");
	dayNightUpdate();

	//  If it's time to do a new frame.
	if (frameAlarm.check()
	        &&  tileLockFlag == 0) {
		static int flipper = 0;

		//  Get the actor we're controlling.
		Actor       *a = getCenterActor();

		audioEnvironmentSetAggression(isCenterActorAggressive());

		//  Check combat mood once every 8 frames or so.
		//  Otherwise, check for combat start/stop
		//  (Kludge to balance CPU usage).

		if ((++flipper & 0xF) == 0)
			CheckCombatMood();
		else if (timeSinceLastAggressiveAct() < 60
		         &&  areThereActiveEnemies()) {
			if (!inCombat) {
				inCombat = true;
				startCombat();
			}
		} else {
			if (inCombat) {
				inCombat = false;
				endCombat();
			}
		}

		if (inCombat) {
			if (!a->isMoving() && a->isInterruptable() && lockUINest == 0) {
				if (!combatPaused) {
					combatPaused = true;
					pauseCombat();
				}
			} else {
				if (combatPaused) {
					combatPaused = false;
					resumeCombat();
				}
			}
		}

		updateCalender();

		// update the text status line
		StatusLine->experationCheck();

		doBackgroundSimulation();

		// do an alarm check for container views
		if (containerObjTextAlarm.check()) {
			ContainerView::objTextAlarm = true;
		}

		if (ContainerView::objTextAlarm == true) {
			// if the mouse is in a container...
			if (ContainerView::mouseInView) {
				mouseInfo.setText(ContainerView::mouseText);
			}
		}


		if (G_BASE.isMousePanel(tileMapControl)) {
#if DEBUG
			showObjectTerrain(a);
#endif

			//  If mouse is near edge of screen, then run.
			runFlag =       lastMousePos.x < runThreshhold
			                ||  lastMousePos.x >= tileRect.width - runThreshhold
			                ||  lastMousePos.y < runThreshhold
			                ||  lastMousePos.y >= tileRect.height - runThreshhold;

			//  Calculate the mouse's position on the tile map.
			if (runFlag) {
				//  Calculate the mouse's position on the tilemap,
				//  without regard to the actual shape of the terrain.
				tilePickPos = pickTilePos(lastMousePos, a->getLocation());
				tilePickExactPos = tilePickPos;
				pickedTAI = NULL;
			} else {
				//  Calculate the mouse's position on the tilemap,
				//  including the shape of the terrain. Actually
				//  this returns two seperate coords: The exact point
				//  clicked on, and the projection on the floor
				//  beneath the clicked point.
				tilePickExactPos = pickTile(lastMousePos,
				                            a->getLocation(),
				                            &tilePickPos,
				                            &pickedTAI);
			}


			pickedObject = pickObject(lastMousePos, objPickPos);
			GameObject  *item = GameObject::objectAddress(pickedObject);

			// Find Out If Terrain Or Object Is Deeper

			if (!item->isObscured() || (!isActor(item) && !objRoofRipped(item))) {
				if (tilePickExactPos.z > objPickPos.z) {
					pickedObject = Nothing;
					item = GameObject::objectAddress(pickedObject);
				}
			}

			//  Determine if the mouse is pointing at a new object
			if (pickedObject != lastPickedObject) {
				lastPickedObject = pickedObject;

				//  Remove current mouse cursor text and gauge
				mouseInfo.setText(NULL);
				mouseInfo.clearGauge();

				//  If mouse in on object set alarm to determine when
				//  to display the object's name
				if (pickedObject != Nothing)
					dispObjNameAlarm.set(ticksPerSecond / 2);
			}

			if (pickedObject != Nothing) {
				//  Determine if it is time to display the name of the
				//  object at which the mouse is pointing
				if (dispObjNameAlarm.check()) {
					const   int bufSize = 40;
					char    cursorText[bufSize];

					// get the object text into the buffer
					item->objCursorText(cursorText, bufSize);

					mouseInfo.setText(cursorText);
					if (isActor(pickedObject)) {
						a = (Actor *)GameObject::objectAddress(pickedObject);
						mouseInfo.setGauge(a->getStats()->vitality, a->getBaseStats()->vitality);
					} else {
						mouseInfo.clearGauge();
					}
				}
			}

			evalMouseState();
		}

		if (navigationDelayed && delayedNavigation.delay.check()) {
			if (delayedNavigation.pathFindFlag)
				navigatePath(delayedNavigation.walkToPos);
			else
				navigateDirect(delayedNavigation.walkToPos, false);
			navigationDelayed = false;
		}

		updateContainerWindows();

		updateActiveRegions();

		checkTimers();

		checkSensors();


		updateObjectStates();
		updateActorStates();

		//  Update positions of all objects
		moveActors(0);               // for objects with motion task.

		//  Update the states of all active terrain
		moveActiveTerrain(0);            // for terrain with activity tasks.

		//  Set the time of the next frame
		frameAlarm.set(framePeriod);
		updateMainDisplay();

		if (inCombat || postDisplayFrame++ > 2)
			taskChek = true;
	} else if (postDisplayFrame) {
		taskChek = true;
	}
	if (taskChek) {
		postDisplayFrame = 0;
		updateActorTasks();
	}
}

//-----------------------------------------------------------------------

extern void toggleAgression(PlayerActorID bro, bool all);
extern void toggleBanding(PlayerActorID bro, bool all);
extern void toggleIndivMode(void);

void TileModeHandleKey(int16 key, int16 qual) {
	TilePoint Pos, ActorTP;
	Actor   *a = getCenterActor();
	Location l(a->getLocation(), a->IDParent());

	//GameObject *object = (GameObject *)getCenterActor();

	lastUnusedKey = '\0';
	//This is for moving center actor in cardinal directions
	//by amount specified by loc const int moveDist and using keypad
	//without num lock on

#if CHEATMOVE
	cheatMove(key);
#endif
	//  If there is currently a speech balloon up, and the
	//  speech balloon has embedded buttons, then disallow
	//  user input -- except that for now we will still allow
	//  the special 'quit' key.
	if (speechButtonCount > 0) {
		if (key != 0x1b && key != 'b') return;
	}

	//-----------------------------------------------------------------------

	switch (tolower(key)) {

	case ' ':
		abortSpeech();
		if (uiKeysEnabled) {
			if (tileMapControl->isSticky()) {
				tileMapControl->setSticky(false);
				mousePressed = false;
				setMouseImage(kMouseArrowImage, 0, 0);
				evalMouseState();
			}
			MotionTask::wait(*a);
		}
		break;

	case 'a':
		if (uiKeysEnabled)
			toggleAgression(getCenterActorPlayerID(), qual & qualifierShift);
		break;
	case 'b':
		if (uiKeysEnabled)
			toggleBanding(getCenterActorPlayerID(), qual & qualifierShift);
		break;
	case '\t':
		if (uiKeysEnabled)
			toggleIndivMode();
		break;
	case '1':
		if (uiKeysEnabled)
			setCenterBrother(FTA_JULIAN);
		break;
	case '2':
		if (uiKeysEnabled)
			setCenterBrother(FTA_PHILIP);
		break;
	case '3':
		if (uiKeysEnabled)
			setCenterBrother(FTA_KEVIN);
		break;
	case 'o':
		if (uiKeysEnabled)
			OptionsDialog();
		break;

	//  Keyboard equivalents for mental containers
	case 'i':
		if (uiKeysEnabled)
			OpenMindContainer(getCenterActorPlayerID(), true, 0);
		break;
	case 's':
		if (uiKeysEnabled)
			OpenMindContainer(getCenterActorPlayerID(), true, 1);
		break;
	case 'k':
		if (uiKeysEnabled)
			OpenMindContainer(getCenterActorPlayerID(), true, 2);
		break;

	case 'm':
		toggleMusic();
		break;


#if DEBUG
	case 'q':
	case 0x1b:
		endGame();
		break;

#else
	case 0x1b:
		if (uiKeysEnabled)
			OptionsDialog();
		break;
#endif


#if DEBUG
	case 'l':
		RShowMem();
		break;

	case 't':
		extern bool showTile;
		showTile = !showTile;
		break;

	// basically for changing the light level

	case 'c': {
		extern bool autoMapCheat;
		autoMapCheat = !autoMapCheat;
	}
	break;

	case 'h':
		calender.hour += 1;

		if (calender.hour >= 24) {
			calender.hour = 0;
		}

		break;

	case 'w':
		calender.hour = 9;
		calender.days++;
		dayNightUpdate();

		break;
#endif

	default:
		if (uiKeysEnabled)
			lastUnusedKey = key;

	}
}

//  A debugging function to show the queue of recent mouse events.

#if DEBUG
static char mouseHistory[33];

void showMouseEvent(char eventType) {
	memmove(mouseHistory + 1, mouseHistory, 31);
	mouseHistory[0] = eventType;
	mouseHistory[32] = '\0';
	WriteStatusF(5, mouseHistory);
}
#endif

//-----------------------------------------------------------------------
//	Handle mouse actions on the tile map "control".

static APPFUNC(cmdClickTileMap) {

	static bool dblClick = false;

	//  REM: This code needs to be moved elsewhere. We put it
	//  here for testing purposes only. It should actually go on
	//  it's own panel, which overrides all other panels, and
	//  that should be part of a special "speech button" mode.

	if (!uiKeysEnabled) return;

	switch (ev.eventType) {
	case gEventRMouseDown:

#if CHEATMOVE
		selectedObject = pickedObject;
#endif
		if (isActor(pickedObject)) {
			PlayerActorID       playerID;

			if (actorIDToPlayerID(pickedObject, playerID))
				setAggression(playerID, !isAggressive(playerID));
		}
		break;

	case gEventMouseMove:
	case gEventMouseDrag:
		if (ev.value & gGenericControl::leave) {
			mousePressed = false;

			if (mouseInfo.getObject() == NULL)
				mouseInfo.setIntent(GrabInfo::WalkTo);
			mouseInfo.setDoable(true);

			//  Remove any mouse text
			lastPickedObject = Nothing;
			mouseInfo.setText(NULL);
			mouseInfo.clearGauge();
		}
		lastMousePos = ev.mouse;
		break;

	case gEventMouseDown:

		mousePressed = true;

		clickActionDone = false;

		{
			//  Get the center actor's ID and a pointer to the center
			//  actor's structure
			ObjectID    centerActorID = getCenterActorID();
			Actor       *centerActorPtr =
			    (Actor *)GameObject::objectAddress(centerActorID);

			if ((mouseObject = mouseInfo.getObject()) != NULL) {
				//  If we are using an intangible object (spell) then consider
				//  the owner of the spell to be the center actor for the rest
				//  of this action.
				if (mouseObject->proto()->containmentSet() & (ProtoObj::isIntangible | ProtoObj::isSpell | ProtoObj::isSkill)) {
					ObjectID    possessor = mouseObject->possessor();

					if (possessor != Nothing) {
						centerActorID = possessor;
						centerActorPtr = (Actor *)GameObject::objectAddress(possessor);
					}
				}

				if (pickedObject != Nothing) {
					//  we dropped the object onto another object
					if (mouseInfo.getDoable()) {
						int16   intent = mouseInfo.getIntent();

						mouseInfo.replaceObject();
						if (intent == GrabInfo::Use) {
							MotionTask::useObjectOnObject(
							    *centerActorPtr,
							    *mouseObject,
							    *GameObject::objectAddress(pickedObject));
						} else if (intent == GrabInfo::Drop) {
							MotionTask::dropObjectOnObject(
							    *centerActorPtr,
							    *mouseObject,
							    *GameObject::objectAddress(pickedObject),
							    mouseInfo.getMoveCount());
						}

						((gGenericControl *)ev.panel)->disableDblClick();
						clickActionDone = true;
					} else if (mouseInfo.getIntent() == GrabInfo::Use) {
						mouseInfo.replaceObject();
						clickActionDone = true;
					}
				} else if (pickedTAI != NULL) {
					//  we dropped the object onto active terrain

					if (mouseInfo.getDoable()) {
						if (mouseInfo.getIntent() == GrabInfo::Drop
						        ||  mouseInfo.getIntent() == GrabInfo::Use) {
							int16   intent = mouseInfo.getIntent();

							mouseInfo.replaceObject();
							if (intent == GrabInfo::Drop) {
								MotionTask::dropObjectOnTAI(
								    *centerActorPtr,
								    *mouseObject,
								    *pickedTAI,
								    Location(tilePickPos, currentWorld->thisID()));
							} else {
								TilePoint   TAILoc;

								TAILoc = getClosestPointOnTAI(pickedTAI, centerActorPtr);

								if (centerActorPtr->inReach(TAILoc)
								        && (centerActorPtr->inRange(TAILoc, 8)
								            ||  lineOfSight(
								                centerActorPtr,
								                TAILoc,
								                terrainTransparent)))
									MotionTask::useObjectOnTAI(
									    *centerActorPtr,
									    *mouseObject,
									    *pickedTAI);
							}

							((gGenericControl *)ev.panel)->disableDblClick();
							clickActionDone = true;
						}
					} else if (mouseInfo.getIntent() == GrabInfo::Use) {
						mouseInfo.replaceObject();
						clickActionDone = true;
					}
				} else if (pickedObject == Nothing) {
					//  we dropped the object on the ground

					if (mouseInfo.getIntent() == GrabInfo::Drop
					        &&  mouseInfo.getDoable()) {
						mouseInfo.replaceObject();
						MotionTask::dropObject(
						    *centerActorPtr,
						    *mouseObject,
						    Location(tilePickPos, currentWorld->thisID()),
						    mouseInfo.getMoveCount());
						((gGenericControl *)ev.panel)->disableDblClick();
						clickActionDone = true;
					} else if (mouseInfo.getIntent() == GrabInfo::Use
					           &&  mouseInfo.getDoable()) {
						// New for spells - this enables objects to be used on a
						//   general location (for area spells etc)
						mouseInfo.replaceObject();
						MotionTask::useObjectOnLocation(
						    *centerActorPtr,
						    *mouseObject,
						    Location(tilePickPos, currentWorld->thisID()));
						clickActionDone = true;
					} else if (mouseInfo.getIntent() == GrabInfo::Use) {
						mouseInfo.replaceObject();
						clickActionDone = true;
					}
				}
			} else if (pickedObject != Nothing) {
				//GameObject      *obj = GameObject::objectAddress(pickedObject);

				if (mouseInfo.getDoable()) {
					PlayerActorID   pID;

					if (actorIDToPlayerID(pickedObject, pID) && !isBrotherDead(pID)) {
						setCenterBrother(pID);
						clickActionDone = true;
					} else if (mouseInfo.getIntent() == GrabInfo::PickUp
					           ||  mouseInfo.getIntent() == GrabInfo::Open) {
						GameObject  *pickedObjPtr =
						    GameObject::objectAddress(pickedObject);
						int16       quantity = 1;

						MotionTask::turnTowards(
						    *centerActorPtr,
						    GameObject::objectAddress(pickedObject)->getLocation());

						if (pickedObjPtr->proto()->flags & ResourceObjectPrototype::objPropMergeable)
							quantity = pickedObjPtr->getExtra();

						if (pickedObjPtr->take(centerActorID, quantity))
							clickActionDone = true;
					} else if (mouseInfo.getIntent() == GrabInfo::Attack) {
						centerActorPtr->attack(
						    GameObject::objectAddress(pickedObject));
						((gGenericControl *)ev.panel)->disableDblClick();
					}
				}

#if DEBUG && TEST1
				if (isActor(pickedObject))
					pickedActor = pickedObject;
#endif

			}
			//  We're not pointing at an object and the mouse cursor
			//  does not have an object
			else {
				if (mouseInfo.getIntent() == GrabInfo::WalkTo
				        &&  mouseInfo.getDoable()) {
					if (pickedTAI == NULL) {
						navigateDirect(walkToPos, false);
						//      ( ( gGenericControl * )ev.panel )->disableDblClick();
					} else {
						navigationDelayed = true;
						delayedNavigation.walkToPos = walkToPos;
						delayedNavigation.pathFindFlag = false;
						delayedNavigation.delay.set(ticksPerSecond / 2);
					}
					pathFindAlarm.set(ticksPerSecond / 2);
				}
			}
		}
		break;

	case gEventMouseUp:

		mousePressed = false;

		if (dblClick)
			dblClick = false;
		else {
			if (pathFindAlarm.check()) { // mouse click was too long for path find
				if (mouseInfo.getIntent() == GrabInfo::WalkTo) {
					Actor   *a = getCenterActor();

					if (a->moveTask && a->moveTask->isWalk())
						a->moveTask->finishWalk();
				}
				navigationDelayed = false;
			} else {
				if (navigationDelayed) {
					delayedNavigation.walkToPos = walkToPos;
					delayedNavigation.pathFindFlag = true;
					delayedNavigation.delay.set(ticksPerSecond / 2);
				} else {
					Actor   *a = getCenterActor();

					if ((walkToPos - a->getLocation()).quickHDistance() > 24)
						navigatePath(walkToPos);
					else
						navigateDirect(walkToPos, false);
				}
			}
		}
		break;

	case gEventDoubleClick:

		dblClick = true;

		navigationDelayed = false;

		if ((mouseObject = mouseInfo.getObject()) != NULL) {
			mouseInfo.replaceObject();
			MotionTask::useObject(*getCenterActor(), *mouseObject);
		} else if (pickedObject != Nothing) {
			GameObject      *obj = GameObject::objectAddress(pickedObject);

			if (mouseInfo.getDoable()) {
				//  Double-click on an actor is the same as "greet".
				if (isActor(pickedObject)
				        &&  !((Actor *)obj)->isDead()
				        &&  !isCenterActorAggressive()) {
					ActorProto  *proto = (ActorProto *)obj->proto();

					proto->greetActor(pickedObject, getCenterActorID());
				} else if (mouseInfo.getIntent() == GrabInfo::PickUp
				           ||  mouseInfo.getIntent() == GrabInfo::Open
				           || (isActor(pickedObject) && ((Actor *)obj)->isDead())) {
					GameObject  *pickedObjPtr =
					    GameObject::objectAddress(pickedObject);

					MotionTask::useObject(*getCenterActor(), *pickedObjPtr);
					clickActionDone = true;
				}
			}
		} else if (pickedTAI != NULL) {
			Actor       *a = getCenterActor();
			TilePoint   TAILoc;

			TAILoc = getClosestPointOnTAI(pickedTAI, a);

			if (a->inRange(TAILoc, 32)
			        && (a->inRange(TAILoc, 8)
			            ||  lineOfSight(a, TAILoc, terrainTransparent)))
				MotionTask::useTAI(*a, *pickedTAI);
		} else {
			tileMapControl->setSticky(true);
			setMouseImage(kMouseAutoWalkImage, -8, -8);
			mousePressed = true;
		}
		break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------
//	Sets up a motion task for the main character.

void navigateDirect(TilePoint pick, bool runFlag_) {
	Actor   *a = getCenterActor();          // addr of actor we control

	if (a) {
		updateAlarm.set(ticksPerSecond / 2);

		//  REM: Do running here...

		MotionTask::walkToDirect(*a, pick, runFlag_, false);
	}
}

//-----------------------------------------------------------------------
//	Sets up a motion task and a path find request for the main character.

void navigatePath(TilePoint pick) {
	Actor   *a = getCenterActor();          // addr of actor we control

	if (a) {
		if (a->isMoving())
			//  if motion task already exists, change the target
			a->moveTask->changeTarget(pick);
		else
			//  else create a new motion task
			MotionTask::walkTo(*a, pick, false, false);
	}
}

#if CHEATMOVE
void cheatMove(int16 key) {
	if (selectedObject == Nothing) return;

	if (tolower(key) == 'n') {
		nudge = !nudge;
		return;
	}

	union {
		int16       key1;
		char        key_ch[2];
	} get;

	GameObject  *obj = GameObject::objectAddress(selectedObject);
	TilePoint   t = obj->getLocation();
	int         moveDist = nudge ? 1 : 64;

	get.key1 = key;

	if (get.key_ch[0] == 0) {
		switch (get.key_ch[1]) {
		case 72:        //Up
			t.u += moveDist;
			t.v += moveDist;
			obj->move(t);
			break;

		case 80:        //Down
			t.u -= moveDist;
			t.v -= moveDist;
			obj->move(t);
			break;

		case 73:        //Up Right
			t.u += moveDist;
			obj->move(t);
			break;

		case 71:        //Up Left
			t.v += moveDist;
			obj->move(t);
			break;

		case 81:        //Down Right
			t.v -= moveDist;
			obj->move(t);
			break;

		case 79:        //Down Left
			t.u -= moveDist;
			obj->move(t);
			break;

		case 75:        //Left
			t.u -= moveDist;
			t.v += moveDist;
			obj->move(t);
			break;

		case 77:        //Right
			t.u += moveDist;
			t.v -= moveDist;
			obj->move(t);
			break;
		}
		WriteStatusF(3, "U %d V %d Z %d", t.u, t.v, t.z);
	}
}
#endif

/* ===================================================================== *
   gStickyDragControl class: a gGenericControl with a sticky mouse
 * ===================================================================== */

gStickyDragControl::gStickyDragControl(gPanelList &list, const Rect16 &box,
                                       uint16 ident, AppFunc *cmd)
	: gGenericControl(list, box, ident, cmd) {
	sticky = false;
}

void gStickyDragControl::deactivate(void) {
	if (sticky) setMouseImage(kMouseArrowImage, 0, 0);
	sticky = false;
	gGenericControl::deactivate();
}

//void gStickyDragControl::pointerMove( gPanelMessage & )
//{
//	notify( gEventMouseMove, 0 );
//}

bool gStickyDragControl::pointerHit(gPanelMessage &msg) {
	if (sticky) setMouseImage(kMouseArrowImage, 0, 0);
	sticky = false;
	return gGenericControl::pointerHit(msg);
}

void gStickyDragControl::pointerRelease(gPanelMessage &msg) {
	if (sticky == false)
		gGenericControl::pointerRelease(msg);
}

void noStickyMap(void) {
	((gPanel *)tileMapControl)->deactivate();
	mousePressed = false;
}

} // end of namespace Saga2
