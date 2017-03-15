#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

/* Single order struct. */
struct order{
  int trader_id;
  int item_code;
  unsigned int price;
  unsigned int quantity;
  struct order* next;
  struct order* prev;
};

/* A trade is an executed order. */
struct trade{
  int buyer;
  int seller;
  int item_code;
  int price;
  int quantity;
  struct trade* next;
  // struct trade* prev;
};

/* Type 1 = Buy; Type 2 = Sell. */
void insert_order(int, struct order*, struct order *buy_orders[10], struct order *sell_orders[10], 
                  struct trade *trades);
void insert_trade(struct trade*, struct order *buy_orders[10], struct order *sell_orders[10], 
                  struct trade *trades);
void order_status(int, struct order *buy_orders[10], struct order *sell_orders[10], 
                  struct trade *trades);
void trade_status(int, int, struct order *buy_orders[10], struct order *sell_orders[10], 
                  struct trade *trades);
void execute(int, struct order*, struct order *buy_orders[10], struct order *sell_orders[10], 
                  struct trade *trades);