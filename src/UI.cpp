#include "UI.hpp"
#include <iostream>
#include <fstream>
#include <limits>
#include <iomanip>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

UI::UI(const std::string& menuPath, const std::string& orderPath)
    : menuFile(menuPath), orderFile(orderPath), currentOrderId(1) {
    // Determine the next order ID by reading the order file
    std::ifstream file(orderFile);
    if (file.is_open()) {
        std::string line;
        int maxId = 0;
        while (std::getline(file, line)) {
            if (line.find("訂單編號: #") != std::string::npos) {
                try {
                    size_t pos = line.find("#") + 1;
                    int id = std::stoi(line.substr(pos));
                    if (id > maxId) maxId = id;
                } catch (...) {}
            }
        }
        currentOrderId = maxId + 1;
        file.close();
    }
}

void UI::clearScreen() const {
#ifdef _WIN32
    // Print ANSI clear escape code which works in modern Windows Terminal
    std::cout << "\033[2J\033[H";
#else
    std::cout << "\033[2J\033[H";
#endif
}

void UI::printHeader(const std::string& title) const {
    std::cout << BOLD << CYAN << "==================================================" << RESET << "\n";
    std::cout << BOLD << YELLOW << "            " << title << RESET << "\n";
    std::cout << BOLD << CYAN << "==================================================" << RESET << "\n";
}

void UI::printFooter() const {
    std::cout << BOLD << CYAN << "==================================================" << RESET << "\n";
}

void UI::waitEnter() const {
    std::cout << "\n按 [Enter] 鍵繼續...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int UI::getValidInteger(const std::string& prompt, int min, int max) const {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (val >= min && val <= max) {
                return val;
            }
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cout << RED << "錯誤：輸入超出範圍或格式無效，請重新輸入 (" << min << "-" << max << ")！" << RESET << "\n";
    }
}

double UI::getValidDouble(const std::string& prompt, double min, double max) const {
    double val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (val >= min && val <= max) {
                return val;
            }
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cout << RED << "錯誤：輸入超出範圍或格式無效，請重新輸入！" << RESET << "\n";
    }
}

std::string UI::getNonEmptyString(const std::string& prompt) const {
    std::string val;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, val);
        // Trim whitespace
        val.erase(val.begin(), std::find_if(val.begin(), val.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        val.erase(std::find_if(val.rbegin(), val.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), val.end());

        if (!val.empty()) {
            return val;
        }
        std::cout << RED << "錯誤：輸入欄位不能為空！" << RESET << "\n";
    }
}

void UI::displayMenu(const Menu& menu) const {
    std::cout << BOLD << GREEN << "\n【 主餐區 (Food) 】" << RESET << "\n";
    for (const auto& item : menu.getItems()) {
        if (item->getCategory() == "Food") {
            std::cout << "  [" << item->getId() << "] " << item->getDescription() << "\n";
        }
    }

    std::cout << BOLD << GREEN << "\n【 飲品區 (Drink) 】" << RESET << "\n";
    for (const auto& item : menu.getItems()) {
        if (item->getCategory() == "Drink") {
            std::cout << "  [" << item->getId() << "] " << item->getDescription() << "\n";
        }
    }

    std::cout << BOLD << GREEN << "\n【 超值套餐 (Combo) 】" << RESET << "\n";
    for (const auto& item : menu.getItems()) {
        if (item->getCategory() == "Combo") {
            std::cout << "  [" << item->getId() << "] " << item->getDescription() << "\n";
        }
    }
    std::cout << "\n";
}

void UI::displayCart(const Order& order) const {
    std::cout << BOLD << MAGENTA << "\n【 您的購物車內容 】" << RESET << "\n";
    const auto& items = order.getCartItems();
    if (items.empty()) {
        std::cout << "   (購物車目前是空的)\n";
        return;
    }
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << items[i]->getDescription() << "\n";
        std::cout << "     小計: $" << std::fixed << std::setprecision(0) << items[i]->getPrice() << " 元\n";
    }
    std::cout << "--------------------------------------------------\n";
    std::cout << BOLD << " 累計金額: $" << std::fixed << std::setprecision(0) << order.calculateTotal() << " 元" << RESET << "\n\n";
}

void UI::customizeItem(const std::shared_ptr<MenuItem>& item) const {
    if (auto food = std::dynamic_pointer_cast<FoodItem>(item)) {
        std::cout << BOLD << "\n客製化 - " << food->getName() << RESET << "\n";
        int spicy = getValidInteger("請選擇辣度 (0:不辣, 1:微辣, 2:中辣, 3:大辣): ", 0, 3);
        food->setSpicyLevel(spicy);

        food->clearExtraIngredients();
        int addExtra = getValidInteger("是否需要加點配料？(0:不需要, 1:需要): ", 0, 1);
        if (addExtra == 1) {
            std::cout << "可加點項目: 1.加起司(+$15) 2.加培根(+$20) 3.加蛋(+$10)\n";
            while (true) {
                int opt = getValidInteger("請輸入加點項目編號 (0:結束加點): ", 0, 3);
                if (opt == 0) break;
                if (opt == 1) {
                    food->addExtraIngredient("加起司");
                    food->setPrice(food->getPrice() + 15);
                } else if (opt == 2) {
                    food->addExtraIngredient("加培根");
                    food->setPrice(food->getPrice() + 20);
                } else if (opt == 3) {
                    food->addExtraIngredient("加蛋");
                    food->setPrice(food->getPrice() + 10);
                }
                std::cout << GREEN << "已加入加點配料！" << RESET << "\n";
            }
        }
    }
    else if (auto drink = std::dynamic_pointer_cast<DrinkItem>(item)) {
        std::cout << BOLD << "\n客製化 - " << drink->getName() << RESET << "\n";
        std::cout << "冰塊選項: 1.正常冰 2.少冰 3.微冰 4.去冰\n";
        int iceOpt = getValidInteger("請選擇冰塊量: ", 1, 4);
        std::string iceText[] = {"正常冰", "少冰", "微冰", "去冰"};
        drink->setIceLevel(iceText[iceOpt - 1]);

        std::cout << "甜度選項: 1.正常甜 2.少糖 3.半糖 4.微糖 5.無糖\n";
        int sweetOpt = getValidInteger("請選擇甜度: ", 1, 5);
        std::string sweetText[] = {"正常甜", "少糖", "半糖", "微糖", "無糖"};
        drink->setSweetnessLevel(sweetText[sweetOpt - 1]);
    }
    else if (auto combo = std::dynamic_pointer_cast<ComboItem>(item)) {
        std::cout << BOLD << "\n客製化 - " << combo->getName() << RESET << "\n";
        std::cout << "套餐包含以下品項，是否需要個別設定客製化？\n";
        int opt = getValidInteger("1.是 2.否 (直接使用預設): ", 1, 2);
        if (opt == 1) {
            for (auto& subItem : combo->getSubItems()) {
                customizeItem(subItem);
            }
        }
    }
}

void UI::runCustomerMode(Menu& menu, Order& order) {
    while (true) {
        clearScreen();
        printHeader("顧客點餐系統");
        displayMenu(menu);
        displayCart(order);

        std::cout << BOLD << "選項清單：" << RESET << "\n";
        std::cout << "  [1] 輸入 ID 將商品加入購物車\n";
        std::cout << "  [2] 從購物車移除商品\n";
        std::cout << "  [3] 結帳送出訂單\n";
        std::cout << "  [4] 返回主選單 (購物車將被清空)\n";
        
        int opt = getValidInteger("\n請輸入選項編號: ", 1, 4);
        if (opt == 1) {
            int itemId = getValidInteger("請輸入餐點 ID: ", 1, 9999);
            auto item = menu.findItemById(itemId);
            if (item) {
                // Clone the item to customize
                auto cartItem = item->clone();
                customizeItem(cartItem);
                order.addItem(cartItem);
                std::cout << GREEN << "\n已成功將「" << cartItem->getName() << "」加入購物車！" << RESET << "\n";
            } else {
                std::cout << RED << "\n找不到該商品 ID，請重新輸入！" << RESET << "\n";
            }
            waitEnter();
        }
        else if (opt == 2) {
            if (order.getCartItems().empty()) {
                std::cout << RED << "\n購物車目前沒有任何商品可移除！" << RESET << "\n";
            } else {
                int index = getValidInteger("請輸入要移除的項目編號 (1-" + std::to_string(order.getCartItems().size()) + "): ", 1, order.getCartItems().size());
                order.removeItem(index - 1);
                std::cout << GREEN << "\n商品已成功從購物車移除！" << RESET << "\n";
            }
            waitEnter();
        }
        else if (opt == 3) {
            if (order.getCartItems().empty()) {
                std::cout << RED << "\n購物車為空，無法結帳！" << RESET << "\n";
                waitEnter();
            } else {
                clearScreen();
                printHeader("訂單明細確認");
                std::string receipt = order.generateReceiptString(currentOrderId);
                std::cout << receipt;
                
                int confirm = getValidInteger("確認無誤並付款結帳？ (1.確認結帳 2.取消): ", 1, 2);
                if (confirm == 1) {
                    if (order.checkout(orderFile, currentOrderId)) {
                        std::cout << GREEN << "\n結帳成功！收據已存入歷史訂單庫中。" << RESET << "\n";
                        currentOrderId++;
                        order.clear();
                    } else {
                        std::cout << RED << "\n結帳失敗，寫入紀錄時出錯！" << RESET << "\n";
                    }
                } else {
                    std::cout << YELLOW << "\n已取消結帳，返回購物車。" << RESET << "\n";
                }
                waitEnter();
            }
        }
        else if (opt == 4) {
            order.clear();
            break;
        }
    }
}

void UI::runAdminMode(Menu& menu) {
    while (true) {
        clearScreen();
        printHeader("管理員後台控制系統");
        
        std::cout << "  [1] 查看所有菜單品項\n";
        std::cout << "  [2] 新增菜單品項\n";
        std::cout << "  [3] 刪除菜單品項\n";
        std::cout << "  [4] 查看歷史交易明細\n";
        std::cout << "  [5] 返回主選單\n";

        int opt = getValidInteger("\n請輸入選項編號: ", 1, 5);
        if (opt == 1) {
            clearScreen();
            printHeader("目前菜單一覽");
            displayMenu(menu);
            waitEnter();
        }
        else if (opt == 2) {
            clearScreen();
            printHeader("新增菜單品項");
            std::cout << "請選擇商品種類: 1.主餐(Food) 2.飲料(Drink) 3.組合套餐(Combo)\n";
            int type = getValidInteger("選擇種類: ", 1, 3);
            
            int newId = menu.getNextAvailableId();
            std::string name = getNonEmptyString("請輸入品項名稱: ");

            if (type == 1) {
                double price = getValidDouble("請輸入單價: ", 0, 10000);
                auto food = std::make_shared<FoodItem>(newId, name, price);
                menu.addItem(food);
                std::cout << GREEN << "\n成功新增主餐商品！" << RESET << "\n";
            }
            else if (type == 2) {
                double price = getValidDouble("請輸入單價: ", 0, 10000);
                auto drink = std::make_shared<DrinkItem>(newId, name, price);
                menu.addItem(drink);
                std::cout << GREEN << "\n成功新增飲料商品！" << RESET << "\n";
            }
            else if (type == 3) {
                double discount = getValidDouble("請輸入套餐折扣折數 (0.01-1.0，如 0.85 代表打八五折): ", 0.01, 1.0);
                auto combo = std::make_shared<ComboItem>(newId, name, discount);
                
                std::cout << "\n選擇要加入此套餐的商品 ID (輸入 0 結束)：\n";
                displayMenu(menu);
                while (true) {
                    int subId = getValidInteger("請輸入商品 ID (0鍵結束): ", 0, 9999);
                    if (subId == 0) break;
                    auto subItem = menu.findItemById(subId);
                    if (subItem) {
                        combo->addSubItem(subItem->clone());
                        std::cout << GREEN << "已將「" << subItem->getName() << "」加入本套餐！" << RESET << "\n";
                    } else {
                        std::cout << RED << "無效的 ID，請重新輸入！" << RESET << "\n";
                    }
                }
                menu.addItem(combo);
                std::cout << GREEN << "\n成功新增超值套餐項目！" << RESET << "\n";
            }

            // Save back to CSV
            if (menu.saveToFile(menuFile)) {
                std::cout << GREEN << "菜單資料庫已成功儲存至 " << menuFile << "。" << RESET << "\n";
            } else {
                std::cout << RED << "錯誤：無法寫入菜單檔案！" << RESET << "\n";
            }
            waitEnter();
        }
        else if (opt == 3) {
            clearScreen();
            printHeader("刪除菜單品項");
            displayMenu(menu);
            int id = getValidInteger("請輸入要刪除的商品 ID (0代表取消): ", 0, 9999);
            if (id != 0) {
                if (menu.removeItem(id)) {
                    std::cout << GREEN << "\n商品 ID [" << id << "] 已從記憶體刪除。" << RESET << "\n";
                    if (menu.saveToFile(menuFile)) {
                        std::cout << GREEN << "變更已同步儲存至 " << menuFile << "。" << RESET << "\n";
                    } else {
                        std::cout << RED << "錯誤：檔案儲存失敗！" << RESET << "\n";
                    }
                } else {
                    std::cout << RED << "\n找不到該商品 ID，無法刪除！" << RESET << "\n";
                }
            }
            waitEnter();
        }
        else if (opt == 4) {
            clearScreen();
            printHeader("歷史交易收據紀錄");
            std::ifstream file(orderFile);
            if (file.is_open()) {
                std::string line;
                bool hasContent = false;
                while (std::getline(file, line)) {
                    std::cout << line << "\n";
                    hasContent = true;
                }
                if (!hasContent) {
                    std::cout << "目前尚無任何交易紀錄。\n";
                }
                file.close();
            } else {
                std::cout << "交易紀錄檔不存在或無法讀取。\n";
            }
            waitEnter();
        }
        else if (opt == 5) {
            break;
        }
    }
}

void UI::run() {
#ifdef _WIN32
    // Enable UTF-8 encoding in Windows terminal
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    // Enable ANSI coloring support in Windows terminal
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif

    Menu menu;
    if (!menu.loadFromFile(menuFile)) {
        std::cout << RED << BOLD << "警告：無法載入菜單檔案，將建立空的菜單。" << RESET << "\n";
        waitEnter();
    }

    Order order;

    while (true) {
        clearScreen();
        std::cout << BOLD << CYAN << "==================================================" << RESET << "\n";
        std::cout << BOLD << YELLOW << "        歡迎使用 - C++ 智慧點餐收銀系統" << RESET << "\n";
        std::cout << BOLD << CYAN << "==================================================" << RESET << "\n";
        std::cout << "  [1] 進入顧客點餐模式\n";
        std::cout << "  [2] 進入管理員後台控制模式\n";
        std::cout << "  [3] 退出系統\n";
        std::cout << BOLD << CYAN << "==================================================" << RESET << "\n";
        
        int choice = getValidInteger("請輸入選項編號: ", 1, 3);
        if (choice == 1) {
            runCustomerMode(menu, order);
        }
        else if (choice == 2) {
            runAdminMode(menu);
        }
        else if (choice == 3) {
            clearScreen();
            std::cout << BOLD << GREEN << "\n感謝您的使用，系統已安全關閉。再見！\n\n" << RESET;
            break;
        }
    }
}
