#include <fstream>
#include <filesystem>
#include <format>

#include "./include/LevelupClass.h"

#include "./include/common/io_util.h"
#include "./include/common/char_constants.h"
#include "./include/common/copypaste_obj.h"

#include "./imgui.h"

void LevelupClass::write() {
    std::ofstream output;
    output.open(this->_filename, std::ios::binary);

    if (!output.is_open()) {
        throw new std::exception("TB_LVUP.BIN not found to be written!");
    }

    for (size_t characterIndex = 0; characterIndex < this->_levelups.size(); characterIndex++) {
        for (size_t levelupIndex = 0; levelupIndex < maxLevel; levelupIndex++) {
            writeRaw<LevelupStruct>(output, this->_levelups.at(characterIndex).levelups[levelupIndex]);
        }
    }
}

void LevelupClass::read(std::string filename) {
    this->_filename = filename;
    std::ifstream input(this->_filename, std::ios::binary);
    
    if (!input.is_open()) {
        throw new std::exception("TB_LVUP.BIN not found to be read!");
    }

    std::filesystem::path filePath(this->_filename);
    auto fileSize = std::filesystem::file_size(filePath);
    this->_levelups.resize(fileSize / (24 * maxLevel)); // entries are 24 bytes long, and 99 entries per character

    for (size_t characterIndex = 0; characterIndex < this->_levelups.size(); characterIndex++) {
        for (size_t levelupIndex = 0; levelupIndex < maxLevel; levelupIndex++) {
            this->_levelups.at(characterIndex).levelups[levelupIndex] = readRaw<LevelupStruct>(input);
        }
    }

    input.close();
}

void LevelupClass::draw() {
    ImGui::Begin("TB_LVUP");

    if (ImGui::BeginCombo("Character", statIDs[this->_characterIndex])) {
        for (size_t index = 0; index < this->_levelups.size(); index++) {
            ImGui::PushID(index);
            bool is_selected = (index == this->_characterIndex);
            if (ImGui::Selectable(statIDs[index], is_selected)) {
                this->_characterIndex = index;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    if (ImGui::Button("Save")) {
        this->write();
    }

    if (ImGui::BeginCombo("Level", std::format("Level {}", this->_characterLevelupIndex + 1).c_str())) {
        for (size_t index = 0; index < maxLevel; index ++) {
            ImGui::PushID(index);
            bool is_selected = (index == this->_characterLevelupIndex);
            if (ImGui::Selectable(std::format("Level {}", index + 1).c_str(), is_selected)) {
                this->_characterLevelupIndex = index;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }

    ImGui::InputInt("EXP", &this->_levelups.at(this->_characterIndex).levelups[this->_characterLevelupIndex].exp);
    ImGui::InputShort("HP Increase", &this->_levelups.at(this->_characterIndex).levelups[this->_characterLevelupIndex].hpIncrease);
    ImGui::InputShort("MP Increase", &this->_levelups.at(this->_characterIndex).levelups[this->_characterLevelupIndex].mpIncrease);
    ImGui::InputShort("SP Increase", &this->_levelups.at(this->_characterIndex).levelups[this->_characterLevelupIndex].spIncrease);
    ImGui::InputShort("STR Increase", &this->_levelups.at(this->_characterIndex).levelups[this->_characterLevelupIndex].strIncrease);
    ImGui::InputShort("VIT Increase", &this->_levelups.at(this->_characterIndex).levelups[this->_characterLevelupIndex].vitIncrease);
    ImGui::InputShort("ACT Increase", &this->_levelups.at(this->_characterIndex).levelups[this->_characterLevelupIndex].actIncrease);
    ImGui::InputShort("MOV Increase", &this->_levelups.at(this->_characterIndex).levelups[this->_characterLevelupIndex].movIncrease);
    ImGui::InputShort("MAG Increase", &this->_levelups.at(this->_characterIndex).levelups[this->_characterLevelupIndex].magIncrease);
    ImGui::InputShort("MEN Increase", &this->_levelups.at(this->_characterIndex).levelups[this->_characterLevelupIndex].menIncrease);
    ImGui::InputShort("Skill Slot Increase",&this->_levelups.at(this->_characterIndex).levelups[this->_characterLevelupIndex].skillSlotIncrease);

    ImGui::End();
}

void LevelupClass::outputToCSV() {
    std::ofstream output;
    output.open("./csv/TB_LVUP.CSV");

    if (!output.is_open()) {
        return;
    }

    output << "Name,EXP,HP Increase,MP Increase,SP Increase,STR Increase,VIT Increase,ACT Increase,MOV Increase,MAG Increase,MEN Increase,Skill Slot Increase\n";

    for (size_t characterIndex = 0; characterIndex < this->_levelups.size(); characterIndex++) {
        for (size_t characterLevelupIndex = 0; characterLevelupIndex < maxLevel; characterLevelupIndex++) {
            output << statIDs[characterIndex] << ','
                << std::to_string(this->_levelups.at(characterIndex).levelups[characterLevelupIndex].exp) << ','
                << std::to_string(this->_levelups.at(characterIndex).levelups[characterLevelupIndex].hpIncrease) << ','
                << std::to_string(this->_levelups.at(characterIndex).levelups[characterLevelupIndex].mpIncrease) << ','
                << std::to_string(this->_levelups.at(characterIndex).levelups[characterLevelupIndex].spIncrease) << ','
                << std::to_string(this->_levelups.at(characterIndex).levelups[characterLevelupIndex].strIncrease) << ','
                << std::to_string(this->_levelups.at(characterIndex).levelups[characterLevelupIndex].vitIncrease) << ','
                << std::to_string(this->_levelups.at(characterIndex).levelups[characterLevelupIndex].actIncrease) << ','
                << std::to_string(this->_levelups.at(characterIndex).levelups[characterLevelupIndex].movIncrease) << ','
                << std::to_string(this->_levelups.at(characterIndex).levelups[characterLevelupIndex].magIncrease) << ','
                << std::to_string(this->_levelups.at(characterIndex).levelups[characterLevelupIndex].menIncrease) << ','
                << std::to_string(this->_levelups.at(characterIndex).levelups[characterLevelupIndex].skillSlotIncrease) << '\n';
        }
    }

    output.close();
}