#ifndef ORDER_HPP
#define ORDER_HPP

#include "MenuItem.hpp"
#include <vector>
#include <memory>
#include <string>

class Order {
private:
    std::vector<std::shared_ptr<MenuItem>> cartItems;

public:
    Order() = default;

    void addItem(const std::shared_ptr<MenuItem>& item);
    bool removeItem(size_t index);
    double calculateTotal() const;
    void clear();

    const std::vector<std::shared_ptr<MenuItem>>& getCartItems() const;
    bool checkout(const std::string& filename, int orderId) const;
    std::string generateReceiptString(int orderId) const;
};

#endif // ORDER_HPP
