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
* $URL$
* $Id$
*
*/

#include "backends/keymapper/keymapper.h"
#include "common/config-manager.h"
namespace Common {

void Keymapper::Domain::addKeymap(Keymap *map) {
	KeymapMap::iterator it = _keymaps.find(map->getName());
	if (it != _keymaps.end())
		delete _keymaps[map->getName()];
	_keymaps[map->getName()] = map;
}

void Keymapper::Domain::deleteAllKeyMaps() {
	KeymapMap::iterator it;
	for (it = _keymaps.begin(); it != _keymaps.end(); it++)
		delete it->_value;
	_keymaps.clear();
}

Keymap *Keymapper::Domain::getKeymap(const String& name) {
	KeymapMap::iterator it = _keymaps.find(name);
	if (it != _keymaps.end())
		return it->_value;
	else
		return 0;
}

Keymapper::Keymapper(EventManager *evtMgr)
	: _eventMan(evtMgr), _enabled(true), _hardwareKeys(0) {
	_globalDomain.setConfigDomain(ConfMan.getDomain(ConfigManager::kApplicationDomain));
}

Keymapper::~Keymapper() {
	delete _hardwareKeys;
}

void Keymapper::registerHardwareKeySet(HardwareKeySet *keys) {
	if (_hardwareKeys)
		error("Hardware key set already registered!");
	_hardwareKeys = keys;
}

void Keymapper::addGlobalKeymap(Keymap *keymap) {
	initKeymap(_globalDomain.getConfigDomain(), keymap);
	_globalDomain.addKeymap(keymap);
}

void Keymapper::refreshGameDomain() {
	if (_gameDomain.getConfigDomain() != ConfMan.getActiveDomain()) {
		cleanupGameKeymaps();
		_gameDomain.setConfigDomain(ConfMan.getActiveDomain());
	}
}

void Keymapper::addGameKeymap(Keymap *keymap) {
	if (ConfMan.getActiveDomain() == 0)
		error("Call to Keymapper::addGameKeymap when no game loaded");
		
	refreshGameDomain();
	initKeymap(_gameDomain.getConfigDomain(), keymap);
	_gameDomain.addKeymap(keymap);
}

void Keymapper::initKeymap(ConfigManager::Domain *domain, Keymap *map) {
	map->setConfigDomain(domain);
	map->loadMappings(_hardwareKeys);
	if (map->isComplete(_hardwareKeys) == false) {
		map->automaticMapping(_hardwareKeys);
		map->saveMappings();
		ConfMan.flushToDisk();
	}
}

void Keymapper::cleanupGameKeymaps() {
	_gameDomain.deleteAllKeyMaps();
	Stack<MapRecord> newStack;
	for (int i = 0; i < _activeMaps.size(); i++) {
		if (!_activeMaps[i].global)
			newStack.push(_activeMaps[i]);
	}
	_activeMaps = newStack;
}

Keymap *Keymapper::getKeymap(const String& name, bool &global) {
	Keymap *keymap = _gameDomain.getKeymap(name);
	global = false;
	if (!keymap) {
		keymap = _globalDomain.getKeymap(name);
		global = true;
	}
	return keymap;
}

bool Keymapper::pushKeymap(const String& name, bool inherit) {
	bool global;
	Keymap *newMap = getKeymap(name, global);
	if (!newMap) {
		warning("Keymap '%s' not registered", name.c_str());
		return false;
	}
	pushKeymap(newMap, inherit, global);
	return true;
}

void Keymapper::pushKeymap(Keymap *newMap, bool inherit, bool global) {
	MapRecord mr = {newMap, inherit, global};
	_activeMaps.push(mr);
}

void Keymapper::popKeymap() {
	if (!_activeMaps.empty())
		_activeMaps.pop();
}

bool Keymapper::mapKeyDown(const KeyState& key) {
	return mapKey(key, true);
}

bool Keymapper::mapKeyUp(const KeyState& key) {
	return mapKey(key, false);
}

bool Keymapper::mapKey(const KeyState& key, bool isKeyDown) {
	if (!_enabled) return false;
	if (_activeMaps.empty()) return false;

	Action *action = 0;
	for (int i = _activeMaps.size() - 1; !action && i >= 0; --i) {
		MapRecord mr = _activeMaps[i];
		action = mr.keymap->getMappedAction(key);
		if (mr.inherit == false) break;
	}
	if (!action) return false;

	List<Event>::iterator it;
	for (it = action->events.begin(); it != action->events.end(); ++it) {
		Event evt = *it;
		bool pushEvent = true;
		switch (evt.type) {
		case EVENT_KEYDOWN:
			if (!isKeyDown) evt.type = EVENT_KEYUP;
			break;
		case EVENT_KEYUP:
			if (isKeyDown) evt.type = EVENT_KEYDOWN;
			break;
		case EVENT_LBUTTONDOWN:
			if (!isKeyDown) evt.type = EVENT_LBUTTONUP;
			break;
		case EVENT_LBUTTONUP:
			if (isKeyDown) evt.type = EVENT_LBUTTONDOWN;
			break;
		case EVENT_RBUTTONDOWN:
			if (!isKeyDown) evt.type = EVENT_RBUTTONUP;
			break;
		case EVENT_RBUTTONUP:
			if (isKeyDown) evt.type = EVENT_RBUTTONDOWN;
			break;
		case EVENT_MBUTTONDOWN:
			if (!isKeyDown) evt.type = EVENT_MBUTTONUP;
			break;
		case EVENT_MBUTTONUP:
			if (isKeyDown) evt.type = EVENT_MBUTTONDOWN;
			break;
		default:
			// don't deliver other events on key up
			if (!isKeyDown) pushEvent = false;
		}
		if (pushEvent) _eventMan->pushEvent(evt);
	}
	return true;
}

const HardwareKey *Keymapper::getHardwareKey(const KeyState& key) {
	return (_hardwareKeys) ? _hardwareKeys->findHardwareKey(key) : 0;
}

} // end of namespace Common
