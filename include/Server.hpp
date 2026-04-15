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
        std::map<int, Ticket> m_sold_tickets;
        mutable std::mutex ticket_mutex;

        void consoleInterface();

    public:
        void addTicket(int ticketId, Ticket ticket);
        bool reserveTicket(int ticketId, std::string clientName);
        int findFreeTicket(std::string type) const;
        void finalizeSale(int ticketId);
        void cancelReservation(int ticketId);
        void checkTimeouts();
        Ticket getTicketDetails(int ticketId);
        std::vector<Ticket> getAvailableTickets();
        void run();
        void handleClient(sf::TcpSocket* socket, Server& server);


};