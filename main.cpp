#include "List.h"
#include "RBTree.h"
#include <unistd.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <thread>
#include <atomic>
#include <string>
#include "stm/stm.h"
#include "stm/context.h"

#define NUM_ELEMENTS 10000

stm::Context cxt;
stm::TVar<List<int>> global_list;
stm::TVar<RBTree<int>> global_tree;


void insert_to_tree(int c)
{
  auto f_insert_to_tree = [&](RBTree<int> t)
  {
    return stm::writeTVar(global_tree, t.inserted(c));
  };
  auto push_transaction = stm::withTVar<RBTree<int>, stm::Unit>(global_tree, f_insert_to_tree);
  stm::atomically(cxt, push_transaction);
}

int pop_from_tree(int c)
{
  auto f_pop_from_tree = [&](RBTree<int> t)
  {
    if(t.isEmpty())
      return stm::retry<int>();
    else
    {
      bool flag = t.member(c);
      if(flag == true)
      {
      	int h = c;
      	auto write_tvar = stm::writeTVar(global_tree, t.inserted(h));
      	auto f_return_h = [h](stm::Unit)
        {
          return stm::pure(h);
        };
        return stm::bind<stm::Unit, int>(write_tvar, f_return_h);
      }    
    }
  };
  auto pop_transaction = stm::withTVar<RBTree<int>, int>(global_tree, f_pop_from_tree);
  return stm::atomically(cxt, pop_transaction);
}

void fill_tree()
{
  for(int i = 0; i < NUM_ELEMENTS; i++)
  {
    insert_to_tree(i);
  }
}

void read_from_tree(int* acc)
{
  for(int i = 0; i < NUM_ELEMENTS; i++)
  {
    *acc += pop_from_tree(i);
  }
}

int main()
{
    int acc1 = 0;
    int acc2 = 0;
    int acc3 = 0;

    global_tree = stm::newTVarIO<RBTree<int>>(cxt, RBTree<int>());
    std::thread fl1(fill_tree);
    std::thread fl2(fill_tree);
    std::thread fl3(fill_tree);
    std::thread rd1(read_from_tree, &acc1);
    std::thread rd2(read_from_tree, &acc2);
    std::thread rd3(read_from_tree, &acc3);


    fl1.join();
    fl2.join();
    fl3.join();
    rd1.join();
    rd2.join();
    rd3.join();

    int acc = acc1 + acc2 + acc3;

    std::cout << "acc1 = " << acc1 << std::endl;
    std::cout << "acc2 = " << acc2 << std::endl;
    std::cout << "acc3 = " << acc3 << std::endl;
    std::cout << "acc = " << acc << std::endl;

    return 0;

}
