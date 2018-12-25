#include<stdio.h>
#include<string.h>
#ifndef GUESSER
#define GUESSER
#define DEBUGGER
#define ANS_LEN 20
#define TYPE_CHAR 10
#define true 1
#define false 0
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
int newAssignerIndexFlag = 0;
int specialWorker = 0;
int superWorker = 0;
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
void resetSilbing(int index);

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
            nodes[index].avail[i] = -1;

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
int stopFlag = false;
/*  DFS walk tree  */
int getDFSnext(){
    int offset = distance(getRightChildIndex(DFSwalkIndex));
    if(offset >= 1){// && nodes[DFSwalkIndex].visited == 0){      // Right side not reach end
        stack[top++] = getLeftChildIndex(DFSwalkIndex); // Push left into task
        DFSwalkIndex = getRightChildIndex(DFSwalkIndex);
    }
    else{
        do{
            DFSwalkIndex = stack[--top];
            resolverIndexChangeFlag = 1;
        }while(nodes[getParentIndex(DFSwalkIndex)].rightIndex - nodes[getParentIndex(DFSwalkIndex)].leftIndex == 2);
        printf("Pop %d\n", DFSwalkIndex);
        if(top < 0)    stopFlag = true;
        else {
            printf("DFS_NextNext\n");
            //getDFSnext();
        }
    }
    return DFSwalkIndex;
}
#pragma endregion
#pragma region Manager
static inline void taskManager(){
    // Maintain task counter
    resolverCounter = assignerCounter;
    assignerCounter++;
    if(assignerCounter>9){
        if(assignerIndex != 0)
            resolverIndexChangeFlag = 1;
        assignerCounter = 0;
        resolverIndex = assignerIndex;
        assignerIndex = getDFSnext();
        //while(nodes[assignerIndex].visited && distance(assignerIndex))
        //    assignerIndex = getDFSnext();
        //resetSilbing(assignerIndex);
        newAssignerIndexFlag = 1;
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
    while(nodes[index].avail[answer]==0) answer++;
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
    //printf("Avail: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9\nCount: ");
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
void resetSilbing(int index){
    if(!isLeftNode(index)){  // && newAssignerIndexFlag == 1
        // New cycle begin at right node 
        // A new cycle, clear old changes
        newAssignerIndexFlag = 0;
        printf("\nReset sibling, at %d(%d, %d)\n", index, nodes[index].leftIndex, nodes[index].rightIndex);
        /*if(distance(getParentIndex(index)) == 1){ // Leaf
            int leftAnsIndex = nodes[getParentIndex(index)].leftIndex;
            setArrSingle(leftAnsIndex, 0);
        }*/
        if(distance(index) == 1){ // Leaf
            printf("Single\n");
            int answer = 0;
            //while((nodes[getParentIndex(index)].avail[answer]
            //    - nodes[index].avail[answer] != 1)
            //    && answer < TYPE_CHAR) answer++;
            int leftAnsIndex = nodes[getParentIndex(index)].leftIndex;
            setArrSingle(leftAnsIndex, 0);
        }
        else{
            printf("Multi\n");
            int leftSiblingIndex = getLeftSiblingIndex(index); 
            setArr(nodes[leftSiblingIndex].leftIndex, nodes[leftSiblingIndex].rightIndex, 0);
        }
        printf("New Sibling = ");
        for(int i = 0; i<ANS_LEN; i++)
            printf("%c ", ans[i]);
        printf("\n");
    }
}
void guessRestNumber(int index){
    printf("Guess Avail:\n");
    printAvail(index);
    if(nodes[index].critical == 0){
        // Only one number
        int answer = 0;
        while(nodes[index].avail[answer] == 0) answer++;
        setArrSingle(nodes[index].leftIndex, answer);
        setArrSingle(nodes[index].rightIndex, answer);
    }
    else{
        int answer = 0;
        while(nodes[index].avail[answer] == 0) answer++;
        setArrSingle(nodes[index].leftIndex, answer);
        answer++;
        while(nodes[index].avail[answer] == 0) answer++;
        setArrSingle(nodes[index].rightIndex, answer);
    }
    #ifdef DEBUGGER
    for(int i = 0; i<ANS_LEN; i++)
        printf("%c ", ans[i]);
    printf("\n");
    #endif
}
int numOfZero[ANS_LEN];
char *guess(char *clue){
    //printf("Stage %d ", stage);
    stepCounter++;
    /* In first call, the clue is useless, and need to initialize variables */
    if(stage == 0){ 
        init();
        for(int i = 0; i<ANS_LEN; i++)
            numOfZero[i] = -1;
        numOfZero[ANS_LEN-1] = 0;
    }
    #pragma region Task Resolver
    /* Task Resolver */
    /* Except for first call, analyze the clue */
    else{
        clueDecoder(&A, &B, clue);
        printf(": %d-> %d\n", resolverCounter, A);
        //printf("step= %d, %d A %d B\n",stepCounter, A, B);
        if(resolverIndex == 0) {    // Head node
            nodes[resolverIndex].avail[resolverCounter] = A;
            //printAvail(resolverIndex);
        }
        else if(specialWorker){
            printf("LeafA = %d\n", A);
            if(A <= lastLeafA+1){
                nodeLeafExchange(resolverIndex);
                printf("Wrong Guess\n");
            }
            else
                printf("Right Guess\n");
            specialWorker = 0;
        }
        else if(superWorker){
            superWorker = 0;
            specialWorker = 1;
            lastLeafA = A;
            nodeLeafExchange(resolverIndex);
            resolverIndex = assignerIndex;
            assignerIndex = getDFSnext();
            return ans;
        }
        else{
            //clueA[resolverCounter] = A;
            nodes[resolverIndex].avail[resolverCounter] = A;
            //if(resolverCounter>=0 && resolverCounter<TYPE_CHAR)
            //    clueCounter++;
            if(resolverIndexChangeFlag == 1){
                printf("Resolver Report(%d, %d)\n", assignerIndex, resolverIndex);
                /* Calculate Rr0 */
                int clueCounter = 0;
                int sumA = 0;
                int Nr = 0;
                int Ncorrect = 0;
                printf("Item: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9\nClue: ");
                for(int i=0; i<ANS_LEN; i++)
                    printf("%d, ", nodes[resolverIndex].avail[i]);
                printf("\n");
                int numZeroLen = nodes[resolverIndex].rightIndex+1;
                if(numZeroLen<ANS_LEN-1){
                    for(;numZeroLen<ANS_LEN-1 && numOfZero[numZeroLen]==-1;numZeroLen++) ;
                    
                    printf("Num of zero=");
                    for(int i=0;i<ANS_LEN;i++)
                        printf("%d ", numOfZero[i]);
                    printf("\n");
                    printf("End = %d\n", numZeroLen);
                    if(ans[ANS_LEN-1] == '0')
                        numOfZero[ANS_LEN-1] = 1;
                    for(;numZeroLen>nodes[resolverIndex].rightIndex+1; numZeroLen--){
                        numOfZero[numZeroLen-1] = numOfZero[numZeroLen];
                        if(ans[numZeroLen-1] == '0'){
                            numOfZero[numZeroLen-1]++;
                            printf("detect at %d\n", numZeroLen-1);
                        }
                    }
                    printf("Num of zero=");
                    for(int i=0;i<ANS_LEN;i++)
                        printf("%d ", numOfZero[i]);
                    printf("\n");
                    Nr = numOfZero[nodes[resolverIndex].rightIndex+1];
                    Ncorrect = ANS_LEN - nodes[resolverIndex].rightIndex - 1;
                }
                for(int i=0;i<TYPE_CHAR;i++){
                    sumA += nodes[resolverIndex].avail[i];
                }
                for(int i=0;i<TYPE_CHAR;i++)
                    if(nodes[resolverIndex].avail[i] <= 0)
                        clueCounter++;
                printf("clue= %d\n", clueCounter);
                int parentIndex = getParentIndex(resolverIndex);
                //int R0 = nodes[parentIndex].avail[0];
                int R0 = nodes[0].avail[0] - Nr + Ncorrect;
                //int R0 = nodes[resolverIndex].avail[0];
                int N = 1 + nodes[resolverIndex].rightIndex - nodes[resolverIndex].leftIndex;
                //nodes[resolverIndex].avail[0] = 1 - (R0 - N + sumA)/9;
                //setAvail(resolverIndex, 0, ((9-clueCounter)*R0+N-sumA)/(10-clueCounter));
                setAvail(resolverIndex, 0, ((10-clueCounter)*R0+N-sumA)/(10-clueCounter));
                /* Calculate Right side avail */
                for(int i=1; i<TYPE_CHAR; i++){
                    if(nodes[parentIndex].avail[i] != 0)
                        setAvail(resolverIndex, i, nodes[resolverIndex].avail[0] + nodes[resolverIndex].avail[i] - R0);
                    //clueA[i] = 0;
                }
                printf("Right:");
                printf("sumA= %d, R0= %d, N= %d, n=%d, Nr=%d, Nc=%d\n", sumA, R0, N, clueCounter, Nr, Ncorrect);
                printAvail(resolverIndex);
                clueCounter = 0;
                nodes[resolverIndex].visited = 1;

                /* Calculate Left side avail */
                /* Leaf */
                //int leftSilbingIndex = getLeftChildIndex(parentIndex);
                //nodes[leftSilbingIndex].visited = 1;
                if(distance(resolverIndex) == 1){
                    lastLeafA = A;
                    int answer = 0;
                    while(nodes[parentIndex].avail[answer] - nodes[resolverIndex].avail[answer] != 1) answer++;
                    setArrSingle(nodes[parentIndex].leftIndex, answer);
                    printf("Set confirmed single data %d\n", answer);
                }
                /* Segment */
                else{
                    int leftSilbingIndex = getLeftChildIndex(parentIndex);
                    for(int i=0; i<TYPE_CHAR; i++){
                        setAvail(leftSilbingIndex, i
                        , nodes[parentIndex].avail[i] - nodes[resolverIndex].avail[i]);
                    }
                    /* Mark as visited to ignore it in future visiting by DFS */
                    //if(distance(leftSilbingIndex)>1)
                    nodes[leftSilbingIndex].visited = 1;
                    printf("Left :");
                    //printf("sumA= %d, R0= %d, N= %d\n", sumA, R0, N);
                    printAvail(leftSilbingIndex);
                    
                    /* Deal with known result */
                    /* Deal with critical condition which answer can be determined */
                    //checkAnswerCanBeDetermine(leftSilbingIndex);
                }
                /* Deal with the situation that only have two space */
                resolverIndexChangeFlag = 0;
                
                if(distance(resolverIndex) == 1){
                    specialWorker = 1;
                    lastLeafA = A;
                    printf("LeafA = %d\n", lastLeafA);
                    printf("Critical1= %d\n", nodes[resolverIndex].critical);
                    printf("Guess1=  ");
                    guessRestNumber(resolverIndex);
                    return ans;
                }
                else{
                    //checkAnswerCanBeDetermine(resolverIndex);
                }
            }
            /* In regular, restore the clue */
            //else clueA[resolverCounter] = A;// Process clue from 0~9
        
        }
    }
    #pragma endregion Task Resolver
    resolverIndex = assignerIndex;
    if(newAssignerIndexFlag)
        resetSilbing(assignerIndex);
    #pragma region Task Assigner
    /* Task Assigner */
    //printf("Assigner(%d, %d): \n", assignerIndex, resolverIndex);
    /*if(specialWorker){
        int counter = 0;
        ans[ nodes[assignerIndex].leftIndex ] = getRestAvailNum(&counter, assignerIndex);
        ans[ nodes[assignerIndex].rightIndex ] = getRestAvailNum(&counter, assignerIndex);
    }*/
    if(nodes[resolverIndex].visited == 1 && distance(assignerIndex)==1){
        superWorker = 1;
        int answer = 0;
        printf("LeafA = %d\n", lastLeafA);
        printf("Critical2= %d\n", nodes[resolverIndex].critical);
        printf("Guess2=  ");
        guessRestNumber(resolverIndex);
        return ans;
    }
    else{
        if(nodes[assignerIndex].visited){
            assignerIndex = getDFSnext();
            resolverIndex = assignerIndex;
            resolverCounter = assignerCounter = 0;
            //newAssignerIndexFlag = 1;
            resolverIndexChangeFlag = 0;
            printf("---------skip------%d\n", assignerIndex);
        }
        //if(newAssignerIndexFlag)
        //    resetSilbing(assignerIndex);
        while(assignerCounter!=0 && nodes[getParentIndex(assignerIndex)].avail[assignerCounter] == 0 ){
            //printf("Fast forward: %d->%d\n",assignerIndex, assignerCounter);
            setAvail(assignerIndex, assignerCounter, 0);
            taskManager();
            
            if(newAssignerIndexFlag)
                resetSilbing(assignerIndex);
        }
        setArr(nodes[assignerIndex].leftIndex, nodes[assignerIndex].rightIndex, assignerCounter);
    }
        
    // TODO: zero problem
    #pragma endregion Task Assigner
    printf("(%d, %d) = ", assignerIndex, assignerCounter);
    for(int i = 0; i<ANS_LEN; i++)
        printf("%c ", ans[i]);
    //printf("\n");
    stage++;
    if(stopFlag) getchar();
    taskManager();
    return ans;
}
#endif