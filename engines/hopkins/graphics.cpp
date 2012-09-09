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

#include "common/system.h"
#include "common/file.h"
#include "common/rect.h"
#include "engines/util.h"
#include "hopkins/files.h"
#include "hopkins/globals.h"
#include "hopkins/graphics.h"
#include "hopkins/hopkins.h"

namespace Hopkins {

GraphicsManager::GraphicsManager() {
	SDL_MODEYES = false;
	SDL_ECHELLE = 0;
	XSCREEN = YSCREEN = 0;
	WinScan = 0;
	Winbpp = 0;
	PAL_PIXELS = NULL;
	nbrligne = 0;
	Linear = false;
	VideoPtr = NULL;
	start_x = 0;
	ofscroll = 0;
	SCROLL = 0;
	PCX_L = PCX_H = 0;
	DOUBLE_ECRAN = false;
	OLD_SCROLL = 0;
	MANU_SCROLL = 1;
	SPEED_SCROLL = 16;
	nbrligne2 = 0;
	Agr_x = Agr_y = 0;
	Agr_Flag_x = Agr_Flag_y = 0;
}

GraphicsManager::~GraphicsManager() {
}


void GraphicsManager::SET_MODE(int width, int height) {
	if (!SDL_MODEYES) {
		SDL_ECHELLE = 0;

		if (GLOBALS.XSETMODE == 1)
			SDL_ECHELLE = 0;
		if (GLOBALS.XSETMODE == 2)
			SDL_ECHELLE = 25;
		if (GLOBALS.XSETMODE == 3)
			SDL_ECHELLE = 50;
		if (GLOBALS.XSETMODE == 4)
			SDL_ECHELLE = 75;
		if (GLOBALS.XSETMODE == 5)
			SDL_ECHELLE = GLOBALS.XZOOM;
		
		int bpp = 8;
		if (GLOBALS.XFORCE8 == 1)
			bpp = 8;
		if (GLOBALS.XFORCE16 == 1)
			bpp = 16;

		if (SDL_ECHELLE) {
			error("TODO: Implement zooming support"); 
			//width = Reel_Zoom(width, SDL_ECHELLE);
			//height = Reel_Zoom(height, SDL_ECHELLE);
		}

		if (bpp == 8) {
			initGraphics(width, height, true);
		} else {
			Graphics::PixelFormat pixelFormat(2, 5, 5, 5, 0, 10, 5, 0, 0);
			initGraphics(width, height, true, &pixelFormat);
		}

		// Init surfaces
		//VESA_SCREEN = dos_malloc2(0x96000u);
		//VESA_BUFFER = dos_malloc2(0x96000u);

		VideoPtr = NULL;
		XSCREEN = width;
		YSCREEN = height;

		Linear = true;
		Winbpp = bpp;
		WinScan = width;

		PAL_PIXELS = SD_PIXELS;
		nbrligne = width;

		for (int idx = 0; idx < 256; ++idx) {
			cmap[idx].r = cmap[idx].g = cmap[idx].b = 0;
		}

		SDL_MODEYES = true;
	} else {
		error("Called SET_MODE multiple times");
	}
}

void GraphicsManager::DD_Lock() {
	VideoPtr = g_system->lockScreen();
}

void GraphicsManager::DD_Unlock() {
	assert(VideoPtr);
	g_system->unlockScreen();
}

void GraphicsManager::Cls_Video() {
	assert(VideoPtr);

	VideoPtr->fillRect(Common::Rect(0, 0, XSCREEN, YSCREEN), 0);
}

void GraphicsManager::LOAD_IMAGE(const Common::String &file) {
	Common::String filename	= Common::String::format("%s.PCX", file);
	CHARGE_ECRAN(filename);
	GraphicsManager::INIT_TABLE(165, 170, Palette);
}

void GraphicsManager::CHARGE_ECRAN(const Common::String &file) {
	int v1;
	byte *v4;
	byte *v5;
	Common::File f;

	FileManager::DMESS1();

	v1 = 1;
	if (FileManager::RECHERCHE_CAT(file, 6)) {
		FileManager::CONSTRUIT_FICHIER(GLOBALS.HOPIMAGE, file);
		if (!f.open(GLOBALS.NFICHIER))
			error("CHARGE_ECRAN - %s", file.c_str());

		f.seek(0, SEEK_END);
		f.close();
		v1 = 0;
	}

	SCROLL_ECRAN(0);
	A_PCXSCREEN_WIDTH_SCREEN_HEIGHT((byte *)VESA_SCREEN.pixels, file, Palette, v1);

	SCROLL = 0;
	OLD_SCROLL = 0;
	Cls_Pal();

	if (!DOUBLE_ECRAN) {
		souris_max();
		SCANLINE(SCREEN_WIDTH);
		GLOBALS.max_x = SCREEN_WIDTH;
		DD_Lock();
		Cls_Video();
		if (Winbpp == 2) {
			if (SDL_ECHELLE)
				m_scroll16A((byte *)VESA_SCREEN.pixels, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
			else
				m_scroll16((byte *)VESA_SCREEN.pixels, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
		} else if (Winbpp == 1) {
			if (!SDL_ECHELLE)
				m_scroll2((byte *)VESA_SCREEN.pixels, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
			else 
				m_scroll2A((byte *)VESA_SCREEN.pixels, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
		}
	} else {
		SCANLINE(0x500u);
		GLOBALS.max_x = SCREEN_WIDTH * 2;
		DD_Lock();
		Cls_Video();
		DD_Unlock();

		if (MANU_SCROLL == 1) {
			DD_Lock();
			if (Winbpp == 2) {
				if (SDL_ECHELLE)
					m_scroll16A((byte *)VESA_SCREEN.pixels, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
				else
					m_scroll16((byte *)VESA_SCREEN.pixels, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
			}
			if (Winbpp == 1) {
				if (!SDL_ECHELLE)
					m_scroll2((byte *)VESA_SCREEN.pixels, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
				else
					m_scroll2A((byte *)VESA_SCREEN.pixels, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
			}

			DD_Unlock();
		}
	}

	v4 = (byte *)VESA_BUFFER.pixels;
	v5 = (byte *)VESA_SCREEN.pixels;
	memcpy(v4, v5, 614396);

	v5 = (byte *)v5 + 614396;
	v4 = (byte *)v4 + 614396;
	WRITE_LE_UINT16(v4, READ_LE_UINT16(v5));
	*((byte *)v4 + 2) = *((byte *)v5 + 2);
}

void GraphicsManager::INIT_TABLE(int a1, int a2, byte *a3) {
	for (int idx = 0; idx < 256; ++idx)
		TABLE_COUL[idx] = idx;
  
	Trans_bloc(TABLE_COUL, a3, 256, a1, a2);

	for (int idx = 0; idx < 256; ++idx) {
		byte v = TABLE_COUL[idx];
		if (v > 27)
			TABLE_COUL[idx] = 0;
		if (!v)
			TABLE_COUL[idx] = 0;
	}

	TABLE_COUL[0] = 1;
}

int GraphicsManager::SCROLL_ECRAN(int amount) {
	int result = CLIP(amount, 0, SCREEN_WIDTH);
	start_x = result;
	ofscroll = result;
	SCROLL = result;
	return result;
}

void GraphicsManager::Trans_bloc(byte *destP, byte *srcP, int count, int param1, int param2) {
	byte *v5;
	int v6;
	int v7;
	int v8;
	unsigned int v11;
	int v12;
	int v13;
	int v14;
	int v15;
	int v16;
	int v17;
	unsigned int v18;
	char v19;
	int v20;
	bool breakFlag;

	v5 = destP;
	v6 = count - 1;
	do {
		breakFlag = v6;
		v7 = *(byte *)v5++;
		v8 = (unsigned int)(3 * v7);

		// TODO: Ensure this is the right calculation
		v11 = *(byte *)(v8 + srcP) + *(byte *)(v8 + srcP + 1)
				+ *(byte *)(v8 + srcP + 2);

		v12 = 0;
		for (;;) {
			v13 = v12 + 1;
			if ( v13 == 38 )
				break;

			v20 = v13;
			v8 = 3 * v8;
			v14 = *(byte *)(v8 + srcP);
			v15 = v14;
			v14 = *(byte *)(v8 + srcP + 1);
			v16 = v14 + v15;
			v14 = *(byte *)(v8 + srcP + 2);
			v17 = v14 + v16;
			v12 = v20;
			v18 = param1 + v17;
			if (v18 >= v11 && (unsigned int)(v18 - param2) <= v11) {
				v19 = v20;
				if (!v20)
					v19 = 1;
				*(byte *)(v5 - 1) = v19;
				break;
			}
		}

		v6 = breakFlag - 1;
	} while ( !breakFlag);
}

// TODO: See if it's feasible and/or desirable to change this to use the Common PCX decoder
void GraphicsManager::A_PCXSCREEN_WIDTH_SCREEN_HEIGHT(byte *surface, const Common::String &file, byte *palette, bool typeFlag) {
	int filesize; 
	signed __int16 v6;
	int v7;
	__int16 v8;
	int v9; 
	int v10;
	int v11;
	unsigned __int8 v12; 
	int v13;
	int v14;
	char v15; 
	int v18;
	int v19; 
	int v20;
	unsigned int v21;
	int v22;
	int32 v23;
	byte *ptr;
	Common::File f;

	// Clear the passed surface
	memset(surface, 0, SCREEN_WIDTH * 2 * SCREEN_HEIGHT);

	if (typeFlag) {
		FileManager::CONSTRUIT_FICHIER(GLOBALS.HOPIMAGE, "PIC.RES");
		if (!f.open(GLOBALS.NFICHIER))
			error("(nom)Erreur en cours de lecture.");
		f.seek(GLOBALS.CAT_POSI);

		v7 = GLOBALS.CAT_TAILLE - 896;
		v8 = f.read(HEADER_PCX, 128);

		v6 = READ_LE_UINT16(&HEADER_PCX[8]) + 1;
		v20 = READ_LE_UINT16(&HEADER_PCX[10]) + 1;
		if ((READ_LE_UINT16(&HEADER_PCX[8]) + 1) <= SCREEN_WIDTH) {
			DOUBLE_ECRAN = false;
		} else {
			v6 = SCREEN_WIDTH * 2;
			DOUBLE_ECRAN = true;
		}
		if (v20 > SCREEN_HEIGHT)
			v20 = SCREEN_HEIGHT;
		PCX_L = v6;
		PCX_H = v20;
		if (v8 == -1)
		  error("Erreur en cours de lecture.");
	} else {
		FileManager::CONSTRUIT_FICHIER(GLOBALS.HOPIMAGE, file);
		if (!f.open(GLOBALS.NFICHIER))
		  error("(nom)Erreur en cours de lecture.");

		filesize = f.size();
		int bytesRead = f.read(HEADER_PCX, 128);
		if (bytesRead < 128)
			error("Erreur en cours de lecture.");

		v6 = READ_LE_UINT16(&HEADER_PCX[8]) + 1;
		v20 = READ_LE_UINT16(&HEADER_PCX[10]) + 1;
		if (v6 <= SCREEN_WIDTH) {
			DOUBLE_ECRAN = false;
		} else {
			v6 = SCREEN_WIDTH * 2;
			DOUBLE_ECRAN = true;
		}
		if (v20 > SCREEN_HEIGHT)
			v20 = SCREEN_HEIGHT;
		PCX_L = v6;
		PCX_H = v20;
		v7 = filesize - 896;
	}

	ptr = GLOBALS.dos_malloc2(0xEE60u);
	if (v7 >= 60000) {
		v21 = v7 / 60000 + 1;
		v23 = 60000 * (v7 / 60000) - v7;
    
		if (((uint32)v23 & 0x80000000u) != 0)
			v23 = -v23;
		f.read(ptr, 60000);
		v7 = 60000;
	} else {
		v21 = 1;
		v23 = v7;
		f.read(ptr, v7);
	}
	v22 = v21 - 1;
	v18 = 0;
	v9 = 0;
	v10 = 0;
	v19 = v6;
  
	do {
		if (v9 == v7) {
			v9 = 0;
			--v22;
			v7 = 60000;
			if ( !v22 )
				v7 = v23;
			v11 = v10;
			f.read(ptr, v7);
			v10 = v11;
		}

		v12 = *((byte *)ptr + v9++);
		if (v12 > 0xC0u) {
			v13 = v12 - 192;
			if (v9 == v7) {
				v9 = 0;
				--v22;
				v7 = 60000;
				if ( v22 == 1 )
					v7 = v23;
				v14 = v10;
				f.read(ptr, v7);
				v10 = v14;
			}
			v15 = *((byte *)ptr + v9++);

			do {
				*((byte *)surface + v10++) = v15;
				++v18;
				--v13;
			} while (v13);
		} else {
			*((byte *)surface + v10++) = v12;
			++v18;
		}
	} while (v18 < v19 * v20);

	if (typeFlag) {
		f.seek(GLOBALS.CAT_TAILLE + GLOBALS.CAT_POSI - 768);
	} else {
		filesize = f.size();
		f.seek(filesize - 768);
	}

	if (f.read(palette, PALETTE_SIZE * 3) != (PALETTE_SIZE * 3))
		error("A_PCXSCREEN_WIDTH_SCREEN_HEIGHT");
  
	f.close();
	GLOBALS.dos_free2(ptr);
}

void GraphicsManager::Cls_Pal() {
	warning("TODO");
}
void GraphicsManager::souris_max() {
	warning("TODO");
}

void GraphicsManager::SCANLINE(int width) {
	// Original has no implementatoin
}

void GraphicsManager::m_scroll(const byte *surface, int xs, int ys, int width, int height, int destX, int destY) {
	const byte *v7;
	byte *v8;
	int v9;
	int v10;
	byte *v11;
	const byte *v12;
	unsigned int v13;

	assert(VideoPtr);
	v7 = xs + nbrligne2 * ys + surface;
	v8 = destX + WinScan * destY + (byte *)VideoPtr->pixels;
	v9 = height;
	do {
		v10 = v9;
		memcpy((byte *)v8, (const byte *)v7, 4 * (width >> 2));
		v12 = (const byte *)(v7 + 4 * (width >> 2));
		v11 = (byte *)(v8 + 4 * (width >> 2));
		v13 = width - 4 * (width >> 2);
		memcpy(v11, v12, v13);
		v8 = ((byte *)v11 + v13 + WinScan - width);
		v7 = ((byte *)v12 + v13 + nbrligne2 - width);
		v9 = v10 - 1;
	} while (v10 != 1);
}

void GraphicsManager::m_scroll2(const byte *surface, int xs, int ys, int width, int height, int destX, int destY) {
	const byte *v7;
	byte *v8;
	int v9;
	int v10;
	int v11;

	assert(VideoPtr);
	v7 = (const byte *)(xs + nbrligne2 * ys + surface);
	v8 = (byte *)(destX + WinScan * destY + (byte *)VideoPtr->pixels);
	v9 = WinScan - SCREEN_WIDTH;
	v10 = nbrligne2 - SCREEN_WIDTH;
	v11 = height;

	do {
		memcpy(v8, v7, SCREEN_WIDTH);
		v8 = (byte *)v8 + v9 + SCREEN_WIDTH;
		v7 = (const byte *)v7 + v10 + SCREEN_WIDTH;
		--v11;
	} while (v11);
}

void GraphicsManager::m_scroll2A(const byte *surface, int xs, int ys, int width, int height, int destX, int destY) {
	const byte *v7;
	const byte *v8;
	int v9;
	int v10;
	byte v11;
	const byte *v12;
	const byte *v13;

	assert(VideoPtr);
	v7 = xs + nbrligne2 * ys + surface;
	v8 = destX + WinScan * destY + (byte *)VideoPtr->pixels;
	v9 = height;
	Agr_x = 0;
	Agr_y = 0;
	Agr_Flag_y = 0;
	do {
		for (;;) {
			v13 = v8;
			v12 = v7;
			v10 = width;
			Agr_x = 0;
			do {
				v11 = *(byte *)v7;
				*(byte *)v8++ = *(byte *)v7++;
				Agr_x += SDL_ECHELLE;
				if ((unsigned int)Agr_x >= 100) {
					Agr_x -= 100;
					*(byte *)v8++ = v11;
				}
				--v10;
			} while ( v10 );
	      
			v7 = v12;
			v8 = WinScan + v13;
			if (Agr_Flag_y)
				break;

			Agr_y += SDL_ECHELLE;
			if ((unsigned int)Agr_y < 100)
				break;

			Agr_y -= 100;
			Agr_Flag_y = 1;
		}

		Agr_Flag_y = 0;
		v7 = nbrligne2 + v12;
		--v9;
	} while (v9);
}

void GraphicsManager::m_scroll16(const byte *surface, int xs, int ys, int width, int height, int destX, int destY) {
	const byte *v7;
	const byte *v8;
	int v9;
	int v10;
	const byte *v11;
	int v12;
	const byte *v13;
	const byte *v14;

	assert(VideoPtr);
	v7 = xs + nbrligne2 * ys + surface;
	v8 = destX + destX + WinScan * destY + (byte *)VideoPtr->pixels;
	v9 = height;

	do {
		v14 = v8;
		v13 = v7;
		v10 = width;
		v12 = v9;
		v11 = PAL_PIXELS;
    
		do {
			*(uint16 *)v8 = *(uint16 *)(v11 + 2 * *(byte *)v7++);
			v8 += 2;
			--v10;
		} while (v10);
    
		v7 = nbrligne2 + v13;
		v8 = WinScan + v14;
		v9 = v12 - 1;
	} while (v12 != 1);
}

void GraphicsManager::m_scroll16A(const byte *surface, int xs, int ys, int width, int height, int destX, int destY) {
	const byte *v7;
	const byte *v8;
	int v9;
	int v10;
	const byte *v11;
	int v12;
	int v13;
	const byte *v14;
	const byte *v15;

	assert(VideoPtr);
	v7 = xs + nbrligne2 * ys + surface;
	v8 = destX + destX + WinScan * destY + (byte *)VideoPtr->pixels;
	v9 = height;
	Agr_x = 0;
	Agr_y = 0;
	Agr_Flag_y = 0;

	do {
		for (;;) {
			v15 = v8;
			v14 = v7;
			v10 = width;
			v13 = v9;
			v11 = PAL_PIXELS;
			Agr_x = 0;

			do {
				v12 = *(uint16 *)(v11 + 2 * *(byte *)v7);
				*(uint16 *)v8 = v12;
				++v7;
				v8 += 2;
				Agr_x += SDL_ECHELLE;
				if ((unsigned int)Agr_x >= 100) {
					Agr_x -= 100;
					*(uint16 *)v8 = v12;
					v8 += 2;
				}
        
				--v10;
			} while (v10);

			v9 = v13;
			v7 = v14;
			v8 = WinScan + v15;
			if (Agr_Flag_y == 1)
				break;
			
			Agr_y += SDL_ECHELLE;
			
			if ((unsigned int)Agr_y < 100)
				break;
      
			Agr_y -= 100;
			Agr_Flag_y = 1;
		}

		Agr_Flag_y = 0;
		v7 = nbrligne2 + v14;
		v9 = v13 - 1;
	} while (v13 != 1);
}

/*------------------------------------------------------------------------*/

byte *ObjectManager::CHANGE_OBJET(int objIndex) {
	byte *result = ObjectManager::CAPTURE_OBJET(objIndex, 1);
	GLOBALS.Bufferobjet = result;
	GLOBALS.Nouv_objet = 1;
	GLOBALS.OBJET_EN_COURS = objIndex;
	return result;
}

byte *ObjectManager::CAPTURE_OBJET(int objIndex, int mode) {
	byte *result = NULL;
	byte *dataP;

	dataP = 0;
	int val1 = GLOBALS.ObjetW[objIndex].field0;
	int val2 = GLOBALS.ObjetW[objIndex].field1;

	if (mode == 1)
	    ++val2;
	if (val1 != GLOBALS.NUM_FICHIER_OBJ) {
		if (GLOBALS.ADR_FICHIER_OBJ != PTRNUL)
			ObjectManager::DEL_FICHIER_OBJ();
		if (val1 == 1) {
			FileManager::CONSTRUIT_SYSTEM("OBJET1.SPR");
			GLOBALS.ADR_FICHIER_OBJ = ObjectManager::CHARGE_SPRITE(GLOBALS.NFICHIER);
		}
		GLOBALS.NUM_FICHIER_OBJ = val1;
	}

	int width = ObjectManager::Get_Largeur(GLOBALS.ADR_FICHIER_OBJ, val2);
	int height = ObjectManager::Get_Hauteur(GLOBALS.ADR_FICHIER_OBJ, val2);
	GLOBALS.OBJL = width;
	GLOBALS.OBJH = height;

	switch (mode) {
	case 0:
		dataP = GLOBALS.dos_malloc2(height * width);
		if (dataP == PTRNUL)
			error("CAPTURE_OBJET");
			
		ObjectManager::capture_mem_sprite(GLOBALS.ADR_FICHIER_OBJ, dataP, val2);
		break;

	case 1:
		ObjectManager::sprite_alone(GLOBALS.ADR_FICHIER_OBJ, GLOBALS.Bufferobjet, val2);
		result = GLOBALS.Bufferobjet;
		break;

	case 3:
		ObjectManager::capture_mem_sprite(GLOBALS.ADR_FICHIER_OBJ, GLOBALS.INVENTAIRE_OBJET, val2);
		result = GLOBALS.INVENTAIRE_OBJET;
		break;

	default:
		result = dataP;
		break;
	}

	return result;
}

int ObjectManager::Get_Largeur(const byte *objectData, int objIndex) {
	const byte *objP = objectData + 3;
	for (int i = objIndex; i; --i)
		objP += READ_LE_UINT32(objP) + 16;

	return READ_LE_UINT16(objP + 4);
}

int ObjectManager::Get_Hauteur(const byte *objectData, int objIndex) {
	const byte *objP = objectData + 3;
	for (int i = objIndex; i; --i)
		objP += READ_LE_UINT32(objP) + 16;

	return READ_LE_UINT16(objP + 6);
}

int ObjectManager::sprite_alone(const byte *objectData, byte *sprite, int objIndex) {
	const byte *objP = objectData + 3;
	for (int i = objIndex; i; --i) {
		objP += READ_LE_UINT32(objP) + 16;
	}

	objP += 4;
	int result = READ_LE_UINT16(objP) * READ_LE_UINT16(objP + 2);

	memcpy(sprite + 3, objP - 4, result + 16);
	return result;
}

byte *ObjectManager::DEL_FICHIER_OBJ() {
	GLOBALS.NUM_FICHIER_OBJ = 0;
	if (GLOBALS.ADR_FICHIER_OBJ != PTRNUL)
		GLOBALS.ADR_FICHIER_OBJ = FileManager::LIBERE_FICHIER(GLOBALS.ADR_FICHIER_OBJ);
  
	byte *result = PTRNUL;
	GLOBALS.ADR_FICHIER_OBJ = PTRNUL;
	return result;
}

byte *ObjectManager::CHARGE_SPRITE(const Common::String &file) {
	FileManager::DMESS1();
	return FileManager::CHARGE_FICHIER(file);
}

int ObjectManager::capture_mem_sprite(const byte *objectData, byte *sprite, int objIndex) {
	const byte *objP = objectData + 3;
	for (int i = objIndex; i; --i) {
		objP += READ_LE_UINT32(objP) + 16;
	}

	objP += 4;
	int result = READ_LE_UINT16(objP) * READ_LE_UINT16(objP + 2);

	memcpy(sprite, objP + 12, result);
	return result;
}

int ObjectManager::AJOUTE_OBJET(int objIndex) {
	bool flag = false;
	int arrIndex = 0;
	do {
		++arrIndex;
		if (!GLOBALS.INVENTAIRE[arrIndex])
			flag = true;
		if (arrIndex == 32)
			flag = true;
	} while (!flag);
  
	GLOBALS.INVENTAIRE[arrIndex] = objIndex;
	return arrIndex;
}

} // End of namespace Hopkins
