#pragma once
#include <map>
#include <string>
#include <ctime>
#include <vector>
#include <thread>
#include <mutex>
#include <SFML/Network.hpp>
#include "Ticket.hpp"


class Server {
    private:
        std::map<int, Ticket> m_tickets;
        // std::map<int, Ticket> m_reserved_tickets;
        mutable std::mutex ticket_mutex;
    public:
        void addTicket(int ticketId, Ticket ticket);
        bool reserveTicket(int ticketId);
        int findFreeTicket(std::string type) const;
        Ticket getTicketDetails(int ticketId);
        std::vector<Ticket> getAvailableTickets();
        void run();
        void handleClient(sf::TcpSocket* socket, Server& server);


};