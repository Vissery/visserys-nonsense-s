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
#include "cutscene.h"
#include "dragons.h"
#include "dragons/actor.h"
#include "dragons/actorresource.h"
#include "dragons/background.h"
#include "dragons/inventory.h"
#include "dragons/cursor.h"
#include "dragons/dragonini.h"
#include "dragons/scene.h"
#include "dragons/screen.h"
#include "dragons/talk.h"

namespace Dragons {

CutScene::CutScene(DragonsEngine *vm): _vm(vm) {
	loadPalettes();
}

CutScene::~CutScene() {
	if (_palettes) {
		free(_palettes);
	}
}

void CutScene::scene1() {
	// TODO spcLoadScene1 knights around the table.
	bool isFlag10Set = _vm->isFlagSet(ENGINE_FLAG_10);

	DragonINI *flicker = _vm->_dragonINIResource->getFlickerRecord();

	DAT_80063514 = 0xb00;
	_vm->_dragonINIResource->setFlickerRecord(NULL);

	_vm->setUnkFlags(ENGINE_UNK1_FLAG_2);
	//fade_related_calls_with_1f();
	_vm->clearFlags(ENGINE_FLAG_10);

	_vm->_inventory->setActorFlag400();
	_vm->_cursor->setActorFlag400();

//	scr_tilemap1_w = 0x28;
//	DAT_8006a3f0 = DAT_8006a3ec;

//	load_actor_file(0x81);
//	load_actor_file(0x7d);
//	load_actor_file(0x7e);
//	load_actor_file(0x8f);
//	load_actor_file(0xaa);
	_vm->setFlags(ENGINE_FLAG_20000);
	wideAngleEveryoneAtTable();
	_vm->waitForFrames(0x5a);
	closeUpShotOnActor(0xd3, 0, 0x233, 0x17a); //close up on king
	//playSoundFromTxtIndex(0x4e26);
	if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,4,0,0x4e26,0x2e01) != 2) {
		fadeScreenAndResetActor(DAT_80072de8);
		closeUpShotOnActor(0xd8, 0, 0xfd, 0x60); // cut to flicker
		//playSoundFromTxtIndex(0x4ea2);
		if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,2,0,0x4ea2,0x701) != 2) {
			fadeScreenAndResetActor(DAT_80072de8);
			closeUpShotOnActor(0xd3, 0, 0x233, 0x17a); //close up on king
			//playSoundFromTxtIndex(0x4eec);
			if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,5,0,0x4eec,0x2e01) != 2) {
				fadeScreenAndResetActor(DAT_80072de8);
				wideAngleEveryoneAtTable();  // shot of whole room
				if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,4,0,0x5000,0x2e01) != 2 &&
					_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_800830c0,0x1d,0x1c,0x5074,0x501) != 2 &&
					_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072df0,9,5,0x511c,0xc01) != 2 &&
					_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_800830c0,0x1d,0x1c,0x5138,0x501) != 2) {
					closeUpShotOnActor(0xd7, 0, 0x312, 0x260); //close up on flame
					//playSoundFromTxtIndex(0x5152);
					if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,2,0,0x5152,0x3e01) != 2) {
						fadeScreenAndResetActor(DAT_80072de8);
						closeUpShotOnActor(0xd8, 0, 0xfd, 0x60); //close up flicker
						//playSoundFromTxtIndex(0x51fc);
						if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,3,0,0x51fc,0x701) != 2) {
							_vm->playOrStopSound(0x8004);
							_vm->waitForFrames(0x28);
							_vm->playOrStopSound(0x8003);
							fadeScreenAndResetActor(DAT_80072de8);
							DAT_80063514 = DAT_80063514 | 0x40;
							FUN_8003d8e8(0xd6,0,0x37a,0x280);
							DAT_80072dec = _vm->_actorManager->loadActor(0xd5,0,0x2d6,0xc6,3); //load humans
							DAT_80072df0 = _vm->_actorManager->loadActor(0xd3,2,0x87,199,3);
							_vm->waitForFramesAllowSkip(4);
							// call_fade_related_1f();
							if (FUN_8003dab8(0x52d6,0,0,0x701,1) != 2) {
								// TODO callMaybeResetData();
								//playSoundFromTxtIndex(0x530c);
								if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,2,0,0x530c,0x3c01) != 2) {
									while (0x10 < _vm->_scene->_camera.x) {
										_vm->_scene->_camera.x = _vm->_scene->_camera.x + -0x10;
										_vm->waitForFrames(1);
									}
									_vm->_scene->_camera.x = 0;
									//playSoundFromTxtIndex(0x54dc);
									if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072df0,6,2,0x54dc,0x2e01) != 2) {
										fadeScreenAndResetActor(DAT_80072de8);
										fadeScreenAndResetActor(DAT_80072dec);
										fadeScreenAndResetActor(DAT_80072df0);
										wideAngleEveryoneAtTable();
										if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072e08,3,2,0x55d4,0xc01) != 2) {
											closeUpShotOnActor(0xd4, 0, 0x8a, 0); //close up chancellor
											//playSoundFromTxtIndex(0x562c);
											if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,2,0,0x562c,0xc01) != 2) {
												fadeScreenAndResetActor(DAT_80072de8);
												FUN_8003d8e8(0xd6,0,0x37a,0x280);
												DAT_80072dec = _vm->_actorManager->loadActor(0xd5,4,0x2d6,0xc6,3);
												_vm->waitForFramesAllowSkip(4);
												// call_fade_related_1f();
												if (FUN_8003dab8(0x5780,0x14,0,0xc01,1) != 2) {
													DAT_80063514 = DAT_80063514 | 0x40;
													fadeScreenAndResetActor(DAT_80072de8);
													fadeScreenAndResetActor(DAT_80072dec);
													closeUpShotOnActor(0xd7, 0, 0x312, 0x260); // close up flame
													//playSoundFromTxtIndex(0x581c);
													if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,3,0,0x581c,0x3e01) != 2) {
														fadeScreenAndResetActor(DAT_80072de8);
														closeUpShotOnActor(0xd4, 0, 0x8a, 0); //close up chancellor
														//playSoundFromTxtIndex(0x5942);
														if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,4,0,0x5942,0xc01) != 2) {
															fadeScreenAndResetActor(DAT_80072de8);
															closeUpShotOnActor(0xd3, 2, 0x87, 0); // close up king
															//playSoundFromTxtIndex(0x5aaa);
															if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,7,2,0x5aaa,0x2e01) != 2) {
																fadeScreenAndResetActor(DAT_80072de8);
																FUN_8003d8e8(0xd6,0,0x37a,0x280);
																DAT_80072dec = _vm->_actorManager->loadActor(0xd5,0,0x2d6,0xc6,3);
																_vm->waitForFramesAllowSkip(4);
																// call_fade_related_1f();
																//playSoundFromTxtIndex(0x5afc);
																if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,3,0,0x5afc,0x3c01) != 2) {
																	fadeScreenAndResetActor(DAT_80072de8);
																	fadeScreenAndResetActor(DAT_80072dec);
																	DAT_80063514 = DAT_80063514 & 0xffbf;
																	closeUpKnightsAtTable(); // close up of knights at table.
																	_vm->playOrStopSound(0x8003);
																	//playSoundFromTxtIndex(0x5b96);
																	if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,8,4,0x5b96,0xc01) != 2) {
																		//playSoundFromTxtIndex(0x5c4a);
																		if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,2,0,0x5c4a,0x2e01) != 2) {
																			//playSoundFromTxtIndex(0x5dc8);
																			DAT_80072df0->updateSequence(0xf);
																			DAT_80072df4->updateSequence(0xd);
																			DAT_80072df8->updateSequence(0xe);
																			if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,0xc,4,0x5dc8,0xc01) != 2) {
																				DAT_80072df0->updateSequence(6);
																				DAT_80072df4->updateSequence(0);
																				DAT_80072df8->updateSequence(2);
																				FUN_8003d8e8(0xd3,2,0x28d,0x250);
																				DAT_80072dec = _vm->_actorManager->loadActor(0xd7,0,0x348,199,3);
																				_vm->waitForFramesAllowSkip(4); // close up of king and flame
																				//call_fade_related_1f();
																				//playSoundFromTxtIndex(0x5ef2);
																				if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,4,0,0x5ef2,0x3e01) != 2) {
																					fadeScreenAndResetActor(DAT_80072de8);
																					fadeScreenAndResetActor(DAT_80072dec);
																					closeUpShotOnActor(0xd3, 0, 0x233,
																									   0x17a); // close up of king
																					//playSoundFromTxtIndex(0x6000);
																					if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,8,0,0x6000,0x2e01) != 2) {
																						fadeScreenAndResetActor(DAT_80072de8);
																						closeUpKnightsAtTable(); // close up knights at table
																						//playSoundFromTxtIndex(0x7dcc);
																						DAT_80072df0->updateSequence(0x13);
																						DAT_80072df4->updateSequence(0x10);
																						DAT_80072df8->updateSequence(0x11);
																						if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,0x12,6,0x7dcc,0xc01) != 2) {
																							wideAngleEveryoneAtTable(); //whole room shot
																							if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,0x17,0x16,0x60ee,0x701) != 2) {
																								closeUpKnightsAtTable();
																								DAT_80072df0->updateSequence(6);
																								DAT_80072df4->updateSequence(0);
																								DAT_80072df8->updateSequence(2);
																								DAT_80072dec->updateSequence(4);
																								//playSoundFromTxtIndex(0x5de8);
																								if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072df8,0xb,2,0x5de8,0xc01) != 2) {
																									wideAngleEveryoneAtTable();
																									_vm->playOrStopSound(1);
																									DAT_80072df0->updateSequence(8);
																									DAT_80072df4->updateSequence(0xd);
																									DAT_80072df8->updateSequence(0x11);
																									DAT_80072dfc->updateSequence(0x15);
																									DAT_80072df0->waitUntilFlag8And4AreSet();
																									DAT_80072df4->waitUntilFlag8And4AreSet();
																									DAT_80072df8->waitUntilFlag8And4AreSet();
																									DAT_80072dfc->waitUntilFlag8And4AreSet();
																									// DisableVSyncEvent();
																									// TODO load_actor_file(0x82);
																									// EnableVSyncEvent();
																									_vm->waitForFramesAllowSkip(0x3b);
																									_vm->clearFlags(ENGINE_FLAG_20000);
																									// fade_related_calls_with_1f();
																									// DisableVSyncEvent();
//TODO
//																									actorId_arg = 2;
//																									puVar4 = scrData_offset_200 + 4;
//																									do {
//																										*puVar4 = 0xffff;
//																										puVar4[1] = 0xffff;
//																										puVar3 = scrData_offset_200;
//																										actorId_arg = actorId_arg + 1;
//																										puVar4 = puVar4 + 2;
//																									} while (actorId_arg < 0x20);
//																									sVar9 = 0xe;
//																									scrData_offset_200[1] = 0x11;
//																									puVar3[2] = 199;
//																									*puVar3 = 0;
//																									puVar3[3] = 1;

																									// EnableVSyncEvent();
																									changeBackgroundPosition(
																											2, 0);
																									_vm->_actorManager->clearActorFlags(2);
																									DAT_80072de8 = _vm->_actorManager->loadActor(0x82,0,0x60,0x114,1);
																									DAT_80072dec = _vm->_actorManager->loadActor(0x82,2,0x91,0x113,1);
																									DAT_80072df0 = _vm->_actorManager->loadActor(0x82,1,0xd0,199,1);
																									DAT_80072df4 = _vm->_actorManager->loadActor(0x82,3,0xb6,0x113,1);
																									DAT_80072de8 = _vm->_actorManager->loadActor(0x82,4,0x98,0x40,1);

																									DAT_80072de8->setFlag(ACTOR_FLAG_100);
																									DAT_80072de8->setFlag(ACTOR_FLAG_800);
																									DAT_80072de8->setFlag(ACTOR_FLAG_8000);
																									DAT_80072de8->field_7c = 0x20000;
																									DAT_80072de8->priorityLayer = 3;

																									DAT_80072dec->setFlag(ACTOR_FLAG_100);
																									DAT_80072dec->setFlag(ACTOR_FLAG_800);
																									DAT_80072dec->setFlag(ACTOR_FLAG_8000);
																									DAT_80072dec->field_7c = 0x18000;
																									DAT_80072dec->priorityLayer = 3;

																									DAT_80072df0->setFlag(ACTOR_FLAG_100);
																									DAT_80072df0->setFlag(ACTOR_FLAG_800);
																									DAT_80072df0->setFlag(ACTOR_FLAG_8000);
																									DAT_80072df0->field_7c = 0x14000;
																									DAT_80072df0->priorityLayer = 3;

																									DAT_80072df4->setFlag(ACTOR_FLAG_100);
																									DAT_80072df4->setFlag(ACTOR_FLAG_800);
																									DAT_80072df4->setFlag(ACTOR_FLAG_8000);
																									DAT_80072df4->field_7c = 0x1c000;
																									DAT_80072df4->priorityLayer = 3;

																									DAT_80072df8->setFlag(ACTOR_FLAG_100);
																									DAT_80072df8->setFlag(ACTOR_FLAG_800);
																									DAT_80072df8->setFlag(ACTOR_FLAG_8000);
																									DAT_80072df8->priorityLayer = 3;

																									_vm->waitForFramesAllowSkip(0xe);
																									// call_fade_related_1f();
																									DAT_80072df0->pathfinding_maybe(0xe8,0xa8,2);
																									while (DAT_80072df0->isFlagSet(ACTOR_FLAG_10)) {
																										_vm->waitForFrames(1);
																									}
																									DAT_80072de8->pathfinding_maybe(0x97,0x37,2);
																									DAT_80072dec->pathfinding_maybe(0x97,0x37,2);
																									DAT_80072df4->pathfinding_maybe(0x97,0x37,2);
																									DAT_80072df0->waitUntilFlag8SetThenSet1000AndWaitFor4();
																									DAT_80072df0->updateSequence(6);
																									uint16 dialog[2000];
																									dialog[0] = 0;
																									_vm->_talk->loadText(0x5ea2, dialog, 2000);

																									_vm->_talk->displayDialogAroundPoint(dialog,0x27,0xc,0xc01,0,0x5ea2);
																									DAT_80072df0->waitUntilFlag8And4AreSet();
																									DAT_80072df0->x_pos = 0xcf;
																									DAT_80072df0->y_pos = 0x90;
																									DAT_80072df0->pathfinding_maybe(0x97, 0x37, 2);
																									DAT_80072df0->updateSequence(7);
//TODO
//																									if (((DAT_8008e7e8 != 0) || (DAT_8008e848 != 0))
//																										|| ((DAT_8008e844 != 0 || (DAT_8008e874 != 0)))
//																											) {
//																										FUN_8001a7c4((uint)DAT_8008e7e8,
//																													 (uint)DAT_8008e844,
//																													 (uint)DAT_8008e848,
//																													 (uint)DAT_8008e874);
//																									}

																									dialog[0] = 0;
																									_vm->_talk->loadText(0x5ecc, dialog, 2000);
																									_vm->_talk->displayDialogAroundPoint(dialog,0x14,6,0xc01,0,0x5ecc);
																									_vm->waitForFrames(0x3c);
																								}
																							}
																						}
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	// fade_related_calls_with_1f();
	_vm->clearFlags(ENGINE_FLAG_20000);
	// DisableVSyncEvent();
	//file_read_to_buffer(s_cursor.act_80011c44,actor_dictionary);
	// EnableVSyncEvent();
	if (isFlag10Set) {
		_vm->setFlags(ENGINE_FLAG_10);
	} else {
		_vm->clearFlags(ENGINE_FLAG_10);
	}

	_vm->_dragonINIResource->setFlickerRecord(flicker);
	cursorInventoryClearFlag400();
	_vm->clearUnkFlags(ENGINE_UNK1_FLAG_2);

}

//fadeScreenAndResetActor
void CutScene::fadeScreenAndResetActor(Actor *actor) {
	// TODO fade_related_calls_with_1f();
	//DisableVSyncEvent();
	actor->reset_maybe();
	//EnableVSyncEvent();

}

void CutScene::closeUpShotOnActor(uint16 resourceId, uint16 sequenceId, int16 x, uint32 param_4) {
	FUN_8003d8e8(resourceId, sequenceId, x, param_4);
	_vm->waitForFrames(5);
	// TODO call_fade_related_1f();
}


void CutScene::FUN_8003d8e8(uint16 resourceId, uint16 sequenceId, int16 x, uint32 param_4) {
	// fade_related_calls_with_1f();
	_vm->_actorManager->clearActorFlags(2);
	//DisableVSyncEvent();
	DAT_80072de8 = _vm->_actorManager->loadActor(resourceId, sequenceId, x, 199, 3);
	//EnableVSyncEvent();
	changeBackgroundPosition(3, param_4);

}

void CutScene::wideAngleEveryoneAtTable() {
	FUN_8003d388();
	// call_fade_related_1f();

}

void CutScene::FUN_8003d388() {
	uint sequenceId;

	// fade_related_calls_with_1f();
	_vm->_actorManager->clearActorFlags(2);
	if ((DAT_80063514 & 0x80) == 0) {
		DAT_80072de8 = _vm->_actorManager->loadActor(0x7e,0x16,0x40,0xa0,1);
	}
	DAT_80072dec = _vm->_actorManager->loadActor(0x7e,0,0xbf,0xba,1);
	if ((DAT_80063514 & 8) == 0) {
		DAT_80072df0 = _vm->_actorManager->loadActor(0x7e,5,0x94,0x82,1);
	}
	if ((DAT_80063514 & 0x10) == 0) {
		DAT_80072df4 = _vm->_actorManager->loadActor(0x7e,10,0x6f,0x95,1);
	}
	if ((DAT_80063514 & 4) == 0) {
		DAT_80072df8 = _vm->_actorManager->loadActor(0x7e,0xe,0xa9,0x87,1);
	}
	if ((DAT_80063514 & 0x20) == 0) {
		DAT_80072dfc = _vm->_actorManager->loadActor(0x7e,0x12,0xcd,0x8e,1);
	}
	if ((DAT_80063514 & 1) == 0) {
		DAT_80072e04 = _vm->_actorManager->loadActor(0x7e,0x19,0x10e,0x89,1);
	}
	if ((DAT_80063514 & 2) == 0) {
		DAT_80072e08 = _vm->_actorManager->loadActor(0x8f,2,100,0xbc,1);
	}
	if ((DAT_80063514 & 0x40) != 0) {
		DAT_80072e0c = _vm->_actorManager->loadActor(0x8f,0,0xd2,100,1);
		DAT_800830a0 = _vm->_actorManager->loadActor(0x8f,1,0xe6,0x6e,1);
	}
	DAT_800830b8 = _vm->_actorManager->loadActor(0xaa,0,0x2e,0x2d,1);
	DAT_800830b8->setFlag(ACTOR_FLAG_8000);
	DAT_800830bc = _vm->_actorManager->loadActor(0xaa,1,0x115,0x22,1);
	DAT_800830bc->setFlag(ACTOR_FLAG_100);
	DAT_800830bc->setFlag(ACTOR_FLAG_8000);
	DAT_800830bc->priorityLayer = 4;
	if ((DAT_80063514 & 0x100) != 0) {
		DAT_800830c0 = _vm->_actorManager->loadActor(0x7e,0x1c,0x21,0x87,1);
	}
	if ((DAT_80063514 & 0x200) != 0) {
		if ((DAT_80063514 & 0x800) == 0) {
			sequenceId = 2;
		}
		else {
			sequenceId = 4;
		}
		DAT_800830d4 = _vm->_actorManager->loadActor(0xaa,sequenceId,0xf4,199,1);
		DAT_800830d4->setFlag(ACTOR_FLAG_8000);
	}
	if ((DAT_80063514 & 0x400) != 0) {
		DAT_800830dc = _vm->_actorManager->loadActor(0xaa,3,0xf4,199,1);
		DAT_800830dc->setFlag(ACTOR_FLAG_8000);
	}
	changeBackgroundPosition(0, 0);
	_vm->waitForFramesAllowSkip(0xe);
}


void CutScene::closeUpKnightsAtTable() {
	//fade_related_calls_with_1f();
	_vm->_actorManager->clearActorFlags(2);
	DAT_80072de8 = _vm->_actorManager->loadActor(0x7d,0,2,199,1);
	DAT_80072dec = _vm->_actorManager->loadActor(0x81,4,2,199,1);
	DAT_80072df0 = _vm->_actorManager->loadActor(0x81,6,2,199,1);
	DAT_80072df4 = _vm->_actorManager->loadActor(0x81,0,2,199,1);
	DAT_80072df8 = _vm->_actorManager->loadActor(0x81,2,2,199,1);
	changeBackgroundPosition(1, 0);
	_vm->waitForFrames(0xf);
	//call_fade_related_1f();

}

uint16 CutScene::FUN_8003dab8(uint32 textId,uint16 x,uint16 y,uint16 param_4,int16 param_5)
{
	uint16 dialog[2000];
	dialog[0] = 0;
	_vm->_talk->loadText(textId, dialog, 2000);

	_vm->_talk->displayDialogAroundPoint(dialog,x,y,param_4,param_5,textId);
	return 1; //TODO this should return (uint)dialogText & 0xffff;
}

void CutScene::cursorInventoryClearFlag400() {
	_vm->_cursor->clearActorFlag400();
	_vm->_inventory->clearActorFlag400();
}

void CutScene::changeBackgroundPosition(uint16 newPosition, int16 sParm2)
{
	if (newPosition == 1) {
		_vm->_scene->setBgLayerPriority(0); //TODO investigate why this is 0 not 1
		_vm->_scene->setMgLayerPriority(1); //TODO investigate why this is 1 not 2
		_vm->_scene->_camera.x = sParm2 + 0x3c0;
		_vm->_scene->setFgLayerPriority(0);
		_vm->_screen->loadPalette(0, _palettes + 2 * 512);
		for (int i = 2; i < 0x17; i++) {
			Actor *actor = _vm->_actorManager->getActor(i);
			actor->x_pos += 0x3c0;
		}
	}
	else {
		if (newPosition < 2) {
			if (newPosition != 0) {
				return;
			}
			_vm->_screen->loadPalette(0, _palettes + 0 * 512);
			_vm->_scene->setMgLayerPriority(0);
			_vm->_scene->setFgLayerPriority(0);
			_vm->_scene->_camera.x = 0;
		}
		else {
			if (newPosition == 2) {
				_vm->_screen->loadPalette(0, _palettes + 3 * 512);
				_vm->_scene->setMgLayerPriority(2);
				_vm->_scene->setFgLayerPriority(3);
				_vm->_scene->_camera.x = 0;
			}
			else {
				if (newPosition != 3) {
					return;
				}
				_vm->_screen->loadPalette(0, _palettes + 1 * 512);
				_vm->_scene->setMgLayerPriority(2);
				_vm->_scene->setFgLayerPriority(0);
				_vm->_scene->_camera.x = sParm2;
			}
		}
		_vm->_scene->setBgLayerPriority(1);
	}
}

void CutScene::diamondScene() {
	Actor *actorId;
	Actor *actorId_00;
	Actor *actorId_01;
	Actor *actorId_02;
	Actor *actorId_03;
	byte palette[512];

	_vm->setUnkFlags(ENGINE_UNK1_FLAG_2);
	actorId = _vm->getINI(0x257)->actor;
	actorId_03 = _vm->getINI(0x259)->actor;
	actorId_01 = _vm->getINI(0x258)->actor;
	actorId_03->setFlag(ACTOR_FLAG_100);
	actorId_03->priorityLayer = 4;
	actorId_00 = _vm->getINI(0x256)->actor;
	_vm->setFlags(ENGINE_FLAG_20000);
	actorId_02 = _vm->getINI(0x25a)->actor;
	if ((_vm->_talk->somethingTextAndSpeechAndAnimRelated(actorId_02,1,0,0x4294a,0x2601) != 2) && !actorId->actorSetSequenceAndWaitAllowSkip(2)) {
		actorId->updateSequence(3);
		if (!actorId_01->actorSetSequenceAndWaitAllowSkip(0x18)) {
			_vm->waitForFramesAllowSkip(0x2c);
			//TODO fade_related_calls_with_1f();
			memcpy(palette, _vm->_scene->getPalette(), 512);
			_vm->_screen->loadPalette(0, actorId_00->_actorResource->getPalette());
			_vm->_scene->_camera.x = 0x140;
			//TODO call_fade_related_1f();
			if (!actorId_00->actorSetSequenceAndWaitAllowSkip(0)) {
				// TODO is this needed playSoundFromTxtIndex(0x42A66);
				if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(actorId_00,1,2,0x42a66,0x3c01) != 2) {
					_vm->waitForFramesAllowSkip(0x13);
					//TODO fade_related_calls_with_1f();
					_vm->_screen->loadPalette(0, palette);
					_vm->_scene->_camera.x = 0;
					//TODO call_fade_related_1f();
					actorId_01->updateSequence(0x19);
					_vm->waitForFramesAllowSkip(0xf);
					actorId->updateSequence(4);
					_vm->waitForFramesAllowSkip(0x17);
					actorId_03->updateSequence(9);
					actorId_03->x_pos = 0x82;
					actorId_03->y_pos = 0xc4;
					actorId_03->priorityLayer = 4;
					if (!actorId->waitUntilFlag4IsSetAllowSkip()) {
						actorId->updateSequence(5);
						if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(actorId_01,0x10,2,0x42ac2,0x3c01) != 2 &&
							_vm->_talk->somethingTextAndSpeechAndAnimRelated(actorId_02,1,0,0x42b56,0x2601) != 2) {
							_vm->waitForFramesAllowSkip(0x3b);
						}
					}
				}
			}
		}
	}
	_vm->clearUnkFlags(ENGINE_UNK1_FLAG_2);
	_vm->clearFlags(ENGINE_FLAG_20000);
}

void CutScene::knightsSavedBackAtCastle() {
	DragonINI *uVar1;
	uint actorId;
	bool isFlag0x10Set;

	uVar1 = _vm->_dragonINIResource->getFlickerRecord();
	DAT_80063514 = 0xa00;
	_vm->_dragonINIResource->setFlickerRecord(NULL);
	_vm->setUnkFlags(ENGINE_UNK1_FLAG_2);
	isFlag0x10Set = _vm->isFlagSet(ENGINE_FLAG_10);
	//TODO fade_related_calls_with_1f();
	_vm->clearFlags(ENGINE_FLAG_10);
	_vm->_inventory->setActorFlag400();
	_vm->_cursor->setActorFlag400();

//	scr_tilemap1_w = 0x28;
//	DAT_8006a3f0 = DAT_8006a3ec;
//	load_actor_file(0x81);
//	load_actor_file(0x7d);
//	load_actor_file(0x7e);
//	load_actor_file(0x8f);
//	load_actor_file(0xaa);

	_vm->setFlags(ENGINE_FLAG_20000);
	closeUpKnightsAtTable();
//	playSoundFromTxtIndex(0x7854);
	if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,10,4,0x7854,0xc01) != 2) {
		closeUpShotOnActor(0xd8,0,0xfd,0x60);
//		playSoundFromTxtIndex(0x78c6);
		if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,4,0,0x78c6,0x701) != 2) {
			fadeScreenAndResetActor(DAT_80072de8);
			closeUpKnightsAtTable();
//			playSoundFromTxtIndex(0x78e8);
			if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,3,0,0x78e8,0x2e01) != 2) {
				wideAngleEveryoneAtTable();
				if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072e04,0x1a,0x19,0x7a1e,0x3e01) != 2) {
					DAT_80072e04->field_7c = 0x10000;
					DAT_80072e04->setFlag(ACTOR_FLAG_800);
					DAT_80072e04->updateSequence(0x21);
					DAT_80072e04->pathfinding_maybe(0x13f,0x6e,2);
					// wait for pathfinding to complete
					while (DAT_80072e04->isFlagSet(ACTOR_FLAG_10)) { //TODO move to method on Actor waitForPathfinding() ??
						//empty
					}
					DAT_80063514 = DAT_80063514 | 1;
					closeUpShotOnActor(0xd3,0,0x233,0x17a);
//					playSoundFromTxtIndex(0x7aba);
					if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,9,0,0x7aba,0x2e01) != 2) {
						fadeScreenAndResetActor(DAT_80072de8);
						closeUpShotOnActor(0xd8,0,0xfd,0x60);
//						playSoundFromTxtIndex(0x7b60);
						if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,5,0,0x7b60,0x701) != 2) {
							fadeScreenAndResetActor(DAT_80072de8);
							wideAngleEveryoneAtTable();
							if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072df0,9,5,0x7c20,0xc01) != 2) {
								if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,0x17,0x16,0x7c9c,0x701) != 2) {
									_vm->playOrStopSound(0x800f);
									_vm->waitForFrames(10);
									if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,0x17,0x16,0x7cf2,0x701) != 2) {
										closeUpKnightsAtTable();
//										playSoundFromTxtIndex(0x7dcc);
										DAT_80072df0->updateSequence(0x13);
										DAT_80072df4->updateSequence(0x10);
										DAT_80072df8->updateSequence(0x11);
										actorId = _vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,0x12,6,0x7dcc,0xc01);
										if ((actorId & 0xffff) != 2) {
											closeUpShotOnActor(0xd3,0,0x233,0x17a);
//											playSoundFromTxtIndex(0x7e1a);
											if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,10,0,0x7e1a,0x2e01) != 2) {
												fadeScreenAndResetActor(DAT_80072de8);
												wideAngleEveryoneAtTable();
												if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,4,0,0x7e96,0x2e01) != 2) {
													_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,0x17,0x16,0x7f0a,0x701);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	// fade_related_calls_with_1f();
	//DisableVSyncEvent();
//	file_read_to_buffer(s_cursor.act_80011c44,actor_dictionary);
	//EnableVSyncEvent();
	if(isFlag0x10Set) {
		_vm->setFlags(ENGINE_FLAG_10);
	}
	_vm->clearFlags(ENGINE_FLAG_20000);
	_vm->_dragonINIResource->setFlickerRecord(uVar1);
	cursorInventoryClearFlag400();
	_vm->clearUnkFlags(ENGINE_UNK1_FLAG_2);
}

void CutScene::loadPalettes() {
	Common::File fd;
	if (!fd.open("dragon.exe")) {
		error("Failed to open dragon.exe");
	}
	fd.seek(0x5336c); //TODO handle other game variants

	_palettes = (byte *)malloc(256 * 2 * 4);
	fd.read(_palettes, 256 * 2 * 4);
}

void CutScene::flameReturnsCutScene() {
	DragonINI *uVar1;
	uint engineFlag10Set;

	uVar1 = _vm->_dragonINIResource->getFlickerRecord();
	DAT_80063514 = 0x3f;
	_vm->_dragonINIResource->setFlickerRecord(NULL);
	_vm->setUnkFlags(ENGINE_UNK1_FLAG_2);
	engineFlag10Set = _vm->isFlagSet(ENGINE_FLAG_10);
	//fade_related_calls_with_1f();
	_vm->clearFlags(ENGINE_FLAG_10);
	_vm->_cursor->setActorFlag400();
	_vm->_inventory->setActorFlag400();
//	scr_tilemap1_w = 0x28;
//	DAT_8006a3f0 = DAT_8006a3ec;
//	load_actor_file(0x81);
//	load_actor_file(0x7d);
//	load_actor_file(0x7e);
//	load_actor_file(0x8f);
//	load_actor_file(0xaa);
	DAT_80063514 = (DAT_80063514 & 0xfffe) | 0x600;
	FUN_8003d388();
	DAT_80072de8->updateSequence(0x1f);
	DAT_80072e04->x_pos = 0x10b;
	DAT_80072e04->y_pos = 99;
	DAT_80072de8->x_pos = 0x10a;
	DAT_80072de8->y_pos = 0x5a;
	DAT_80072de8->field_7c = 0x10000;
	DAT_80072e04->field_7c = 0x10000;
	DAT_80072de8->setFlag(ACTOR_FLAG_800);
	DAT_80072e04->setFlag(ACTOR_FLAG_800);
	//	call_fade_related_1f();
	_vm->setFlags(ENGINE_FLAG_20000);
	if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,4,0,0x8ab2,0x2e01) != 2) {
		DAT_80072de8->updateSequence(0x1e);
		DAT_80072de8->pathfinding_maybe(0xb0,0x6b,2);
		do {
		} while (DAT_80072de8->isFlagSet(ACTOR_FLAG_10));
		DAT_80072de8->updateSequence(0x1f);
		if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,4,0,0x8b40,0x2e01) != 2) {
			DAT_80072e04->updateSequence(0x1b);
			DAT_80072e04->pathfinding_maybe(0xd5,0x6b,2);
			do {
			} while (DAT_80072e04->isFlagSet(ACTOR_FLAG_10));
			if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072e04,0x1a,0x19,0x8bb6,0x3e01) != 2) {
				if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,4,0,0x8bd8,0x2e01) != 2) {
					closeUpShotOnActor(0xd8,0,0xfd,0x60);
//					playSoundFromTxtIndex(0x8c70);
					if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,7,0,0x8c70,0x701) != 2) {
						fadeScreenAndResetActor(DAT_80072de8);
						closeUpShotOnActor(0xd3,0,0x233,0x17a);
//						playSoundFromTxtIndex(0x8cd2);
						if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,0xc,0,0x8cd2,0x2e01) != 2) {
							fadeScreenAndResetActor(DAT_80072de8);
							closeUpShotOnActor(0xd7,0,0x312,0x260);
//							playSoundFromTxtIndex(0x8e1e);
							if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,5,0,0x8e1e,0x3e01) != 2) {
								fadeScreenAndResetActor(DAT_80072de8);
							}
						}
					}
				}
			}
		}
	}
//	fade_related_calls_with_1f();
//	DisableVSyncEvent();
//	file_read_to_buffer(s_cursor.act_80011c44,actor_dictionary);
//	EnableVSyncEvent();
	_vm->clearFlags(ENGINE_FLAG_20000);
	if(engineFlag10Set) {
		_vm->setFlags(ENGINE_FLAG_10);
	}
	_vm->_dragonINIResource->setFlickerRecord(uVar1);
	cursorInventoryClearFlag400();
	_vm->clearUnkFlags(ENGINE_UNK1_FLAG_2);
}

void CutScene::knightsSavedAgain() {
	DragonINI *flicker;
	bool engineFlag10Set;

	flicker = _vm->_dragonINIResource->getFlickerRecord();
	DAT_80063514 = 0;
	_vm->_dragonINIResource->setFlickerRecord(NULL);
	_vm->setUnkFlags(ENGINE_UNK1_FLAG_2);
	engineFlag10Set = _vm->isFlagSet(ENGINE_FLAG_10);
	//fade_related_calls_with_1f();

	_vm->clearFlags(ENGINE_FLAG_10);
	_vm->_cursor->setActorFlag400();
	_vm->_inventory->setActorFlag400();

//	scr_tilemap1_w = 0x28;
//TODO what is this?	DAT_8006a3f0 = DAT_8006a3ec;

//	load_actor_file(0x81);
//	load_actor_file(0x7d);
//	load_actor_file(0x7e);
//	load_actor_file(0x8f);
//	load_actor_file(0xaa);
	wideAngleEveryoneAtTable();
	_vm->setFlags(ENGINE_FLAG_20000);
	_vm->waitForFramesAllowSkip(0x3b);
	closeUpShotOnActor(0xd3,0,0x233,0x17a);
	//playSoundFromTxtIndex(0x9000);
	if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,0xd,0,0x9000,0x2e01) != 2) {
		fadeScreenAndResetActor(DAT_80072de8);
		closeUpKnightsAtTable();
//		playSoundFromTxtIndex(0x90de);
		if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,4,0,0x90de,0x2e01) != 2) {
			closeUpShotOnActor(0xd8,0,0xfd,0x60);
//			playSoundFromTxtIndex(0x921c);
			if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,8,0,0x921c,0x701) != 2) {
				fadeScreenAndResetActor(DAT_80072de8);
				closeUpKnightsAtTable();
//				playSoundFromTxtIndex(0x92aa);
				if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,9,4,0x92aa,0xc01) != 2) {
					closeUpShotOnActor(0xd7,0,0x312,0x260);
//					playSoundFromTxtIndex(0x932c);
					if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,6,0,0x932c,0x3e01) != 2) {
						fadeScreenAndResetActor(DAT_80072de8);
						closeUpShotOnActor(0xd3,2,0x87,0);
//						playSoundFromTxtIndex(0x93d6);
						if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,0xe,2,0x93d6,0x2e01) != 2) {
							fadeScreenAndResetActor(DAT_80072de8);
							closeUpKnightsAtTable();
//							playSoundFromTxtIndex(0x7dcc);
							DAT_80072df0->updateSequence(0x13);
							DAT_80072df4->updateSequence(0x10);
							DAT_80072df8->updateSequence(0x11);
							if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072dec,0x12,6,0x7dcc,0xc01) != 2) {
								closeUpShotOnActor(0xd8,0,0xfd,0x60);
//								playSoundFromTxtIndex(0x948c);
								if (_vm->_talk->somethingTextAndSpeechAndAnimRelated(DAT_80072de8,9,0,0x948c,0x701) != 2) {
									fadeScreenAndResetActor(DAT_80072de8);
									_vm->waitForFramesAllowSkip(0x3b);
								}
							}
						}
					}
				}
			}
		}
	}
	//fade_related_calls_with_1f();
//	DisableVSyncEvent();
//	file_read_to_buffer(s_cursor.act_80011c44,actor_dictionary);
//	EnableVSyncEvent();
	_vm->clearFlags(ENGINE_FLAG_20000);
	if(engineFlag10Set) {
		_vm->setFlags(ENGINE_FLAG_10);
	}
	_vm->_dragonINIResource->setFlickerRecord(flicker);
	cursorInventoryClearFlag400();
	_vm->clearUnkFlags(ENGINE_UNK1_FLAG_2);
}

static uint16 tournamentUpdateCameraX = 0;

void tournamentUpdateFunction() {
	tournamentUpdateCameraX++;
	if (tournamentUpdateCameraX > 0x280) {
		return;
	}
	getEngine()->_scene->_camera.x = tournamentUpdateCameraX;
}

void CutScene::tournamentCutScene() {
	uint actorId;
	ushort uVar1;
	uint16 dialogText[1000];
//	undefined2 local_218 [256];

	uVar1 = 0;
	actorId = 0;
//	do {
//		uVar1 = uVar1 + 1;
//		local_218[actorId] = *(undefined2 *)(actorId * 2 + scrFileData_maybe);
//		actorId = (uint)uVar1;
//	} while (uVar1 < 0x100);

	tournamentUpdateCameraX = 0x140;
	_vm->setVsyncUpdateFunction(tournamentUpdateFunction);
	_vm->_talk->loadText(0x4C40C, dialogText, 1000);
	_vm->_talk->displayDialogAroundPoint(dialogText,0,0,0x1e01,1,0x4C40C);

	_vm->_talk->loadText(0x4C530, dialogText, 1000);
	_vm->_talk->displayDialogAroundPoint(dialogText,0,0,0xc01,1,0x4C530);

	_vm->_talk->loadText(0x4C588, dialogText, 1000);
	_vm->_talk->displayDialogAroundPoint(dialogText,0,0,0x1e01,1, 0x4C588);

	_vm->_talk->loadText(0x4C6B0, dialogText, 1000);
	_vm->_talk->displayDialogAroundPoint(dialogText,0,0,0xc01,1, 0x4C6B0);

	_vm->_talk->loadText(0x4C6E8, dialogText, 1000);
	_vm->_talk->displayDialogAroundPoint(dialogText,0,0,0x1e01,1, 0x4C6E8);
	_vm->setVsyncUpdateFunction(NULL);
	_vm->setFlags(ENGINE_FLAG_20000);
	// fade_related_calls_with_1f();
	Actor *actor = _vm->_dragonINIResource->getRecord(0x02BE)->actor;
	_vm->_screen->loadPalette(0, actor->_actorResource->getPalette());
	_vm->_scene->_camera.x = 0;
	_vm->playOrStopSound(0);
//	call_fade_related_1f();
	_vm->waitForFrames(300);
	actor->setFlag(ACTOR_FLAG_1000);
	actor->waitUntilFlag8And4AreSet();
	_vm->waitForFrames(0x3c);
//	fade_related_calls_with_1f();
	_vm->_screen->loadPalette(0, _vm->_scene->getPalette());
	_vm->playOrStopSound(0x4000);
	_vm->_scene->_camera.x = 0x3c0;
//	call_fade_related_1f();
	_vm->_talk->loadText(0x4C814, dialogText, 1000);
	_vm->_talk->displayDialogAroundPoint(dialogText,0,0,0xc01,1, 0x4C814);
	_vm->_talk->loadText(0x4C852, dialogText, 1000);
	_vm->_talk->displayDialogAroundPoint(dialogText,0,0,0x1e01,1, 0x4C852);
	_vm->setFlags(ENGINE_FLAG_20000);
//	fade_related_calls_with_1f();
}


} // End of namespace Dragons
