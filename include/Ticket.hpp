#pragma once
#include <string>
#include <SFML/Network.hpp>
struct Ticket {
    std::string type;
    int price;
    bool reservation;
};

inline sf::Packet& operator<<(sf::Packet& packet, const Ticket& ticket) {
    return packet << ticket.type << ticket.price << ticket.reservation;
}
inline sf::Packet& operator>>(sf::Packet& packet, Ticket& ticket) {
    return packet >> ticket.type >> ticket.price >> ticket.reservation;
}