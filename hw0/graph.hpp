#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

namespace hw0
{

class Graph
{
public:

  Graph( uint32_t num_nodes )
  {
    /* TODO: Implement the constructor for the `Graph` class. */
    this->num_nodes = num_nodes;
    adj_list = std::vector<std::vector<int>>(num_nodes);
  }

  ~Graph()
  {
    /* The destructor of the `Graph` class. */
    adj_list.clear();
  }

  void add_edge( uint32_t from, uint32_t to )
  {
    /* TODO: Add a new edge in your data structure. You can remove the following line. */
    if (from == to)
    {
      return;
    } else if (std::find(adj_list[from].begin(), adj_list[from].end(), to) == adj_list[from].end())
    {
      /* The edge has not been added before */
      adj_list[from].push_back(to);
      std::cout << "[i] add edge from " << from << " to " << to << std::endl;
      add_edge(to, from);
    }
  }

  uint32_t num_degree_k( uint32_t k ) const
  {
    /* TODO: Count the number of nodes with degree `k`. */
    int count = 0;
    for (int i=0; i<num_nodes; i++)
    {
      if (adj_list[i].size() == k)
      {
        count++;
      }
    }
    return count;
  }

private:
  /* TODO: Define your data structure. Declare the data members of `Graph` here. 
           Feel free to define and implement other classes (e.g. `Node`) if you want. */
  int num_nodes;
  std::vector<std::vector<int>> adj_list;
};

} // namespace hw0