#include "Server.hpp"
#include <iostream>

bool Server::reserveTicket(int ticketId) {
    std::lock_guard<std::mutex> lock(ticket_mutex);
    if(m_tickets.count(ticketId) && !m_tickets[ticketId].reservation) {
        m_tickets[ticketId].reservation = true;
        return true;
    }
    return false;
}

Ticket Server::getTicketDetails(int ticketId) {
    std::lock_guard<std::mutex> lock(ticket_mutex);
    if (m_tickets.find(ticketId) != m_tickets.end()) {
        return m_tickets[ticketId];
    }
    return {};
}

std::vector<Ticket> Server::getAvailableTickets() {
    std::lock_guard<std::mutex> lock(ticket_mutex);
    std::vector<Ticket> available;
    for(const auto & [id, ticket] : m_tickets) {
        if(!ticket.reservation) {
            available.push_back(ticket);
        }
    }
    return available;
}
int Server::findFreeTicket(std::string type) {
    std::lock_guard<std::mutex> lock(ticket_mutex);
    for(const auto& [id, ticket] : m_tickets) {
        if(!ticket.reservation && ticket.type == type) {
            return id;
        }
    }
    return -1;
}
void Server::run() {
    sf::TcpListener listener;
    std::cout << "Server run\n";
    if(listener.listen(53000) == sf::Socket::Status::Done) {
        while (true) {
            sf::TcpSocket* client = new sf::TcpSocket;
            if (listener.accept(*client) == sf::Socket::Status::Done) {
                std::thread([this, client] () {
                    this->handleClient(client, std::ref(*this));
                }).detach();
            } else {
                delete client;
            }
        }
    }
}
void Server::handleClient(sf::TcpSocket* socket, Server& server) {
    sf::Packet packet;

    while(socket->receive(packet) == sf::Socket::Status::Done) {
        std::string command;
        packet >> command;
        if (command == "RESERVE") {
            std::string ticketType;
            packet >> ticketType;
            std::cout << ticketType << std::endl;
            int id = findFreeTicket(ticketType);
            sf::Packet response;
            if(-1 != id) {
                bool success = server.reserveTicket(id);

                if(success) {
                    response << "SUCCESS" << id << m_tickets[id].price;
                } else {
                    response << "FAIL";
                }
                if(socket->send(response) == sf::Socket::Status::Done){};
            } else {
                response << "FAIL";
                if(socket->send(response) == sf::Socket::Status::Done){};
            }
        }
    }
    delete socket;
}