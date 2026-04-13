#include "Client.hpp"
#include <vector>
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
        std::cout << "Min number of money: " << dp[change_int] << std::endl;
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

void Client::clientLoop() {
    sf::TcpSocket *socket = new sf::TcpSocket;
    if(socket->connect(sf::IpAddress::resolve("127.0.0.1").value(), 53000) != sf::Socket::Status::Done) {
        std::cerr << "Could not connect to server!\n";
        return;
    }
    bool active = true;
    
    while(active) {
        std::string type;
        std::cout << "Enter ticket type or exit: ";
        std::cin >> type;
        if(type == "exit" || type == "EXIT") break;

        sf::Packet packet;
        packet << "RESERVE" << type;
        if(socket->send(packet) == sf::Socket::Status::Done) {
            sf::Packet receivePacket;
            if (socket->receive(receivePacket) == sf::Socket::Status::Done) {
                std::string status;
                int ticketId;
                double price;

                receivePacket >> status;
                if( status == "SUCCESS") {
                    receivePacket >> ticketId >> price;
                    std::cout << "Reserved ticket ID: " << ticketId << "; ticket price: " << price << std::endl;
                } else {
                    std::cout << "Reservation failed. Ticket not available.\n";
                }
            }
        }
    }
}