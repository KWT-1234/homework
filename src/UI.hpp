#ifndef UI_HPP
#define UI_HPP

#include "Menu.hpp"
#include "Order.hpp"
#include <string>

class UI {
private:
    std::string menuFile;
    std::string orderFile;
    int currentOrderId;

    // ANSI Color Codes
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";

    void clearScreen() const;
    void printHeader(const std::string& title) const;
    void printFooter() const;
    void waitEnter() const;

    int getValidInteger(const std::string& prompt, int min, int max) const;
    double getValidDouble(const std::string& prompt, double min, double max) const;
    std::string getNonEmptyString(const std::string& prompt) const;

    void displayMenu(const Menu& menu) const;
    void displayCart(const Order& order) const;
    void customizeItem(const std::shared_ptr<MenuItem>& item) const;

    void runCustomerMode(Menu& menu, Order& order);
    void runAdminMode(Menu& menu);

public:
    UI(const std::string& menuPath, const std::string& orderPath);
    void run();
};

#endif // UI_HPP
