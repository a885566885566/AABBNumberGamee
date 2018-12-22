#include<stdio.h>
#include<string.h>
#ifndef GUESSER
#define GUESSER
#define DEBUGGER
#define ANS_LEN 10
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
// 131072+(ANS_LEN-65536)
NODE nodes[131100]; 

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
// Number of nodes require to restore all ans, which means 
// there is ANS_LEN leaves in tree


#pragma region TreeOperation
static inline int getParentIndex(int index){
    return (index-1)/2;
}
static inline int getLeftChildIndex(int index){
    return 2*index+1;
}
static inline int getRightChildIndex(int index){
    return 2*index+2;
}
static inline int isLeftNode(int index){
    return index % 2;
}
static inline int getLeftSiblingIndex(int index){
    return getLeftChildIndex(getParentIndex(index));
}
static inline void setAvail(int index, int number, int value){
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

        int middle = (right+left+1)/2;
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
static inline int distance(int index){
    return nodes[index].rightIndex - nodes[index].leftIndex;
}
/*  DFS walk tree  */
int getDFSnext(){
    int offset = distance(getRightChildIndex(DFSwalkIndex));
    if(offset >= 1 && nodes[DFSwalkIndex].visited == 0){      // Right side not reach end
        stack[top++] = getLeftChildIndex(DFSwalkIndex); // Push left into task
        DFSwalkIndex = getRightChildIndex(DFSwalkIndex);
    }
    else{
        DFSwalkIndex = stack[--top];
        if(top == 0)    printf("End of Search\n");
    }
    return DFSwalkIndex;
}
#pragma endregion
#pragma region Manager
static inline void taskManager(){
    // Maintain task counter
    resolverCounter = assignerCounter;
    assignerCounter++;
    resolverIndex = assignerIndex;
    if(assignerCounter>9){
        if(assignerIndex != 0)
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
void setArr(int left, int right, char value){
    for(int i = left; i<=right; i++)
        ans[i] = value + '0';
}
void setArrSingle(int index, char value){
    ans[index] = value + '0';
}
static inline int getRestAvailNum(int* start, int index){
    int answer = *start;
    while(nodes[resolverIndex].avail[answer]==0) answer++;
    *start = answer;
    return answer;
}
static inline int getRestAvailNumSingle(int index){
    int answer = 0;
    while(nodes[resolverIndex].avail[answer]==0) answer++;
    return answer;
}
static inline void checkAnswerCanBeDetermine(int index){
    if(nodes[index].critical == 1){
        int answer = getRestAvailNumSingle(index);
        setArr(nodes[index].leftIndex, 
                nodes[index].rightIndex,
                answer);
        nodes[getRightChildIndex(index)].visited = 1;
        nodes[getLeftChildIndex(index)].visited = 1;
        //setVisitedRecursive(index, nodes[index].leftIndex, 
        //        nodes[index].rightIndex);
    }
}
static inline void nodeLeafExchange(int index){
    int tmp = ans[nodes[index].rightIndex];
    ans[nodes[index].rightIndex] = ans[nodes[index].leftIndex];
    ans[nodes[index].leftIndex] = tmp;
}
void printTree(){
    int startIndex = 0;
    int miniStack[100];
    int miniTop = 0;
    while(miniTop>=0){
        int offset = distance(getRightChildIndex(startIndex));
        printf("Index= %d, (%d, %d)\n", startIndex, nodes[startIndex].leftIndex, nodes[startIndex].rightIndex);
        if(offset >= 1){      // Right side not reach end
            stack[miniTop++] = getLeftChildIndex(startIndex); // Push left into task
            startIndex = getRightChildIndex(startIndex);
        }
        else{
            startIndex = stack[--miniTop];
        }
    }
    printf("Tree End\n");
}
void printAvail(int index){
    printf("Avail: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9\nCount: ");
    for(int i=0; i<TYPE_CHAR; i++)
        printf("%d, ", nodes[index].avail[i]);
    printf("\n");
}
void init(){
    for(int i = 0; i<ANS_LEN; i++){
        ans[i] = 0;
        stack[i] = -1;
    }
    treeConstruct(0, 0, ANS_LEN-1);
    for(int i=0;i<TYPE_CHAR;i++)
        nodes[0].avail[i] = -1;
    #ifdef DEBUGGER
    printTree();
    printf("Inited\n");
    #endif
}
void clueDecoder(int *A, int *B, char* clue){
    //*A = *B = 0;
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

char *guess(char *clue){
    printf("Stage %d ", stage);
    stepCounter++;
    /* In first call, the clue is useless, and need to initialize variables */
    if(stage == 0){ 
        init();
    }
    #pragma region Task Resolver
    /* Task Resolver */
    /* Except for first call, analyze the clue */
    else{
        clueDecoder(&A, &B, clue);
        printf("step= %d, %d A %d B\n",stepCounter, A, B);
        if(resolverIndex == 0) {    // Head node
            nodes[resolverIndex].avail[resolverCounter] = A;
            printAvail(resolverIndex);
        }
        else if(specialWorker){
            if(A < lastLeafA)
                nodeLeafExchange(resolverIndex);
            specialWorker = 0;
        }
        else{
            if(resolverIndexChangeFlag == 1){
                printf("Resolver Report(%d, %d)\n", assignerIndex, resolverIndex);
                /* Calculate Rr0 */
                int sumA = 0;
                printf("Clue: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9\nClue: ");
                for(int i=0; i<TYPE_CHAR; i++)
                    printf("%d, ", clueA[i]);
                printf("\n");
                for(int i=0;i<TYPE_CHAR;i++){
                    sumA += clueA[i];
                    clueA[i] = 0;
                }
                int parentIndex = getParentIndex(resolverIndex);
                int R0 = nodes[parentIndex].avail[0];
                //int R0 = nodes[resolverIndex].avail[0];
                int N = 1 + nodes[resolverIndex].rightIndex - nodes[resolverIndex].leftIndex;
                //nodes[resolverIndex].avail[0] = 1 - (R0 - N + sumA)/9;
                setAvail(resolverIndex, 0, 1 - (R0 - N + sumA)/9);
                
                /* Calculate Right side avail */
                for(int i=1; i<TYPE_CHAR; i++){
                    if(nodes[resolverIndex].avail[i] != 0)
                        setAvail(resolverIndex, i, nodes[resolverIndex].avail[0] + clueA[i] - R0);
                }
                printf("Preview\n");
                printf("sumA= %d, R0= %d, N= %d\n", sumA, R0, N);
                printAvail(resolverIndex);
                nodes[resolverIndex].visited = 1;
                checkAnswerCanBeDetermine(resolverIndex);

                /* Calculate Left side avail */
                /* Leaf */
                if(distance(resolverIndex) == 1){
                    lastLeafA = A;
                    int answer = 0;
                    while(nodes[parentIndex].avail[answer] - nodes[resolverIndex].avail[answer] != 1) answer++;
                    setArrSingle(nodes[parentIndex].leftIndex, answer);
                }
                /* Segment */
                else{
                    int leftSilbingIndex = getLeftChildIndex(parentIndex);
                    for(int i=0; i<TYPE_CHAR; i++){
                        setAvail(leftSilbingIndex, i
                        , nodes[parentIndex].avail[i] - nodes[resolverIndex].avail[i]);
                    }
                    /* Mark as visited to ignore it in future visiting by DFS */
                    nodes[leftSilbingIndex].visited = 1;
                    
                    /* Deal with known result */
                    /* Deal with critical condition which answer can be determined */
                    checkAnswerCanBeDetermine(leftSilbingIndex);
                }
                /* Deal with the situation that only have two space */
                
                //printAvail(resolverIndex);
                resolverIndexChangeFlag = 0;
            }
            /* In regular, restore the clue */
            else clueA[resolverCounter] = A;// Process clue from 0~9
        }
    }
    #pragma endregion Task Resolver

    #pragma region Task Assigner
    /* Task Assigner */
    printf("Assigner(%d, %d): \n", assignerIndex, resolverIndex);
    if(specialWorker){
        int counter = 0;
        ans[ nodes[assignerIndex].leftIndex ] = getRestAvailNum(&counter, assignerIndex);
        ans[ nodes[assignerIndex].rightIndex ] = getRestAvailNum(&counter, assignerIndex);
    }
    else{
        if(resolverCounter - assignerCounter > 0){  
            // A new cycle, clear old changes
            printf("\nNew cycle begin, at %d\n", assignerIndex);
            printAvail(assignerIndex);
            int leftSiblingIndex = getLeftSiblingIndex(assignerIndex); 
            setArr(nodes[leftSiblingIndex].leftIndex, nodes[leftSiblingIndex].rightIndex, 0);
        }
        while(assignerCounter!=0 && nodes[getParentIndex(assignerIndex)].avail[assignerCounter] == 0 ){
            printf("Fast forward: %d->%d\n",assignerIndex, assignerCounter);
            setAvail(assignerIndex, assignerCounter, 0);
            taskManager();
        }
        setArr(nodes[assignerIndex].leftIndex, nodes[assignerIndex].rightIndex, assignerCounter);
    }
        
    // TODO: zero problem
    #pragma endregion Task Assigner
    printf("Answer = ");
    for(int i = 0; i<ANS_LEN; i++)
        printf("%c ", ans[i]);
    printf("\n");
    stage++;
    taskManager();
    return ans;
}
#endif