#include<stdio.h>
#include<string.h>
#ifndef GUESSER
#define GUESSER
#define ANS_LEN 78763
#define TYPE_CHAR 10
typedef struct _NODE{
    int visited;
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
void treeConstruct(int index, int left, int right){
    if(right - left > 1){  // Node init
        nodes[index].leftIndex = left;
        nodes[index].rightIndex = right;
        nodes[index].visited = 0;
        for(int i=0;i<TYPE_CHAR;i++){
            nodes[index].leftAvail[i] = 0;
            nodes[index].rightAvail[i] = 0;
        }
        int middle = (right+left)/2;
        treeConstruct(getLeftChildIndex(index), left, middle);
        treeConstruct(getRightChildIndex(index), middle+1, right);
    }
    else{
        ans[left] = 0;
        ans[right] = 0;
    }
}
inline int isLeaf(int index){
    if(nodes[index].rightIndex - nodes[index].leftIndex > 1) return 0;
    return 1;
}
/*  DFS walk tree  */
int getDFSnext(){   
    if(!isLeaf(getRightChildIndex(DFSwalkIndex))){      // Right side not reach end
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
void setArr(int left, int right, char value){
    for(int i = left; i<=right; i++)
        ans[i] = value;
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
    if(stage == 0){ // First 
        init();
        stage = 1;
    }
    else{     // Process clue
        clueDecoder(&A, &B, clue);
        printf("step= %5d, %d A %d B\n",stepCounter, A, B);
        /* Stage1 */
        
        if(resolverIndex == 0) { // First
            nodes[resolverIndex].avail[resolverCounter] = A;
        }
        else{                    
            if(resolverCounter == 9){    // The end of a task, counter has been reset to 0
                int sumA = 0;
                for(int i=0;i<TYPE_CHAR;i++)
                    sumA += clueA[i];
                int parentIndex = getParentIndex(resolverIndex);
                int R0 = nodes[parentIndex].avail[0];
                int N = 1 + nodes[resolverIndex].rightIndex - nodes[resolverIndex].leftIndex;
                nodes[resolverIndex].avail[0] = 1 - (R0 - N + sumA)/9;
                for(int i=1; i<TYPE_CHAR; i++){ // Right side 
                    if(nodes[resolverIndex].avail[i] != -1)
                        nodes[resolverIndex].avail[i] = nodes[resolverIndex].avail[0] + clueA[i] - R0;
                }
                for(int i=0; i<TYPE_CHAR; i++){ // Left side
                    nodes[getLeftChildIndex(parentIndex)].avail[i]
                     = nodes[parentIndex].avail[i] - nodes[resolverIndex].avail[i];
                }
                nodes[resolverIndex].visited = 1;
                nodes[getLeftChildIndex(parentIndex)].visited = 1;
            }
            else clue[resolverCounter] = A;// Process clue from 0~9
        }
    }
    if(stage == 1){
        if(resolverCounter - assignerCounter > 0){  // A new cycle, clear old changes
            int leftSiblingIndex = getLeftChildIndex(getParentIndex(assignerIndex));
            setArr(nodes[leftSiblingIndex].leftIndex, nodes[leftSiblingIndex].rightIndex, 0);
        }
        while(nodes[getParentIndex(assignerIndex)].avail[assignerCounter] == 0){
            nodes[assignerIndex].avail[assignerCounter] = 0;
            taskManager();
        }
        setArr(nodes[assignerIndex].leftIndex, nodes[assignerIndex].rightIndex, taskCounter);
    }
    taskManager();
    return ans;
}
#endif