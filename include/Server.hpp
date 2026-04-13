#include <map>
#include <string>
#include <ctime>
#include <vector>
#include <thread>
#include <mutex>
#include <SFML/Network.hpp>

struct Ticket {
    std::string type;
    int price;
    bool reservation;
    std::time_t timestamp;
};

class Server {
    private:
        std::map<int, Ticket> m_tickets;
        std::mutex ticket_mutex;
    public:
        bool reserveTicket(int ticketId);
        int findFreeTicket(std::string type);
        Ticket getTicketDetails(int ticketId);
        std::vector<Ticket> getAvailableTickets();
        void run();
        void handleClient(sf::TcpSocket* socket, Server& server);


};