#include "Client.hpp"
void fillWithCoins(Client& client) {
    std::vector<int> denominations = {
        1, 2, 5, 10, 20, 50,          
        100, 200, 500,                
        1000, 2000, 5000, 10000, 20000
    };
    for (int den : denominations) {
        double val_in_pln = den / 100.0;
        client.setNumberOfCoins(val_in_pln, 3);
    }
}
int main() {
    Client client;
    fillWithCoins(client);
    client.clientLoop();
    return 0;
}