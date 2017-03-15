#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "list.h"

// extern struct order* buy_orders[10] = {NULL};
// extern struct order* sell_orders[10] = {NULL};
// extern struct trade* trades;

/* Type 1 = Buy; Type 2 = Sell. */
void insert_order(int type, struct order *ord, struct order *buy_orders[10], struct order *sell_orders[10], 
                  struct trade *trades)
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
    head = ord;
    flag = 1;
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
  if(type==1)
    buy_orders[ord->item_code] = head;
  else
    sell_orders[ord->item_code] = head;
  execute(type, ord, buy_orders, sell_orders, trades);
}

void insert_trade(struct trade* tr, struct order *buy_orders[10], struct order *sell_orders[10], 
                  struct trade *trades)
{
    tr->next = trades;
    trades = tr;
}

void order_status(int new_socket, struct order *buy_orders[10], struct order *sell_orders[10], 
                  struct trade *trades)
{
    int i;
    char msg[1024] = {0}, temp[1024] = {0};
    memset(msg, 0, 1024);
    for(i=0;i<10;i++)
    {
      if(sell_orders[i]!=NULL) {
        sprintf(msg, "%d", i+1);
        sprintf(temp, "%d", sell_orders[i]->price);
        strcat(msg, " ");
        strcat(msg, temp);
        sprintf(temp, "%d", sell_orders[i]->quantity);
        strcat(msg, " ");
        strcat(msg, temp);
        sleep(1); send(new_socket, msg, strlen(msg) + 1, 0);
        printf("mehul %s\n", msg);
        printf("Item Code %d \t\t Best Price %d \t\t Quantity %d\n", i+1, sell_orders[i]->price,
            sell_orders[i]->quantity);
      }
      memset(msg, 0, 1024);
    }
    strcpy(msg, "end1\0");
    sleep(1); send(new_socket, msg, strlen(msg) + 1, 0);
        printf("mehul %s\n", msg);
    for(i=0;i<10;i++)
    {
        if(buy_orders[i]!=NULL) {
        sprintf(msg, "%d", i+1);
        sprintf(temp, "%d", buy_orders[i]->price);
        strcat(msg, " ");
        strcat(msg, temp);
        sprintf(temp, "%d", buy_orders[i]->quantity);
        strcat(msg, " ");
        strcat(msg, temp);
        sleep(1); send(new_socket, msg, strlen(msg) + 1, 0);
        printf("mehul %s\n", msg);
        printf("Item Code %d \t\t Best Price %d \t\t Quantity %d\n", i+1, buy_orders[i]->price,
            buy_orders[i]->quantity);
      }
      memset(msg, 0, 1024);
    }
    strcpy(msg, "end2\0");
    sleep(1); send(new_socket, msg, strlen(msg) + 1, 0);
        printf("mehul %s\n", msg);
}

void trade_status(int trader_id, int new_socket, struct order *buy_orders[10], struct order *sell_orders[10], 
                  struct trade *trades)
{
    struct trade* t = trades;

    char msg[1024] = {0}, temp[1024] = {0};
    while(t!=NULL)
    {
        if(t->seller == trader_id || t->buyer == trader_id)
        {
            int counterparty;
            if(t->seller==trader_id)
            {
                strcpy(msg, "Sell_Order");
                strcat(msg, " ");
                // printf("Sell Order\n");
                // counterparty = t->buyer;
            }
            else
            {
                strcpy(msg, "Buy_Order");
                strcat(msg, " ");
                // printf("Buy Order\n");
                counterparty = t->seller;
            }

            sprintf(temp, "%d", t->item_code);
            strcat(msg, temp);
            strcat(msg, " ");
            sprintf(temp, "%d", t->price);
            strcat(msg, temp);
            strcat(msg, " ");
            sprintf(temp, "%d", t->quantity);
            strcat(msg, temp);
            strcat(msg, " ");
            sprintf(temp, "%d", counterparty+1);
            strcat(msg, temp);
            sleep(1); 
            send(new_socket, msg, strlen(msg) + 1, 0);

            // printf("Item Code - %d \t\t Price - %d \t\t Quantity - %d \t\t Counterparty Code - %d\n",
                // t->item_code, t->price, t->quantity, counterparty);
        }
        t = t->next;
    }
    strcpy(msg, "end\0");
    sleep(1); 
    send(new_socket, msg, strlen(msg) + 1, 0);
}

void execute(int type, struct order *ord, struct order *buy_orders[10], struct order *sell_orders[10], 
                  struct trade *trades)
{
  // return;
  struct order* o;
    if (type == 1) {
      o = sell_orders[ord->item_code];
      while (o != NULL && ord->quantity > 0) {
        if (o->price <= ord->price && o->quantity > 0) {
          int m = (o->quantity < ord->quantity ? o->quantity : ord->quantity);
          printf("Trading sell %d\n", m);
          o->quantity = o->quantity - m;
          ord->quantity = ord->quantity - m;

          struct trade* tr = (struct trade*)mmap(NULL, sizeof(struct trade), PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
          tr->buyer = ord->trader_id;
          tr->seller = o->trader_id;
          tr->item_code = ord->item_code;
          tr->price = o->price;
          tr->quantity = m;
          insert_trade(tr, buy_orders, sell_orders, trades);
        }
        o=o->next;
      }
    } else {
      o = buy_orders[ord->item_code];
      while (o != NULL && ord->quantity > 0) {
        if (o->price >= ord->price && o->quantity > 0) {
          int m = (o->quantity < ord->quantity ? o->quantity : ord->quantity);
          o->quantity = o->quantity - m;
          printf("Trading sell %d\n", m);
          ord->quantity = ord->quantity - m;

          struct trade* tr = (struct trade*)mmap(NULL, sizeof(struct trade), PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
          tr->buyer = o->trader_id;
          tr->seller = ord->trader_id;
          tr->item_code = ord->item_code;
          tr->price = ord->price;
          tr->quantity = m;
          insert_trade(tr, buy_orders, sell_orders, trades);
        }
        o=o->next;
      }
    }
}