#ifndef MENU_HPP
#define MENU_HPP

#include "MenuItem.hpp"
#include <vector>
#include <memory>
#include <string>

class Menu {
private:
    std::vector<std::shared_ptr<MenuItem>> items;

public:
    Menu() = default;

    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;

    std::shared_ptr<MenuItem> findItemById(int id) const;
    const std::vector<std::shared_ptr<MenuItem>>& getItems() const;
    
    void addItem(const std::shared_ptr<MenuItem>& item);
    bool removeItem(int id);
    int getNextAvailableId() const;
};

#endif // MENU_HPP
