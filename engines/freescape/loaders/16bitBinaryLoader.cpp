//
//  16bitBinaryLoader.cpp
//  Phantasma
//
//  Created by Thomas Harte on 17/12/2013.
//  Copyright (c) 2013 Thomas Harte. All rights reserved.
//

//#include "freescape/language/parser.h"

#include "common/array.h"
#include "common/debug.h"
#include "common/file.h"

#include "freescape/area.h"
#include "freescape/language/16bitDetokeniser.h"
#include "freescape/language/instruction.h"
#include "freescape/language/parser.h"
#include "freescape/loaders/16bitBinaryLoader.h"
#include "freescape/loaders/loader.h"
#include "freescape/objects/geometricobject.h"
#include "freescape/objects/object.h"

namespace Freescape {

typedef enum {
		First = 0x0000,
		Border = 0x4524,
} ChunkType;

static Object *load16bitObject(StreamLoader &stream) {
	// get object flags and type
	uint8 objectFlags = stream.get8();
	Object::Type objectType = (Object::Type)stream.get8();

	/*
		Notes to self:

			0 = no flags
			128 = Movable, Animated
			134 = Movable, Animated, Default invis, invis
			6 = Default invis, Invis
			32 = collided
	*/

	// get unknown value
	uint16 skippedShort = stream.get16();
	debug("skippedShort: %d", skippedShort);

	// grab location, size
	Math::Vector3d position, size;
	position.x() = stream.get16();
	position.y() = stream.get16();
	position.z() = stream.get16();
	size.x() = stream.get16();
	size.y() = stream.get16();
	size.z() = stream.get16();

	// object ID
	uint16 objectID = stream.get16();

	// size of object on disk; we've accounted for 20 bytes
	// already so we can subtract that to get the remaining
	// length beyond here
	uint32 byteSizeOfObject = (uint32)(stream.get16() << 1) - 20;

	debug("Object %d ; type %d ; flags %d ; size %d", (int)objectID, (int)objectType, (int)objectFlags, byteSizeOfObject);
	debug("Location: %f, %f, %f", position.x(), position.y(), position.z());
	debug("Size: %f, %f, %f", size.x(), size.y(), size.z());

	switch (objectType) {
	default: {
		// read the appropriate number of colours
		int numberOfColours = GeometricObject::numberOfColoursForObjectOfType(objectType);
		Common::Array<uint8> *colours = new Common::Array<uint8>;
		for (uint8 colour = 0; colour < numberOfColours/2; colour++) {
			uint8 c1 = stream.get8();
			byteSizeOfObject--;
			uint8 c2 = stream.get8();
			byteSizeOfObject--;
			colours->push_back( (c1 & 0x0f) | ((c2 & 0x0f) << 4));
			debug("color[%d] = %d", 2*colour, (c1 & 0x0f) | ((c2 & 0x0f) << 4));
			colours->push_back(c1 >> 4 | c2 & 0xf0);
			debug("color[%d] = %d", 2*colour+1, c1 >> 4 | c2 & 0xf0);
		}

		// read extra vertices if required...
		int numberOfOrdinates = GeometricObject::numberOfOrdinatesForType(objectType);
		debug("number of ordinates %d", numberOfOrdinates);
		Common::Array<uint16> *ordinates = nullptr;

		if (numberOfOrdinates) {
			assert(byteSizeOfObject > 0);
			ordinates = new Common::Array<uint16>;

			for (int ordinate = 0; ordinate < numberOfOrdinates; ordinate++) {
				ordinates->push_back(stream.get16());
				byteSizeOfObject -= 2;
			}
		}

		// grab the object condition, if there is one
		FCLInstructionVector instructions;
		if (byteSizeOfObject > 0) {
		// 	uint32 offset = stream.getFileOffset();
		    Common::Array<uint8> *conditionData = stream.nextBytes(byteSizeOfObject);
		// 	byteSizeOfObject = byteSizeOfObject - (offset - stream.getFileOffset());

			Common::String *conditionSource = detokenise16bitCondition(*conditionData);
		 	debug("Condition: %s", conditionSource->c_str());
			byteSizeOfObject = 0;
		// 	//instructions = getInstructions(conditionSource);
		}

		debug("Skipping %d bytes", byteSizeOfObject);
		stream.skipBytes(byteSizeOfObject);
		debug("End of object at %x", stream.getFileOffset());

		// create an object
		return new GeometricObject(
			objectType,
			objectID,
			objectFlags,
			position,
			size,
			colours,
			ordinates,
			instructions);
	} break;

	case Object::Entrance:
		debug("Skipping %d bytes", byteSizeOfObject);
		stream.skipBytes(byteSizeOfObject);
		debug("End of object at %x", stream.getFileOffset());
		return new Entrance(objectID, position, size); // size will be always 0,0,0?
		break;
	case Object::Sensor:
	case Object::Group:
		break;
	}

	// skip whatever we didn't understand
	//cout << "bytes we don't understand:" << endl;
	//int i = 0;
	//int j = 0;
	//for (i = 0; i < byteSizeOfObject/2; i++)
	//	cout << i << stream.get16() << endl;
	stream.skipBytes(byteSizeOfObject);
	debug("End of object at %x", stream.getFileOffset());

	return nullptr;
}

void load16bitInstrument(StreamLoader &stream) {
	uint16 zero = stream.get16();
	assert( zero == 0);
	uint16 type = stream.get16();
	uint16 x = stream.get16();
	uint16 y = stream.get16();
	uint16 length = stream.get16();
	uint16 height = stream.get16();
	stream.get16();
	stream.get16();
	uint16 lb = stream.get16();
	uint16 rt = stream.get16();
	uint16 v = stream.get16();
	uint16 fgcolor = stream.get16();
	uint16 bgcolor = stream.get16();

	stream.get16();
	stream.get16();
	stream.get16();
	stream.get16();
	debug("type %d ; x %d ; y %d ; length %d ; height %d ; lb %d ; rt %d ; variable: %d", type, x, y, length, height, lb, rt, v);
}

Area *load16bitArea(StreamLoader &stream) {
	// the lowest bit of this value seems to indicate
	// horizon on or off; this is as much as I currently know
	uint16 skippedValue = stream.get16();
	uint16 numberOfObjects = stream.get16();
	uint16 areaNumber = stream.get16();

	debug("Area %d", areaNumber);
	debug("Skipped value %d", skippedValue);
	debug("Objects: %d", numberOfObjects);

	// I've yet to decipher this fully
	stream.get16();
	stream.get16();
	stream.get16();


	uint8 skyColor = stream.get8();
	skyColor = (stream.get8() << 4) | skyColor;

	debug("Sky color %x", skyColor);
	uint8 groundColor = stream.get8();
	groundColor = (stream.get8() << 4) | groundColor;
	debug("Ground color %x", groundColor);
	stream.skipBytes(14);

	// this is just a complete guess
	/*Common::Array<uint8> palette;
	uint32 i;
	for (i = 0; i < 7*3; i++) {
		uint8 c = stream.get8();
		palette.push_back(c);
		debug("color %d", c);
	}
	stream.get8(); // ????*/
	//for (int paletteEntry = 0; paletteEntry < 22; paletteEntry++) {
	//	uint8 paletteColour = stream.get8() << 2;
	//	debug("Palette colour (?) %x", paletteColour);
	//}

	// we'll need to collate all objects and entrances; it's likely a
	// plain C array would do but maps are safer and the total application
	// cost is going to be negligible regardless
	ObjectMap *objectsByID = new ObjectMap;
	ObjectMap *entrancesByID = new ObjectMap;

	// get the objects or whatever; entrances use a unique numbering
	// system and have the high bit of their IDs set in the original file
	for (uint16 object = 0; object < numberOfObjects; object++) {
		Object *newObject = load16bitObject(stream);

		if (newObject) {
			if (newObject->getType() == Object::Entrance) {
				(*entrancesByID)[newObject->getObjectID() & 0x7fff] = newObject;
			} else {
				(*objectsByID)[newObject->getObjectID()] = newObject;
			}
		}
	}

	uint16 numberOfLocalConditions = stream.get16();
	debug("Number of conditions: %d", numberOfLocalConditions);
	for (uint16 localCondition = 0; localCondition < numberOfLocalConditions; localCondition++) {
		// 12 bytes for the name of the condition;
		// we don't care
		stream.skipBytes(12);

		// get the length and the data itself, converting from
		// shorts to bytes
		uint32 lengthOfCondition = (uint32)stream.get16() << 1;
		debug("Length of condition: %d", lengthOfCondition);
		if (lengthOfCondition == 0) {
			break;
		}

		// get the condition
		Common::Array<uint8> *conditionData = stream.nextBytes(lengthOfCondition);

		debug("Local condition %d at %x", localCondition + 1, stream.getFileOffset());
		debug("%s", detokenise16bitCondition(*conditionData)->c_str());
	}

	return (new Area(areaNumber, objectsByID, entrancesByID, 1, skyColor, groundColor));
}

void FreescapeEngine::load16bitBinary(Common::SeekableReadStream *file) {
	const uint32 fileSize = file->size();
	byte *buf = (byte *)malloc(fileSize);
	file->read(buf, fileSize);

	Common::Array<uint8> binary;

	uint32 i = 0;
	while (i < fileSize) {
		binary.push_back(buf[i]);
		i++;
	}

	StreamLoader streamLoader(binary);

	Common::Array<uint8>::size_type baseOffset = 0;

	// check whether this looks like an Amiga game; if so it'll start with AM
	// XOR'd with the repeating byte pattern 0x71, 0xc1 or with the pattern
	// 0x88 0x2c if it's on the ST (though the signature will still be AM)
	uint16 platformID = streamLoader.get16();
	debug("%d", platformID);

	if (
		//		(platformID != 0x4120) && (platformID != 0x5043)
		(platformID == 12428) || (platformID == 51553)) {
		// TODO: record original platform type, so we can decode the palette
		// and possibly the border properly
		//cout << "AMIGA" << endl;

		streamLoader.setReadMask((platformID >> 8) ^ 'A', (platformID & 0xff) ^ 'M');
	} else {
		debug("DOS");
		// find DOS end of file and consume it
		while (!streamLoader.eof()) {
			uint8 b = streamLoader.get8();
			if (b == 0x1a)
				break;
		}
		streamLoader.get8();

		// advance to the next two-byte boundary if necessary
		streamLoader.alignPointer();

		// skip bytes with meaning unknown
		streamLoader.get16();

		// this brings us to the beginning of the embedded
		// .KIT file, so we'll grab the base offset for
		// finding areas later
		baseOffset = streamLoader.getFileOffset();

		// check that the next two bytes are "PC", then
		// skip the number that comes after
		if (streamLoader.get8() != 'C' || streamLoader.get8() != 'P')
			error("invalid header");
	}

	// skip an unknown meaning
	streamLoader.get16();

	// start grabbing some of the basics...

	uint16 numberOfAreas = streamLoader.get16();
	uint16 sm = streamLoader.get16();
	debug("Something??: %d", sm); // meaning unknown

	debug("Number of areas: %d", numberOfAreas);

	uint16 windowCentreX = streamLoader.get16();
	uint16 windowCentreY = streamLoader.get16();
	uint16 windowWidth = streamLoader.get16();
	uint16 windowHeight = streamLoader.get16();

	debug("Window centre: (%d, %d)", windowCentreX, windowCentreY);
	debug("Window size: (%d, %d)", windowWidth, windowHeight);

	uint16 scaleX = streamLoader.get16();
	uint16 scaleY = streamLoader.get16();
	uint16 scaleZ = streamLoader.get16();

	debug("Scale %d, %d, %d", scaleX, scaleY, scaleZ);
	uint16 timerReload = streamLoader.get16();

	debug("Timer: every %d 50Hz frames", timerReload);

	uint16 maximumActivationDistance = streamLoader.get16();
	uint16 maximumFallDistance = streamLoader.get16();
	uint16 maximumClimbDistance = streamLoader.get16();

	debug("Maximum activation distance: %d", maximumActivationDistance);
	debug("Maximum fall distance: %d", maximumFallDistance);
	debug("Maximum climb distance: %d", maximumClimbDistance);

	uint16 startArea = streamLoader.get16();
	uint16 startEntrance = streamLoader.get16();

	debug("Start at entrance %d in area %d", startEntrance, startArea);

	uint16 playerHeight = streamLoader.get16();
	uint16 playerStep = streamLoader.get16();
	uint16 playerAngle = streamLoader.get16();

	debug("Height %d, step %d, angle %d", playerHeight, playerStep, playerAngle);

	uint16 startVehicle = streamLoader.get16();
	uint16 executeGlobalCondition = streamLoader.get16();

	debug("Start vehicle %d, execute global condition %d", startVehicle, executeGlobalCondition);

	// I haven't figured out what the next 106
	// bytes mean, so we'll skip them — global objects
	// maybe? Likely not 106 bytes in every file.
	//
	// ADDED: having rediscovered my source for the 8bit
	// file format, could this be shading information by
	// analogy with that?
	/*cout << "global unknown:";
	int i;
	int j;
	for (i = 0; i < 106/2; i++)
		cout << streamLoader.get16() << endl;*/

	streamLoader.skipBytes(106);

	// at this point I should properly load the border/key/mouse
	// bindings, but I'm not worried about it for now.
	//
	// Format is:
	//		(left x, top y, right x, bottom y) - all 16 bit
	//		keyboard key as an ASCII character (or zero for none)
	//		mouse button masl; 00 = both, 01 = left, 02 = right
	//
	// So, 10 bytes per binding. Bindings are listed in the order:
	//
	//	move forwards, move backwards, move right, move left, rise,
	//	fall, turn left, turn right, look up, look down, tilt left,
	//	tilt right, face forward, u-turn, change vehicle type,
	//	select this vehicle, quit game, fire, activate object,
	//	centre cursor on/off, save game position, load game position
	//
	// So 35 total. Which means this area takes up 350 bytes.
	//cout << "more global unknown:";
	//for (i = 0; i < 350/2; i++)
	//	cout << streamLoader.get16() << endl;

	streamLoader.skipBytes(350);

	// there are then file pointers for every area — these are
	// the number of shorts from the 'PC' tag, so multiply by
	// two for bytes. Each is four bytes
	uint32 *fileOffsetForArea = new uint32[numberOfAreas];
	for (uint16 area = 0; area < numberOfAreas; area++)
		fileOffsetForArea[area] = (uint32)streamLoader.get32() << 1;

	// now come the global conditions
	uint16 numberOfGlobalConditions = streamLoader.get16();
	for (uint16 globalCondition = 0; globalCondition < numberOfGlobalConditions; globalCondition++) {
		// 12 bytes for the name of the condition;
		// we don't care
		streamLoader.skipBytes(12);

		// get the length and the data itself, converting from
		// shorts to bytes
		uint32 lengthOfCondition = (uint32)streamLoader.get16() << 1;

		// get the condition
		Common::Array<uint8> *conditionData = streamLoader.nextBytes(lengthOfCondition);

		debug("Global condition %d at %x", globalCondition + 1, streamLoader.getFileOffset());
		debug("%s", detokenise16bitCondition(*conditionData)->c_str());
	}

	// grab the areas
	AreaMap *areaMap = new AreaMap;
	for (uint16 area = 0; area < numberOfAreas; area++) {
		debug("Area offset %d", fileOffsetForArea[area]);

		streamLoader.setFileOffset(fileOffsetForArea[area] + baseOffset);
		Area *newArea = load16bitArea(streamLoader);

		if (newArea) {
			(*areaMap)[newArea->getAreaID()] = newArea;
		}
	}
	//load16bitInstrument(streamLoader);

	Common::Array<uint8>::size_type o;
	Common::Array<uint8> *raw_border = nullptr;
	Common::Array<uint8> *raw_palette = nullptr;
	uint16 chunkType = 0;
	uint16 chunkSize = 0;
	uint16 colorNumber = 0;
	debug("End of areas at %x", streamLoader.getFileOffset());
	while (!streamLoader.eof()) {
		o = streamLoader.getFileOffset();
		chunkType = streamLoader.get16();
		if (chunkType == First) {
			chunkSize = streamLoader.rget16();
			if (chunkSize != 0xac) {
				debug("skip %x", chunkType);
				streamLoader.setFileOffset(o+2);
			} else {
				debug("First chunk found at %x with size %x", o, chunkSize);
				streamLoader.skipBytes(chunkSize-2);
			}
		}
		else if (chunkType == Border) {
			chunkSize = streamLoader.rget16();
			debug("Border found at %x with size %x", o, chunkSize);

			if (chunkSize == 320*200 / 4)
				colorNumber = 4; // CGA
			else if (chunkSize == 320*200 / 2)
				colorNumber = 16; // EGA
			else if (chunkSize == 320*200)
				colorNumber = 256; // VGA
			else
				error("Unexpected size of image %d", chunkSize);

			raw_border = streamLoader.nextBytes(chunkSize);
			raw_palette = new Common::Array<uint8>();
			debug("Palete follows at %x", streamLoader.getFileOffset());
			for (i = 0; i < colorNumber*3; i++)
				raw_palette->push_back(streamLoader.get8() << 2);

			debug("Palete finishes at %x", streamLoader.getFileOffset());
			chunkSize = streamLoader.rget16();
			debug("Something else of size %x at %x??", chunkSize, streamLoader.getFileOffset());
			streamLoader.skipBytes(chunkSize);
		}
		else {
			debug("skip %x", chunkType);
			//chunkSize = streamLoader.rget16();
			//if (chunkSize > 0 && streamLoader.getFileOffset() + chunkSize-2 == 0x73ea)
			//	error("Found at %x!", o);
			//streamLoader.setFileOffset(o+2);
			//error("Unknown chunk %x find at %x with size %x", chunkType, o, chunkSize);
		}
	}

	if (raw_palette) {
		Graphics::PixelBuffer *palette = new Graphics::PixelBuffer(_gfx->_palettePixelFormat, colorNumber, DisposeAfterUse::NO);
		*palette = raw_palette->data();
		_gfx->_palette = palette;
	}

	if (raw_border) {
		Graphics::PixelBuffer *border = new Graphics::PixelBuffer(_gfx->_originalPixelFormat, 320*200, DisposeAfterUse::NO);
		*border = raw_border->data();
		_border = _gfx->convertFromPalette(border);
	}

	delete[] fileOffsetForArea;
	_playerHeight = playerHeight;
	_startArea = startArea;
	_startEntrance = startEntrance;
	_colorNumber = colorNumber;
	_areasByAreaID = areaMap;
	_scale = Math::Vector3d(1, 1, 1);
	_binaryBits = 16;
	//return Binary{16, startArea, areaMap, raw_border, raw_palette, colorNumber};
}

} // namespace Freescape