/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the AUTHORS
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

#include "engines/stark/ui/menu/settingsmenu.h"
#include "engines/stark/ui/cursor.h"
#include "engines/stark/services/services.h"
#include "engines/stark/services/userinterface.h"
#include "engines/stark/services/staticprovider.h"

#include "engines/stark/visual/image.h"

#include "engines/stark/resources/sound.h"

namespace Stark {

SettingsMenuScreen::SettingsMenuScreen(Gfx::Driver *gfx, Cursor *cursor) :
		StaticLocationScreen(gfx, cursor, "OptionLocation", Screen::kScreenSettingsMenu),
		_soundManager() {
}

SettingsMenuScreen::~SettingsMenuScreen() {
}

void SettingsMenuScreen::open() {
	StaticLocationScreen::open();

	_soundManager.load();

	_widgets.push_back(new StaticLocationWidget(
			// This is the background image
			"The Longest Journey",
			nullptr,
			nullptr));
	
	_widgets.push_back(new StaticLocationWidget(
			"Return",
			CLICK_HANDLER(SettingsMenuScreen, backHandler),
			nullptr));
	_widgets.back()->setupSounds(3, 4);
	
	_widgets.push_back(new StaticLocationWidget(
			"Back",
			CLICK_HANDLER(SettingsMenuScreen, backHandler),
			nullptr));
	_widgets.back()->setupSounds(3, 4);
	
	_widgets.push_back(new StaticLocationWidget(
			"GSettings",
			nullptr,
			nullptr));
	
	_widgets.push_back(new CheckboxWidget(
			"AprilHighRes",
			true,
			nullptr,
			MOVE_HANDLER(SettingsMenuScreen, textHandler<kHighRes>)));
	_widgets.back()->setupSounds(3, 4);
	
	_widgets.push_back(new StaticLocationWidget(
			"HighResHelp",
			nullptr,
			nullptr));
	_widgets.back()->setVisible(false);
	
	_widgets.push_back(new CheckboxWidget(
			"Subtitles",
			true,
			nullptr,
			MOVE_HANDLER(SettingsMenuScreen, textHandler<kSubtitles>)));
	_widgets.back()->setupSounds(3, 4);
	
	_widgets.push_back(new StaticLocationWidget(
			"SubtitleHelp",
			nullptr,
			nullptr));
	_widgets.back()->setVisible(false);

	_widgets.push_back(new CheckboxWidget(
			"SpecialFX",
			true,
			nullptr,
			MOVE_HANDLER(SettingsMenuScreen, textHandler<kSpecialFX>)));
	_widgets.back()->setupSounds(3, 4);
	
	_widgets.push_back(new StaticLocationWidget(
			"SpecialFXHelp",
			nullptr,
			nullptr));
	_widgets.back()->setVisible(false);

	_widgets.push_back(new CheckboxWidget(
			"Shadows",
			true,
			nullptr,
			MOVE_HANDLER(SettingsMenuScreen, textHandler<kShadows>)));
	_widgets.back()->setupSounds(3, 4);
	
	_widgets.push_back(new StaticLocationWidget(
			"ShadowsHelp",
			nullptr,
			nullptr));
	_widgets.back()->setVisible(false);

	_widgets.push_back(new CheckboxWidget(
			"HighResFMV",
			true,
			nullptr,
			MOVE_HANDLER(SettingsMenuScreen, textHandler<kHighResFMV>)));
	_widgets.back()->setupSounds(3, 4);
	
	_widgets.push_back(new StaticLocationWidget(
			"FMVHelp",
			nullptr,
			nullptr));
	_widgets.back()->setVisible(false);

	_widgets.push_back(new StaticLocationWidget(
			"VSettings",
			nullptr,
			nullptr));
	
	_widgets.push_back(new VolumeWidget(
			"Voice",
			_cursor,
			_soundManager, 0,
			MOVE_HANDLER(SettingsMenuScreen, textHandler<kVoice>)));
	
	_widgets.push_back(new StaticLocationWidget(
			"VoiceHelp",
			nullptr,
			nullptr));
	_widgets.back()->setVisible(false);

	_widgets.push_back(new StaticLocationWidget(
			"Music",
			nullptr,
			MOVE_HANDLER(SettingsMenuScreen, textHandler<kMusic>)));
	
	_widgets.push_back(new StaticLocationWidget(
			"MusicHelp",
			nullptr,
			nullptr));
	_widgets.back()->setVisible(false);

	_widgets.push_back(new StaticLocationWidget(
			"Sfx",
			nullptr,
			MOVE_HANDLER(SettingsMenuScreen, textHandler<kSfx>)));
	
	_widgets.push_back(new StaticLocationWidget(
			"SfxHelp",
			nullptr,
			nullptr));
	_widgets.back()->setVisible(false);

	if (!isDemo()) {
		_widgets.push_back(new CheckboxWidget(
				"AllowFF",
				true,
				nullptr,
				MOVE_HANDLER(SettingsMenuScreen, textHandler<kAllowFF>)));
		_widgets.back()->setupSounds(3, 4);
		
		_widgets.push_back(new StaticLocationWidget(
				"AllowFFHelp",
				nullptr,
				nullptr));
		_widgets.back()->setVisible(false);
	}
}

void SettingsMenuScreen::close() {
	StaticLocationScreen::close();
	_soundManager.stop();
}

void SettingsMenuScreen::onMouseMove(const Common::Point &pos) {
	StaticLocationScreen::onMouseMove(pos);
	_soundManager.update();
}

void SettingsMenuScreen::handleMouseUp() {
	if (!_widgets.empty()) {
		_soundManager.endLoop();
		_widgets[kWidgetVoice]->onMouseUp();
		_widgets[kWidgetMusic]->onMouseUp();
		_widgets[kWidgetSfx]->onMouseUp();
	}
}

template<SettingsMenuScreen::HelpTextIndex N>
void SettingsMenuScreen::textHandler(StaticLocationWidget &widget, const Common::Point &mousePos) {
	if (widget.isVisible()) {
		if (widget.isMouseInside(mousePos)) {
			widget.setTextColor(_textColorHovered);
			_widgets[N]->setVisible(true);
		} else {
			widget.setTextColor(_textColorDefault);
			_widgets[N]->setVisible(false);
		}
	}
}

void SettingsMenuScreen::backHandler() {
	StarkUserInterface->backPrevScreen();
}

CheckboxWidget::CheckboxWidget(const char *renderEntryName, bool isChecked,
							   WidgetOnClickCallback *onClickCallback,
	            			   WidgetOnMouseMoveCallback *onMouseMoveCallback) :
		StaticLocationWidget(renderEntryName, onClickCallback, onMouseMoveCallback),
		_isChecked(isChecked) {
	// Load images
	_checkBoxImage[0] = StarkStaticProvider->getUIElement(StaticProvider::kCheckMark, 0);
	_checkBoxImage[1] = StarkStaticProvider->getUIElement(StaticProvider::kCheckMark, 1);
	_checkboxWidth = _checkBoxImage[0]->getWidth();
	_checkboxHeight = _checkBoxImage[0]->getHeight();
	_currentImage = _checkBoxImage[_isChecked];

	// Set positions
	Common::Point textPosition = getPosition();
	_position.x = textPosition.x - _checkboxWidth - 8;
	_position.y = textPosition.y - 4;
}

void CheckboxWidget::render() {
	StaticLocationWidget::render();
	_currentImage->render(_position, true);
}

bool CheckboxWidget::isMouseInside(const Common::Point &mousePos) const {
	return StaticLocationWidget::isMouseInside(mousePos) || isMouseInsideCheckbox(mousePos);
}

void CheckboxWidget::onClick() {
	StaticLocationWidget::onClick();
	_isChecked = !_isChecked;
	_currentImage = _checkBoxImage[_isChecked];
}

bool CheckboxWidget::isMouseInsideCheckbox(const Common::Point &mousePos) const {
	return mousePos.x >= _position.x && mousePos.x <= _position.x + _checkboxWidth &&
		   mousePos.y >= _position.y && mousePos.y <= _position.y + _checkboxHeight;
}

VolumeWidget::VolumeWidget(const char *renderEntryName, Cursor *cursor,
						   SoundManager &soundManager, int soundIndex,
						   WidgetOnMouseMoveCallback *onMouseMoveCallback) :
		StaticLocationWidget(renderEntryName, nullptr, onMouseMoveCallback),
		_cursor(cursor),
		_soundManager(soundManager),
		_soundIndex(soundIndex),
		_isDragged(false) {
	// Load images
	_sliderImage = StarkStaticProvider->getUIElement(StaticProvider::kVolume, 0);
	_bgImage = StarkStaticProvider->getUIElement(StaticProvider::kVolume, 1);
	_bgWidth = _bgImage->getWidth();
	_bgHeight = _bgImage->getHeight();
	_sliderWidth = _sliderImage->getWidth();

	// Set positions
	// TODO: get the real position
	_bgPosition.x = 300; _bgPosition.y = 300;
	_minX = _bgPosition.x - _sliderWidth / 2;
	_maxX = _bgPosition.x + _bgWidth + _sliderWidth / 2;
	_sliderPosition.x = _minX;
	_sliderPosition.y = _bgPosition.y + (_bgHeight - _sliderImage->getHeight()) / 2;
}

void VolumeWidget::render() {
	StaticLocationWidget::render();
	_sliderImage->render(_sliderPosition, true);
	_bgImage->render(_bgPosition, true);
}

bool VolumeWidget::isMouseInside(const Common::Point &mousePos) const {
	return StaticLocationWidget::isMouseInside(mousePos) || isMouseInsideBg(mousePos);
}

void VolumeWidget::onClick() {
	if (isMouseInsideBg(_cursor->getMousePosition())) {
		_isDragged = true;
		_soundManager.play(_soundIndex);
	}
}

void VolumeWidget::onMouseMove(const Common::Point &mousePos) {
	if (isMouseInsideBg(mousePos)) {
		setTextColor(_textColorBgHovered);
	} else {
		StaticLocationWidget::onMouseMove(mousePos);
	}

	if (_isDragged) {
		int posX = mousePos.x - _sliderWidth / 2;
		if (posX < _minX) {
			_sliderPosition.x = _minX;
		} else if (posX > _maxX) {
			_sliderPosition.x = _maxX;
		} else {
			_sliderPosition.x = posX;
		}
	}
}

void VolumeWidget::onMouseUp() {
	_isDragged = false;
}

bool VolumeWidget::isMouseInsideBg(const Common::Point &mousePos) const {
	return mousePos.x >= _bgPosition.x && mousePos.x <= _bgPosition.x + _bgWidth &&
		   mousePos.y >= _bgPosition.y && mousePos.y <= _bgPosition.y + _bgHeight;
}

SoundManager::SoundManager() : 
		_currentSound(nullptr),
		_isLopping(false) {
}

void SoundManager::load() {
	for (int i = 0; i < 3; ++i) {
		_sounds[i] = StarkStaticProvider->getLocationSound(i);
		_sounds[i]->setLooping(false);
	}
}

void SoundManager::play(int index) {
	stop();
	_currentSound = _sounds[index];
	_currentSound->play();
	_isLopping = true;
}

void SoundManager::endLoop() {
	_isLopping = false;
}

void SoundManager::stop() {
	if (_currentSound) {
		_currentSound->stop();
		_currentSound = nullptr;
	}
	_isLopping = false;
}

void SoundManager::update() {
	if (_currentSound && !_currentSound->isPlaying()) {
		if (_isLopping) {
			_currentSound->play();
		} else {
			_currentSound->stop();
			_currentSound = nullptr;
		}
	}
}

} // End of namespace Stark
