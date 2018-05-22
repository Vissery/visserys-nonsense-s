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

#include "pink/pink.h"
#include "pink/objects/module.h"
#include "pink/objects/pages/game_page.h"

namespace Pink {

ModuleProxy::ModuleProxy(const Common::String &name)
        : NamedObject(name)
{}

ModuleProxy::ModuleProxy() {}

Module::Module(PinkEngine *game, const Common::String &name)
        : NamedObject(name), _game(game), _page(nullptr)
{}

Module::~Module() {
    for (uint i = 0; i < _pages.size(); ++i) {
        delete _pages[i];
    }
}

void Module::load(Archive &archive){
    archive.mapObject(this);
    NamedObject::deserialize(archive);

    archive.readString(); // skip directory

    _invMgr.deserialize(archive);
    _pages.deserialize(archive);
}

void Module::init(bool isLoadingSave, const Common::String &pageName) {
    // 0 0  - new game
    // 0 1 - module changed
    // 1 0 - from save
    if (!pageName.empty())
        _page = findPage(pageName);
    else if (!_page)
        _page = _pages[0];

    _page->init(isLoadingSave);
}

void Module::changePage(const Common::String &pageName) {
    GamePage *page = nullptr;
    page = findPage(pageName);
    assert(_page != page);

    _page->unload();

    _page = page;
    _page->init(kLoadingNewGame);
}

GamePage *Module::findPage(const Common::String &pageName) const {
	for (uint i = 0; i < _pages.size(); ++i) {
		if (_pages[i]->getName() == pageName)
			return _pages[i];
	}
	return nullptr;
}

PinkEngine *Module::getGame() const {
    return _game;
}

bool Module::checkValueOfVariable(Common::String &variable, Common::String &value) {
    if (!_variables.contains(variable))
        return value == kUndefined;
    return _variables[variable] == value;
}

void Module::setVariable(Common::String &variable, Common::String &value) {
    _variables[variable] = value;
}

InventoryMgr *Module::getInventoryMgr() {
    return &_invMgr;
}

} // End of namespace Pink
