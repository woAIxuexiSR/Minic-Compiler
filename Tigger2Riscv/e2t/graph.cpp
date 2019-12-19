#include "graph.h"

extern int reg, mem;
vector<VarStore> var_store;

ConflictGraph::ConflictGraph()
{
	g.clear(); sum.clear();
	vars = 0;
}

ConflictGraph::ConflictGraph(int size)
{
	g.resize(size);
	sum.resize(size);
	vars = size;
	for(int i = 0; i < size; ++i)
		g[i].resize(size);
}

void ConflictGraph::add_edge(int i, int j)
{
	if(g[i][j] != 1)
	{
		g[i][j] = g[j][i] = 1;
		sum[i] += 1;
		sum[j] += 1;
	}
}

void ConflictGraph::allocate_reg()
{
	stack<int> p;
	vector<bool> check(vars);

	while(p.size() < vars)
	{
		bool flag = 0;
		for(int i = 0; i < vars; ++i)
		{
			if(!check[i] && sum[i] < REGNUM)
			{
				p.push(i);
				check[i] = 1;
				flag = 1;
				for(int j = 0; j < vars; ++j)
					if(g[i][j]) sum[j] -= 1;
			}
		}	

		if(!flag)
		{
			for(int i = 0; i < vars; ++i)
				if(!check[i])
				{
					p.push(i);
					check[i] = 1;
					for(int j = 0; j < vars; ++j)
						if(g[i][j]) sum[j] -= 1;
					break;
				}
		}
	}

	reg = 0, mem = 0;
	while(!p.empty())
	{
		int t = p.top();
		p.pop();

		for(int i = 0; i < reg; ++i)
		{
			bool flag = 1;
			for(int j = 0; j < vars; ++j)
				if(g[t][j] && var_store[j].type == 1 && var_store[j].index == i)
				{
					flag = 0;
					break;
				}

			if(!flag) continue;
			else
			{
				var_store[t].type = 1;
				var_store[t].index = i;
				break;
			}
		}

		if(var_store[t].type == -1)
		{
			if(reg < REGNUM)
			{
				var_store[t].type = 1;
				var_store[t].index = reg++;
			}
			else
			{
				var_store[t].type = 2;
				var_store[t].index = mem++;
			}
		}
	}
}
