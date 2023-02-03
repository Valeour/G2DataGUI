#include <fstream>
#include <filesystem>
#include <random>
#include <format>

#include "./include/MdtsClass.h"

#include "./include/common/io_util.h"
#include "./include/common/string_manip.h"
#include "./include/common/copypaste_obj.h"

#include "./include/JsonDefinitions.h"

#include "./include/common/imgui_wrappers.h"
#include "./imgui.h"

void Mdts::write() {
	std::fstream output;
	uint32_t offset = 0;

	for (size_t i = 0; i < this->_mdts.size(); i++) {
		output.open(this->_mdts.at(i).filename, std::ios::binary | std::ios::in | std::ios::out);

		if (!output.is_open()) {
			throw new std::exception(this->_mdts.at(i).filename.c_str());
		}

		// write in map entries
		output.seekg(this->_mdts.at(i).header.offsetMapEntries, std::ios::beg);
		for (auto& val : this->_mdts.at(i).mapEntries) {
			writeRaw<MapEntriesStruct>(output, val);
		}

		// write in instances
		output.seekg(this->_mdts.at(i).header.offsetInstances, std::ios::beg);
		for (auto& val : this->_mdts.at(i).instances) {
			writeRaw<InstancesStruct>(output, val);
		}

		// write in HTA
		output.seekg(this->_mdts.at(i).header.offsetHTA, std::ios::beg);
		for (auto& val : this->_mdts.at(i).HTA) {
			writeRaw<HTAStruct>(output, val);
		}

		// write in enemy positions
		output.seekg(this->_mdts.at(i).header.offsetEnemyPos, std::ios::beg);
		for (auto& val : this->_mdts.at(i).enemyPositions) {
			writeRaw<EnemyPositionStruct>(output, val);
		}

		// write in enemy groups
		output.seekg(this->_mdts.at(i).header.offsetEnemyGroup, std::ios::beg);
		for (auto& val : this->_mdts.at(i).enemyGroups) {
			writeRaw<EnemyGroupStruct>(output, val);
		}

		// write in MOS
		output.seekg(this->_mdts.at(i).header.offsetMOS, std::ios::beg);
		for (auto& val : this->_mdts.at(i).MOS) {
			writeRaw<MOSStruct>(output, val);
		}

		// write in icons
		output.seekg(this->_mdts.at(i).header.offsetIcons, std::ios::beg);
		for (auto& val : this->_mdts.at(i).icons) {
			writeRaw<IconStruct>(output, val);
		}

		// write in shop
		if (this->_mdts.at(i).shop.size()) {
			for (ImU16 j = 0; j < 12; j++) {
				if (this->_mdts.at(i).shop[0].weapons[j].item) {
					this->_mdts.at(i).shop[0].weapons[j].item += 0x0800;
				}

				if (this->_mdts.at(i).shop[0].armors[j].item) {
					this->_mdts.at(i).shop[0].armors[j].item += 0x0800;
				}

				if (this->_mdts.at(i).shop[0].jewelry[j].item) {
					this->_mdts.at(i).shop[0].jewelry[j].item += 0x0800;
				}

				if (this->_mdts.at(i).shop[0].items[j].item) {
					this->_mdts.at(i).shop[0].items[j].item += 0x0800;
				}

				if (this->_mdts.at(i).shop[0].regionals[j].item) {
					this->_mdts.at(i).shop[0].regionals[j].item += 0x0800;
				}
			}

			output.seekg(this->_mdts[i].header.offsetShop, std::ios::beg);
			writeRaw<ShopStruct>(output, this->_mdts[i].shop[0]);

			for (ImU16 j = 0; j < 12; j++) {
				if (this->_mdts.at(i).shop[0].weapons[j].item) {
					this->_mdts.at(i).shop[0].weapons[j].item -= 0x0800;
				}

				if (this->_mdts.at(i).shop[0].armors[j].item) {
					this->_mdts.at(i).shop[0].armors[j].item -= 0x0800;
				}

				if (this->_mdts.at(i).shop[0].jewelry[j].item) {
					this->_mdts.at(i).shop[0].jewelry[j].item -= 0x0800;
				}

				if (this->_mdts.at(i).shop[0].items[j].item) {
					this->_mdts.at(i).shop[0].items[j].item -= 0x0800;
				}

				if (this->_mdts.at(i).shop[0].regionals[j].item) {
					this->_mdts.at(i).shop[0].regionals[j].item -= 0x0800;
				}
			}
		}

		output.close();
	}
}

void Mdts::read() {
	uint8_t readChar = 0;
	std::ifstream input;
	uint32_t offset = 0;

	for (const auto& p : std::filesystem::directory_iterator(this->_directory)) {
		if (std::filesystem::is_directory(p)) {
			for (const auto& q : std::filesystem::directory_iterator(p)) {
				std::string filename = q.path().string();

				if (!std::strstr(filename.c_str(), ".mdt")) {
					continue;
				}

				input.open(filename.c_str(), std::ios::binary);

				if (!input.is_open()) {
					throw new std::exception(filename.c_str());
				}

				this->_mdts.emplace_back(MdtStruct());
				this->_mdts.back().header = readRaw<mdtHeader>(input);

				// read in map entries
				this->_mdts.back().mapEntries.resize(this->_mdts.back().header.numMapEntries);
				input.seekg(this->_mdts.back().header.offsetMapEntries, std::ios::beg);
				for (size_t i = 0; i < this->_mdts.back().mapEntries.size(); i++) {
					this->_mdts.back().mapEntries[i] = readRaw<MapEntriesStruct>(input);
				}

				// read in instances
				this->_mdts.back().instances.resize(this->_mdts.back().header.numInstances);
				input.seekg(this->_mdts.back().header.offsetInstances, std::ios::beg);
				for (size_t i = 0; i < this->_mdts.back().instances.size(); i++) {
					this->_mdts.back().instances[i] = readRaw<InstancesStruct>(input);
				}

				// read in HTA
				this->_mdts.back().HTA.resize(this->_mdts.back().header.numHTA);
				input.seekg(this->_mdts.back().header.offsetHTA, std::ios::beg);
				for (size_t i = 0; i < this->_mdts.back().HTA.size(); i++) {
					this->_mdts.back().HTA[i] = readRaw<HTAStruct>(input);
				}

				// read in enemy positions
				this->_mdts.back().enemyPositions.resize(this->_mdts.back().header.numEnemyPos);
				input.seekg(this->_mdts.back().header.offsetEnemyPos, std::ios::beg);
				for (size_t i = 0; i < this->_mdts.back().enemyPositions.size(); i++) {
					this->_mdts.back().enemyPositions[i] = readRaw<EnemyPositionStruct>(input);
				}

				// read in enemy groups
				this->_mdts.back().enemyGroups.resize(this->_mdts.back().header.numEnemyGroup);
				input.seekg(this->_mdts.back().header.offsetEnemyGroup, std::ios::beg);
				for (size_t i = 0; i < this->_mdts.back().enemyGroups.size(); i++) {
					this->_mdts.back().enemyGroups[i] = readRaw<EnemyGroupStruct>(input);
				}

				// read in MOS
				this->_mdts.back().MOS.resize(this->_mdts.back().header.numMOS);
				input.seekg(this->_mdts.back().header.offsetMOS, std::ios::beg);
				for (size_t i = 0; i < this->_mdts.back().MOS.size(); i++)
					this->_mdts.back().MOS[i] = readRaw<MOSStruct>(input);

				// read in icons
				this->_mdts.back().icons.resize(this->_mdts.back().header.numIcons);
				input.seekg(this->_mdts.back().header.offsetIcons, std::ios::beg);
				for (size_t i = 0; i < this->_mdts.back().icons.size(); i++) {
					this->_mdts.back().icons[i] = readRaw<IconStruct>(input);
				}


				// read in shop
				if (this->_mdts.back().header.offsetShop) {
					this->_mdts.back().shop.resize(1);
					input.seekg(this->_mdts.back().header.offsetShop, std::ios::beg);

					this->_mdts.back().shop[0] = readRaw<ShopStruct>(input);

					for (ImU16 i = 0; i < 12; i++) {
						if (this->_mdts.back().shop[0].weapons[i].item) {
							this->_mdts.back().shop[0].weapons[i].item -= 0x0800;
						}

						if (this->_mdts.back().shop[0].armors[i].item) {
							this->_mdts.back().shop[0].armors[i].item -= 0x0800;
						}

						if (this->_mdts.back().shop[0].jewelry[i].item) {
							this->_mdts.back().shop[0].jewelry[i].item -= 0x0800;
						}

						if (this->_mdts.back().shop[0].items[i].item) {
							this->_mdts.back().shop[0].items[i].item -= 0x0800;
						}

						if (this->_mdts.back().shop[0].regionals[i].item) {
							this->_mdts.back().shop[0].regionals[i].item -= 0x0800;
						}
					}
				}
				
				input.seekg(0x78, std::ios::beg);
				uint32_t sectionLength = readRaw<uint32_t>(input);
				uint32_t sectionOffset = readRaw<uint32_t>(input);

				// dialogue section exists, lets do our thing
				if (sectionLength && sectionOffset) {
					input.seekg(sectionOffset, std::ios::beg);
					uint32_t diagHeaderSize = readRaw<uint32_t>(input);
					input.seekg(sectionOffset, std::ios::beg);

					for (size_t offset = 10; offset < diagHeaderSize; offset += 4) {
						input.seekg(offset, std::ios::cur);
						uint16_t diagOffset = readRaw<uint16_t>(input);
						// check if it's padding, if so, ignore it
						if (diagOffset == 0xFFFF) {
							continue;
						}
						input.seekg(sectionOffset + diagHeaderSize, std::ios::beg);
						input.seekg(diagOffset * 8, std::ios::cur);

						// check if it's the proper bytecode for a map name textbox
						if ((readRaw<uint8_t>(input) == (uint8_t)0x17) && (readRaw<uint8_t>(input) == (uint8_t)0x80)) {
							input.seekg(2, std::ios::cur);
							readChar = readRaw<uint8_t>(input);
							while (readChar != 0x17) {
								this->_mdts.back().mapname.push_back(readChar);
								readChar = readRaw<uint8_t>(input);
							}

							// anything that contains the string "to " is not the current map name
							if (this->_mdts.back().mapname.find("to ") != std::string::npos) {
								this->_mdts.back().mapname.erase(this->_mdts.back().mapname.begin(), this->_mdts.back().mapname.end());
								input.seekg(sectionOffset, std::ios::beg);
								continue;
							}
							else break;

						}
						else {
							input.seekg(sectionOffset, std::ios::beg);
						}

					}

				}

				this->_mdts.back().filename = filename;
				this->_mdts.back().filenameChr = filename.substr(0, filename.length() - 4) + ".chr";

				input.close();
			}
		}
	}
}

void Mdts::draw() {
	ImGui::Begin("AREAS");

	if (ImGui::Button("Save")) {
		this->write();
	}

	if (ImGui::BeginCombo("MDT Index", this->_mdts.at(this->_mdtIndex).mapname.c_str())) {
		for (size_t i = 0; i < this->_mdts.size(); i++) {
			ImGui::PushID((int)i);
			bool is_selected = (i == this->_mdtIndex);
			if (ImGui::Selectable(this->_mdts.at(i).mapname.c_str(), is_selected)) {
				this->_mdtIndex = i;
				this->_mapEntryIndex = 0;
				this->_instanceIndex = 0;
				this->_htaIndex = 0;
				this->_ePosIndex = 0;
				this->_eGroupIndex = 0;
				this->_eGroupPosIndex = 0;
				this->_mosIndex = 0;
				this->_iconIndex = 0;
				this->_shopIndex = 0;
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
			ImGui::PopID();
		}

		ImGui::EndCombo();
	}

	if (this->_mdts.at(this->_mdtIndex).mapEntries.size()) {
		if (ImGui::CollapsingHeader("Map Entries")) {
			if (ImGui::BeginCombo("Map Entry #", std::to_string(this->_mdts.at(this->_mdtIndex).mapEntries[this->_mapEntryIndex].ID).c_str())) {
				for (size_t i = 0; i < this->_mdts.at(this->_mdtIndex).mapEntries.size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_mapEntryIndex);
					if (ImGui::Selectable(std::to_string(this->_mdts.at(this->_mdtIndex).mapEntries[i].ID).c_str(), is_selected)) {
						this->_mapEntryIndex = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			drawInput("Connected Map", &this->_mdts.at(this->_mdtIndex).mapEntries[this->_mapEntryIndex].ID);
			drawInputN("X/Y/Z Position", &this->_mdts.at(this->_mdtIndex).mapEntries[this->_mapEntryIndex].xPos, 3);
			drawInput("Direction", &this->_mdts.at(this->_mdtIndex).mapEntries[this->_mapEntryIndex].direction);
			drawInput("Unknown #1", &this->_mdts.at(this->_mdtIndex).mapEntries[this->_mapEntryIndex].unknown);
			drawInput("Unknown #2", &this->_mdts.at(this->_mdtIndex).mapEntries[this->_mapEntryIndex].unknown1);
			drawInput("Unknown #3", &this->_mdts.at(this->_mdtIndex).mapEntries[this->_mapEntryIndex].unknown2);
		}
	}

	if (this->_mdts.at(this->_mdtIndex).instances.size()) {
		if (ImGui::CollapsingHeader("Instances")) {
			if (ImGui::BeginCombo("Instance #", std::to_string(this->_mdts.at(this->_mdtIndex).instances[this->_instanceIndex].ID).c_str())) {
				for (size_t i = 0; i < this->_mdts.at(this->_mdtIndex).mapEntries.size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_instanceIndex);
					if (ImGui::Selectable(std::to_string(this->_mdts.at(this->_mdtIndex).instances[i].ID).c_str(), is_selected)) {
						this->_instanceIndex = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			drawInput("ID", &this->_mdts.at(this->_mdtIndex).instances[this->_instanceIndex].ID);
			drawInput("Index", &this->_mdts.at(this->_mdtIndex).instances[this->_instanceIndex].index);
			drawInput("Unknown", &this->_mdts.at(this->_mdtIndex).instances[this->_instanceIndex].unknown);
			drawInput("Translation", &this->_mdts.at(this->_mdtIndex).instances[this->_instanceIndex].translation);
			drawInputN("X/Y/Z Position", &this->_mdts.at(this->_mdtIndex).instances[this->_instanceIndex].xPos, 3);
			drawInputN("X/Y/Z Angle", &this->_mdts.at(this->_mdtIndex).instances[this->_instanceIndex].xAngle, 3);
			drawInputN("CX/CY/CZ", &this->_mdts.at(this->_mdtIndex).instances[this->_instanceIndex].cx, 3);
		}
	}

	if (this->_mdts.at(this->_mdtIndex).HTA.size()) {
		if (ImGui::CollapsingHeader("HTA")) {
			if (ImGui::BeginCombo("HTA #", std::format("HTA {}", this->_htaIndex + 1).c_str())) {
				for (size_t i = 0; i < this->_mdts.at(this->_mdtIndex).HTA.size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_htaIndex);
					if (ImGui::Selectable(std::format("HTA {}", i + 1).c_str(), is_selected)) {
						this->_htaIndex = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			drawInput("Shape", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].shape);
			drawInput("Type", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].type);
			drawInput("Trigger", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].trigger);
			drawInput("Unknown #1", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].unknown);
			drawInput("Unknown #2", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].unknown1);
			drawInputN("X/Y/Z Min", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].xMin, 3);
			drawInputN("X/Y/Z Max", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].xMax, 3);
			drawInput("Unknown #3", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].unknown2);
			drawInput("Unknown #4", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].unknown3);
			drawInput("Unknown #5", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].unknown4);
			drawInput("Unknown #6", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].unknown5);
			drawInput("Unknown #7", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].unknown6);
			drawInput("Unknown #8", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].unknown7);
			drawInput("Unknown #9", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].unknown8);
			drawInput("Unknown #10", &this->_mdts.at(this->_mdtIndex).HTA[this->_htaIndex].unknown9);
		}
	}

	if (this->_mdts.at(this->_mdtIndex).enemyPositions.size()) {
		if (ImGui::CollapsingHeader("Enemy Positions")) {
			if (ImGui::BeginCombo("Enemy Pos #", std::format("Enemy Position {}", this->_ePosIndex + 1).c_str())) {
				for (size_t i = 0; i < this->_mdts.at(this->_mdtIndex).enemyPositions.size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_ePosIndex);
					if (ImGui::Selectable(std::format("Enemy Position {}", i + 1).c_str(), is_selected)) {
						this->_ePosIndex = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			drawInput("Index", &this->_mdts.at(this->_mdtIndex).enemyPositions[this->_ePosIndex].index);
			drawInput("Unknown #1", &this->_mdts.at(this->_mdtIndex).enemyPositions[this->_ePosIndex].unknown);
			drawInputN("X/Z Min", &this->_mdts.at(this->_mdtIndex).enemyPositions[this->_ePosIndex].xPosMin, 2);
			drawInputN("X/Z Max", &this->_mdts.at(this->_mdtIndex).enemyPositions[this->_ePosIndex].xPosMax, 2);
			drawInputN("X/Y/Z Pos", &this->_mdts.at(this->_mdtIndex).enemyPositions[this->_ePosIndex].xPos, 3);
			drawInputN("X/Y/Z #1", &this->_mdts.at(this->_mdtIndex).enemyPositions[this->_ePosIndex].unknownX, 3);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Unknown");
			drawInputN("X/Y/Z #2", &this->_mdts.at(this->_mdtIndex).enemyPositions[this->_ePosIndex].unknownX1, 3);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Unknown");
			drawInputN("X/Y/Z #3", &this->_mdts.at(this->_mdtIndex).enemyPositions[this->_ePosIndex].unknownX2, 3);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Unknown");
		}
	}

	if (this->_mdts.at(this->_mdtIndex).enemyGroups.size()) {
		if (ImGui::CollapsingHeader("Enemy Groups")) {
			if (ImGui::BeginCombo("Enemy Group #", std::format("Enemy Group {}", this->_eGroupIndex + 1).c_str())) {
				for (size_t i = 0; i < this->_mdts.at(this->_mdtIndex).enemyGroups.size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_eGroupIndex);
					if (ImGui::Selectable(std::format("Enemy Group {}", i + 1).c_str(), is_selected)) {
						this->_eGroupIndex = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			drawInput("Group Index", &this->_mdts.at(this->_mdtIndex).enemyGroups[this->_eGroupIndex].index);

			if (ImGui::BeginCombo("Enemy #", std::format("Enemy {}", this->_eGroupPosIndex + 1).c_str())) {
				// there can only ever be 4 unique enemies in a group, as far as I can tell
				for (size_t i = 0; i < 4; i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_eGroupPosIndex);
					if (ImGui::Selectable(std::format("Enemy {}", i+ 1).c_str(), is_selected)) {
						this->_eGroupPosIndex = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			drawInput("Enemy Index", &this->_mdts.at(this->_mdtIndex).enemyGroups[this->_eGroupIndex].enemies[this->_eGroupPosIndex].index);
			drawInput("# of Enemy", &this->_mdts.at(this->_mdtIndex).enemyGroups[this->_eGroupIndex].enemies[this->_eGroupPosIndex].numEnemy);
			drawInput("Enemy Offset", &this->_mdts.at(this->_mdtIndex).enemyGroups[this->_eGroupIndex].enemies[this->_eGroupPosIndex].enemyOffset);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("References enemy/boss.csv");
			drawInput("Unknown #1", &this->_mdts.at(this->_mdtIndex).enemyGroups[this->_eGroupIndex].enemies[this->_eGroupPosIndex].unknown);
			drawInput("Unknown #2", &this->_mdts.at(this->_mdtIndex).enemyGroups[this->_eGroupIndex].enemies[this->_eGroupPosIndex].unknown1);
			drawInput("Unknown #3", &this->_mdts.at(this->_mdtIndex).enemyGroups[this->_eGroupIndex].enemies[this->_eGroupPosIndex].unknown2);
			drawInput("Unknown #4", &this->_mdts.at(this->_mdtIndex).enemyGroups[this->_eGroupIndex].enemies[this->_eGroupPosIndex].unknown3);
		}
	}

	if (this->_mdts.at(this->_mdtIndex).MOS.size()) {
		if (ImGui::CollapsingHeader("MOS")) {
			if (ImGui::BeginCombo("MOS #", std::format("MOS {}", this->_mosIndex + 1).c_str())) {
				for (size_t i = 0; i < this->_mdts.at(this->_mdtIndex).MOS.size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_mosIndex);
					if (ImGui::Selectable(std::format("MOS {}", i + 1).c_str(), is_selected)) {
						this->_mosIndex = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			drawInput("ID", &this->_mdts.at(this->_mdtIndex).MOS[this->_mosIndex].ID);
			drawInput("Index", &this->_mdts.at(this->_mdtIndex).MOS[this->_mosIndex].index);
			drawInput("Unknown #1", &this->_mdts.at(this->_mdtIndex).MOS[this->_mosIndex].unknown);
			drawInputN("X/Y/Z Pos", &this->_mdts.at(this->_mdtIndex).MOS[this->_mosIndex].xPos, 3);
			drawInput("Unknown #2", &this->_mdts.at(this->_mdtIndex).MOS[this->_mosIndex].unknown1);
			drawInput("Unknown #3", &this->_mdts.at(this->_mdtIndex).MOS[this->_mosIndex].unknown2);
			drawInput("Unknown #4", &this->_mdts.at(this->_mdtIndex).MOS[this->_mosIndex].unknown3);
			drawInput("Unknown #5", &this->_mdts.at(this->_mdtIndex).MOS[this->_mosIndex].unknown4);
			drawInput("Unknown #6", &this->_mdts.at(this->_mdtIndex).MOS[this->_mosIndex].unknown5);
		}
	}

	if (this->_mdts.at(this->_mdtIndex).icons.size()) {
		if (ImGui::CollapsingHeader("Icons")) {
			if (ImGui::BeginCombo("Icon #", std::format("Icon {}", this->_iconIndex + 1).c_str())) {
				for (size_t i = 0; i < this->_mdts.at(this->_mdtIndex).icons.size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_iconIndex);
					if (ImGui::Selectable(std::format("Icon {}", i + 1).c_str(), is_selected)) {
						this->_iconIndex = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			drawInput("ID", &this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].ID);
			drawInput("Unknown #1", &this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].unknown);
			drawInput("Unknown #2", &this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].unknown1);
			drawInputN("X/Y/Z Pos", &this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].xPos, 3);
			drawInput("Unknown #3", &this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].unknown2);
			drawInput("Y Angle", &this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].yAngle);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Unsure");

			if (ImGui::BeginCombo("Item #1", this->_items->at(this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].item1).name)) {
				for (size_t i = 0; i < this->_items->size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].item1);
					if (ImGui::Selectable(this->_items->at(i).name, is_selected)) {
						this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].item1 = (uint16_t)i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Item #2", this->_items->at(this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].item2).name)) {
				for (size_t i = 0; i < this->_items->size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].item2);
					if (ImGui::Selectable(this->_items->at(i).name, is_selected))
						this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].item2 = (uint16_t)i;
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Item #3", this->_items->at(this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].item3).name)) {

				for (size_t i = 0; i < this->_items->size(); i++) {

					ImGui::PushID((int)i);
					bool is_selected = (i == this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].item3);
					if (ImGui::Selectable(this->_items->at(i).name, is_selected)) {
						this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].item3 = (uint16_t)i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			drawInput("Flag", &this->_mdts.at(this->_mdtIndex).icons[this->_iconIndex].flag);
		}
	}

	if (this->_mdts.at(this->_mdtIndex).shop.size()) {
		if (ImGui::CollapsingHeader("Shops")) {
			if (ImGui::BeginCombo("Weapons #", std::format("Weapons {}", this->_shopWeaponIndex + 1).c_str())) {
				for (size_t i = 0; i < 12; i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_shopWeaponIndex);
					if (ImGui::Selectable(std::format("Weapons {}", i + 1).c_str(), is_selected)) {
						this->_shopWeaponIndex = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Weapon Item", this->_items->at(this->_mdts.at(this->_mdtIndex).shop[0].weapons[this->_shopWeaponIndex].item).name)) {
				for (size_t i = 0; i < this->_items->size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_mdts.at(this->_mdtIndex).shop[0].weapons[this->_shopWeaponIndex].item);
					if (ImGui::Selectable(this->_items->at(i).name, is_selected)) {
						this->_mdts.at(this->_mdtIndex).shop[0].weapons[this->_shopWeaponIndex].item = (uint16_t)i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Armors #", std::format("Armors {}", this->_shopArmorIndex + 1).c_str())) {
				for (size_t i = 0; i < 12; i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_shopArmorIndex);
					if (ImGui::Selectable(std::format("Armors {}", i + 1).c_str(), is_selected)) {
						this->_shopArmorIndex = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Armor Item", this->_items->at(this->_mdts.at(this->_mdtIndex).shop[0].armors[this->_shopArmorIndex].item).name)) {
				for (size_t i = 0; i < this->_items->size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_mdts.at(this->_mdtIndex).shop[0].armors[this->_shopArmorIndex].item);
					if (ImGui::Selectable(this->_items->at(i).name, is_selected)) {
						this->_mdts.at(this->_mdtIndex).shop[0].armors[this->_shopArmorIndex].item = (uint16_t)i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Jewelry #", std::format("Jewelry {}", this->_shopJewelIndex + 1).c_str())) {
				for (size_t i = 0; i < 12; i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_shopJewelIndex);
					if (ImGui::Selectable(std::format("Jewelry {}", i + 1).c_str(), is_selected)) {
						this->_shopJewelIndex = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Jewelry Item", this->_items->at(this->_mdts.at(this->_mdtIndex).shop[0].jewelry[this->_shopJewelIndex].item).name)) {
				for (size_t i = 0; i < this->_items->size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_mdts.at(this->_mdtIndex).shop[0].jewelry[this->_shopJewelIndex].item);
					if (ImGui::Selectable(this->_items->at(i).name, is_selected)) {
						this->_mdts.at(this->_mdtIndex).shop[0].jewelry[this->_shopJewelIndex].item = (uint16_t)i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Items #", std::format("Item {}", this->_shopItemIndex + 1).c_str())) {
				for (size_t i = 0; i < 12; i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_shopItemIndex);
					if (ImGui::Selectable(std::format("Item {}", i + 1).c_str(), is_selected)) {
						this->_shopItemIndex = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Item Item", this->_items->at(this->_mdts.at(this->_mdtIndex).shop[0].items[this->_shopItemIndex].item).name)) {
				for (size_t i = 0; i < this->_items->size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_mdts.at(this->_mdtIndex).shop[0].items[this->_shopItemIndex].item);
					if (ImGui::Selectable(this->_items->at(i).name, is_selected)) {
						this->_mdts.at(this->_mdtIndex).shop[0].items[this->_shopItemIndex].item = (uint16_t)i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Regionals #", std::format("Regionals {}", this->_shopRegionalIndex + 1).c_str())) {
				for (size_t i = 0; i < 12; i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_shopRegionalIndex);
					if (ImGui::Selectable(std::format("Regionals {}", i + 1).c_str(), is_selected)) {
						this->_shopRegionalIndex = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Regional Item", this->_items->at(this->_mdts.at(this->_mdtIndex).shop[0].regionals[this->_shopRegionalIndex].item).name)) {
				for (size_t i = 0; i < this->_items->size(); i++) {
					ImGui::PushID((int)i);
					bool is_selected = (i == this->_mdts.at(this->_mdtIndex).shop[0].regionals[this->_shopRegionalIndex].item);
					if (ImGui::Selectable(this->_items->at(i).name, is_selected)) {
						this->_mdts.at(this->_mdtIndex).shop[0].regionals[this->_shopRegionalIndex].item = (uint16_t)i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}
		}
	}

	// we are ignoring models for now until we understand the NJCM format
	//drawModel(models, &mdt[mapID], canClose);

	ImGui::End();

}

void Mdts::outputToCSV() {
	std::ofstream output;
	output.open("./csv/MAP_NAMES.CSV");
	output << "File,Map\n";

	for (const auto& val : this->_mdts) {
		output << val.filename << ",\"" << val.mapname << "\"\n";
	}

	output.close();
}

void Mdts::randomize() {
	std::random_device rd;
	std::mt19937 g(rd());

	for (auto& mdt : this->_mdts) {
		for (auto& icon : mdt.icons) {
			if (icon.item1) {
				do {
					icon.item1 = (uint16_t)(g() % this->_items->size());
				} while (std::string(this->_items->at(icon.item1).name).find_first_not_of(' ') == std::string::npos);
			}

			if (icon.item2) {
				do {
					icon.item2 = (uint16_t)(g() % this->_items->size());
				} while (std::string(this->_items->at(icon.item2).name).find_first_not_of(' ') == std::string::npos);

			}

			if (icon.item3) {
				do {
					icon.item3 = (uint16_t)(g() % this->_items->size());
				} while (std::string(this->_items->at(icon.item3).name).find_first_not_of(' ') == std::string::npos);
			}
		}

		for (auto& shop : mdt.shop) {
			// weapons
			for (size_t i = 0; i < 12; i++) {
				do {
					shop.weapons[i].item = 300 + g() % 200;
				} while (std::string(this->_items->at(shop.weapons[i].item).name).find_first_not_of(' ') == std::string::npos);
			}

			//	armors
			for (size_t i = 0; i < 12; i++) {
				do {
					shop.armors[i].item = 500 + g() % 200;
				} while (std::string(this->_items->at(shop.armors[i].item).name).find_first_not_of(' ') == std::string::npos);
			}

			//	accessories
			for (size_t i = 0; i < 12; i++) {
				do {
					shop.jewelry[i].item = 700 + g() % 99;
				} while (std::string(this->_items->at(shop.jewelry[i].item).name).find_first_not_of(' ') == std::string::npos);
			}

			//	items
			for (size_t i = 0; i < 12; i++) {
				do {
					shop.items[i].item = 100 + g() % 100;
				} while (std::string(this->_items->at(shop.items[i].item).name).find_first_not_of(' ') == std::string::npos);
			}

			//	regionals
			for (size_t i = 0; i < 12; i++) {
				do {
					shop.regionals[i].item = 200 + g() % 100;
				} while (std::string(this->_items->at(shop.regionals[i].item).name).find_first_not_of(' ') == std::string::npos);
			}
		}
	}
}