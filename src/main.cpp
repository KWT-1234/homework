#include "UI.hpp"

int main() {
    UI ui("data/menu.csv", "data/orders.txt");
    ui.run();
    return 0;
}
