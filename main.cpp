#include "ui/Menu.h"

#include <iostream>
#include <new>

int main() {
    try {
        ui::Menu menu;
        menu.run();
    } catch (const std::bad_alloc&) {
        std::cerr << "Memory allocation failed; the application cannot continue safely.\n";
        return 1;
    }
    return 0;
}
