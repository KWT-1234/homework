#include "Order.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>

void Order::addItem(const std::shared_ptr<MenuItem>& item) {
    if (item) {
        // Clone the item so that customization doesn't alter the master menu template
        cartItems.push_back(item->clone());
    }
}

bool Order::removeItem(size_t index) {
    if (index < cartItems.size()) {
        cartItems.erase(cartItems.begin() + index);
        return true;
    }
    return false;
}

double Order::calculateTotal() const {
    double total = 0.0;
    for (const auto& item : cartItems) {
        total += item->getPrice();
    }
    return total;
}

void Order::clear() {
    cartItems.clear();
}

const std::vector<std::shared_ptr<MenuItem>>& Order::getCartItems() const {
    return cartItems;
}

std::string Order::generateReceiptString(int orderId) const {
    std::stringstream ss;
    
    // Get current time
    std::time_t t = std::time(nullptr);
    char timeStr[100];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

    ss << "==================================================\n";
    ss << "               美味點餐系統 收據\n";
    ss << "==================================================\n";
    ss << " 訂單編號: #" << std::setw(4) << std::setfill('0') << orderId << "\n";
    ss << " 交易時間: " << timeStr << "\n";
    ss << "--------------------------------------------------\n";
    
    for (size_t i = 0; i < cartItems.size(); ++i) {
        ss << " " << (i + 1) << ". " << cartItems[i]->getDescription() << "\n";
        ss << "    小計: $" << std::fixed << std::setprecision(0) << cartItems[i]->getPrice() << "\n";
    }
    
    ss << "--------------------------------------------------\n";
    ss << " 總品項數: " << cartItems.size() << " 項\n";
    ss << " 應付總額: $" << std::fixed << std::setprecision(0) << calculateTotal() << " 元\n";
    ss << "==================================================\n";
    ss << "             謝謝光臨，期待您再次光臨！\n";
    ss << "==================================================\n\n";
    
    return ss.str();
}

bool Order::checkout(const std::string& filename, int orderId) const {
    if (cartItems.empty()) {
        return false;
    }

    std::string receipt = generateReceiptString(orderId);
    
    // Write (append) to history orders file
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    file << receipt;
    file.close();
    
    return true;
}
