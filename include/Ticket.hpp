#pragma once
#include <string>
#include <SFML/Network.hpp>
#include <iostream>
#include <chrono>
struct Ticket {
    std::string type;
    int price;
    bool reservation;
    std::chrono::steady_clock::time_point reservationEndTime;
    std::string clientName = "";
};

inline sf::Packet& operator<<(sf::Packet& packet, const Ticket& ticket) {
    return packet << ticket.type << ticket.price << ticket.reservation;
}

inline std::ostream& operator<<(std::ostream& os, const Ticket& ticket) {
    return os << ticket.type << ":\nPrice\t" << static_cast<double>(ticket.price)/100.0 
    << "\nClient\t" << ticket.clientName;
}
inline sf::Packet& operator>>(sf::Packet& packet, Ticket& ticket) {
    return packet >> ticket.type >> ticket.price >> ticket.reservation;
}