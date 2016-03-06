//
// Created by Dan on 3/5/2016.
//

#ifndef PARTIALORDERPLANNER_EXTRA_H
#define PARTIALORDERPLANNER_EXTRA_H

#include <PartialOrderPlan.h>

inline std::vector<unsigned> topologicalSort(PartialOrderPlan plan, unsigned vert)
{
    std::vector<unsigned> totalOrderPlan;
    unsigned numVertices = plan.ordering.size();
    int* parent = new int[numVertices];

    for (unsigned i = 0; i < numVertices; ++i)
    {
        parent[i] = -1;
    }

    std::list<unsigned> stack;

    parent[vert] = -1;
    stack.push_back(vert);

    std::vector<TemporalLink>::iterator itr;

    while(!stack.empty())
    {
        vert = stack.back();
        //cout << vert << ": ";

        for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
        {
            if (parent[itr->targetOperator] < 0 && itr->isBefore)
            {
                //cout << "isBefore (" << itr->isBefore << ") -> " << itr->targetOperator << "| ";
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

        //cout << "\n";
    }

    return totalOrderPlan;
}

inline void BFS(PartialOrderPlan plan, unsigned vert)
{
    unsigned numVertices = plan.steps.size();
    bool* visited = new bool[numVertices];
    for (unsigned i = 0; i < numVertices; ++i)
    {
        visited[i] = false;
    }

    std::list<unsigned> queue;

    visited[vert] = true;
    queue.push_back(vert);

    std::vector<TemporalLink>::iterator itr;

    while(!queue.empty())
    {
        vert = queue.front();
        queue.pop_front();

        for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
        {
            if (!visited[itr->targetOperator])
            {
                visited[itr->targetOperator] = true;
                queue.push_back(itr->targetOperator);
            }
        }

        std::cout << "\n";
    }
}

inline void DFSVisit(PartialOrderPlan plan, unsigned vert)
{
    unsigned numVertices = plan.ordering.size();
    int* parent = new int[numVertices];

    for (unsigned i = 0; i < numVertices; ++i)
    {
        parent[i] = -1;
    }

    std::list<unsigned> stack;

    parent[vert] = -1;
    stack.push_back(vert);

    std::vector<TemporalLink>::iterator itr;

    while(!stack.empty())
    {
        vert = stack.back();
        std::cout << vert << ": ";

        for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
        {
            if (parent[itr->targetOperator] < 0 && itr->isBefore)
            {
                std::cout << "isBefore (" << itr->isBefore << ") -> " << itr->targetOperator << "| ";
                parent[itr->targetOperator] = vert;
                stack.push_back(itr->targetOperator);
                break;
            }
        }

        if (itr == plan.ordering[vert].end())
        {
            stack.pop_back();
        }

        std::cout << "\n";
    }
}

inline bool hasCycle(PartialOrderPlan plan, unsigned vert)
{
    unsigned numVertices = plan.ordering.size();
    int* parent = new int[numVertices];

    for (unsigned i = 0; i < numVertices; ++i)
    {
        parent[i] = -1;
    }

    std::list<unsigned> stack;

    parent[vert] = -1;
    stack.push_back(vert);

    std::vector<TemporalLink>::iterator itr;

    while(!stack.empty())
    {
        vert = stack.back();
        std::cout << "Visiting: " << plan.steps[vert].name << std::endl;

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
                            std::cout << "\nCycle Found! With: " << plan.steps[itr->targetOperator].name << "\n" << std::endl;
                            return true;
                        }
                    }
                }
            }
        }

        if (itr == plan.ordering[vert].end())
        {
            std::cout << "Finished Visiting: " << vert << std::endl;
            stack.pop_back();
        }

        std::cout << "\n";
    }

    return false;
}

inline void printPlanInformation(PartialOrderPlan plan)
{
    std::cout << std::endl;
    for (auto l = plan.steps.begin(); l != plan.steps.end(); ++l)
    {
        std::cout << l->second.name << std::endl;
    }
    std::cout << std::endl;

    int id = 0;
    for (auto itr = plan.links.begin(); itr != plan.links.end(); ++itr)
    {
        std::vector<Goal> links = *itr;

        for (auto j = links.begin(); j != links.end(); ++j)
        {
            std::cout << "Final: Causal Link From: " << plan.steps.at(id).name << " To: " << plan.steps.at(j->step).name << " For Condition: " << j->condition << std::endl;
        }
        ++id;
    }

    std::cout << std::endl;
    // Display plan's ordering
    id = 0;
    for (auto i = plan.ordering.begin(); i != plan.ordering.end(); ++i)
    {
        std::vector<TemporalLink> order = *i;

        for (auto t = order.begin(); t != order.end(); ++t)
        {
            std::cout << "Temporal Link For " << plan.steps.at(id).name << " To: " << plan.steps.at(t->targetOperator).name << " Is Before? " << t->isBefore << std::endl;
        }
        ++id;
    }

    std::cout << std::endl << "Building total order plan" << std::endl;
    std::vector<unsigned> totalOrderPlan = topologicalSort(plan, plan.start);//getTotalOrderPlan(plans[0]);

    std::cout << std::endl;
    std::cout << "Total Order Plan (One of them)" << std::endl;
    std::cout << std::endl;

    for (auto step = totalOrderPlan.begin(); step != totalOrderPlan.end(); ++step)
    {
        std::unordered_map<unsigned, Operator>::iterator op = plan.steps.find(*step);
        if (op != plan.steps.end())
            std::cout << op->second.name << std::endl;
        else
            std::cout << "Oops: something is wrong" << std::endl;
    }
}

inline std::vector<unsigned> getTotalOrderPlan(PartialOrderPlan plan)
{
    std::cout << std::endl << "Building total order plan" << std::endl;
    std::vector<unsigned> totalOrderPlan = topologicalSort(plan, plan.start);

    std::cout << std::endl;
    std::cout << "Total Order Plan (One of them)" << std::endl;
    std::cout << std::endl;

    for (auto step = totalOrderPlan.begin(); step != totalOrderPlan.end(); ++step)
    {
        std::unordered_map<unsigned, Operator>::iterator op = plan.steps.find(*step);
        if (op != plan.steps.end())
            std::cout << op->second.name << std::endl;
        else
            std::cout << "Oops: something is wrong" << std::endl;
    }

    std::cout << std::endl;

    return totalOrderPlan;
}

#endif //PARTIALORDERPLANNER_EXTRA_H
