#include "Client.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <SFML/Network.hpp>

int Client::getNumberOfCoins(const double value) const {
    int temp = static_cast<int>(value * 100);
    return m_money.at(temp);
}
void Client::setNumberOfCoins(const double value, const int number) {
    int temp = static_cast<int>(value * 100);
    m_money.at(temp) = number;
}
int Client::getNumberOfCoins(const int value) const {
    return m_money.at(value);
}
void Client::setNumberOfCoins(const int value, const int number) {
    m_money.at(value) = number;
}
void Client::incrementNumberOfCoins(const double value) {
    int number_of_coins = getNumberOfCoins(value);
    setNumberOfCoins(value, number_of_coins + 1);
}
void Client::decrementNumberOfCoins(const double value) {
    int number_of_coins = getNumberOfCoins(value);
    if(number_of_coins > 0) {
        setNumberOfCoins(value, number_of_coins - 1);
    }
}
void Client::incrementNumberOfCoins(const int value) {
    int number_of_coins = getNumberOfCoins(value);
    setNumberOfCoins(value, number_of_coins + 1);
}
void Client::decrementNumberOfCoins(const int value) {
    int number_of_coins = getNumberOfCoins(value);
    if(number_of_coins > 0) {
        setNumberOfCoins(value, number_of_coins - 1);
    }
}
bool Client::givingChange(const double change_val) {
    int change_int = std::round(change_val * 100);
    std::vector<int> available_coins;
    for (const auto& [nominal, count] : m_money) {
        for (size_t i = 0; i < count; ++i) {
            available_coins.push_back(nominal);
        }
    }

    std::vector<int> dp(change_int + 1, 2147483640);
    std::vector<int> parent(change_int + 1, -1);
    dp[0] = 0;
    

    for (int i = 0; i < available_coins.size(); ++i) {
        int coin = available_coins[i];
        for (int j = change_int; j >= coin; --j) {
            if (dp[j - coin] != 2147483640) {
                if(dp[j - coin] + 1 < dp[j]) {
                    dp[j] = dp[j - coin] + 1;
                    parent[j] = i;    
                }
            }
        }
    }

    m_change.clear();
    if (dp[change_int] > change_int) {
        return false;
    } else {
        // std::cout << "Min number of money: " << dp[change_int] << std::endl;
        std::cout << "Coins used:\n";
        int current_remaining = change_int;
        while(current_remaining > 0) {
            int coin_val = available_coins[parent[current_remaining]];
            m_change.push_back(coin_val);
            double coin_val_double = static_cast<double>(coin_val / 100.0);
            std::cout << coin_val_double << " ";
            decrementNumberOfCoins(coin_val_double);
            current_remaining -= coin_val;
        }
        std::cout << std::endl;
        return true;
    }
}

void Client::returnChangeToMachine() {
    for(const int coin : m_change) {
        incrementNumberOfCoins(coin);
    }
    m_change.clear();
}
void Client::printMoney() const {
    for (const auto& [nominal, count] : m_money) {
        std::cout << static_cast<double>(nominal/100.0) << ": " << count << std::endl; 
    }
}
std::set<std::pair<std::string, int>> Client::getTicketsTypes(std::vector<Ticket> tickets) const {
    std::set<std::pair<std::string, int>> unique_types;
    for (const auto& ticket : tickets) {
        unique_types.insert(std::make_pair(ticket.type, ticket.price));
    }
    return unique_types;
}

void Client::showAvailableTickets(sf::TcpSocket& socket) {
    sf::Packet request;
    request << "GET_LIST"; 
    if(socket.send(request) == sf::Socket::Status::Done) {
        sf::Packet packet;
        if(socket.receive(packet) == sf::Socket::Status::Done) {
            std::string cmd;
            packet >> cmd;
            if (cmd == "LIST") {
                std::uint32_t size;
                packet >> size;
                std::vector<Ticket> available_tickets;
                for(std::uint32_t i = 0; i < size; ++i) {
                    Ticket t;
                    packet >> t;
                    available_tickets.push_back(t);
                }
                auto ticket_types = getTicketsTypes(available_tickets);
                for(const auto& [name, price] : ticket_types) {
                    std::cout << name << ": " << (price / 100.0) << " PLN" << std::endl;
                }
            }
        }
    }
}

void Client::buyTicket(sf::TcpSocket& socket) {
    std::string type;
    std::cout << "\nEnter ticket type to reserve (or 'cancel' to cancel): ";
    std::cin >> type;
    if(type == "cancel") return;
    std::string clientName;
    std::cin.ignore();
    std::cout << "Type your name:\n"; 
    std::getline(std::cin, clientName);
    if(clientName.empty()) {
        std::cout << "Wrong name. Type correct credentials!\n";
        return;
    }
    sf::Packet packet;
    packet << "RESERVE" << type << clientName;
    if(socket.send(packet) == sf::Socket::Status::Done) {
        sf::Packet receivePacket;
        if (socket.receive(receivePacket) == sf::Socket::Status::Done) {
            std::string status;
            receivePacket >> status;
            if(status == "SUCCESS") {
                int ticketId, price;
                receivePacket >> ticketId >> price;
                std::cout << "Reserved ID: " << ticketId << " Price: " << (price/100.0) << std::endl;
                while(true){
                    std::cout << "Pay the price (or 'cancel'): ";
                    std::string answer;
                    std::cin >> answer;
                    if(answer == "CANCEL" || answer == "C" || 
                        answer == "cancel" || answer == "Cancel"){
                        sf::Packet cancelPacket;
                        cancelPacket << "CANCEL" << ticketId;
                        if(socket.send(cancelPacket) == sf::Socket::Status::Done) {
                            std::cout << "Transaction cancelled" << std::endl;
                        }
                        std::cout << "Transaction cancelled" << std::endl;
                        break;
                    }
                    try {
                        double payedPrice = std::stod(answer);
                        int payedPriceInt = static_cast<int>(payedPrice*100);
                        int change = payedPriceInt - price;
                        if(change >= 0) {
                            std::cout << "Change: " << static_cast<double>(change)/100.0 << std::endl;
                            if(change == 0 || givingChange(static_cast<double>(change)/100.0)) {
                                sf::Packet confirmationPacket;
                                confirmationPacket << "CONFIRM" << ticketId;
                                if(socket.send(confirmationPacket) == sf::Socket::Status::Done) {
                                    sf::Packet timeoutPacket;
                                    if(socket.receive(timeoutPacket) == sf::Socket::Status::Done) {
                                        std::string timeoutResponse;
                                        timeoutPacket >> timeoutResponse;
                                        if("OK" == timeoutResponse) {
                                            std::cout << "Transaction successful!\nTicket " 
                                            << type << ": " << ticketId << std::endl;
                                        } else if ("TIMEOUT" == timeoutResponse) {
                                            std::cout << "Reservation timed out!" << std::endl;
                                            returnChangeToMachine();
                                            return;
                                        }
                                    }

                                } 
                            } else {
                                sf::Packet cancelPacket;
                                cancelPacket << "CANCEL" << ticketId /*<< m_full_name.first << m_full_name.second*/;
                                if(socket.send(cancelPacket) == sf::Socket::Status::Done) {
                                    std::cout << "Transaction aborted: no possibility of giving change.\n";
                                }
                            }
                            break;
                        } 
                        else {
                            std::cout << payedPrice << " is not enough to pay. ";
                            return;
                        }
                    } catch (...) {
                    std::cout << "Invalid input. Enter a number or cancel.\n";
                    }
                } 
            } else {
                std::cout << "Reservation failed.\n";
            }
        }
    }
}
void Client::clientLoop() {
    sf::TcpSocket socket;
    if(socket.connect(sf::IpAddress::resolve("127.0.0.1").value(), 53000) != sf::Socket::Status::Done) {
        std::cerr << "Could not connect to server!\n";
        return;
    }
    
    showAvailableTickets(socket);
    bool active = true;
    while(active) {
        std::cout << std::endl << "Type:\n" <<
        "1) 'BUY' - to buy ticket!\n" <<
        "2) 'TICKETS' - to show available tickets!\n" <<
        "4) 'SHOW_COINS' - only for dev.\n" <<
        "3) 'EXIT' - to exit machine.\nYour command: ";

        std::string command;
        std::cin >> command;
        if("BUY" == command || "1" == command) {
            buyTicket(socket);
        } else if("TICKETS" == command || "2" == command) {
            showAvailableTickets(socket);
        } else if("EXIT" == command || "3" == command) {
            break;
        } else if("SHOW_COINS" == command || "4" == command) {
            printMoney();
        }
    }
}