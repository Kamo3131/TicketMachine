#pragma once
#include <map>
#include <utility>
#include <string>
#include <set>
#include <vector>
#include "Ticket.hpp"

class Client {
    private:
        /**
         * First value is the value of the money, and the second one is quantity.
        */
        std::map<int, std::size_t> m_money = {
        {1, 0}, {2, 0},
        {5, 0}, {10, 0}, {20, 0}, 
        {50, 0}, {100, 0}, {200, 0}, 
        {500, 0}, {1000, 0}, {2000, 0}, 
        {5000, 0}, {10000, 0}, {20000, 0}};
        std::pair<std::string, std::string> m_full_name = {"", ""};
        std::set<std::pair<std::string, int>> getTicketsTypes(std::vector<Ticket> tickets) const;
    public:
        void printMoney() const;
        int getNumberOfCoins(const double value) const;
        void setNumberOfCoins(const double value, const int number);
        void incrementNumberOfCoins(const double value);
        void decrementNumberOfCoins(const double value);
        void setFullName(const std::string first_name, const std::string last_name);
        void givingChange(const double change);
        void clientLoop();
        std::pair<std::string, std::string> getFullName() const;
};