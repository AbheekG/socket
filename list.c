#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// /* Single order struct. */
struct order{
  int trader_id;
  int item_code;
  unsigned int price;
  unsigned int quantity;
  struct order* next;
  struct order* prev;
};

// /* A trade is an executed order. */
struct trade{
  int buyer;
  int seller;
  int item_code;
  int price;
  int quantity;
  struct trade* next;
  struct trade* prev;
};

struct order* buy_orders[10] = {NULL};
struct order* sell_orders[10] = {NULL};
struct trade* trades;

/* Type 1 = Buy; Type 2 = Sell. */
void insert_order(int type, struct order *ord)
{
  // Index into the array for the required list.
  struct order *o, *head;
  if(type==1)
    o = buy_orders[ord->item_code];
  else
    o = sell_orders[ord->item_code];
  head = o;
  int flag = 0;
  if(o==NULL)
  {
    o = ord;
    ord->next = NULL;
    ord->prev = o;
  }
  else
      while(o!=NULL && flag!=1)
      {  
        // Buy orders are sorted in decreasing order.
        if(type==1 && o->price < ord->price || type==2 && o->price>ord->price)
        {
            if(head==o)
            {
                ord->next = head;
                head->prev = ord;
                head = ord;
            }
            else
            {
                ord->prev = o->prev;
                ord->next = o;
                (o->prev)->next = ord;
                o->prev = ord;
            }
            flag = 1;
        }
        else
            o = o->next;
      }
  if(flag == 0)
    printf("Should not print ever\n");
}

void insert_trade(struct trade* tr)
{
    tr->next = trades;
    trades = tr;
}

void order_status()
{
    int i;
    printf("\nSell Orders\n");
    printf("===========\n");
    for(i=0;i<10;i++)
    {
        printf("Item Code %d \t\t Best Price %d \t\t Quantity %d\n", i+1, sell_orders[i]->price,
            sell_orders[i]->quantity);
    }
    printf("\nBuy Orders\n");
    printf("==========\n");
    for(i=0;i<10;i++)
    {
        printf("Item Code %d \t\t Best Price %d \t\t Quantity %d\n", i+1, buy_orders[i]->price,
            buy_orders[i]->quantity);
    }
}

void trade_status(int trader_id)
{
    struct trade* t = trades;
    while(t!=NULL)
    {
        if(t->seller == trader_id || t->buyer == trader_id)
        {
            int counterparty;
            if(t->seller==trader_id)
            {
                printf("Sell Order\n");
                counterparty = t->buyer;
            }
            else
            {
                printf("Buy Order\n");
                counterparty = t->seller;
            }

            printf("Item Code - %d \t\t Price - %d \t\t Quantity - %d \t\t Counterparty Code - %d\n",
                t->item_code, t->price, t->quantity, counterparty);
        }
        t = t->next;
    }
}

void execute()
{
    
}