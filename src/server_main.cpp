#include "Server.hpp"
#include <iostream>

int main() {
    Server server;
    // std::cout << "Server main\n";
    Ticket ulgowy = {"Ulgowy", 320, false, std::chrono::steady_clock::time_point{}};
    Ticket zwyczajny = {"Zwyczajny", 450, false, std::chrono::steady_clock::time_point{}};
    Ticket studencki = {"Studencki", 225, false, std::chrono::steady_clock::time_point{}};
        for(int i = 0; i < 30; i+=3) {
            server.addTicket(i, ulgowy);
            server.addTicket(i+1, zwyczajny); 
            server.addTicket(i+2, studencki);     
        }
    server.run();
    return 0;
}