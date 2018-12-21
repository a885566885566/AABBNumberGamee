#include<stdio.h>
#include<string.h>
#ifndef GUESSER
#define GUESSER
#define DEBUGGER
#define ANS_LEN 78763
#define TYPE_CHAR 10
typedef struct _NODE{
    int visited;
    int critical;
    int leftIndex;
    int rightIndex;
    short avail[TYPE_CHAR];
    //short rightAvail[TYPE_CHAR];
}NODE;
char ans[ANS_LEN];
NODE nodes[131072+(ANS_LEN-65536)]; 
// Number of nodes require to restore all ans, which means 
// there is ANS_LEN leaves in tree
#pragma region TreeOperation
inline int getParentIndex(int index){
    return (index-1)/2;
}
inline int getLeftChildIndex(int index){
    return 2*index+1;
}
inline int getRightChildIndex(int index){
    return 2*index+2;
}
inline int isLeftNode(int index){
    return index % 2;
}
inline void setAvail(int index, int number, int value){
    nodes[index].avail[number] = value;
    if(value == 0)  nodes[index].critical--;
}

/*
Tree Node Structure
                   left, right
         *      -> 0   , 2      A node contain 3 leaves
        / \                     Make single node appear at left leaf
           *    -> 1   , 2      A node contain 2 leaves, which can be determined in one check
           /\
               
*/
void treeConstruct(int index, int left, int right){
    if(right - left >= 1){  // Node init
        nodes[index].leftIndex = left;
        nodes[index].rightIndex = right;
        nodes[index].visited = 0;
        nodes[index].critical = 9;
        for(int i=0;i<TYPE_CHAR;i++)
            nodes[index].avail[i] = 0;

        int middle = (right+left)/2;
        treeConstruct(getRightChildIndex(index), middle, right);
        treeConstruct(getLeftChildIndex(index), left, middle-1);
        // Left side will be limited forcely, because the 
        // DFS method we adopted a right side first policy
    }
    else{
        ans[left] = 0;
        ans[right] = 0;
    }
}
void setVisitedRecursive(int index, int left, int right){
    if(right - left >= 1){
        nodes[index].visited = 1;
        int middle = (right+left)/2;
        setVisitedRecursive(getRightChildIndex(index), middle, right);
        setVisitedRecursive(getLeftChildIndex(index), left, middle-1);
    }
}
inline int distance(int index){
    return nodes[index].rightIndex - nodes[index].leftIndex;
/*  DFS walk tree  */
void getDFSnext(){
    int offset = distance(getRightChildIndex(DFSwalkIndex));
    if(offset > 1){      // Right side not reach end
        stack[top++] = getLeftChildIndex(DFSwalkIndex); // Push left into task
        DFSwalkIndex = getRightChildIndex(DFSwalkIndex);
    }
    else{
        DFSwalkIndex = stack[--top];
        if(top == 0)    printf("End of Search\n");
    }
}
#pragma endregion
#pragma region Manager
inline void taskManager(){
    // Maintain task counter
    resolverCounter = assignerCounter;
    assignerCounter++;
    if(assignerCounter>=9){
        resolverIndexChangeFlag = 1;
        if(distance(assignerIndex) == 1)
            specialWorker = 1;
        assignerCounter = 0;
        resolverIndex = assignerIndex;
        assignerIndex = getDFSnext();
        while(nodes[assignerIndex].visited)
            assignerIndex = getDFSnext();
    }
}
#pragma endregion
inline void checkAnswerCanBeDetermine(int index){
    if(nodes[index].critical == 1){
        int answer = getRestAvailNum(index);
        setArr(nodes[index].leftIndex, 
                nodes[index].rightIndex,
                answer);
        setVisitedRecursive(index, nodes[index].leftIndex, 
                nodes[index].rightIndex);
    }
}
void setArr(int left, int right, char value){
    for(int i = left; i<=right; i++)
        ans[i] = value;
}
void setArr(int index, char value){
    ans[index] = value;
}
inline void getRestAvailNum(int* start=0, int index){
    int answer = *start;
    while(nodes[resolverIndex].avail[answer]==0) answer++;
    *start = answer;
    return answer;
}
inline void nodeLeafExchange(int index){
    int tmp = ans[nodes[index].rightIndex];
    ans[nodes[index].rightIndex] = ans[nodes[index].leftIndex];
    ans[nodes[index].leftIndex] = tmp;
}
void init(){
    for(int i = 0; i<ANS_LEN; i++){
        ans[i] = 0;
        stack[i] = -1;
    }
    treeConstruct(0, 0, ANS_LEN-1);
}
void clueDecoder(int *A, int *B, char* clue){
    *A = *B = 0;
    int i=0, sum=0;
    for(; i<12; i++){
        if(clue[i] == 'A') break;
        sum = sum*10 + clue[i]-'0';
    }
    *A = sum;
    sum = 0;
    i++;
    for(; i<12; i++){
        if(clue[i] == 'B') break;
        sum = sum*10 + clue[i]-'0';
    }
    *B = sum;
}

/* Global Variables setup */ 
int stage = 0;
int dataNeedConfirm = 0;
int lastLeafA = 0;
int resolverIndexChangeFlag = 0;
int specialWorker = 0;
int assignerCounter = 0, resolverCounter;
int assignerIndex = 0, resolverIndex = 0;
int stat[TYPE_CHAR];
int stack[ANS_LEN];
int top = 0;
int DFSwalkIndex = 0;
int A, B;
int stepCounter = 0;
int clueA[TYPE_CHAR];
char *guess(char *clue){
    stepCounter++;
    /* In first call, the clue is useless, and need to initialize variables */
    if(stage == 0){ 
        init();
        stage = 1;
    }
    #pragma region Task Resolver
    /* Task Resolver */
    /* Except for first call, analyze the clue */
    else{
        clueDecoder(&A, &B, clue);
        printf("step= %5d, %d A %d B\n",stepCounter, A, B);
        if(resolverIndex == 0) {    // Head node
            nodes[resolverIndex].avail[resolverCounter] = A;
        }
        else if(specialWorker){
            if(A < lastLeafA)
                nodeLeafExchange(resolverIndex);
            specialWorker = 0;
        }
        else{
            if(resolverIndexChangeFlag == 1){
                /* Calculate Rr0 */
                int sumA = 0;
                for(int i=0;i<TYPE_CHAR;i++)
                    sumA += clueA[i];
                int parentIndex = getParentIndex(resolverIndex);
                int R0 = nodes[parentIndex].avail[0];
                int N = 1 + nodes[resolverIndex].rightIndex - nodes[resolverIndex].leftIndex;
                //nodes[resolverIndex].avail[0] = 1 - (R0 - N + sumA)/9;
                setAvail(resolverIndex, 0, 1 - (R0 - N + sumA)/9);
                
                /* Calculate Right side avail */
                for(int i=1; i<TYPE_CHAR; i++){
                    if(nodes[resolverIndex].avail[i] != 0)
                        setAvail(resolverIndex, i, nodes[resolverIndex].avail[0] + clueA[i] - R0);
                }
                nodes[resolverIndex].visited = 1;
                checkAnswerCanBeDetermine(resolverIndex);

                /* Calculate Left side avail */
                /* Leaf */
                if(distance(resolverIndex) == 1){
                    lastLeafA = A;
                    int answer = 0;
                    while(nodes[parentIndex].avail[answer] - nodes[resolverIndex].avail[answer] != 1) answer++;
                    setArr(nodes[parentIndex].leftIndex, answer);
                }
                /* Segment */
                else{
                    int leftSilbingIndex = getLeftChildIndex(parentIndex);
                    for(int i=0; i<TYPE_CHAR; i++){
                        setAvail(leftSilbingIndex, 
                        , nodes[parentIndex].avail[i] - nodes[resolverIndex].avail[i]);
                    }
                    /* Mark as visited to ignore it in future visiting by DFS */
                    nodes[leftSilbingIndex].visited = 1;
                    
                    /* Deal with known result */
                    /* Deal with critical condition which answer can be determined */
                    checkAnswerCanBeDetermine(leftSilbingIndex);
                }
                /* Deal with the situation that only have two space */
                resolverIndexChangeFlag = 0;
            }
            /* In regular, restore the clue */
            else clue[resolverCounter] = A;// Process clue from 0~9
        }
    }
    #pragma endregion Task Resolver

    #pragma region Task Assigner
    /* Task Assigner */
    if(specialWorker){
        int counter = 0;
        ans[ nodes[assignerIndex].leftIndex ] = getRestAvailNum(&counter, assignerIndex);
        ans[ nodes[assignerIndex].rightIndex ] = getRestAvailNum(&counter, assignerIndex);
    }
    else{
        if(resolverCounter - assignerCounter > 0){  
            // A new cycle, clear old changes
            int leftSiblingIndex = getLeftChildIndex(getParentIndex(assignerIndex));
            setArr(nodes[leftSiblingIndex].leftIndex, nodes[leftSiblingIndex].rightIndex, 0);
        }
        while(nodes[getParentIndex(assignerIndex)].avail[assignerCounter] == 0){
            setAvail(assignerIndex, assignerCounter, 0);
            taskManager();
        }
        setArr(nodes[assignerIndex].leftIndex, nodes[assignerIndex].rightIndex, taskCounter);
    }
        
    // TODO: zero problem
    #pragma endregion Task Assigner

    taskManager();
    return ans;
}
#endif