#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <unordered_map>
#include <list>
#include <queue>
#include <string>

using namespace std;

static const long NONE = -1;

enum Goals
{
    tableCleared,
    onTableCloth,
    outGlasses,
    outPlates,
    outSilverware
};

enum SussmanGoals
{
    ConA, // 0
    AonC, // 1
    BonA, // 2
    BonC, // 3
    AonB, // 4
    ConB, // 5
    AOnTable, // 6
    BOnTable, // 7
    COnTable, // 8
    clearB, // 9
    clearC, // 10
    clearA, // 11
    holdingA, // 12
    holdingB, // 13
    holdingC, // 14
    handEmpty // 15
};

enum BriefcaseGoals
{
    dictionaryAtOffice = 0, // 0
    paycheckAtOffice = 1, // 1
    dictionaryAtHome = 2, // 2
    paycheckAtHome = 3, // 3
    dictionaryInBriefcase = 4, // 4
    paycheckInBriefcase = 5, // 5
    briefcaseAtHome = 6, // 6
    briefcaseAtOffice = 7 // 7
};

enum FruitBowlGoals
{
    holdingBanana, // 0
    holdingOrange, // 1
    holdingPear, // 2
    pearOnTable, // 3
    pearInBowl, // 4
    bananaOnTable, // 5
    bananaInBowl, // 6
    orangeOnTable, // 7
    orangeInBowl, // 8
    handsEmpty, // 9
    hungry, // 10
    notHungry, // 11
    orangeNotEaten, // 12
    orangeEaten, // 13
    bananaNotEaten, // 14
    bananaEaten, // 15
    pearNotEaten, // 16
    pearEaten // 17
};

struct Operator
{
    string name;
    long id;
    vector<int> preconditions;
    vector<int> addedEffects;
    vector<int> subtractedEffects;

    Operator(string op = "") : name(op)
    {
        id = -1;
    }
};

struct Goal
{
    int condition;
    long step; // the step that the condition is for
    Goal(long owner, int precondition) : condition(precondition), step(owner) {}
};

struct TemporalLink
{
    TemporalLink(long target, bool beforeTarget) :
            targetOperator(target),
            isBefore(beforeTarget) {}

    //Operator targetOperator;
    long targetOperator;
    bool isBefore; // if step1 < step2 (if step1 must happen before step2)
};

struct CausalLink
{
    CausalLink(long target, int satisfiedCondition) :
            targetOperator(target),
            condition(satisfiedCondition) {}

    //Operator targetOperator;
    long targetOperator;
    int condition;
};

struct State
{
public:
    State() {}

    vector<int> variables;

    void addVariables(vector<int> vars)
    {
        vector<int>::iterator itr = vars.begin();

        while (itr != vars.end())
        {
            if (find(begin(variables), end(variables), *itr) == variables.end())
            {
                variables.push_back(*itr);
            }

            ++itr;
        }
    }

    void removeVariables(vector<int> vars)
    {
        vector<int>::iterator itr = vars.begin();
        while (itr != vars.end())
        {
            variables.erase(remove(variables.begin(), variables.end(), *itr), variables.end());
            ++itr;
        }
    }

};

struct Threat
{
    long vulnerableOperator;
    CausalLink vulnerableLink;
    long operatorsThreat;

    Threat (long vulnerableOp, CausalLink causalLink, long threat)
            : vulnerableOperator(vulnerableOp), vulnerableLink(causalLink), operatorsThreat(threat) {}
};

struct Plan
{
    // TODO: determine if unordered_map is the best fit for this
    State worldState;
    unordered_map<long, Operator> steps;
    vector< vector<TemporalLink> > ordering;
    vector< vector<CausalLink> > links;
    vector<Threat> threats;
    queue<Goal> open;
    long start;
    long end;

    // PONDER: Is it better to leave plan as plain data?
    void addGoals(Operator newOperator)
    {
        vector<int>::iterator itr = newOperator.preconditions.begin();

        for (;itr != newOperator.preconditions.end();++itr)
        {
            open.push(Goal(newOperator.id, *itr));
        }
    }

    void addTemporalLink(long op, long target, bool isBefore)
    {
        TemporalLink temporalLinkBefore(target, isBefore);
        ordering[op].push_back(temporalLinkBefore);

        TemporalLink temporalLinkAfterNew(op, !isBefore);
        ordering[target].push_back(temporalLinkAfterNew);
    }

    // TODO: operator addition should always add the temporal links for start and finish. Might not need the operatorBefore if it will always be start.
    void operatorAddition(Operator& newOperator,
                              const long& causalTarget,
                              const int& precondition,
                              const long& operatorBefore,
                              const long& operatorAfter)
    {
        newOperator.id = ordering.size();

        ordering.push_back({});

        if (operatorBefore >= 0)
        {
            addTemporalLink(newOperator.id, operatorBefore, true);
        }

        if (operatorAfter >= 0)
        {
            addTemporalLink(newOperator.id, operatorAfter, false);
        }

        if (causalTarget >= 0)
        {
            CausalLink causalLink(causalTarget, precondition);
            links.push_back({causalLink});
        }
        else
        {
            links.push_back({});
        }

        addGoals(newOperator);

        steps[newOperator.id] = newOperator;
    }
};

//TODO: this method should be turned into a constructor probably
Plan makeInitialPlan(Operator& initialState, Operator& endGoal)
{
    Plan plan;
    //NOTE: can't form temporal links right from the start as Operator's ids are initially set to -1 :(...
    plan.operatorAddition(initialState, NONE, NONE, NONE, NONE);
    plan.operatorAddition(endGoal, NONE, NONE, NONE, NONE);

    plan.addTemporalLink(initialState.id, endGoal.id, true);

    plan.start = initialState.id;
    plan.end = endGoal.id;

    return plan;
}

Goal* selectSubgoal(Plan& plan)
{
    if (plan.open.size() == 0)
    {
        return nullptr;
    }

    Goal* goal = new Goal(plan.open.front());
    plan.open.pop();

    cout << "- Goal Selected -" << endl;
    cout << "Goal Condition: " << goal->condition << " For Step: " << goal->step << endl;

    return goal;
}

bool BFSVertComesBeforeVert2(Plan plan, long vert, long vert2)
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
        queue.pop_front();

        if (vert == vert2) return true;

        for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
        {
            if (!visited[itr->targetOperator])
            {
                if (itr->isBefore)
                {
                    visited[itr->targetOperator] = true;
                    queue.push_back(itr->targetOperator);
                }
            }
        }
    }

    return false;
}

bool BFSVertComesAfterVert2(Plan plan, long vert, long vert2)
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
        queue.pop_front();

        if (vert == vert2) return true;

        for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
        {
            if (!visited[itr->targetOperator])
            {
                if (!itr->isBefore)
                {
                    visited[itr->targetOperator] = true;
                    queue.push_back(itr->targetOperator);
                }
            }
        }
    }

    return false;
}

void BFS(Plan plan, long vert)
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

void promote(Plan& plan, Threat threat)
{
    cout << "~ Promotion: " << threat.operatorsThreat << " After: " << threat.vulnerableLink.targetOperator << endl;
    plan.addTemporalLink(threat.operatorsThreat, threat.vulnerableLink.targetOperator, false);
}

void demote(Plan& plan, const Threat& threat)
{
    cout << "~ Demotion: " << threat.operatorsThreat << " Before: " << threat.vulnerableOperator << endl;
    plan.addTemporalLink(threat.operatorsThreat, threat.vulnerableOperator, true);
}

bool isConsistent(Plan& plan, const Threat& threat)
{
    vector<TemporalLink> temporalLinks = plan.ordering[threat.operatorsThreat];

    for (auto link = temporalLinks.begin(); link != temporalLinks.end(); ++link)
    {
        vector<TemporalLink> targetsTemporalLinks = plan.ordering[link->targetOperator];
        for (auto targetLink = targetsTemporalLinks.begin(); targetLink != targetsTemporalLinks.end(); ++targetLink)
        {
            if (targetLink->targetOperator == threat.operatorsThreat && link->isBefore == targetLink->isBefore)
            {
                return false;
            }
        }
    }

    return true;
}

void resolveThreats(queue<Plan>& partialPlans, Plan& plan)
{
    if (!plan.threats.empty())
    {
        Threat threat(plan.threats.back());
        plan.threats.pop_back();

        Plan* newPartialPlan = new Plan(plan);
        promote(*newPartialPlan, threat);

        if (isConsistent(*newPartialPlan, threat))
            partialPlans.push(*newPartialPlan);

        newPartialPlan = new Plan(plan);
        demote(*newPartialPlan, threat);

        if (isConsistent(*newPartialPlan, threat))
            partialPlans.push(*newPartialPlan);
    }
}

// Checks to see if there are any operators existing that could threaten this link
void findThreats(Plan& plan, long newOperator, CausalLink causalLink)
{
    vector<Operator> potentialThreats;

    for (auto stepItr = plan.steps.begin(); stepItr != plan.steps.end(); ++stepItr)
    {
        for (auto condItr = stepItr->second.subtractedEffects.begin(); condItr != stepItr->second.subtractedEffects.end(); ++condItr)
        {
            if (*condItr == causalLink.condition)
            {
                potentialThreats.push_back(stepItr->second);
            }
        }
    }

    for (auto potential = potentialThreats.begin(); potential != potentialThreats.end(); ++potential)
    {
        if (!BFSVertComesBeforeVert2(plan, potential->id, newOperator) && !BFSVertComesAfterVert2(plan, potential->id, causalLink.targetOperator)) // is within the danger interval
        {
            Threat* newThreat = new Threat(newOperator, causalLink, potential->id);
            cout << "- Threat Found -" << endl;
            cout << "New Operator: " << newOperator << endl;
            cout << "Causal Target Operator: " << causalLink.targetOperator << endl;
            cout << "Causal Condition: " << causalLink.condition << endl;
            cout << "Operator Threat: " << potential->id << endl;
            plan.threats.push_back(*newThreat);
        }
    }

    // now check to see if the operators subtracted effects threaten any other operators
}

// Checks if newOperator is a threat to any existing links (needed for SE) - not addition I guess? :P
void findThreats2(Plan& plan, Operator newOperator)
{
    vector<Threat> potentialThreats;

    for (auto subItr = newOperator.subtractedEffects.begin(); subItr != newOperator.subtractedEffects.end(); ++subItr)
    {
        for (long step = 0; step < plan.links.size(); ++step)
        {
            vector<CausalLink> stepsLinks = plan.links[0];
            for (auto linkItr = stepsLinks.begin(); linkItr != stepsLinks.end(); ++linkItr)
            {
                if (linkItr->condition == (*subItr))
                {
                    Threat* newThreat = new Threat(step, *linkItr, newOperator.id);
                    potentialThreats.push_back(*newThreat);
                }
            }
        }
    }

    for (auto potential = potentialThreats.begin(); potential != potentialThreats.end(); ++potential)
    {
        if (!BFSVertComesBeforeVert2(plan, newOperator.id, potential->vulnerableOperator) && !BFSVertComesAfterVert2(plan, newOperator.id, potential->vulnerableLink.targetOperator)) // is within the danger interval
        {
            /*cout << "- Threat Found -" << endl;
            cout << "New Operator: " << newOperator << endl;
            cout << "Causal Target Operator: " << causalLink.targetOperator << endl;
            cout << "Causal Condition: " << causalLink.condition << endl;
            cout << "Operator Threat: " << potential->id << endl;*/
            plan.threats.push_back(*potential);
        }
    }
}

bool causualLinkFromAToB(Plan plan, long stepA, long stepB)
{
    for (auto linkIt = plan.links[stepA].begin(); linkIt != plan.links[stepA].end(); ++linkIt)
    {
        if (linkIt->targetOperator == stepB)
        {
            return true;
        }
    }

    return false;
}

void DFSVisit(Plan plan, long vert)
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
            if (parent[itr->targetOperator] > -1)
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
vector<long> topologicalSort(Plan plan, long vert)
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

void chooseOperator(queue<Plan>& partialPlans, Plan& plan, vector<Operator> operators, Goal goal)
{
    vector<Operator> chosen;

    for (auto stepItr = plan.steps.begin(); stepItr != plan.steps.end(); ++stepItr)
    {
        for (auto condItr = stepItr->second.addedEffects.begin(); condItr != stepItr->second.addedEffects.end(); ++condItr)
        {
            if (*condItr == goal.condition /*&& (!causualLinkFromAToB(plan, goal.step, stepItr->first))*/)
            {
                chosen.push_back(stepItr->second);
            }
        }
    }


    for (auto chosenItr = chosen.begin(); chosenItr != chosen.end(); ++chosenItr)
    {
        // check to see if the chosen operator comes before the one in question
        if (!BFSVertComesAfterVert2(plan, chosenItr->id, goal.step))
        {
            // Simple establishment/use existing operator to satisfy goal
            Plan* partialPlan = new Plan(plan);
            CausalLink* causalLink = new CausalLink(goal.step, goal.condition);
            if (!(goal.step == partialPlan->end || goal.step == partialPlan->start || chosenItr->id == partialPlan->end || chosenItr->id == partialPlan->start)) // TODO: have a better way to ensure temporal links are properly added and there aren't ugly checks like this...
                partialPlan->addTemporalLink(goal.step, chosenItr->id, false);
            partialPlan->links[chosenItr->id].push_back(*causalLink);
            cout << "SE - New Causal Link From: " << partialPlan->steps.at(chosenItr->id).name << " To: " << partialPlan->steps.at(causalLink->targetOperator).name << " For Condition: " << causalLink->condition << endl;
            findThreats(*partialPlan, chosenItr->id, *causalLink); // TODO: make this nicer... eliminate the repetition
            findThreats2(*partialPlan, partialPlan->steps[goal.step]);
            partialPlans.push(*partialPlan);
        }
    }

    for (Operator option : operators)
    {
        for (auto itr = option.addedEffects.begin();itr != option.addedEffects.end(); ++itr)
        {
            if (*itr == goal.condition)
            {
                Plan* partialPlan = new Plan(plan);
                CausalLink* causalLink = new CausalLink(goal.step, goal.condition);
                partialPlan->operatorAddition(option, goal.step, goal.condition, goal.step, NONE);
                partialPlan->addTemporalLink(option.id, partialPlan->start, false);
                if (goal.step != partialPlan->end)
                    partialPlan->addTemporalLink(option.id, partialPlan->end, true);
                cout << "A - New Causal Link From: " << partialPlan->steps.at(option.id).name << " To: " << partialPlan->steps.at(causalLink->targetOperator).name << " For Condition: " << causalLink->condition << endl;
                findThreats(*partialPlan, option.id, *causalLink); // TODO: here's that repetition that I mentioned up there ^
                findThreats2(*partialPlan, partialPlan->steps[goal.step]); // TODO: do I need this here? I know I do for SE but A as well?
                partialPlans.push(*partialPlan);
            }
        }
    }
}

vector<Plan> pop(Operator initialState, Operator endGoal, vector<Operator> operators)
{
    Plan plan = makeInitialPlan(initialState, endGoal);

    vector<Plan> finishedPlans;
    queue<Plan> partialPlans;
    partialPlans.push(plan);

    /*
    ConA, // 0
    AonC, // 1
    BonA, // 2
    BonC, // 3
    AonB, // 4
    ConB, // 5
    AOnTable, // 6
    BOnTable, // 7
    COnTable, // 8
    clearB, // 9
    clearC, // 10
    clearA, // 11
    holdingA, // 12
    holdingB, // 13
    holdingC, // 14
    handEmpty // 15
    */

    while (!partialPlans.empty())
    {
        plan = partialPlans.front();
        partialPlans.pop();

        if (plan.threats.empty())
        {
            if (plan.open.empty() && plan.threats.empty()) // check if this plan is a solution
            {
                finishedPlans.push_back(plan);
                break;
            }

            Goal* goal = selectSubgoal(plan);

            if (goal != nullptr)
                chooseOperator(partialPlans, plan, operators, *goal);

        }
        else
        {
            resolveThreats(partialPlans, plan);
        }

    }

    return finishedPlans;
}

struct UnfinishedTotalOrderPlan
{
    bool* visited;
    vector<long> plan;
    list<long> nextSteps;

    /*UnfinishedTotalOrderPlan(bool* vertsVisited, vector<long> planSoFar, vector<long> newSteps)
    {
        visited = new bool[](vertsVisited);
        plan = vector<long>(planSoFar);
        nextSteps = vector<long>(newSteps);
    }

    UnfinishedTotalOrderPlan(int numVertices)
    {
        visited = new bool[numVertices];
    }*/

};

void addNewPlanForEachPermutation(queue<UnfinishedTotalOrderPlan>& unfinishedPlans, const UnfinishedTotalOrderPlan& currentPlan, vector<long> newOperators)
{
    sort (newOperators.begin(), newOperators.end());

    do
    {
        UnfinishedTotalOrderPlan newPlan(currentPlan);

        for (auto opItr = newOperators.begin(); opItr != newOperators.end(); ++opItr)
        {
            newPlan.nextSteps.push_back(*opItr);
        }

        unfinishedPlans.push(newPlan);
    }
    while (next_permutation(newOperators.begin(), newOperators.end()));
}

bool processNextSteps(queue<UnfinishedTotalOrderPlan>& unfinishedPlans, UnfinishedTotalOrderPlan& totalOrderPlan, Plan plan)
{
    long vert = totalOrderPlan.nextSteps.front();
    totalOrderPlan.plan.push_back(vert);
    totalOrderPlan.nextSteps.pop_front();

    vector<long> newlyAdded;

    for (auto itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
    {
        if (!totalOrderPlan.visited[itr->targetOperator])
        {
            if (itr->isBefore && itr->targetOperator != plan.end)
            {
                totalOrderPlan.visited[itr->targetOperator] = true;
                newlyAdded.push_back(itr->targetOperator);
            }
        }
    }

    if (newlyAdded.empty() && totalOrderPlan.nextSteps.empty())
        return true; // plan complete

    addNewPlanForEachPermutation(unfinishedPlans, totalOrderPlan, newlyAdded);

    return false;
}

vector<vector<long>> generateTotalOrderPlans(const Plan& plan)
{
    vector<vector<long>> finishedPlans;
    queue<UnfinishedTotalOrderPlan> unfinishedPlans;
    UnfinishedTotalOrderPlan initialPlan;

    long numVertices = plan.steps.size();
    initialPlan.visited = new bool[numVertices];
    for (unsigned int i = 0; i < numVertices; ++i)
    {
        initialPlan.visited[i] = false;
    }

    vector<long> nextSteps;

    // Get first steps (this is a bit of an awkward step)
    for (auto temporalLink = plan.ordering[plan.start].begin(); temporalLink != plan.ordering[plan.start].end(); ++temporalLink)
    {
        long target = temporalLink->targetOperator;
        bool next = true;

        for (auto targetTemporalLink = plan.ordering[target].begin(); targetTemporalLink != plan.ordering[target].end(); ++targetTemporalLink)
        {
            if (!targetTemporalLink->isBefore && targetTemporalLink->targetOperator != plan.start)
            {
                next = false;
            }
        }

        if (next)
        {
            initialPlan.visited[temporalLink->targetOperator] = true;
            nextSteps.push_back(temporalLink->targetOperator);
        }
    }

    addNewPlanForEachPermutation(unfinishedPlans, initialPlan, nextSteps);

    while (!unfinishedPlans.empty()) // process unfinished plans until they're all finished
    {
        UnfinishedTotalOrderPlan totalOrderPlan = unfinishedPlans.front();
        unfinishedPlans.pop();

        if (processNextSteps(unfinishedPlans, totalOrderPlan, plan))
        {
            finishedPlans.push_back(totalOrderPlan.plan);
        }
    }

    return finishedPlans;
}

vector<long> getTotalOrderPlan(Plan plan)
{
    long numVertices = plan.steps.size();
    bool* visited = new bool[numVertices];
    for (unsigned int i = 0; i < numVertices; ++i)
    {
        visited[i] = false;
    }

    vector<long> totalOrderPlan;
    list<long> queue;

    // Get first steps (this is a bit of an awkward step)
    for (auto temporalLink = plan.ordering[plan.start].begin(); temporalLink != plan.ordering[plan.start].end(); ++temporalLink)
    {
        long target = temporalLink->targetOperator;
        bool next = true;

        for (auto targetTemporalLink = plan.ordering[target].begin(); targetTemporalLink != plan.ordering[target].end(); ++targetTemporalLink)
        {
            if (!targetTemporalLink->isBefore && targetTemporalLink->targetOperator != plan.start)
            {
                next = false;
            }
        }

        if (next)
        {
            visited[temporalLink->targetOperator] = true;
            queue.push_back(temporalLink->targetOperator);
        }
    }

    // TODO: Spawn multiple plans depending on how many potencial starts there are.

    vector<TemporalLink>::iterator itr;

    while(!queue.empty())
    {
        long vert = queue.front();
        totalOrderPlan.push_back(vert);
        queue.pop_front();

        vector<long> newlyAdded;

        for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
        {
            if (!visited[itr->targetOperator])
            {
                if (itr->isBefore && itr->targetOperator != plan.end)
                {
                    visited[itr->targetOperator] = true;
                    newlyAdded.push_back(itr->targetOperator);
                    queue.push_back(itr->targetOperator);
                }
            }
        }

        //TODO: check how many are possible next steps, from those generate all the permutations and spawn that many we plans.
    }

    return totalOrderPlan;
}

int main()
{
    // TODO: check how long the algorithm takes
    // TODO: store this "setting the table" example somewhere
    Operator start("start");
    Operator finish("finish");

    /*start.addedEffects = {tableCleared};
    finish.preconditions = {onTableCloth, outGlasses, outPlates, outSilverware};

    Operator layTableCloth("layTableCloth");
    layTableCloth.preconditions = {tableCleared};
    layTableCloth.addedEffects = {onTableCloth};
    layTableCloth.subtractedEffects = {tableCleared};
    Operator putOutGlasses("putOutGlasses");
    putOutGlasses.addedEffects = {outGlasses};
    putOutGlasses.subtractedEffects = {tableCleared};
    Operator putOutPlates("putOutPlates");
    putOutPlates.addedEffects = {outPlates};
    putOutPlates.subtractedEffects = {tableCleared};
    Operator putOutSilverware("putOutSilverware");
    putOutSilverware.addedEffects = {outSilverware};
    putOutSilverware.subtractedEffects = {tableCleared};

    vector<Operator> operators = {layTableCloth, putOutGlasses, putOutPlates, putOutSilverware};

    vector<Plan> plans = pop(start, finish, operators);*/

    // TODO: get the planner to solve the sussman problem (Temporal links between Stack (A,B) and Stack (B,C) missing?)

    //start.addedEffects = {ConA, handEmpty, AOnTable, BOnTable, clearB, clearC};
    //finish.preconditions = {AonB, BonC};
    start.addedEffects = {ConA, handEmpty, AOnTable, BOnTable, clearB, clearC};
    finish.preconditions = {AonB, BonC};

    Operator unstackCA("Unstack(C,A)");
    unstackCA.preconditions = {clearC, ConA, handEmpty};
    unstackCA.addedEffects = {holdingC, clearA};
    unstackCA.subtractedEffects = {clearC, ConA, handEmpty};

    Operator pickupA("Pickup(A)");
    pickupA.preconditions = {AOnTable, clearA, handEmpty};
    pickupA.addedEffects = {holdingA};
    pickupA.subtractedEffects = {clearA, AOnTable, handEmpty};

    Operator pickupB("Pickup(B)");
    pickupB.preconditions = {BOnTable, clearB, handEmpty};
    pickupB.addedEffects = {holdingB};
    pickupB.subtractedEffects = {BOnTable, clearB, handEmpty};

    Operator pickupC("Pickup(C)");
    pickupC.preconditions = {COnTable, clearC, handEmpty};
    pickupC.addedEffects = {holdingC};
    pickupC.subtractedEffects = {COnTable, clearC, handEmpty};

    Operator stackAB("Stack(A,B)");
    stackAB.preconditions = {holdingA, clearB};
    stackAB.addedEffects = {AonB, clearA, handEmpty};
    stackAB.subtractedEffects = {holdingA, clearB};

    Operator stackBC("Stack(B,C)");
    stackBC.preconditions = {holdingB, clearC};
    stackBC.addedEffects = {BonC, clearB, handEmpty};
    stackBC.subtractedEffects = {holdingB, clearC};

    Operator putdownC("Putdown(C)");
    putdownC.preconditions = {holdingC};
    putdownC.addedEffects = {COnTable, clearC, handEmpty};
    putdownC.subtractedEffects = {holdingC};


    vector<Operator> operators = {unstackCA, pickupA, pickupB, stackAB, stackBC, putdownC};
    //vector<Plan> plans = pop(start, finish, operators);

    // TODO: Briefcase domain
    /*start.addedEffects = {briefcaseAtHome, dictionaryAtHome, paycheckInBriefcase};
    finish.preconditions = {dictionaryAtOffice, paycheckAtHome};

    // TODO: Building new operators should be made much quicker/easier than this
    Operator homeToOffice("HomeToOffice");
    homeToOffice.preconditions = {briefcaseAtHome};
    homeToOffice.addedEffects = {briefcaseAtOffice};
    homeToOffice.subtractedEffects = {briefcaseAtHome};

    Operator officeToHome("OfficeToHome");
    officeToHome.preconditions = {briefcaseAtOffice};
    officeToHome.addedEffects = {briefcaseAtHome};
    officeToHome.subtractedEffects = {briefcaseAtOffice};

    Operator takeOutPaycheckAtHome("takeOutPaycheckAtHome");
    takeOutPaycheckAtHome.preconditions = {paycheckInBriefcase, briefcaseAtHome};
    takeOutPaycheckAtHome.addedEffects = {paycheckAtHome};
    takeOutPaycheckAtHome.subtractedEffects = {paycheckInBriefcase};

    Operator takeOutPaycheckAtOffice("takeOutPaycheckAtOffice");
    takeOutPaycheckAtOffice.preconditions = {paycheckInBriefcase, briefcaseAtOffice};
    takeOutPaycheckAtOffice.addedEffects = {paycheckAtOffice};
    takeOutPaycheckAtOffice.subtractedEffects = {paycheckInBriefcase};

    Operator putPaycheckInAtHome("putPaycheckInAtHome");
    putPaycheckInAtHome.preconditions = {paycheckAtHome, briefcaseAtHome};
    putPaycheckInAtHome.addedEffects = {paycheckInBriefcase};
    putPaycheckInAtHome.subtractedEffects = {paycheckAtHome};

    Operator putPaycheckInAtOffice("putPaycheckInAtOffice");
    putPaycheckInAtOffice.preconditions = {paycheckAtOffice, briefcaseAtOffice};
    putPaycheckInAtOffice.addedEffects = {paycheckInBriefcase};
    putPaycheckInAtOffice.subtractedEffects = {paycheckAtOffice};

    Operator takeOutDictionaryAtHome("takeOutDictionaryAtHome");
    takeOutDictionaryAtHome.preconditions = {dictionaryInBriefcase, briefcaseAtHome};
    takeOutDictionaryAtHome.addedEffects = {dictionaryAtHome};
    takeOutDictionaryAtHome.subtractedEffects = {dictionaryInBriefcase};

    Operator takeOutDictionaryAtOffice("takeOutDictionaryAtOffice");
    takeOutDictionaryAtOffice.preconditions = {dictionaryInBriefcase, briefcaseAtOffice};
    takeOutDictionaryAtOffice.addedEffects = {dictionaryAtOffice};
    takeOutDictionaryAtOffice.subtractedEffects = {dictionaryInBriefcase};

    Operator putDictionaryInAtHome("putDictionaryInAtHome");
    putDictionaryInAtHome.preconditions = {dictionaryAtHome, briefcaseAtHome};
    putDictionaryInAtHome.addedEffects = {dictionaryInBriefcase};
    putDictionaryInAtHome.subtractedEffects = {dictionaryAtHome};

    Operator putDictionaryInAtOffice("putDictionaryInAtOffice");
    putDictionaryInAtOffice.preconditions = {dictionaryAtOffice, briefcaseAtOffice};
    putDictionaryInAtOffice.addedEffects = {dictionaryInBriefcase};
    putDictionaryInAtOffice.subtractedEffects = {dictionaryAtOffice};

    vector<Operator> operators = {officeToHome,
                                  homeToOffice,
                                  takeOutPaycheckAtHome,
                                  takeOutPaycheckAtOffice,
                                  putPaycheckInAtHome,
                                  putPaycheckInAtOffice,
                                  takeOutDictionaryAtHome,
                                  takeOutDictionaryAtOffice,
                                  putDictionaryInAtHome,
                                  putDictionaryInAtOffice};*/


    // TODO: Fruit Bowl Domain 1

    /*start.addedEffects = {holdingBanana, orangeOnTable, orangeNotEaten, bananaNotEaten, hungry};
    finish.preconditions = {orangeInBowl, notHungry};

    Operator pickupOrangeFromTable("PickupOrangeFromTable");
    pickupOrangeFromTable.preconditions = {handsEmpty, orangeNotEaten, orangeOnTable};
    pickupOrangeFromTable.addedEffects = {holdingOrange};
    pickupOrangeFromTable.subtractedEffects = {handsEmpty, orangeOnTable};

    Operator pickupBananaFromTable("PickupBananaFromTable");
    pickupBananaFromTable.preconditions = {handsEmpty, bananaNotEaten, bananaOnTable};
    pickupBananaFromTable.addedEffects = {holdingBanana};
    pickupBananaFromTable.subtractedEffects = {handsEmpty, bananaOnTable};

    Operator pickupPearFromTable("PickupPearFromTable");
    pickupPearFromTable.preconditions = {handsEmpty, pearNotEaten, pearOnTable};
    pickupPearFromTable.addedEffects = {holdingPear};
    pickupPearFromTable.subtractedEffects = {handsEmpty, pearOnTable};

    Operator putOrangeInBowl("PutOrangeInBowl");
    putOrangeInBowl.preconditions = {holdingOrange, orangeNotEaten};
    putOrangeInBowl.addedEffects = {orangeInBowl, handsEmpty};
    putOrangeInBowl.subtractedEffects = {holdingOrange};

    Operator putBananaInBowl("PutBananaInBowl");
    putBananaInBowl.preconditions = {holdingBanana, bananaNotEaten};
    putBananaInBowl.addedEffects = {bananaInBowl, handsEmpty};
    putBananaInBowl.subtractedEffects = {holdingBanana};

    Operator eatBanana("EatBanana");
    eatBanana.preconditions = {hungry, holdingBanana, bananaNotEaten};
    eatBanana.addedEffects = {bananaEaten, notHungry, handsEmpty};
    eatBanana.subtractedEffects = {bananaNotEaten, hungry};


    vector <Operator> operators = {pickupOrangeFromTable, putOrangeInBowl, putBananaInBowl, eatBanana};*/

    // TODO: Fruit Bowl Domain 2

    /*start.addedEffects = {holdingPear, orangeOnTable, pearOnTable, pearNotEaten, orangeNotEaten, bananaNotEaten, hungry};
    finish.preconditions = {pearInBowl, notHungry};

    Operator pickupOrangeFromTable("PickupOrangeFromTable");
    pickupOrangeFromTable.preconditions = {handsEmpty, orangeNotEaten, orangeOnTable};
    pickupOrangeFromTable.addedEffects = {holdingOrange};
    pickupOrangeFromTable.subtractedEffects = {handsEmpty, orangeOnTable};

    Operator pickupBananaFromTable("PickupBananaFromTable");
    pickupBananaFromTable.preconditions = {handsEmpty, bananaNotEaten, bananaOnTable};
    pickupBananaFromTable.addedEffects = {holdingBanana};
    pickupBananaFromTable.subtractedEffects = {handsEmpty, bananaOnTable};

    Operator pickupPearFromTable("PickupPearFromTable");
    pickupPearFromTable.preconditions = {handsEmpty, pearNotEaten, pearOnTable};
    pickupPearFromTable.addedEffects = {holdingPear};
    pickupPearFromTable.subtractedEffects = {handsEmpty, pearOnTable};

    Operator putPearInBowl("PutPearInBowl");
    putPearInBowl.preconditions = {holdingPear, pearNotEaten};
    putPearInBowl.addedEffects = {pearInBowl, handsEmpty};
    putPearInBowl.subtractedEffects = {holdingPear};

    Operator putOrangeInBowl("PutOrangeInBowl");
    putOrangeInBowl.preconditions = {holdingOrange, orangeNotEaten};
    putOrangeInBowl.addedEffects = {orangeInBowl, handsEmpty};
    putOrangeInBowl.subtractedEffects = {holdingOrange};

    Operator putBananaInBowl("PutBananaInBowl");
    putBananaInBowl.preconditions = {holdingBanana, bananaNotEaten};
    putBananaInBowl.addedEffects = {bananaInBowl, handsEmpty};
    putBananaInBowl.subtractedEffects = {holdingBanana};

    Operator eatBanana("EatBanana");
    eatBanana.preconditions = {hungry, holdingBanana, bananaNotEaten};
    eatBanana.addedEffects = {bananaEaten, notHungry, handsEmpty};
    eatBanana.subtractedEffects = {bananaNotEaten, hungry, holdingBanana};

    Operator eatOrange("EatOrange");
    eatOrange.preconditions = {hungry, holdingOrange, orangeNotEaten};
    eatOrange.addedEffects = {notHungry, orangeEaten, handsEmpty};
    eatOrange.subtractedEffects = {orangeNotEaten, hungry, holdingOrange};

    Operator eatPear("EatPear");
    eatPear.preconditions = {hungry, holdingPear, pearNotEaten};
    eatPear.addedEffects = {notHungry, pearEaten, handsEmpty};
    eatPear.subtractedEffects = {pearNotEaten, holdingPear, hungry};

    vector <Operator> operators = {putPearInBowl, eatPear};*/

    vector<Plan> plans = pop(start, finish, operators);

    cout << endl;
    cout << "Number of potencial plans: " << plans.size() << endl;
    cout << endl;


    // Display plan's Causal links
    if (plans.size() > 0)
    {
        Plan plan = plans[0];

        cout << endl;
        for (auto l = plan.steps.begin(); l != plan.steps.end(); ++l)
        {
            cout << l->second.name << endl;
        }
        cout << endl;

        int id = 0;
        for (auto itr = plan.links.begin(); itr != plan.links.end(); ++itr)
        {
            vector<CausalLink> links = *itr;

            for (auto j = links.begin(); j != links.end(); ++j)
            {
                cout << "Final: Causal Link From: " << plan.steps.at(id).name << " To: " << plan.steps.at(j->targetOperator).name << " For Condition: " << j->condition << endl;
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

        /*vector<vector<long>> totalOrderPlans = generateTotalOrderPlans(plans[0]);
        cout << endl;
        cout << "Total Order Plan (One of them)" << endl;
        cout << endl;

        for (auto totalOrderPlan = totalOrderPlans.begin(); totalOrderPlan != totalOrderPlans.end(); ++totalOrderPlan)
        {
            cout << endl;
            cout << "++ Possible Plan ++" << endl;
            cout << endl;

            for (auto step = totalOrderPlan->begin(); step != totalOrderPlan->end(); ++step)
            {
                unordered_map<long, Operator>::const_iterator op = plans[0].steps.find(*step);
                if (op != plans[0].steps.end())
                    cout << op->second.name << endl;
                else
                    cout << "Oops: something is wrong" << endl;
            }

            cout << "=================" << endl;
        }*/
    }

    return 0;
}


