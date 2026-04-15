#include "Server.hpp"
#include <iostream>

void Server::addTicket(int ticketId, Ticket ticket) {
    m_tickets[ticketId] = ticket;
}

bool Server::reserveTicket(int ticketId, std::string clientName) {
    std::lock_guard<std::mutex> lock(ticket_mutex);
    if(m_tickets.count(ticketId) && !m_tickets[ticketId].reservation) {
        m_tickets[ticketId].reservation = true;
        m_tickets[ticketId].clientName = clientName;
        m_tickets[ticketId].reservationEndTime = std::chrono::steady_clock::now() 
        + std::chrono::seconds(60);
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
void Server::checkTimeouts() {
    std::lock_guard<std::mutex> lock(ticket_mutex);
    auto now = std::chrono::steady_clock::now();
    
    for(auto& [id, ticket] : m_tickets) {
        if( ticket.reservation && now > ticket.reservationEndTime) {
            ticket.reservation = false;
            ticket.clientName = "";
            std::cout << "Reservation for ticket " << id << " timed out!\n";
        }
    }
}
std::vector<Ticket> Server::getAvailableTickets() {
    checkTimeouts();
    std::lock_guard<std::mutex> lock(ticket_mutex);
    std::vector<Ticket> available;
    for(const auto & [id, ticket] : m_tickets) {
        if(!ticket.reservation) {
            available.push_back(ticket);
        }
    }
    return available;
}
int Server::findFreeTicket(std::string type) const {
    std::lock_guard<std::mutex> lock(ticket_mutex);
    for(const auto& [id, ticket] : m_tickets) {
        if(!ticket.reservation && ticket.type == type) {
            return id;
        }
    }
    return -1;
}
void Server::finalizeSale(int ticketId) {
    std::lock_guard<std::mutex> lock(ticket_mutex);
    m_tickets[ticketId].reservation = false;
    m_sold_tickets[ticketId] = m_tickets[ticketId];
    m_tickets.erase(ticketId); 
}

void Server::cancelReservation(int ticketId) {
    std::lock_guard<std::mutex> lock(ticket_mutex);
    if (m_tickets.count(ticketId)) {
        m_tickets[ticketId].reservation = false;
    }
}
void Server::consoleInterface() {
    std::string command;
    while(true) {
        std::cin >> command;
        if(command == "SOLD") {
            std::lock_guard<std::mutex> lock(ticket_mutex);
            if(!m_sold_tickets.empty()) {
                for(const auto& [id, ticket] : m_sold_tickets) {
                    std::cout << ticket << std::endl <<
                    "ID: " << id << std::endl;
                }
            } else {
                std::cout << "No tickets sold\n";
            }
        }
    }
}

void Server::run() {
    std::thread(&Server::consoleInterface, this).detach();
    sf::TcpListener listener;

    if(listener.listen(53000) == sf::Socket::Status::Done) {
        std::cout << "Server is running!\n";
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
    
    sf::Packet initialPacket;
    std::vector<Ticket> available = getAvailableTickets();
    initialPacket << "LIST";
    initialPacket << static_cast<std::uint32_t>(available.size());
    for (const auto & ticket : available) {
        initialPacket << ticket;
    }
    if(socket->send(initialPacket) == sf::Socket::Status::Done) {
        sf::Packet packet;
        while(socket->receive(packet) == sf::Socket::Status::Done) {
            std::string command;
            packet >> command;
            if (command == "RESERVE") {
                std::string ticketType;
                packet >> ticketType;
                std::cout << ticketType << std::endl;
                int id = findFreeTicket(ticketType);
                std::string clientName;
                packet >> clientName;
                sf::Packet response;
                if(-1 != id) {
                    bool success = server.reserveTicket(id, clientName);

                    if(success) {
                        std::cout << m_tickets[id].price << std::endl;
                        response << "SUCCESS" << id << m_tickets[id].price;
                    } else {
                        response << "FAIL";
                    }
                    if(socket->send(response) == sf::Socket::Status::Done){
                        
                    };
                } else {
                    response << "FAIL";
                    if(socket->send(response) == sf::Socket::Status::Done){};
                }
            } else if (command == "CONFIRM") {
                int ticketId;
                packet >> ticketId;
                std::cout << "Client confirmed payment for ticket: " << ticketId << std::endl;

                auto now = std::chrono::steady_clock::now();
                if(m_tickets.count(ticketId) && m_tickets[ticketId].reservation) {
                    if (now <= m_tickets[ticketId].reservationEndTime) {
                        server.finalizeSale(ticketId);
                    } else {
                        m_tickets[ticketId].reservation = false;
                    }
                }
                std::cout << "Tickets sold:\n";
                
                std::lock_guard<std::mutex> lock(ticket_mutex);
                for(const auto& [id, ticket] : m_sold_tickets) {
                    std::cout << id << std::endl;
                }
            } else if (command == "CANCEL") {
                int ticketId;
                packet >> ticketId;
                std::cout << "Client cancelled reservation for ticket: " << ticketId << std::endl;
                server.cancelReservation(ticketId);
            }
        }
    }
    delete socket;
}