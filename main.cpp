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
     bool flag = t.member(c);
     if(flag == true)
	{
	  int h = c;
          auto write_tvar = stm::writeTVar(global_tree, deleted(h,t));
	  auto f_return_h = [h](stm::Unit)
	  {
	    return stm::pure(h);
	  };
	return stm::bind<stm::Unit, int>(write_tvar, f_return_h);
     } 
     else 
     {
	  return stm::pure(-1);
     }

  };
  auto pop_transaction = stm::withTVar<RBTree<int>, int>(global_tree, f_pop_from_tree);
  return stm::atomically(cxt, pop_transaction);
}

void fill_tree(int start)
{
  for(int i = start - 1; i < NUM_ELEMENTS; i += 3)
  {
    std::cout << "writing " << i <<std::endl;
    insert_to_tree(i);
  }
}
void read_from_tree(int* acc)
{
  for(int i = 0; i < NUM_ELEMENTS; i++)
  {
	std::cout << "reading  " << i << std::endl;
	int tmp = pop_from_tree(i);
	if(tmp != -1)
		*acc += tmp;
  }
}

int main()
{
    int acc1 = 0;
    int acc2 = 0;
    int acc3 = 0;

    global_tree = stm::newTVarIO<RBTree<int>>(cxt, RBTree<int>());
    std::thread fl1(fill_tree, 1);
    std::thread fl2(fill_tree, 2);
    std::thread fl3(fill_tree, 3);
    /*std::thread fl4(fill_tree, 4);
    std::thread fl5(fill_tree, 5);
    std::thread fl6(fill_tree, 6);
    std::thread fl7(fill_tree, 7);
    std::thread fl8(fill_tree, 8);
    std::thread fl9(fill_tree, 9);
    std::thread fl10(fill_tree, 10);
    std::thread fl11(fill_tree, 11);
    std::thread fl12(fill_tree, 12);*/
    std::thread rd1(read_from_tree, &acc1);
    std::thread rd2(read_from_tree, &acc2);
    std::thread rd3(read_from_tree, &acc3);


    fl1.join();
    fl2.join();
    fl3.join();
    /*fl4.join();
    fl5.join();
    fl6.join();
    fl7.join();
    fl8.join();
    fl9.join();
    fl10.join();
    fl11.join();
    fl12.join();*/

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
