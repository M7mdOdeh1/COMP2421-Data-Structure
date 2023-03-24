#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node node;
typedef struct Graph graph;
typedef struct costNode costTable;

struct Node{   //vertex node
    char city[50];
    int weight;
    node *next;
};

struct Graph{
    int noOfvertices;
    node *adj[1000];  //adjacency list: array of linked lists
};

struct costNode{
    char city[50];
    char previous[50];
    int cost;
    int isVisited;
};

node *path;
costTable tb[];


void readCitiesFile(graph *g) { //read the file and create the graph(the adjacency list)

	FILE *cities;
	cities = fopen("cities.txt", "r");
	if(cities == NULL){
        printf("File not Found\n");
        return;
	}

	char temp[300];
	int i=0;
	while (fgets(temp, 300, cities)) {
        char city1[50];
        char city2[50];
		char distance[20];
		//source city
		strcpy(city1, strtok(temp, "\t"));
		int index = findCity(city1, g);      //search about city in adjacency list
		if(index == -1)                     //check if the city vertex not exist in adjacency list
            addVertexToList(city1, g);   //add the city vertex to the list

        //destination city
		strcpy(city2, strtok(NULL, "\t"));
		index = findCity(city2, g);      //search about city in adjacency list
		if(index == -1)                     //check if the city vertex not exist in adjacency list
            addVertexToList(city2, g);   //add the city vertex to the list

		//destance
		strcpy(distance, strtok(NULL, "\n"));
		distance[strlen(distance)-2]='\0'; //to ignore "km" after the number
		int distanceInt = atoi(distance); // convert the distance from string to integer

		index = findCity(city1, g); //find the index of the source city in the list

		//insert a destination city to source city list, index is the source city index in the list and distance is the wight
		insertToListAtFirst(city2, distanceInt, g->adj[index]);

		//int index2 = findCity(city2, g);  //allow travel in opposite side
        //insertToListAtFirst(city1, distanceInt, g->adj[index2]);


        i++;
    }
    printf("File Loaded\n");
    fclose(cities);
}

void deleteNodeFromList(char city[], node *list){
    node *temp = list->next;
    while (temp->next!=NULL){
        if(strcasecmp(temp->next->city, city)==0){
            temp->next = temp->next->next;
            break;
        }
        temp = temp->next;
    }


}

//insert an edge between two cities
void insertToListAtFirst(char city[], int weight, node *list){ // insert node to the first index of linked list
    node *newNode, *temp;
    newNode = (node*)malloc(sizeof(node));
    strcpy(newNode->city, city);
    newNode->weight=weight;

    temp = list->next;
    list->next= newNode;
    newNode->next=temp;
}

void insetToListAtLast(char city[], int weight, node *list){
    node *newNode, *temp;
    newNode = (node*)malloc(sizeof(node));
    strcpy(newNode->city, city);
    newNode->weight=weight;
    temp=list->next;
    while (temp->next!=NULL){
        temp=temp->next;
    }

    temp->next = newNode;
    newNode->next= NULL;


}

int findCity(char city[], graph *g){ //find the city name in adjacency list and return its index

    for(int i=0; i< g->noOfvertices; i++){ //loop along adjacency list to find the city vertex
        if(!strcasecmp(g->adj[i]->city, city))
           return i;
    }
    return -1;

}

void addVertexToList(char city[], graph *g){ //add a vertex to the adjacency list
    g->adj[g->noOfvertices] = (graph*)malloc(sizeof (graph)); //create vertex
    if(g->adj[g->noOfvertices]==NULL)
        printf("OUT OF MEMORY\n");

    g->adj[g->noOfvertices]->next= NULL;
    strcpy(g->adj[g->noOfvertices]->city, city);            //assign the name in the vertex
    g->noOfvertices++; //increase the number of vertices in the graph
}

graph* createGraph(graph *g){ //initialize the graph and create it
    g= (graph*)malloc(sizeof(graph));
    if(g==NULL)
        printf("OUT OF MEMORY\n");
    g->noOfvertices =0;

}
void printAllInfo(graph *g){ //print the graph(the adjacency list)

    for(int i=0; i <g->noOfvertices; i++){
        node *temp;
        temp=g->adj[i]->next;
        printf("Source city: %s\n", g->adj[i]->city);
        printf("Destinations: ");
        while(temp!=NULL){
            printf("%s %d\t", temp->city, temp->weight);
            temp = temp->next;
        }
            printf("\n--------------------------------------------\n");

    }
}

//find the minimum cost of a city in the cost table and ignore the visited city
costTable findMinInCostTable(costTable tb[], int size){

    costTable min;
    min.cost=tb[0].cost;
    strcpy(min.city, tb[0].city);
    for(int i=0; i<size; i++){
        if(tb[i].isVisited==1)
            continue;
        if(tb[i].cost<min.cost){
            min.cost= tb[i].cost;
            strcpy(min.city, tb[i].city);
        }
    }
    return min;
}

costTable* dijkstra(char source[], graph *g){
     int n = g->noOfvertices;
   //  costTable tb[n];

     node *list = (node*)malloc(sizeof (node));
     list->next = NULL;

     for(int i=0; i<n ; i++){ //set the initial values of the cost table
        tb[i].isVisited=0;
        strcpy(tb[i].city, g->adj[i]);
        if(strcasecmp(g->adj[i], source)==0){ //check if it the source city
            tb[i].cost=0;           //assign it cost to zero
            insertToListAtFirst(g->adj[i]->city, 0, list);
        }
        else{
            tb[i].cost=99999;        //else assign it to infinity
            insertToListAtFirst(g->adj[i]->city, 99999, list);
        }

     }
     for(int i=0; i<n; i++){
            costTable min = findMinInCostTable(tb, n);
            int index = findCity(min.city, g); //find the city in the adjacency list
            tb[index].isVisited=1;
            node *neighbour = g->adj[index]->next;

            while(neighbour != NULL){  //loop along neighbour
                int cost = neighbour->weight + min.cost;

                for(int j=0; j< g->noOfvertices; j++){
                    if(strcasecmp(neighbour->city, tb[j].city)==0){
                        if(cost<tb[j].cost){
                            tb[j].cost= cost;
                            strcpy(tb[j].previous, min.city);

                        }
                    }
                }

                neighbour = neighbour->next;
            }
    }

        printTable(tb, n);
        return tb;
 }




 void findThePath(char *source, char *destination, graph *g){

     for(int i=0; i<g->noOfvertices; i++){
        if(strcasecmp(destination, tb[i].city)==0){
            if(strcmp(tb[i].previous,source)==0) {
                return;
            }
            int prevCost = previousCost(tb[i].previous, destination, g);

            insetToListAtLast(tb[i].previous, prevCost, path);

            findThePath(source, tb[i].previous, g);
        }
     }

 }

 void printPath(){
     node *temp;
     temp=path->next;
     while (temp!=NULL){
        printf("-%d-%s", path->weight, path->city);
        temp = temp->next;
     }

}

 int previousCost (char* previous,char destination[], graph *g){
    int index = findCity(previous, g);
    node *temp = g->adj[index]->next;

    int cost =99999;
    while(temp != NULL)
    {
        if(strcasecmp(temp->city,destination)==0)
            cost = temp->weight;

        temp = temp->next;
    }

    return cost;
}

 void printTable(costTable tb[],int size){ //print the content of the cost table
     for(int i=0; i< size; i++){
        printf("%s : %dkm\n", tb[i].city, tb[i].cost);
     }
 }

 void saveToFile(){

      FILE *out = fopen("shortest_path.txt", "w");
      node *temp;
      temp = path->next;
      while(temp!=NULL){

        fprintf(out, "-%s-%d ", path->city, path->weight);
      }


 }

int main()
{
    graph *g;
    g = createGraph(g);


    int menu;
    while (1) {
		printf("=============================== Menu =========================================\n");
		printf("1- Load the File\n"
               "2- Enter Source city\n"
               "3- Enter destination city\n"
               "4- Save and Exit\n");
		printf("==============================================================================\n");
		printf("Enter a number: ");

		scanf("%d", &menu);
		char city[50];
		char destination[50];
		switch (menu) {
		case 1: //read the file and create the graph
		    readCitiesFile(g);
		        printAllInfo(g);


			break;
		case 2:  //
		    printf("Enter Source city: ");
		    scanf("%s", city);
            dijkstra(city, g);
			break;

		case 3:  //
		    printf("Enter Destination City: ");
		    scanf("%s", destination);
		    path= (node*)malloc(sizeof(node));
		    findThePath(city, destination, g);
		    printPath();
		    break;

		case 4:  //exit the program

		    saveToFile();
			exit(0);
			break;

		default:
			printf("Enter a number from 1-4\n");

		}

	}

    printf("Hello world!\n");
    return 0;
}
