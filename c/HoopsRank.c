#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Team {
    char name[63]; // name of team
    int metric; // calculated metric for each team
    struct Team* next;
} typedef Team;

void swap(Team* a, Team* b){
    char tempN[63];
    strcpy(tempN, a->name);
    int tempM = a->metric;
    strcpy(a->name,b->name);
    a->metric = b->metric;
    strcpy(b->name, tempN);
    b->metric = tempM;
}

void printAndFree(Team* head){ // frees memory for nodes to prevent memory link
    while(head!=NULL){
        printf("%s %d\n", head->name, head->metric);
        Team* temp = head;
        head = head->next;
        free(temp);
    }
}

int metricCalc(int apgs, int apgg, int ard){
    int result;
    result = (apgs-apgg) + ard;
    return result;
}

void bubbleSort(Team* head){
    int swapped, i;

    Team* temp;
    Team* lptr = NULL; // last pointer aka node with no next node

    if (head == NULL){ // returns if empty linked list
        return;
    }
    do{
        swapped = 0;
        temp = head; // sets temp to head
        while (temp->next != lptr) { // while next node is not null
            if(temp->metric < temp->next->metric){ // if head's metric is less than the next node's metric, 
                swap(temp, temp->next);
                swapped = 1;
            }
            else if(temp->metric == temp->next->metric){
                if(strcmp(temp->name, temp->next->name) > 0){
                    swap(temp, temp->next);
                    swapped = 1;
                }
            }
            temp = temp->next;
        }
        lptr = temp;
    }
    while(swapped);
}

int main(int argc, char *argv[]){
    int num;

    FILE* teamData = fopen(argv[1], "r"); // passes in memory location of current char in file

    Team* head = (Team*) malloc(sizeof(Team)); // head node
    head->metric = 0;
    head->next = NULL;
    strcpy(head->name, "EMPTY");
    int headEmpty = 1;
    Team* curr; // current node

    char name[63]; // name of team
    int apgs; // average number of points the team scores per game
    int apgg; // average number of points the team gives up per game
    int ard; // average rebounding differnetial for the team

    while(1){
        fscanf(teamData, "%s", name);
        if (strcmp(name, "DONE") == 0) { // ends while loop
            break;
        }
        fscanf(teamData, "%d", &apgs);
        fscanf(teamData, "%d", &apgg);
        fscanf(teamData, "%d", &ard);
    
        Team* node = (Team*) malloc(sizeof(Team)); // dynamically allocates space for new node
        node->next = NULL;
        if (headEmpty == 1){ // if head is empty, set head to node
            Team* temp = head;
            head = node;
            free(temp);
            curr = head;
            headEmpty =-1;
        } else {
            curr->next = node;
            curr = curr->next;
        }
        int metValue = metricCalc(apgs, apgg, ard);
        strcpy(node->name,name);
        node->metric = metValue;

    }
    bubbleSort(head);
    printAndFree(head);
    fclose(teamData);

    return EXIT_SUCCESS;
}