#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <iostream>
#include <vector>
#include <stack>

using namespace std;

#define REGNUM 16

class ConflictGraph
{
public:
	vector<vector<bool> > g;
	vector<int> sum;
	int vars;

	ConflictGraph();
	ConflictGraph(int size);
	void add_edge(int i, int j);
	void allocate_reg();
};


// s0-s11 callee save, t0-t6 caller save, a0-a7 caller save convert paramters
/******************
  0-6 : t0-t6
  7-18 : s0-s11
  19-26 : a0-a7
  ******************/

class VarStore
{
public:
	int type; // 1 reg, 2 memory
	int index;

	VarStore():type(-1), index(-1) {}
};

#endif
