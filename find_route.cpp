#include <iostream>
#include <fstream>
using namespace std;

const int SIZE = 50;
const int MAX = 1000;

struct State {
    int city;
    int cost;
    int parent;
};

class RoadMap {
public:

    string cityNames[SIZE];
    int roads[SIZE][SIZE];
    int cityCount;
    int goalCity;

    RoadMap(){
        cityCount=0;
        goalCity=-1;
        for(int i=0;i<SIZE;i++){
            for(int j=0;j<SIZE;j++){
                roads[i][j]=0;
            }
        }
    }

    int getCityIndex(string name){
        for(int i=0;i<cityCount;i++){
            if(cityNames[i]==name)
                return i;
        }
        return -1;
    }

    void addCity(string name){
        if(getCityIndex(name)==-1 && cityCount<SIZE){
            cityNames[cityCount]=name;
            cityCount++;
        }
    }

    // reads the roads from the input file until "END OF INPUT"
    bool readFile(string fileName){

        ifstream file(fileName.c_str());
        if(!file) return false;

        string fromCity[MAX], toCity[MAX];
        int distance[MAX];
        int roadCount=0;

        string word;
        while(file>>word){
            if(word=="END") break;

            fromCity[roadCount]=word;
            file>>toCity[roadCount]>>distance[roadCount];
            roadCount++;
        }

        for(int i=0;i<roadCount;i++){
            addCity(fromCity[i]);
            addCity(toCity[i]);
        }

        for(int i=0;i<cityCount-1;i++){
            for(int j=0;j<cityCount-1-i;j++){
                if(cityNames[j]>cityNames[j+1]){
                    string temp=cityNames[j];
                    cityNames[j]=cityNames[j+1];
                    cityNames[j+1]=temp;
                }
            }
        }

        for(int i=0;i<roadCount;i++){
            int a=getCityIndex(fromCity[i]);
            int b=getCityIndex(toCity[i]);
            roads[a][b]=distance[i];
            roads[b][a]=distance[i];
        }

        return true;
    }

    bool isGoal(State state){
        return state.city==goalCity;
    }

    State move(State state,int nextCity){
        State newState = state;

        newState.cost += roads[state.city][nextCity];
        newState.city = nextCity;

        return newState;
    }
};

class Algorithms {
public:

    int expandedCount=0;
    int generatedCount=0;
    int maxFrontierSize=0;
    int currentDepth=0;

    bool isSame(State a, State b){
        return a.city==b.city;
    }

    bool exists(State states[], int size, State state){
        for(int i=0;i<size;i++){
            if(isSame(states[i],state))
                return true;
        }
        return false;
    }

    void printPath(State states[], int index, RoadMap &roadMap){
        int parentIndex = states[index].parent;
        if(parentIndex==-1) return;

        printPath(states, parentIndex, roadMap);
        cout<<roadMap.cityNames[states[parentIndex].city]<<" to "
            <<roadMap.cityNames[states[index].city]<<", "
            <<states[index].cost-states[parentIndex].cost<<" km\n";
    }

    void printSolution(State states[], int index, RoadMap &roadMap){
        cout<<"distance: "<<states[index].cost<<" km\n";
        cout<<"route:\n";
        printPath(states,index,roadMap);
    }

    void printNoRoute(){
        cout<<"distance: infinity\n";
        cout<<"Route: none\n";
    }

    // Breadth-First Search: expands the shallowest node first (queue)
    void BFS(RoadMap &roadMap, State start){

        State queue[MAX];
        State visited[MAX];

        int front=0, rear=0, visitedCount=0;

        start.parent = -1;
        queue[rear++] = start;
        generatedCount++;

        cerr<<"Expansion order: ";

        while(front < rear){

            if(rear-front > maxFrontierSize) maxFrontierSize = rear-front;

            State current = queue[front++];

            if(exists(visited,visitedCount,current))
                continue;

            visited[visitedCount]=current;
            int index=visitedCount;
            visitedCount++;

            expandedCount++;
            cerr<<roadMap.cityNames[current.city]<<" ";

            if(roadMap.isGoal(current)){
                cerr<<"\n";
                printSolution(visited,index,roadMap);
                return;
            }

            for(int i=0;i<roadMap.cityCount;i++){

                if(roadMap.roads[current.city][i]>0){

                    State nextState = roadMap.move(current,i);

                    if(!exists(visited,visitedCount,nextState) &&
                       !exists(queue,rear,nextState)){

                        nextState.parent=index;
                        queue[rear++]=nextState;
                        generatedCount++;
                    }
                }
            }
        }

        cerr<<"\n";
        printNoRoute();
    }

    // Depth-First Search: goes as deep as possible first (recursion)
    bool DFS_util(RoadMap &roadMap, State current, State visited[], int &visitedCount){

        visited[visitedCount]=current;
        int index=visitedCount;
        visitedCount++;

        expandedCount++;
        cerr<<roadMap.cityNames[current.city]<<" ";

        currentDepth++;
        if(currentDepth > maxFrontierSize) maxFrontierSize = currentDepth;

        if(roadMap.isGoal(current)){
            cerr<<"\n";
            printSolution(visited,index,roadMap);
            return true;
        }

        for(int i=0;i<roadMap.cityCount;i++){

            if(roadMap.roads[current.city][i]>0){

                State nextState = roadMap.move(current,i);
                nextState.parent=index;

                if(!exists(visited,visitedCount,nextState)){
                    generatedCount++;

                    if(DFS_util(roadMap,nextState,visited,visitedCount))
                        return true;
                }
            }
        }

        currentDepth--;
        return false;
    }

    void DFS(RoadMap &roadMap, State start){
        State visited[MAX];
        int visitedCount=0;

        start.parent=-1;
        generatedCount++;

        cerr<<"Expansion order: ";
        if(!DFS_util(roadMap,start,visited,visitedCount)){
            cerr<<"\n";
            printNoRoute();
        }
    }

    // Depth-Limited Search: DFS that stops at a given depth limit
    bool DLS(RoadMap &roadMap, State current, State visited[], int &visitedCount, int limit){

        if(exists(visited,visitedCount,current)) return false;

        visited[visitedCount]=current;
        int index=visitedCount;
        visitedCount++;

        expandedCount++;
        cerr<<roadMap.cityNames[current.city]<<" ";

        if(visitedCount > maxFrontierSize) maxFrontierSize = visitedCount;

        if(roadMap.isGoal(current)){
            cerr<<"\n";
            printSolution(visited,index,roadMap);
            return true;
        }

        if(limit<=0){
            visitedCount--;
            return false;
        }

        for(int i=0;i<roadMap.cityCount;i++){

            if(roadMap.roads[current.city][i]>0){

                State nextState = roadMap.move(current,i);
                nextState.parent=index;
                generatedCount++;

                if(DLS(roadMap,nextState,visited,visitedCount,limit-1))
                    return true;
            }
        }

        visitedCount--;
        return false;
    }

    // Iterative Deepening Search (IDT): runs DLS with limit 0, 1, 2, ...
    void IDS(RoadMap &roadMap, State start){

        for(int depth=0; depth<roadMap.cityCount; depth++){

            State visited[MAX];
            int visitedCount=0;

            start.parent=-1;
            generatedCount++;

            cerr<<"Depth limit "<<depth<<": ";

            if(DLS(roadMap,start,visited,visitedCount,depth))
                return;

            cerr<<"\n";
        }

        printNoRoute();
    }

    // Uniform Cost Search: expands the node with the smallest cost g(n)
    void UCS(RoadMap &roadMap, State start){

        State frontier[MAX];
        State explored[MAX];

        int frontierCount=0, exploredCount=0;

        start.parent=-1;
        frontier[frontierCount++]=start;
        generatedCount++;

        cerr<<"Expansion order: ";

        while(frontierCount>0){

            if(frontierCount > maxFrontierSize) maxFrontierSize = frontierCount;

            int best=0;
            for(int i=1;i<frontierCount;i++){
                if(frontier[i].cost < frontier[best].cost ||
                  (frontier[i].cost == frontier[best].cost &&
                   frontier[i].city < frontier[best].city))
                    best=i;
            }

            State current=frontier[best];

            for(int i=best;i<frontierCount-1;i++){
                frontier[i]=frontier[i+1];
            }
            frontierCount--;

            if(exists(explored,exploredCount,current))
                continue;

            explored[exploredCount]=current;
            int index=exploredCount;
            exploredCount++;

            expandedCount++;
            cerr<<roadMap.cityNames[current.city]<<" ";

            if(roadMap.isGoal(current)){
                cerr<<"\n";
                printSolution(explored,index,roadMap);
                return;
            }

            for(int i=0;i<roadMap.cityCount;i++){

                if(roadMap.roads[current.city][i]>0){

                    State nextState = roadMap.move(current,i);

                    if(!exists(explored,exploredCount,nextState)){
                        nextState.parent=index;
                        frontier[frontierCount++]=nextState;
                        generatedCount++;
                    }
                }
            }
        }

        cerr<<"\n";
        printNoRoute();
    }
};

int main(int argc, char* argv[]){

    string algorithm, fileName, originCity, destinationCity;

    if(argc==5){
        algorithm=argv[1];
        fileName=argv[2];
        originCity=argv[3];
        destinationCity=argv[4];
    }
    else if(argc==4){
        algorithm="UCS";
        fileName=argv[1];
        originCity=argv[2];
        destinationCity=argv[3];
    }
    else{
        cout<<"Usage: find_route algorithm input_filename origin_city destination_city\n";
        cout<<"algorithm = BFS | DFS | IDT | UCS\n";
        return 1;
    }

    RoadMap roadMap;
    if(!roadMap.readFile(fileName)){
        cout<<"Cannot open file "<<fileName<<"\n";
        return 1;
    }

    Algorithms algorithms;

    int startCity = roadMap.getCityIndex(originCity);
    roadMap.goalCity = roadMap.getCityIndex(destinationCity);

    if(startCity==-1 || roadMap.goalCity==-1){
        algorithms.printNoRoute();
        return 0;
    }

    State start;
    start.city=startCity;
    start.cost=0;
    start.parent=-1;

    if(algorithm=="BFS")
        algorithms.BFS(roadMap,start);
    else if(algorithm=="DFS")
        algorithms.DFS(roadMap,start);
    else if(algorithm=="IDT" || algorithm=="IDS")
        algorithms.IDS(roadMap,start);
    else if(algorithm=="UCS")
        algorithms.UCS(roadMap,start);
    else{
        cout<<"Unknown algorithm. Use BFS, DFS, IDT or UCS\n";
        return 1;
    }

    cerr<<"Nodes expanded  : "<<algorithms.expandedCount<<"\n";
    cerr<<"Nodes generated : "<<algorithms.generatedCount<<"\n";
    cerr<<"Max nodes in memory : "<<algorithms.maxFrontierSize<<"\n";

    return 0;
}
