/*
 ============================================================================
 ID          : 1200089
 Author      : Mohammed Owda
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
	int id;
	struct node *Next;
}idNode;

typedef struct passengersNode {
	int id, date[3], time[2];
	char from[50], to[50];
	int matched;
	struct passengersNode *Next;
}pasNode;

typedef struct bussesNode {
    int no,date[3],time[2],price,capacity;
	char from[50], to[50];
	int noOfPassenger;
	idNode *passengersId;
	struct bussesNode *Next;
}busNode;

idNode* findPreviousIdNode(int, idNode*);
pasNode* findPreviousPasNode(int, pasNode*);
busNode* findPreviousBusNode(int, busNode*);

int main() {

    pasNode* passenger;
    busNode* bus;
    idNode* unmatched;

    passenger =(pasNode*)malloc(sizeof(pasNode));
    if(passenger== NULL)
        printf("Out of memory!\n");
    passenger->Next = NULL;

    bus =(busNode*)malloc(sizeof(busNode));
    if(bus== NULL)
        printf("Out of memory!\n");
    bus->Next = NULL;

    unmatched =(idNode*)malloc(sizeof(idNode));
    if(unmatched== NULL)
        printf("Out of memory!\n");
    unmatched->Next = NULL;

    int busCount, pasCount;

	int menu;

	while (1) {
		printf("=============================== Menu =========================================\n");
		printf("1- Load bus information file and print it \t 2- Load the passenger information file and print it\n"
				"3- Assign passengers and print assignment information of all busses\t 4- Print a specific bus information\n"
			    "5- Print unmatched passengers\t 6- Add new passenger\n7-Delete passenger\t"
				"8- Delete bus number\n9- Exit the program\n");
		printf("==============================================================================\n");
		printf("Enter a number: ");

		scanf("%d", &menu);

		switch (menu) {
		case 1:
			busCount = loadBusInfo(bus);
			break;
		case 2:
			pasCount = loadPassengerInfo(passenger);
			break;
		case 3:
			assignPassengers(passenger, bus);   //assign passengers and print them
			printBusList(bus);
			break;
		case 4:{
		    int no;
            printf("Enter the bus number: ");
			scanf("%d", &no);
			printBusInfo(no, bus, passenger);
		}
			break;

		case 5:
			printUnmatchedPassenger(unmatched, passenger);
			break;

		case 6:{    //add passenger and assign him to a bus
		    int id,date[3],time[2];
		    char from[50], to[50];
            printf("Enter id, date(day month year), time(hours minute), from, to \n");
            scanf("%d%d%d%d%d%d%s%s", &id, &date[0], &date[1], &date[2], &time[0], &time[1], from, to);
		    addPassenger(passenger, id, date, time, from, to);
		    assignPassengers(passenger, bus);
		}
			break;

		case 7:{   //delete a passenger by it's id
		    int id;
		    printf("Enter Passenger ID: ");
		    scanf("%d", &id);
		    deletePassenger(id, passenger, bus);
		}
			break;

		case 8:{    //delete bus number
		    int no;
		    printf("Enter the bus number: ");
		    scanf("%d", &no);
		    deleteBusNumber(no, bus, passenger);
		    assignPassengers(passenger, bus);
		}
			break;

		case 9:  //exit the program
			exit(0);
			break;

		default:
			printf("Enter a number from 1-9\n");

		}
	}

	return 0;
}

void printPassengerList(pasNode *L) {
	pasNode *P = L->Next;
	if(L->Next == NULL)
        printf("Empty list\n");
	else
        while(P != NULL){
            printf("ID:%d\t ", P->id);
            P=P->Next;
        }

}
void printBusInfo(int no, busNode *L, pasNode *P){ //print a specific bus info with it's passengers

    busNode* X;
    X = L->Next;

    while(X != NULL && X->no != no)  //find the bus node by bus number
        X = X->Next;

    if(X==NULL){
        printf("Bus not Found\n");
        return;
    }

    printf("No: %d\tDeparture date:%d %d %d\tTime: %d:%d\t From: %s\t To: %s\t price:%d\t number of passenger:%d\tcapacity:%d\n",
                    X->no, X->date[0], X->date[1], X->date[2], X->time[0], X->time[1], X->from, X->to, X->price, X->noOfPassenger, X->capacity);
    printf("Passengers: ");

    idNode *K= X->passengersId->Next;
    for(int i=0; i < X->noOfPassenger; i++){  //print passengers of the bus
            pasNode *N = findPreviousPasNode(K->id, P)->Next;
            printf("-ID:%d  Travel time:%d:%d \t", K->id, N->time[0], N->time[1]);
            K=K->Next;
    }
    printf("\n");



}
void printBusList(busNode *L) {  //print busses info with it's passengers
	busNode *P = L->Next;
	idNode *X ;
	if(L->Next == NULL)
        printf("Empty list\n");
	else
        while(P != NULL){
            printf("No: %d\tDeparture date:%d %d %d\tTime: %d:%d\t From: %s\t To: %s\t price:%d\t number of passenger:%d\tcapacity:%d\n",
                    P->no, P->date[0], P->date[1], P->date[2], P->time[0], P->time[1], P->from, P->to, P->price, P->noOfPassenger, P->capacity);
            printf("Passengers: ");
            X= P->passengersId->Next;

            for(int i=0; i < P->noOfPassenger; i++){  //print passengers of each bus
                printf("%d \t", X->id);
                X=X->Next;
            }
            printf("\n");

            P=P->Next;
        }
}

int loadBusInfo(busNode* L) { // Load buses info and print them and return no of buses
	FILE *bus;
	bus = fopen("busses.txt", "r");
	char *token, temp[300];
	int i;

	while (fgets(temp,300,bus)) { //read the data from the file line by line
		token = strtok(temp, "#");   //split the line into tokens according to $
		int no, date[3], time[2], price, capacity;
		char from[50], to[50];
		int count = 0, busCount=0;

		while (token != NULL) {  //assign each token to it's field
			if (count == 0)
				no = atoi(token); //convert string to integer

			else if (count == 1) {   //convert string to integer then split day, month and year
				int temp2 = atoi(token);
				date[2]= temp2%10000;  //year
				temp2/=10000;
				date[1]=temp2%100;   //month
				temp2/=100;
				date[0]=temp2;    //day
			}

			else if (count == 2){  //split time into hours and minute then convert it to integers
				char *temp3;
				time[0]= atoi(token);
			}


			else if (count == 3){      //assign "from" token
				time[1]= atoi(token);
			}

			else if(count==4){
				strcpy(from, token);
				trim(from);
			}

			else if(count==5){            //assign "to" token
				strcpy(to, token);
				trim(to);
			}

			else if (count == 6)
				price = atoi(token);

			else
				capacity = atoi(token);

			token = strtok(NULL, "#:\n");
			count++;
		}

		addBus(L, no, date,time, from, to, price, capacity ); // add a bus to the list

		printf("No: %d\tTravelling date:%d %d %d\tTime: %d:%d\t From: %s\t To: %s\t price:%d\tcapacity: %d\n",
				no, date[0], date[1], date[2],
				time[0], time[1], from, to, price,
				capacity);
	}

	fclose(bus);
	return i;
}


int loadPassengerInfo(pasNode* L) {  //load passengers info and return number of passengers
	FILE *passenger;
	passenger = fopen("passengers.txt", "r");
	char *token, temp[300];
	int i;

	while (fgets(temp,300,passenger)) { //read the data from the file line by line
		token = strtok(temp, "#");   //split the line into tokens according to $
		int id, date[3], time[2];
		char from[50], to[50];
		int count = 0, pasCount=0;

		while (token != NULL) {  //assign each token to it's field
			if (count == 0)
				id = atoi(token); //convert string to integer

			else if (count == 1) {   //convert string to integer then split day, month and year
				int temp2 = atoi(token);
				date[2]= temp2%10000;  //year
				temp2/=10000;
				date[1]=temp2%100;   //month
				temp2/=100;
				date[0]=temp2;    //day
			}

			else if (count == 2){  //assign hours then convert it to integers
				char *temp3;
				time[0]= atoi(token);
			}


			else if (count == 3){     //assign minutes then convert it to integers
				time[1]= atoi(token);
			}

			else if(count==4){   //assign "from" token
				strcpy(from, token);
				trim(from);
			}

			else{                  //assign "to" token
				strcpy(to, token);
				trim(to);
			}

			token = strtok(NULL, "#:\n");
			count++;
		}
		count++;

		addPassenger(L, id, date, time, from, to);

		printf("ID: %d\tTravelling date:%d %d %d\tTime: %d:%d\t From: %s\t To: %s\n",
						id, date[0], date[1], date[2], time[0],
						time[1], from, to);
	}



	fclose(passenger);
	return i;
}


void addPassenger(pasNode *L, int id, int date[3], int time[2], char from[50],char to[50]) { //add a passenger right to the header node in the list
	pasNode *temp, *P;
	temp = (pasNode*) malloc(sizeof(pasNode));
	if (temp == NULL){
        printf("OUT OF MEMORY");
        exit(0);
	}
	temp->id = id;            //Assign info for the passenger node
    temp->date[0]= date[0];
    temp->date[1]= date[1];
    temp->date[2]= date[2];
    temp->time[0]= time[0];
    temp->time[1]= time[1];
    temp->matched=0;
    strcpy(temp->from, from);
    strcpy(temp->to, to);

	temp->Next = L->Next;
	L->Next = temp;

}

void addBus(busNode *L, int no, int date[3], int time[2], char from[50],char to[50],
             int price, int capacity) {           //add a passenger right to the header node in the list
	busNode *temp, *P;
	idNode *x;
	temp = (busNode*) malloc(sizeof(busNode));
	if (temp == NULL){
        printf("OUT OF MEMORY");
        exit(0);
	}
	temp->no = no;            //Assign info for the bus node
    temp->date[0]= date[0];
    temp->date[1]= date[1];
    temp->date[2]= date[2];
    temp->time[0]= time[0];
    temp->time[1]= time[1];
    temp->price= price;
    temp->capacity= capacity;
    strcpy(temp->from, from);
    strcpy(temp->to, to);
    temp->noOfPassenger=0;
    temp->passengersId = (idNode*)malloc(sizeof(idNode));
    if(temp->passengersId == NULL){
        printf("OUT OF MEMORY");
        exit(0);
    }
    temp->passengersId->Next=NULL;

    temp->Next = L->Next;
	L->Next = temp;


}

void assignPassengers(pasNode *L, busNode *P){  //assign each passenger to a bus
    pasNode *temp;
    busNode *temp2;
    temp= L->Next;


   while (temp != NULL) {  //loop across the passengers
		int timeDifference = 24 * 60;
		busNode *X;
        temp2= P->Next;
		if (!temp->matched) //check if the passenger is already in a bus
			while (temp2 != NULL) {  //loop across buses
			    if(strcmp(temp2->from, temp->from)==0 && strcmp(temp2->to, temp->to)==0)
                  if (temp2->noOfPassenger < temp2->capacity)
					if (temp->date[0] == temp2->date[0]&& temp->date[1] == temp2->date[1] && temp->date[2] == temp2->date[2]) //check the date
						if (temp->time[0] < temp2->time[0]|| (temp->time[0] == temp2->time[0] && temp->time[1] <= temp2->time[1])) { //check the time

							int timeDifference2 = temp2->time[0] * 60+ temp2->time[1] - (temp->time[0] * 60
									               + temp->time[1]); //find the time difference between bus and passenger
							if ((timeDifference2 < timeDifference) && timeDifference2>=0) { //to get the best departure time
								if (temp->matched) {
									removePassengerFromBus(temp->id,X->passengersId);
									X->noOfPassenger--;
									addPassengerToList(temp->id,temp2->passengersId);
									temp2->noOfPassenger++;
								}
								 else if (!temp->matched) {
									addPassengerToList(temp->id,temp2->passengersId);
									X = temp2;
									temp->matched = 1;
									temp2->noOfPassenger++;
								}
							}
						}




				temp2 = temp2->Next;
			}

		temp = temp->Next;
	}
}

void addPassengerToList(int id, idNode *P){  // add passenger's id to a linked list
    idNode *temp;
	temp = (idNode*) malloc(sizeof(idNode));
	if (temp == NULL){
        printf("OUT OF MEMORY");
        exit(0);
	}
	temp->id= id;
	temp->Next = P->Next;
    P->Next = temp;
}

void removePassengerFromBus(int id, idNode *L){ //remove passenger's id from the bus
   	idNode *P, *temp;
	P = findPreviousIdNode(id, L);

	if(!P->Next==NULL){
        temp = P->Next;
        P->Next = temp->Next;
        free(temp);
	}

}

idNode* findPreviousIdNode(int id, idNode *L) { //find previous node in id node
	idNode *P;
	P = L;
	while (P->Next != NULL && P->Next->id != id)
		P = P->Next;
	return P;
}

pasNode* findPreviousPasNode(int id, pasNode *L){ //find previous node in passenger node
    pasNode *P;
	P = L;
	while (P->Next != NULL && P->Next->id != id)
		P = P->Next;
	return P;
}

busNode* findPreviousBusNode(int no, busNode *L) { //find previous node in id node
	busNode *P;
	P = L;
	while (P->Next != NULL && P->Next->no != no)
		P = P->Next;
	return P;
}

void printUnmatchedPassenger(idNode *L, pasNode *P){ //add unmatched passenger in a linked list then print them
    pasNode *X = P->Next;

    printf("Unmatched Passengers: ");
    while(X!=NULL){
        if(!X->matched){  //check if the passenger in a bus
            addPassengerToList(X->id, P);
            printf("%d \t", X->id);
        }
        X=X->Next;
    }
    printf("\n");
}

void deletePassenger(int id, pasNode* L, busNode *B){ //delete a passenger from the list and bus list
      pasNode *P,*temp;
      P = findPreviousPasNode(id, L);
      if(P->Next==NULL){
        printf("Passenger Not Found\n");
        return;
      }
      if(P->Next != NULL){  //delete the passenger node from the passenger list
          temp = P->Next;
          P->Next = temp->Next;
          free(temp);
      }

      busNode *X= B->Next;
      while(X!=NULL){   //find the passenger in which bus and delete it
          if(findPreviousIdNode(id, X->passengersId)->Next != NULL){
             removePassengerFromBus(id, X->passengersId);
             X->noOfPassenger--;
          }

          X=X->Next;
      }


}

void deleteBusNumber(int no, busNode *L, pasNode *P){
    busNode *B;
    B = findPreviousBusNode(no, L)->Next;
    if(B==NULL){
      printf("Bus number not Found\n");
      return;
    }


    idNode *K= B->passengersId->Next;
    for(int i=0; i < B->noOfPassenger; i++){  //delete passenger from the bus
            pasNode *N = findPreviousPasNode(K->id, P)->Next;
            N->matched=0;
            removePassengerFromBus(K->id, B->passengersId);
            K=K->Next;
    }
    busNode *X, *temp;
    X= findPreviousBusNode(no, L);
    if(X->Next !=NULL){
        temp = X->Next;
        X->Next = temp->Next;
        free(temp);
    }



}

void trim(char *str) { // remove all leading and trailing spaces from the string
	int len = strlen(str);
	while (isspace(str[0]) || isspace(str[len - 1])) {
		if (isspace(str[0]))       //check if the first character is space
			for (int j = 0; j < len; j++) { //delete the space and shift the string to the left
				str[j] = str[j + 1];
			}
		if (isspace(str[len - 1]))  //  check if last character is space
			str[len - 1] = '\0';
	}

}




