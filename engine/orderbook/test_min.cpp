#include "Book.hpp"
#include <iostream>
#include <cassert>

int main() {
    long long center_tick = price_to_ticks(18000.00);
    OrderBook ob(10, center_tick, 3);
    
    ob.add(BID, 18000.00, 5, 1001);

    ob.add(BID, 17999.75, 7, 1002);

    ob.add(ASK, 18000.25, 3, 2001);


    bool ok = ob.cancel(1002);
    assert(ok);

    ob.add(ASK, 18000.30, 3, 2005);

    std::cout << "OK add/cancel\n";
    return 0;
}
