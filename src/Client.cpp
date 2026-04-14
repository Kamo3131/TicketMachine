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
void Client::setFullName(const std::string first_name, const std::string last_name) {
    m_full_name.first = first_name;
    m_full_name.second = last_name;
}
std::pair<std::string, std::string> Client::getFullName() const {
    return m_full_name;
}
void Client::givingChange(const double change_val) {
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

    if (dp[change_int] > change_int) {
        std::cout << "Cannot give precise change!" << std::endl;
    } else {
        // std::cout << "Min number of money: " << dp[change_int] << std::endl;
        std::cout << "Coins used:\n";
        int current_remaining = change_int;
        while(current_remaining > 0) {
            int coin_val = available_coins[parent[current_remaining]];
            double coin_val_double = static_cast<double>(coin_val / 100.0);
            std::cout << coin_val_double << " ";
            decrementNumberOfCoins(coin_val_double);
            current_remaining -= coin_val;
        }
        std::cout << std::endl;
    }
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
void Client::clientLoop() {
    sf::TcpSocket socket;
    if(socket.connect(sf::IpAddress::resolve("127.0.0.1").value(), 53000) != sf::Socket::Status::Done) {
        std::cerr << "Could not connect to server!\n";
        return;
    }
    
    sf::Packet initialPacket;
    if(socket.receive(initialPacket) == sf::Socket::Status::Done) {
        std::string cmd;
        initialPacket >> cmd;
        if (cmd == "LIST") {
            std::uint32_t size;
            initialPacket >> size;
            std::vector<Ticket> available_tickets;
            for(std::uint32_t i = 0; i < size; ++i) {
                Ticket t;
                initialPacket >> t;
                available_tickets.push_back(t);
            }
            auto ticket_types = getTicketsTypes(available_tickets);
            for(const auto& [name, price] : ticket_types) {
                std::cout << name << ": " << (price / 100.0) << " PLN" << std::endl;
            }
        }
    }
    bool active = true;
    while(active) {
        std::string type;
        std::cout << "\nEnter ticket type to reserve (or 'exit' to exit): ";
        std::cin >> type;
        if(type == "exit") break;

        sf::Packet packet;
        packet << "RESERVE" << type;
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
                            break;
                        }
                        double payedPrice = std::stod(answer);
                        int payedPriceInt = static_cast<int>(payedPrice*100);
                        int change = payedPriceInt - price;
                        if(change > 0) {
                            std::cout << "Change: " << change << std::endl;
                            givingChange(change);
                            break;
                        } 
                        else if(change == 0) {
                            std::cout << "No change to be given!\n";
                            break;
                        } 
                        else {
                            std::cout << payedPrice << " is not enough to pay. ";
                            continue;
                        }
                    }

                } else {
                    std::cout << "Reservation failed.\n";
                }
            }
        }
    }
}