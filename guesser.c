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
void treeConstruct(int index, int left, int right){
    if(right - left > 0){  // Node init
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
inline int isLeaf(int index){
    if(nodes[index].rightIndex - nodes[index].leftIndex > 1) return 0;
    if(nodes[index].rightIndex - nodes[index].leftIndex == 1) return 1;
    if(nodes[index].rightIndex - nodes[index].leftIndex == 0) return 2;
    return 3;
}
/*  DFS walk tree  */
void getDFSnext(){
    int offset = isLeaf(getRightChildIndex(DFSwalkIndex));
    if(offset >= 1){      // Right side not reach end
        stack[top++] = getLeftChildIndex(DFSwalkIndex); // Push left into task
        DFSwalkIndex = getRightChildIndex(DFSwalkIndex);
    }
    else if(offset == 1){ // Leaf which only have one node, answer confirm
        ans[nodes[DFSwalkIndex].rightIndex] = getRestAvailNum[]
        getDFSnext();
    }
    else{
        DFSwalkIndex = stack[--top];
        if(top == 0)    printf("End of Search\n");
    }
}
#pragma endregion
inline void checkAnswerCanBeDetermine(int index){
    if(nodes[index].critical == 1){
        int answer = getRestAvailNum(index);
        setArr(nodes[index].leftIndex, 
                nodes[index].rightIndex,
                answer);
    }
}
void setArr(int left, int right, char value){
    for(int i = left; i<=right; i++)
        ans[i] = value;
}
inline void getRestAvailNum(int index){
    int answer = -1;
    while(nodes[resolverIndex].avail[++answer]==0) ;
    return answer;
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
#pragma region Manager
inline void taskManager(){
    // Maintain task counter
    resolverCounter = assignerCounter;
    assignerCounter++;
    if(assignerCounter>=9){
        assignerCounter = 0;
        resolverIndex = assignerIndex;
        assignerIndex = getDFSnext();
        while(nodes[assignerIndex].visited)
            assignerIndex = getDFSnext();
    }
}
#pragma endregion

/* Global Variables setup */ 
int stage = 0;
int dataNeedConfirm = 0;
int lastLeafA = 0;
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
        else{
            if(dataNeedComfirm){
                
            }
            /* The end of a task */
            if(resolverCounter == 9){
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
                    if(nodes[resolverIndex].avail[i] != -1)
                        setAvail(resolverIndex, i, nodes[resolverIndex].avail[0] + clueA[i] - R0);
                        //nodes[resolverIndex].avail[i] = nodes[resolverIndex].avail[0] + clueA[i] - R0;
                }
                int leftSilbingIndex = getLeftChildIndex(parentIndex);
                /* Calculate Left side avail */
                for(int i=0; i<TYPE_CHAR; i++){
                    setAvail(leftSilbingIndex, 
                    , nodes[parentIndex].avail[i] - nodes[resolverIndex].avail[i]);
                    //nodes[getLeftChildIndex(parentIndex)].avail[i]
                    // = nodes[parentIndex].avail[i] - nodes[resolverIndex].avail[i];
                }
                if(nodes[leftSilbingIndex].critical == 1){
                    setArr()
                }
                /* Mark as visited to ignore it in future visiting by DFS */
                nodes[resolverIndex].visited = 1;
                nodes[leftSilbingIndex].visited = 1;
                
                /* Deal with known result */
                /* Deal with critical condition which answer can be determined */
                checkAnswerCanBeDetermine(resolverIndex);
                checkAnswerCanBeDetermine(leftSilbingIndex);

                /* Deal with the situation that only have two space */
                if(isLeaf(resolverIndex)){
                    dataNeedComfirm = 1;
                    setArr()
                }
            }
            /* In regular, restore the clue */
            else clue[resolverCounter] = A;// Process clue from 0~9
        }
    }
    #pragma endregion Task Resolver

    #pragma region Task Assigner
    /* Task Assigner */
    if(resolverCounter - assignerCounter > 0){  
        // A new cycle, clear old changes
        int leftSiblingIndex = getLeftChildIndex(getParentIndex(assignerIndex));
        setArr(nodes[leftSiblingIndex].leftIndex, nodes[leftSiblingIndex].rightIndex, 0);
    }
    while(nodes[getParentIndex(assignerIndex)].avail[assignerCounter] == 0){
        setAvail(assignerIndex, assignerCounter, 0);
        //nodes[assignerIndex].avail[assignerCounter] = 0;
        taskManager();
    }
    setArr(nodes[assignerIndex].leftIndex, nodes[assignerIndex].rightIndex, taskCounter);
    // TODO: zero problem
    #pragma endregion Task Assigner

    taskManager();
    return ans;
}
#endif