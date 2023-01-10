#pragma once

#include <vector>

#include "./common/version_check.h"

#include "BaseDataClass.h"
#include "MovesClass.h"

#pragma pack(1)
struct EquipmentStruct {
	uint16_t characterBitflag = 0b00000000;
	int16_t str = 0;
	int16_t vit = 0;
	int16_t act = 0;
	int16_t mov = 0;
	uint8_t effectiveOn = 0;
	int8_t fireAffinity = 0;
	int8_t windAffinity = 0;
	int8_t earthAffinity = 0;
	int8_t lightningAffinity = 0;
	int8_t blizzardAffinity = 0;
	uint8_t ailmentsBitflag = 0b00000000;
	uint8_t ailmentsChance = 0;
	int8_t increaseFirePercent = 0;
	int8_t increaseWindPercent = 0;
	int8_t increaseEarthPercent = 0;
	int8_t increaseLightningPercent = 0;
	int8_t increaseBlizzardPercent = 0;
	int8_t increaseWaterPercent = 0;
	int8_t increaseExplosionPercent = 0;
	int8_t increaseForestPercent = 0;
	uint16_t special = 0;
};
#pragma pack()

#pragma pack(1)
struct UsableStruct {
	uint8_t targetEffect = 0;
	uint8_t targetType = 0;
	uint16_t power = 0;
	uint16_t range = 0;
	uint16_t castTime = 0;
	uint16_t recoveryTime = 0;
	uint16_t animation = 0;
	uint8_t effectiveOn = 0;
	uint8_t unknown1 = 0;
	int16_t ipDamage = 0;
	int16_t ipCancelDamage = 0;
	int16_t knockback = 0;
	uint8_t element = 0;
	uint8_t elementStr = 0;
	uint8_t ailmentsBitflag = 0b00000000;
	uint8_t ailmentsChance = 0;
	int8_t atkMod = 0;
	int8_t defMod = 0;
	int8_t actMod = 0;
	int8_t movMod = 0;
	uint8_t breakChance = 0;
	uint8_t special = 0;
	uint8_t unknown2 = 0;
	uint8_t unknown3 = 0;
};
#pragma pack()

#pragma pack(1)
struct ItemStatsStruct {
	uint8_t entryType = 0;
	uint8_t unknown1 = 0;
	uint8_t unknown2 = 0;
	uint8_t unknown3 = 0;
	uint8_t icon = 0;
	uint8_t unknown4 = 0;
	uint32_t price = 0;
};
#pragma pack()

//	the game only has 700 items total, and this is split into segments with 100 entries(items) each
struct ItemStruct {
	char name[19];
	char description[41];
	ItemStatsStruct stats;
	EquipmentStruct* equipmentOffset = nullptr;	// 0xFFFFFFFF means null
	UsableStruct* usableOffset = nullptr;		// 0xFFFFFFFF means null
	uint32_t id = 0;							// overwrite this to array index, index is used for overworld items and enemy drops
};


class Items : public BaseDataClass {
public:
	Items(const Items&) = delete;
    Items(const Items&&) = delete;

	static Items& getInstance() {
		static Items instance;
		return instance;
	};

	void write();
	void read();
	void draw();
	void outputToCSV();

	void storeMoves(const std::vector<MoveStruct>* moves) { _moves = moves; };
	const std::vector<ItemStruct>* getItems() { return &_items; };

private:
	Items() {
		this->_filename = (Version::getInstance().isHDVersion() ? "./content/data/afs/xls_data/ITEM.BIN" : "./data/afs/xls_data/ITEM.BIN");
	};
	std::vector<ItemStruct> _items;
	size_t _itemIndex = 0;
	bool _hasEquip = false;
	bool _hasUsable = false;
	bool EquipmentCharacterBitFlags[7] = {};
	bool EquipmentAilmentBitFlags[8] = {};
	bool UsableAilmentBitFlags[8] = {};

	const std::vector<MoveStruct>* _moves = nullptr;
};