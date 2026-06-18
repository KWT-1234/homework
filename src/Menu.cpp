#include "Menu.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

bool Menu::loadFromFile(const std::string& filename) {
    items.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    std::vector<std::pair<std::shared_ptr<ComboItem>, std::string>> pendingCombos;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string type, idStr, nameStr, priceOrDiscountStr;
        
        std::getline(ss, type, ',');
        std::getline(ss, idStr, ',');
        std::getline(ss, nameStr, ',');
        std::getline(ss, priceOrDiscountStr, ',');

        int id = std::stoi(idStr);
        double val = std::stod(priceOrDiscountStr);

        if (type == "Food") {
            std::string spicyStr;
            std::getline(ss, spicyStr, ',');
            int spicy = spicyStr.empty() ? 0 : std::stoi(spicyStr);
            auto food = std::make_shared<FoodItem>(id, nameStr, val, spicy);
            
            std::string extrasStr;
            if (std::getline(ss, extrasStr, ',')) {
                std::stringstream ess(extrasStr);
                std::string extra;
                while (std::getline(ess, extra, '|')) {
                    if (!extra.empty()) {
                        food->addExtraIngredient(extra);
                    }
                }
            }
            items.push_back(food);
        }
        else if (type == "Drink") {
            std::string ice, sweetness;
            std::getline(ss, ice, ',');
            std::getline(ss, sweetness, ',');
            auto drink = std::make_shared<DrinkItem>(id, nameStr, val, ice, sweetness);
            items.push_back(drink);
        }
        else if (type == "Combo") {
            auto combo = std::make_shared<ComboItem>(id, nameStr, val);
            std::string subIdsStr;
            std::getline(ss, subIdsStr, ',');
            pendingCombos.push_back({combo, subIdsStr});
        }
    }
    file.close();

    // Second pass: Resolve Combo sub-items
    for (auto& pair : pendingCombos) {
        auto combo = pair.first;
        std::string subIdsStr = pair.second;
        std::stringstream ss(subIdsStr);
        std::string idToken;
        while (std::getline(ss, idToken, '|')) {
            if (idToken.empty()) continue;
            int subId = std::stoi(idToken);
            auto subItem = findItemById(subId);
            if (subItem) {
                // Clone the menu template item to avoid sharing state
                combo->addSubItem(subItem->clone());
            }
        }
        items.push_back(combo);
    }

    return true;
}

bool Menu::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "# Type,ID,Name,Price/Discount,Extra1,Extra2\n";
    for (const auto& item : items) {
        file << item->getCsvString() << "\n";
    }
    file.close();
    return true;
}

std::shared_ptr<MenuItem> Menu::findItemById(int id) const {
    auto it = std::find_if(items.begin(), items.end(), [id](const std::shared_ptr<MenuItem>& item) {
        return item->getId() == id;
    });
    if (it != items.end()) {
        return *it;
    }
    return nullptr;
}

const std::vector<std::shared_ptr<MenuItem>>& Menu::getItems() const {
    return items;
}

void Menu::addItem(const std::shared_ptr<MenuItem>& item) {
    items.push_back(item);
}

bool Menu::removeItem(int id) {
    auto it = std::remove_if(items.begin(), items.end(), [id](const std::shared_ptr<MenuItem>& item) {
        return item->getId() == id;
    });
    if (it != items.end()) {
        items.erase(it, items.end());
        return true;
    }
    return false;
}

int Menu::getNextAvailableId() const {
    int maxId = 0;
    for (const auto& item : items) {
        if (item->getId() > maxId) {
            maxId = item->getId();
        }
    }
    return maxId + 1;
}
