#include "Server.hpp"
#include <iostream>

int main() {
    Server server;
    // std::cout << "Server main\n";
    Ticket ulgowy = {"Ulgowy", 320, false};
    Ticket zwyczajny = {"Zwyczajny", 450, false};
    Ticket studencki = {"Studencki", 225, false};
        for(int i = 0; i < 3; i+=3) {
            server.addTicket(i, ulgowy);
            server.addTicket(i+1, zwyczajny); 
            server.addTicket(i+2, studencki);     
        }
    server.run();
    return 0;
}