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
    vector<Goal> open;
    long start;
    long end;

    // PONDER: Is it better to leave plan as plain data?
    void addGoals(Operator newOperator)
    {
        vector<int>::iterator itr = newOperator.preconditions.begin();

        for (;itr != newOperator.preconditions.end();++itr)
        {
            open.push_back(Goal(newOperator.id, *itr));
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

    Goal* goal = new Goal(plan.open.back());
    plan.open.pop_back();

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
            cout << "New Operator: " << newOperator << endl;
            cout << "Causal Target Operator: " << causalLink.targetOperator << endl;
            cout << "Causal Condition: " << causalLink.condition << endl;
            cout << "Operator Threat: " << potential->id << endl;
            plan.threats.push_back(*newThreat);
        }
    }
}

void chooseOperator(queue<Plan>& partialPlans, Plan& plan, vector<Operator> operators, Goal goal)
{
    vector<Operator> chosen;

    for (auto stepItr = plan.steps.begin(); stepItr != plan.steps.end(); ++stepItr)
    {
        for (auto condItr = stepItr->second.addedEffects.begin(); condItr != stepItr->second.addedEffects.end(); ++condItr)
        {
            if (*condItr == goal.condition)
            {
                chosen.push_back(stepItr->second);
            }
        }
    }

    if (chosen.size() > 0)
    {
        for (auto chosenItr = chosen.begin(); chosenItr != chosen.end(); ++chosenItr)
        {
            // check to see if the chosen operator comes before the one in question
            if (BFSVertComesBeforeVert2(plan, chosenItr->id, goal.step))
            {
                // Simple establishment/use existing operator to satisfy goal
                Plan* partialPlan = new Plan(plan);
                CausalLink* causalLink = new CausalLink(goal.step, goal.condition);
                partialPlan->links[chosenItr->id].push_back(*causalLink);
                cout << "SE - New Causal Link From: " << chosenItr->id << " To: " << causalLink->targetOperator << " For Condition: " << causalLink->condition << endl;
                findThreats(*partialPlan, chosenItr->id, *causalLink); // TODO: make this nicer... eliminate the repetition

                partialPlans.push(*partialPlan);
            }
        }
    }
    else
    {
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
                    cout << "A - New Causal Link From: " << option.id << " To: " << causalLink->targetOperator << " For Condition: " << causalLink->condition << endl;
                    findThreats(*partialPlan, option.id, *causalLink); // TODO: here's that repetition that I mentioned up there ^

                    partialPlans.push(*partialPlan);
                }
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

    while (!partialPlans.empty())
    {
        plan = partialPlans.front();
        partialPlans.pop();

        if (plan.open.empty() && plan.threats.empty()) // check if this plan is a solution
        {
            finishedPlans.push_back(plan);
        }

        Goal* goal = selectSubgoal(plan);

        if (goal != nullptr)
            chooseOperator(partialPlans, plan, operators, *goal);

        resolveThreats(partialPlans, plan);
    }

    return finishedPlans;
}

vector<vector<long>> generateTotalOrderPlans(Plan& partialOrderPlan)
{
    vector<vector<long>> finishedPlans;
    queue<vector<long>> unfinishedPlans;

    unfinishedPlans.push({partialOrderPlan.start});

    while (!unfinishedPlans.empty())
    {
        vector<long> plan = unfinishedPlans.front();
        unfinishedPlans.pop();

        long step = plan.back();
        cout << "Current Step: " << step << endl;

        // Step 1, find next temporal steps
        for (auto temporalLink = partialOrderPlan.ordering[step].begin(); temporalLink != partialOrderPlan.ordering[step].end(); ++temporalLink)
        {

        }

    }
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
        cout << vert << ": ";
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
        /*sort (newlyAdded.begin(), newlyAdded.end());

        do
        {

        }
        while (next_permutation(newlyAdded.begin(), newlyAdded.end()));

        cout << "\n";*/
    }

    return totalOrderPlan;
}

int main()
{
    // TODO: check how long the algorithm takes
    Operator start("start");
    Operator finish("finish");

    start.addedEffects = {tableCleared};
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
                cout << "Final: Causal Link From: " << id << " To: " << j->targetOperator << " For Condition: " << j->condition << endl;
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
                cout << "Temporal Link For " << id << " To: " << t->targetOperator << " Is Before? " << t->isBefore << endl;
            }
            ++id;
        }

        cout << "Building totoal order plan" << endl;
        vector<long> totalOrderPlan = getTotalOrderPlan(plans[0]);

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


