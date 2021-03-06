#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <unordered_map>
#include <list>
#include <queue>
#include "PartialOrderPlanner.h"
#include "tests/Sussman.h"
#include "tests/Briefcase.h"
#include "tests/FruitBowl.h"
#include "tests/Table.h"

using namespace std;

vector<long> topologicalSort(PartialOrderPlan plan, long vert)
{
    vector<long> totalOrderPlan;
    long numVertices = plan.ordering.size();
    long* parent = new long[numVertices];

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        parent[i] = -1;
    }

    list<long> stack;

    parent[vert] = -1;
    stack.push_back(vert);

    vector<TemporalLink>::iterator itr;

    while(!stack.empty())
    {
        vert = stack.back();
        cout << vert << ": ";

        for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
        {
            if (parent[itr->targetOperator] < 0 && itr->isBefore)
            {
                cout << "isBefore (" << itr->isBefore << ") -> " << itr->targetOperator << "| ";
                parent[itr->targetOperator] = vert;
                stack.push_back(itr->targetOperator);
                break;
            }
        }

        if (itr == plan.ordering[vert].end())
        {
            totalOrderPlan.push_back(vert);
            stack.pop_back();
        }

        cout << "\n";
    }

    return totalOrderPlan;
}

void BFS(PartialOrderPlan plan, long vert)
{
    long numVertices = plan.steps.size();
    bool* visited = new bool[numVertices];
    for (unsigned int i = 0; i < numVertices; ++i)
    {
        visited[i] = false;
    }

    list<long> queue;

    visited[vert] = true;
    queue.push_back(vert);

    vector<TemporalLink>::iterator itr;

    while(!queue.empty())
    {
        vert = queue.front();
        cout << vert << ": ";
        queue.pop_front();

        for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
        {
            if (!visited[itr->targetOperator])
            {
                cout << "isBefore (" << itr->isBefore << ") -> " << itr->targetOperator << "| ";
                visited[itr->targetOperator] = true;
                queue.push_back(itr->targetOperator);
            }
        }

        cout << "\n";
    }
}

void DFSVisit(PartialOrderPlan plan, long vert)
{
    long numVertices = plan.ordering.size();
    long* parent = new long[numVertices];

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        parent[i] = -1;
    }

    list<long> stack;

    parent[vert] = -1;
    stack.push_back(vert);

    vector<TemporalLink>::iterator itr;

    while(!stack.empty())
    {
        vert = stack.back();
        cout << vert << ": ";

        for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
        {
            if (parent[itr->targetOperator] < 0 && itr->isBefore)
            {
                cout << "isBefore (" << itr->isBefore << ") -> " << itr->targetOperator << "| ";
                parent[itr->targetOperator] = vert;
                stack.push_back(itr->targetOperator);
                break;
            }
        }

        if (itr == plan.ordering[vert].end())
        {
            stack.pop_back();
        }

        cout << "\n";
    }
}

bool hasCycle(PartialOrderPlan plan, long vert)
{
    long numVertices = plan.ordering.size();
    long* parent = new long[numVertices];

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        parent[i] = -1;
    }

    list<long> stack;

    parent[vert] = -1;
    stack.push_back(vert);

    vector<TemporalLink>::iterator itr;

    while(!stack.empty())
    {
        vert = stack.back();
        cout << "Visiting: " << plan.steps[vert].name << endl;

        for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
        {
            if (itr->isBefore)
            {
                if (parent[itr->targetOperator] < 0)
                {
                    parent[itr->targetOperator] = vert;
                    stack.push_back(itr->targetOperator);
                    break;
                }
                else
                {
                    for (auto ancestor = stack.begin(); ancestor != stack.end(); ++ancestor)
                    {
                        if (*ancestor == itr->targetOperator)
                        {
                            cout << "\nCycle Found! With: " << plan.steps[itr->targetOperator].name << "\n" << endl;
                            return true;
                        }
                    }
                }
            }
        }

        if (itr == plan.ordering[vert].end())
        {
            cout << "Finished Visiting: " << vert << endl;
            stack.pop_back();
        }

        cout << "\n";
    }

    return false;
}

int main()
{
    // TODO: Building new operators should be made much quicker/easier than this
    // TODO: check how long the algorithm takes

    SussmanDomain sussmanDomain;
    BriefcaseDomain briefcaseDomain;
    FruitBowlDomain fruitBowlDomain;
    TableDomain tableDomain;
    PartialOrderPlanner partialOrderPlanner(sussmanDomain.operators);
    vector<PartialOrderPlan> plans = partialOrderPlanner.findPartialOrderPlan(sussmanDomain.start, sussmanDomain.finish);

    // Display plan's Causal links
    if (plans.size() > 0)
    {
        PartialOrderPlan plan = plans[0];

        cout << endl;
        for (auto l = plan.steps.begin(); l != plan.steps.end(); ++l)
        {
            cout << l->second.name << endl;
        }
        cout << endl;

        int id = 0;
        for (auto itr = plan.links.begin(); itr != plan.links.end(); ++itr)
        {
            vector<Goal> links = *itr;

            for (auto j = links.begin(); j != links.end(); ++j)
            {
                cout << "Final: Causal Link From: " << plan.steps.at(id).name << " To: " << plan.steps.at(j->step).name << " For Condition: " << j->condition << endl;
            }
            ++id;
        }

        cout << endl;
        // Display plan's ordering
        id = 0;
        for (auto i = plan.ordering.begin(); i != plan.ordering.end(); ++i)
        {
            vector<TemporalLink> order = *i;

            for (auto t = order.begin(); t != order.end(); ++t)
            {
                cout << "Temporal Link For " << plan.steps.at(id).name << " To: " << plan.steps.at(t->targetOperator).name << " Is Before? " << t->isBefore << endl;
            }
            ++id;
        }

        cout << endl << "Building totoal order plan" << endl;
        vector<long> totalOrderPlan = topologicalSort(plans[0], plans[0].start);//getTotalOrderPlan(plans[0]);

        cout << endl;
        cout << "Total Order Plan (One of them)" << endl;
        cout << endl;

        for (auto step = totalOrderPlan.begin(); step != totalOrderPlan.end(); ++step)
        {
            unordered_map<long, Operator>::const_iterator op = plans[0].steps.find(*step);
            if (op != plans[0].steps.end())
                cout << op->second.name << endl;
            else
                cout << "Oops: something is wrong" << endl;
        }
    }

    return 0;
}


