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

#include "gui/dump-all-dialogs.h"

#include "common/file.h"
#include "common/fs.h"
#include "common/language.h"
#include "common/translation.h"

#include "gui/ThemeEngine.h"
#include "gui/gui-manager.h"
#include "gui/launcher.h"

#include "image/png.h"

namespace GUI {

void saveGUISnapshot(Graphics::Surface surf, const Common::String &filename) {
	Common::DumpFile outFile;
	Common::String outName = Common::String::format("snapshots/%s", filename.c_str());

	if (outFile.open(outName)) {
		Image::writePNG(outFile, surf);
		outFile.finalize();
		outFile.close();
	}
}

void dumpDialogs(const Common::String &dialogName, int res, const Common::String &lang) {
	// Update GUI language
	TransMan.setLanguage(lang);

	// Update resolution
	ConfMan.setInt("last_window_width", res, Common::ConfigManager::kApplicationDomain);
	ConfMan.setInt("last_window_height", 600, Common::ConfigManager::kApplicationDomain);
	g_system->initSize(res, 600);

	Graphics::Surface surf;
	surf.create(g_system->getOverlayWidth(), g_system->getOverlayHeight(), g_system->getOverlayFormat());

	// LauncherDialog
	GUI::LauncherChooser chooser;
	chooser.selectLauncher();
	chooser.runModal();
	g_system->grabOverlay(surf);

	Common::String filename = Common::String::format("%d-%s.png", res, lang.c_str());
	saveGUISnapshot(surf, filename);
}

void dumpAllDialogs() {
	auto originalLang = TransMan.getCurrentLanguage();
	int original_window_width = ConfMan.getInt("last_window_width", Common::ConfigManager::kApplicationDomain);
	int original_window_height = ConfMan.getInt("last_window_height", Common::ConfigManager::kApplicationDomain);
	Common::List<Common::String> list = Common::getLanguageList();

	Common::FSNode dumpDir("snapshots");
	if (!dumpDir.isDirectory())
		dumpDir.createDirectory();

	// Iterate through all languages available
	for (Common::String lang : list) {
		// Iterating through the resolutions doesn't work so you have to manually change it here.
		// TODO: Find a way to iterate through the resolutions using code.
		Common::Array<int> res_to_test = {800, 640, 320};
		dumpDialogs("test", res_to_test[0], lang);
	}

	TransMan.setLanguage(originalLang);
	ConfMan.setInt("last_window_width", original_window_width, Common::ConfigManager::kApplicationDomain);
	ConfMan.setInt("last_window_height", original_window_height, Common::ConfigManager::kApplicationDomain);
	g_system->initSize(original_window_width, original_window_height);
}

} // End of namespace GUI
