#include "orderbook.cpp"
#include <sstream>
#include <string>
// This file is ai-generated and not hand written
static int tests_run = 0;
static int tests_failed = 0;

void check(bool cond, const std::string &name) {
  tests_run++;
  if (cond) {
    std::cout << "[PASS] " << name << "\n";
  } else {
    tests_failed++;
    std::cout << "[FAIL] " << name << "\n";
  }
}

std::string capture_match(OrderBook &ob) {
  std::ostringstream cap;
  std::streambuf *old = std::cout.rdbuf(cap.rdbuf());
  ob.matchOrders();
  std::cout.rdbuf(old);
  return cap.str();
}

void test_bid_ordering() {
  HalfBook b(true);
  b.addOrder({1, 100, 10, Side::BID});
  b.addOrder({2, 90, 5, Side::BID});
  b.addOrder({3, 110, 7, Side::BID});
  PriceLevel *l = b.getBest();
  check(l && l->price == 110, "bid best is highest");
  check(l && l->next && l->next->price == 100, "bid second is 100");
  check(l && l->next && l->next->next && l->next->next->price == 90,
        "bid third is 90");
  check(l && l->next && l->next->next && l->next->next->next == nullptr,
        "bid list terminates");
}

void test_ask_ordering() {
  HalfBook a(false);
  a.addOrder({1, 100, 10, Side::ASK});
  a.addOrder({2, 110, 5, Side::ASK});
  a.addOrder({3, 90, 7, Side::ASK});
  PriceLevel *l = a.getBest();
  check(l && l->price == 90, "ask best is lowest");
  check(l && l->next && l->next->price == 100, "ask second is 100");
  check(l && l->next && l->next->next && l->next->next->price == 110,
        "ask third is 110");
}

void test_level_aggregation() {
  HalfBook b(true);
  b.addOrder({1, 100, 10, Side::BID});
  b.addOrder({2, 100, 15, Side::BID});
  PriceLevel *l = b.getBest();
  check(l && l->price == 100, "single price level");
  check(l && l->total_quantity == 25, "quantities aggregated");
  check(l && l->orders.size() == 2, "two orders queued fifo");
  check(l && l->next == nullptr, "only one level exists");
}

void test_cancel_non_first_order() {
  HalfBook b(true);
  b.addOrder({1, 100, 10, Side::BID});
  b.addOrder({2, 100, 15, Side::BID});
  bool ok = b.cancelOrder(2);
  PriceLevel *l = b.getBest();
  check(ok, "cancel of non-first order returns true");
  check(l && l->total_quantity == 10, "quantity reduced after cancel");
  check(l && l->orders.size() == 1, "one order remains");
}

void test_cancel_empties_level() {
  HalfBook b(true);
  b.addOrder({1, 100, 10, Side::BID});
  b.addOrder({2, 90, 5, Side::BID});
  b.cancelOrder(1);
  PriceLevel *l = b.getBest();
  check(l && l->price == 90, "best advances when level removed");
  b.cancelOrder(2);
  check(b.empty(), "book empty after all cancelled");
}

void test_cancel_missing() {
  HalfBook b(true);
  check(!b.cancelOrder(999), "cancel of missing id returns false");
}

void test_match_full_cross() {
  OrderBook ob;
  ob.addOrder({1, 100, 10, Side::BID});
  ob.addOrder({2, 100, 10, Side::ASK});
  check(capture_match(ob) == "traded 10\n", "equal sizes fully cross");
}

void test_match_partial() {
  OrderBook ob;
  ob.addOrder({1, 100, 10, Side::BID});
  ob.addOrder({2, 100, 4, Side::ASK});
  check(capture_match(ob) == "traded 4\n", "smaller ask fully fills once");
}

void test_match_no_cross() {
  OrderBook ob;
  ob.addOrder({1, 99, 10, Side::BID});
  ob.addOrder({2, 100, 10, Side::ASK});
  check(capture_match(ob).empty(), "no trade when prices do not cross");
}

void test_match_multiple_levels() {
  OrderBook ob;
  ob.addOrder({1, 100, 5, Side::BID});
  ob.addOrder({2, 101, 5, Side::BID});
  ob.addOrder({3, 100, 8, Side::ASK});
  check(capture_match(ob) == "traded 5\ntraded 3\n",
        "sweeps across price levels");
}

void test_orderbook_cancel_routes_both_sides() {
  OrderBook ob;
  ob.addOrder({1, 100, 10, Side::BID});
  ob.addOrder({2, 105, 10, Side::ASK});
  ob.cancelOrder(2);
  ob.cancelOrder(1);
  check(capture_match(ob).empty(), "cancelled orders leave nothing to match");
}

int main() {
  test_bid_ordering();
  test_ask_ordering();
  test_level_aggregation();
  test_cancel_non_first_order();
  test_cancel_empties_level();
  test_cancel_missing();
  test_match_full_cross();
  test_match_partial();
  test_match_no_cross();
  test_match_multiple_levels();
  test_orderbook_cancel_routes_both_sides();

  std::cout << "\n"
            << (tests_run - tests_failed) << "/" << tests_run << " passed\n";
  return tests_failed == 0 ? 0 : 1;
}
