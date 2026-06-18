#ifndef MENU_ITEM_HPP
#define MENU_ITEM_HPP

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>

class MenuItem {
protected:
    int id;
    std::string name;
    double price;
    std::string category; // "Food", "Drink", "Combo", etc.

public:
    MenuItem(int id, const std::string& name, double price, const std::string& category)
        : id(id), name(name), price(price), category(category) {}

    virtual ~MenuItem() = default;

    int getId() const { return id; }
    std::string getName() const { return name; }
    virtual double getPrice() const { return price; }
    std::string getCategory() const { return category; }

    void setName(const std::string& newName) { name = newName; }
    void setPrice(double newPrice) { price = newPrice; }

    virtual std::string getDescription() const = 0;
    virtual std::string getCsvString() const = 0;
    virtual std::shared_ptr<MenuItem> clone() const = 0;
};

// Derived Food class
class FoodItem : public MenuItem {
private:
    int spicyLevel; // 0: 不辣, 1: 微辣, 2: 中辣, 3: 大辣
    std::vector<std::string> extraIngredients;

public:
    FoodItem(int id, const std::string& name, double price, int spicyLevel = 0)
        : MenuItem(id, name, price, "Food"), spicyLevel(spicyLevel) {}

    int getSpicyLevel() const { return spicyLevel; }
    void setSpicyLevel(int level) { spicyLevel = level; }
    
    const std::vector<std::string>& getExtraIngredients() const { return extraIngredients; }
    void addExtraIngredient(const std::string& ingredient) { extraIngredients.push_back(ingredient); }
    void clearExtraIngredients() { extraIngredients.clear(); }

    std::string getDescription() const override {
        std::stringstream ss;
        ss << name << " [單價: $" << std::fixed << std::setprecision(0) << price << "]";
        if (spicyLevel > 0) {
            std::string spicyText[] = {"不辣", "微辣", "中辣", "大辣"};
            ss << " (辣度: " << spicyText[spicyLevel] << ")";
        }
        if (!extraIngredients.empty()) {
            ss << " [加料: ";
            for (size_t i = 0; i < extraIngredients.size(); ++i) {
                ss << extraIngredients[i] << (i == extraIngredients.size() - 1 ? "" : ", ");
            }
            ss << "]";
        }
        return ss.str();
    }

    std::string getCsvString() const override {
        std::stringstream ss;
        ss << "Food," << id << "," << name << "," << price << "," << spicyLevel << ",";
        for (size_t i = 0; i < extraIngredients.size(); ++i) {
            ss << extraIngredients[i] << (i == extraIngredients.size() - 1 ? "" : "|");
        }
        return ss.str();
    }

    std::shared_ptr<MenuItem> clone() const override {
        auto copy = std::make_shared<FoodItem>(id, name, price, spicyLevel);
        copy->extraIngredients = this->extraIngredients;
        return copy;
    }
};

// Derived Drink class
class DrinkItem : public MenuItem {
private:
    std::string iceLevel;       // 去冰、微冰、少冰、正常冰
    std::string sweetnessLevel; // 無糖、微糖、半糖、少糖、正常甜

public:
    DrinkItem(int id, const std::string& name, double price, 
              const std::string& ice = "正常冰", const std::string& sweetness = "正常甜")
        : MenuItem(id, name, price, "Drink"), iceLevel(ice), sweetnessLevel(sweetness) {}

    std::string getIceLevel() const { return iceLevel; }
    void setIceLevel(const std::string& ice) { iceLevel = ice; }

    std::string getSweetnessLevel() const { return sweetnessLevel; }
    void setSweetnessLevel(const std::string& sweetness) { sweetnessLevel = sweetness; }

    std::string getDescription() const override {
        std::stringstream ss;
        ss << name << " [單價: $" << std::fixed << std::setprecision(0) << price << "] (" 
           << sweetnessLevel << "/" << iceLevel << ")";
        return ss.str();
    }

    std::string getCsvString() const override {
        std::stringstream ss;
        ss << "Drink," << id << "," << name << "," << price << "," << iceLevel << "," << sweetnessLevel;
        return ss.str();
    }

    std::shared_ptr<MenuItem> clone() const override {
        return std::make_shared<DrinkItem>(id, name, price, iceLevel, sweetnessLevel);
    }
};

// Derived Combo class
class ComboItem : public MenuItem {
private:
    std::vector<std::shared_ptr<MenuItem>> subItems;
    double discountRate; // 例如 0.9 表示打九折

public:
    ComboItem(int id, const std::string& name, double discountRate = 0.9)
        : MenuItem(id, name, 0.0, "Combo"), discountRate(discountRate) {}

    void addSubItem(const std::shared_ptr<MenuItem>& item) {
        subItems.push_back(item);
    }
    
    const std::vector<std::shared_ptr<MenuItem>>& getSubItems() const {
        return subItems;
    }

    void clearSubItems() {
        subItems.clear();
    }

    double getDiscountRate() const { return discountRate; }
    void setDiscountRate(double rate) { discountRate = rate; }

    double getPrice() const override {
        double total = 0.0;
        for (const auto& item : subItems) {
            total += item->getPrice();
        }
        return total * discountRate;
    }

    std::string getDescription() const override {
        std::stringstream ss;
        ss << name << " [套餐特價: $" << std::fixed << std::setprecision(0) << getPrice() 
           << " (打 " << (discountRate * 10) << " 折)]\n     內容物:";
        
        bool hasFood = false;
        for (const auto& item : subItems) {
            if (item->getCategory() == "Food") {
                if (!hasFood) {
                    ss << "\n       [套餐食物]";
                    hasFood = true;
                }
                ss << "\n         - " << item->getDescription();
            }
        }

        bool hasDrink = false;
        for (const auto& item : subItems) {
            if (item->getCategory() == "Drink") {
                if (!hasDrink) {
                    ss << "\n       [套餐飲料]";
                    hasDrink = true;
                }
                ss << "\n         - " << item->getDescription();
            }
        }
        return ss.str();
    }

    std::string getCsvString() const override {
        std::stringstream ss;
        ss << "Combo," << id << "," << name << "," << discountRate << ",";
        for (size_t i = 0; i < subItems.size(); ++i) {
            ss << subItems[i]->getId() << (i == subItems.size() - 1 ? "" : "|");
        }
        return ss.str();
    }

    std::shared_ptr<MenuItem> clone() const override {
        auto copy = std::make_shared<ComboItem>(id, name, discountRate);
        for (const auto& item : subItems) {
            copy->addSubItem(item->clone());
        }
        return copy;
    }
};

#endif // MENU_ITEM_HPP
