#include <cstddef>
#include <cstdint>
#include <iostream>
#include <list>
#include <optional>
#include <unordered_map>

enum class Side { BID, ASK };

struct Order {
  uint64_t order_id;
  int price; // in cents, never use float for comparison
  uint32_t quantity;
  Side side;
};

struct PriceLevel {
  int price;
  uint32_t total_quantity = 0;
  std::list<Order> orders; // FIFO queue for each price
  PriceLevel *prev = nullptr;
  PriceLevel *next = nullptr;
};

// Tracks where an order lives for O(1) cancel
struct OrderLocation {
  PriceLevel *level;
  std::list<Order>::iterator it;
};

class HalfBook {
  // One side (all bids OR all asks)
  PriceLevel *best = nullptr;                        // head of DLL
  std::unordered_map<int, PriceLevel *> levels;      // price → level
  std::unordered_map<uint64_t, OrderLocation> index; // order_id → location
  bool is_bid_side; // bids: DLL sorted descending; asks: ascending

public:
  explicit HalfBook(bool bid) : is_bid_side(bid) {}
  void addOrder(Order o) {
    if (levels.count(o.price)) {
      PriceLevel *currPriceLevel = levels.at(o.price);
      currPriceLevel->total_quantity += o.quantity;
      currPriceLevel->orders.push_back(o);
      index[o.order_id] = {currPriceLevel,
                           std::prev(currPriceLevel->orders.end())};
      return;
    }

    PriceLevel *newLevel =
        new PriceLevel{o.price, o.quantity, {}, nullptr, nullptr};
    newLevel->orders.push_back(o);
    index[o.order_id] = {newLevel, newLevel->orders.begin()};
    insertLevel(newLevel);
    return;
  }

  bool cancelOrder(uint64_t order_id) {
    if (!index.count(order_id))
      return false;
    OrderLocation orderLoc = index.at(order_id);
    orderLoc.level->total_quantity -= orderLoc.it->quantity;
    orderLoc.level->orders.erase(orderLoc.it);
    index.erase(order_id);
    if (orderLoc.level->orders.empty())
      removeLevel(orderLoc.level);

    return true;
  }
  PriceLevel *getBest() const { return best; }

  void removeLevel(PriceLevel *lvl) {
    if (lvl->prev)
      lvl->prev->next = lvl->next;
    if (lvl->next)
      lvl->next->prev = lvl->prev;
    if (best == lvl)
      best = lvl->next;
    levels.erase(lvl->price);
    delete lvl;
  }

  void insertLevel(PriceLevel *newLevel) {
    if (best == nullptr) {
      best = newLevel;
      levels[newLevel->price] = newLevel;
      return;
    }
    bool newBest = is_bid_side ? newLevel->price > best->price
                               : newLevel->price < best->price;
    if (newBest) {
      newLevel->next = best;
      best->prev = newLevel;
      best = newLevel;
      levels[newLevel->price] = newLevel;
      return;
    }

    PriceLevel *curr = best;
    while (curr->next && (is_bid_side ? curr->next->price > newLevel->price
                                      : curr->next->price < newLevel->price))
      curr = curr->next;
    newLevel->next = curr->next;
    if (curr->next)
      curr->next->prev = newLevel;
    newLevel->prev = curr;
    curr->next = newLevel;

    levels[newLevel->price] = newLevel;
  }

  bool empty() const { return best == nullptr; }
};

class OrderBook {
  HalfBook bids{true};
  HalfBook asks{false};

public:
  void addOrder(Order o) {
    if (o.side == Side::BID)
      bids.addOrder(o);
    else {
      asks.addOrder(o);
    }
  }

  void cancelOrder(uint64_t order_id) {
    if (!bids.cancelOrder(order_id))
      asks.cancelOrder(order_id);
  }
  void matchOrders() {
    while (!bids.empty() && !asks.empty()) {
      PriceLevel *bidLvl = bids.getBest();
      PriceLevel *askLvl = asks.getBest();

      if (bidLvl->price < askLvl->price)
        break;

      Order &buy = bidLvl->orders.front();
      Order &sell = askLvl->orders.front();
      uint32_t traded = std::min(buy.quantity, sell.quantity);
      std::cout << "traded " << traded << "\n";

      buy.quantity -= traded;
      sell.quantity -= traded;
      bidLvl->total_quantity -= traded;
      askLvl->total_quantity -= traded;

      if (buy.quantity == 0)
        bids.cancelOrder(buy.order_id);
      if (sell.quantity == 0)
        asks.cancelOrder(sell.order_id);
    }
  }
  void print() const {
    std::cout << "===== ORDER BOOK =====\n";

    std::cout << "ASKS:\n";
    for (PriceLevel *lvl = asks.getBest(); lvl != nullptr; lvl = lvl->next)
      std::cout << "  " << lvl->price << " x " << lvl->total_quantity << " ("
                << lvl->orders.size() << " orders)\n";

    std::cout << "BIDS:\n";
    for (PriceLevel *lvl = bids.getBest(); lvl != nullptr; lvl = lvl->next)
      std::cout << "  " << lvl->price << " x " << lvl->total_quantity << " ("
                << lvl->orders.size() << " orders)\n";

  }
};
