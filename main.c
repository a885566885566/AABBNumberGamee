#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>

#include "guesser.c"
#define MAX_LENGTH 20
clock_t start,finish;

char *check(char data[MAX_LENGTH], char answer[MAX_LENGTH]) {
    static char clue[13];
    char temp_A[6],temp_B[6];
    int i;
    int A = 0;
    int ans_appear_times[10] = {0, 0, 0, 0, 0, 
                              0, 0, 0, 0, 0};
    int data_appear_times[10] = {0, 0, 0, 0, 0, 
                               0, 0, 0, 0, 0};
    //calculate A
    for(i=0; i<MAX_LENGTH; i++) {
        if(data[i] == answer[i]) {
            A++;
        } 
        else {
            ans_appear_times[answer[i]-'0']++;
            data_appear_times[data[i]-'0']++;
        }  
    }
  // calculate B
    int B = 0;
    for(i=0; i<10; i++) {
        if(data_appear_times[i] >= ans_appear_times[i]) {
            B += ans_appear_times[i];
        } 
        else {
            B += data_appear_times[i];
        }
    }
    if(A==MAX_LENGTH && B==0)	{
  	    return "AC";
    }
    sprintf(temp_A,"%d",A);  sprintf(temp_B,"%d",B);
    strcat(temp_A,"A"); strcat(temp_B,"B"); strcat(temp_A,temp_B);  strcpy(clue,temp_A);
    return clue;
} 

int main(int argc, char *argv[]) {

    char ans[MAX_LENGTH];
    char get_back[MAX_LENGTH]="0A0B";   //initial input to guess function
    srand(time(NULL));
    struct timeval tval_before, tval_after, tval_result;

    int i;
    for(i=0; i<MAX_LENGTH; i++) {
        ans[i] = rand()%10+'0';
    }
    //time start
    gettimeofday(&tval_before, NULL);
    printf("Game Start\n");
    printf("Puzzle = ");
    for(int i = 0; i<ANS_LEN; i++)
        printf("%c ", ans[i]);
    printf("\n");
    //guess(get_back);
    //getchar();
    while(1){	
        //get_back is mean that the result of your guess
  	    strcpy(get_back,check(guess(get_back), ans));
        // your guessed right!!! Congratulation !!!
        if(strcmp(get_back,"AC")==0)	break;
  	    gettimeofday(&tval_after, NULL);
  	    long t = (tval_after.tv_sec*1e6 + tval_after.tv_usec) - (tval_before.tv_sec*1e6 + tval_before.tv_usec);
  	    // convert to we can read
        const int n = snprintf(NULL, 0, "%lu", t);
        assert(n > 0);
        char buf[n+1];
        int c = snprintf(buf, n+1, "%lu", t);
        assert(buf[n] == '\0');
        assert(c == n);
        //print time
        int length = strlen(buf);
        if(length>6) {
            for(int i=0; i<length-6;i++){
                printf("%c",buf[i]);
            }
            printf(",");
            for(int i=length-6; i<length;i++) {
                printf("%c",buf[i]);
            }
            printf("\n"); 
        }
    }
    return 0;
}
