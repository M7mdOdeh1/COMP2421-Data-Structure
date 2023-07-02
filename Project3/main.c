#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct{
    char name[50];
    int creditHours;
    char code[30];
    char department[50];
    char topic[100][50];
    int noOfTopics;
}Course;

typedef struct{
    int size;
    int noOfCourse;
    Course **course;
}hashTable;

hashTable* createHashTable(hashTable*, int);
int totalLinearCollistions=0, totalDoubleCOllisions=0;

char* trim(char *str) { // remove all leading and trailing spaces from the string
	int len = strlen(str);
	while (isspace(str[0]) || isspace(str[len - 1])) {
		if (isspace(str[0]))       //check if the first character is space
			for (int j = 0; j < len; j++) { //delete the space and shift the string to the left
				str[j] = str[j + 1];
			}
		if (isspace(str[len - 1]))  //  check if last character is space
			str[len - 1] = '\0';
	}
	return str;
}

void readCoursesFile(hashTable *linearTable, hashTable *doubleTable) { //

	FILE *courses;
	courses = fopen("offered_courses.txt", "r");
	if(courses == NULL){
        printf("File not Found\n");
        return;
	}

	char *token, temp[300];
	int i=0;

	while (fgets(temp, 300, courses)) {
	    Course course;   //to store the courses info temporary before insert it to the tree
	    course.noOfTopics=0;
	    temp[strlen(temp)-1]='\0'; //to make the last character null instead of \n (new Line)
	    int count=0;
		token = strtok(temp, ":#/,");   //split the line into tokens
		while (token != NULL) {  //assign each token to it's field
			if (count == 0)
			    strcpy(course.name, trim(token)); //assign name of the course

			else if (count == 1)   //assign credit hours
				course.creditHours = atoi(trim(token));

			else if (count == 2)   //assign course code
				strcpy(course.code, trim(token));

			else if (count == 3)  //assign department of the course
                strcpy(course.department, trim(token));

			else {                   //assign topics
			    strcpy(course.topic[count-4], trim(token));
			    course.noOfTopics++;
			}

            token = strtok(NULL, ":#/,");
            count++;
        }
        addCourseToLinear(course, linearTable);
        addCourseToDouble(course, doubleTable);
        i++;
    }
    printf("File Loaded\n");
    fclose(courses);
}

void readFromUser(hashTable *linearTable, hashTable *doubleTable){ //add new course from the user and insert it to the tree
    Course c;
    c.noOfTopics=0;
    char temp[300], *token;
    //read course info from the user
    printf("Enter Course name, credit hours, code, department(each separated by comma):\n");
    scanf("%c");
    fgets(temp, sizeof temp, stdin);
    temp[strlen(temp)-1]= '\0';
    strcpy(c.name, trim(strtok(temp, ",")));
    c.creditHours = atoi(trim(strtok(NULL, ",")));
    strcpy(c.code, trim(strtok(NULL, ",")));
    strcpy(c.department, trim(strtok(NULL, ",")));
    printf("Enter topics(each separated by comma):");

    //reading topics from the user then separate it and store it
    scanf("%c");
    fgets(temp, sizeof temp, stdin);
    temp[strlen(temp)-1]= '\0';
    token = strtok(temp, ",");
    for(int i = 0; token != NULL; i++ ){
        strcpy(c.topic[i], trim(token));
        c.noOfTopics++;
        token = strtok(NULL, ",");
    }
    addCourseToLinear(c, linearTable);
    addCourseToDouble(c, doubleTable);
    printf("Course Added To Both Tables!!!\n");

}

void copyCourse(Course* c1, Course c2){  //copy course info to other course variable
    strcpy(c1->name, c2.name);
            c1->creditHours = c2.creditHours;
            strcpy(c1->code, c2.code);
            strcpy(c1->department, c2.department);
            c1->noOfTopics = c2.noOfTopics;

            for(int i=0; i<c2.noOfTopics; i++){
                strcpy(c1->topic[i], c2.topic[i]);
            }

}

int previousPrime(int n){ //find the first prime number <= n

    if(n < 2)
        return -1;

    while (!isPrime(--n) || n == 2){
    }
    return n;
}

int nextPrime(int n){  //find the first prime number > n

    while (!isPrime(++n)){

    }
    return n;
}

int isPrime(int n){  //check if the number is prime
    if(n<2)
        return 0;
    for(int i=2; i<sqrt(n); i++){
        if(n%i == 0){
            return 0;
        }
    }
    return 1;

}

void reHashLinear(hashTable *ht){ //rehash the linear hash table
    int size= ht->size * 2;
    size= nextPrime(size);

    Course **temp = ht->course ;
    Course **newTable = (Course**)malloc(sizeof(Course*)*size);
    for(int j=0; j<size; j++){
        newTable[j]=NULL;
    }
    ht->course = newTable;

    int oldSize= ht->size;
    ht->size = size;
    ht->noOfCourse=0;

    for(int i=0; i<oldSize; i++){
        if(temp[i]==NULL)
           continue;

        addCourseToDouble(*temp[i], ht);
        free(temp[i]);
    }

}

void reHashDouble(hashTable *ht){ //rehash the double hash table
    int size= ht->size * 2;
    size= nextPrime(size);

    Course **temp = ht->course ;
    Course **newTable = (Course**)malloc(sizeof(Course*)*size);
    for(int j=0; j<size; j++){
        newTable[j]=NULL;
    }
    ht->course = newTable;
    int oldSize= ht->size;
    ht->size = size;
    ht->noOfCourse=0;

    for(int i=0; i<oldSize; i++){
        if(temp[i]==NULL)
           continue;

        addCourseToDouble(*temp[i], ht);
        free(temp[i]);
    }
}

void addCourseToLinear(Course c, hashTable *ht){  //add a course to the linear hash table

    if(((int) (0.75 * (ht->size+1))) <= ht->noOfCourse){  //rehash the linear table if its 0.75 full
        reHashLinear(ht);

    }
    int index = linearHashing(c.name, ht);
    ht->course[index]= (Course*)malloc(sizeof(Course));
    copyCourse(ht->course[index], c);
    ht->noOfCourse++;

}

void addCourseToDouble(Course c, hashTable *ht){  //add a course to the double hash table

    if((int)(0.75 * (ht->size+1)) <= ht->noOfCourse){  //rehash the double table if its 0.75 full
            reHashDouble(ht);
    }
    int index = doubleHashing(c.name, ht);
    ht->course[index]= (Course*)malloc(sizeof(Course));
    copyCourse(ht->course[index], c);
    ht->noOfCourse++;

}

hashTable* createHashTable(hashTable* ht, int tableSize){   //create hash table with initial cells
    ht = (hashTable*) malloc(sizeof (hashTable));
    if(ht==NULL)
        printf("Out of space\n");

    ht->course = (Course**)malloc(sizeof(Course*)*tableSize);
    if(ht->course == NULL)
        printf("Out of space\n");
    ht->noOfCourse=0;
    ht->size= tableSize;
    for(int i=0; i <tableSize; i++){ //make the array empty
            ht->course[i]=NULL;
    }

    return ht;
}

long hash(char *name){  //string hashing

    long n = 0;

    while(*name != '\0')
        n = ((n << 5) + *name++);

    return abs(n) ;
}

int linearHashing(char *name, hashTable *ht){ //find the proper index for linear hashing
    long n = hash(name);
    int index = n % ht->size;
    for(int i=1; ht->course[index]!=NULL && i != ht->size; i++){
        totalLinearCollistions++;
        index = (n+i) % ht->size;

    }
    return index;
}

int doubleHashing(char *name, hashTable *ht){ //find the proper index for double hashing

    long n = hash(name);
    int index = n % ht->size;
    int h1 = index;
    int x = previousPrime(ht->size);  //find prime number < size
    int h2 = x - (n%x);

    for(int i=1; ht->course[index]!=NULL && i != ht->size; i++){
        totalDoubleCOllisions++;
        index = (h1 + i*h2)%ht->size;
    }

    return index;
}

void printCourseInfo(Course c){  //print info of a course
    printf("Course name:%s \t Credit Hours:%d \t Code:%s \t Department:%s    Topics:", c.name,
                c.creditHours, c.code, c.department);
        for(int i=0; i<c.noOfTopics; i++){  //print topics
            printf("%s", c.topic[i]);
            if(i != c.noOfTopics-1)
                printf(", ");
        }
}

void printTablesWithEmpty(hashTable *linearTable, hashTable *doubleTable){ //Print two hashed tables (including empty spots)

    printf("===================================================================================\n");
    printf("Linear hash table:\n");

    for(int i=0; i<linearTable->size; i++){
        printf("%c index %d : ", 254, i);
        if(linearTable->course[i]!=NULL)
            printCourseInfo(*linearTable->course[i]);
        printf("\n");
    }

    printf("===================================================================================\n");
    printf("===================================================================================\n");
    printf("Double hash table:\n");
    for(int i=0; i<doubleTable->size; i++){
        printf("%c index %d : ", 254, i);
        if(doubleTable->course[i]!=NULL)
            printCourseInfo(*doubleTable->course[i]);
        printf("\n");
    }
    printf("====================================================================================\n");

}

void printTablesUsed(hashTable *linearTable, hashTable *doubleTable){ //Print two hashed tables (without empty spots)
    printf("=====================================================================================\n");
    printf("Linear hash table:\n");
    for(int i=0; i<linearTable->size; i++){
        if(linearTable->course[i]!=NULL){
            printf("%c index %d : ", 254, i);
            printCourseInfo(*linearTable->course[i]);
            printf("\n");
        }

    }


    printf("h(t)= h1(t)+t*h2(t)");
    printf("=====================================================================================\n");
    printf("=====================================================================================\n");
    printf("\nDouble hash table:\n");

    for(int i=0; i<doubleTable->size; i++){
        if(doubleTable->course[i]!=NULL){
            printf("%c index %d : ", 254, i);
            printCourseInfo(*doubleTable->course[i]);
            printf("\n");
        }
    }
        printf("==================================================================================\n");
}

int searchTables(char *name, hashTable *ht, int n){ //search for course and ask the user to decide which table to search in
    if(n==0){ //search in linear hash table
        long h = hash(name);
        int index = h % ht->size;

        //keep scanning until find the element or empty slot found(Element not Found) or reach the end of the table
        for(int i=1; i < ht->size ; i++){
            if(ht->course[index]==NULL)
                return -1;

            if (strcasecmp(ht->course[index]->name, name)==0)
                return index;

            index = (h+i) % ht->size;
        }
        return -1;
    }

    else if(n==1){ //search in double hash table

        long h = hash(name);
        int index = h % ht->size;
        int h1 = index;
        int x = previousPrime(ht->size);  //find prime number < size
        int h2 = x - (h%x);

        for(int i=1; ht->size; i++){
            if(ht->course[index]==NULL)
               return -1;

            if (strcasecmp(ht->course[index]->name, name)==0)
                return index;

            index = (h1 + i*h2) % ht->size;
        }
        return -1;
    }
}

void deleteCourse(char *name, hashTable *linearTable, hashTable *doubleTable){ //Delete a specific record (from both tables)
     int linearIndex = searchTables(name, linearTable, 0); //find the index in linear table
     int doubleIndex = searchTables(name, doubleTable, 1); //find the index in double table

     if(linearIndex != -1){
        free(linearTable->course[linearIndex]);
        linearTable->course[linearIndex]=NULL;
        printf("Deleted From Linear Table\n");
     }
     else
        printf("Not Found in Linear Table\n");

     if(doubleIndex != -1){
        free(doubleTable->course[doubleIndex]);
        doubleTable->course[doubleIndex]=NULL;
        printf("Deleted From Double Table\n");
     }
     else
        printf("Not Found in Double Table\n");
}

int calculateLinearCollisions(hashTable *linearTable){ //find the number of collision in the linear table
    int count=0;
    for(int i=0; i< linearTable->size; i++){
        if(linearTable->course[i]==NULL)
            continue;

        long h = hash(linearTable->course[i]->name);
        int index = h % linearTable->size;
        char *name= linearTable->course[i]->name;
        if(index != i){
            for(int j=1; index!=i ; j++){
                count++;
                index = (h+j)%linearTable->size;
            }
        }
    }
    return count;
}

int calculateDoubleCollisions(hashTable *doubleTable){ //find the number of collision in the double table
     int count=0;
    for(int i=0; i< doubleTable->size; i++){
        if(doubleTable->course[i]==NULL)
            continue;

        long h = hash(doubleTable->course[i]->name);
        int index = h % doubleTable->size;
        int h1 = index;
        int x = previousPrime(doubleTable->size);  //find prime number < size
        int h2 = x - (h%x);

        for(int j=1; index!=i; j++){
            count++;
            index = (h1 + j*h2);
            }

    }
    return count;
}

void saveToFile(FILE* out, hashTable *doubleTable){ //save double hashing table to a file
    out= fopen("saved_courses.txt", "w");

    for(int i=0; i<doubleTable->size; i++){
        if(doubleTable->course[i]==NULL){
            fprintf(out, "***Empty Slot***\n");
            continue;
        }
        Course c ;
        copyCourse(&c, *doubleTable->course[i]);
        fprintf(out, "%s:%d#%s#%s/", c.name, c.creditHours, c.code, c.department);
        for(int i=0; i<c.noOfTopics; i++){
            fprintf(out, "%s", c.topic[i]);
            if(i != c.noOfTopics-1)
                fprintf(out, ", ");
        }
        fprintf(out, "\n");
    }
    fclose(out);
}


int main()
{
    FILE *out;
    hashTable *doubleTable, *linearTable;
    doubleTable = createHashTable(doubleTable,7);  //create hash table with initial 3 cells
    linearTable = createHashTable(linearTable,7);

    readCoursesFile(linearTable, doubleTable); //read courses file

    int menu;
    while (1) {
		printf("=============================== Menu =========================================\n");
		printf("1- Print hashed tables (including empty spots) \n2- Print out table size and the load factor"
               "\n3- Print out the used hash functions \n4- Insert a new record to hash table\n"
               "5- Search for a specific word \n6- Delete a specific record \n7-Compare between the two methods\n"
               "8- Save the double hashing table \n9-Exit \n");
		printf("==============================================================================\n");
		printf("Enter a number: ");

		scanf("%d", &menu);

		switch (menu) {
		case 1: //print both tables with empty slots
			printTablesWithEmpty(linearTable, doubleTable);
			break;
		case 2:  //print size and load factor of both tables
		    printf("%c Linear Table:\n", 254);
		    double linearloadFactor = (double)linearTable->noOfCourse/linearTable->size;
		    printf("Size:%d, Load Factor:%lf\n", linearTable->size, linearloadFactor);

		    printf("%c double Table:\n", 254);
		    double doubleloadFactor = (double)doubleTable->noOfCourse/doubleTable->size;
		    printf("Size:%d, Load Factor:%lf\n", doubleTable->size, doubleloadFactor);
			break;

		case 3:  //print used slots in both tables
		    printTablesUsed(linearTable, doubleTable);
		    break;

		case 4:  //insert a new course to both tables
		    readFromUser(linearTable, doubleTable);
			break;

		case 5:{         //search for course and ask the user to decide which table to search in
		    char temp[300];
		    int n, index;
		    printf("Enter 0: search in the Linear Table, 1: search in the Double Table\n");
		    for(scanf("%d", &n); n!=0 && n!= 1; scanf("%d", &n)){
                printf("Please Enter 0 or 1 only");
		    }
		    printf("Enter the name of the course:");
		    scanf("%c");
            fgets(temp, sizeof temp, stdin);
            temp[strlen(temp)-1]= '\0';

            if(n==0)
               index = searchTables(temp, linearTable, 0); //search in linear
            else if(n==1)
               index = searchTables(temp, doubleTable, 1);  //search in double

		    if(index==-1)
                printf("Not Found\n");
            else
                printf("Index = %d\n", index);
		}
			break;

		case 6:{   //Delete a specific record (from both tables)
		    char temp[300];
		    printf("Enter the name of the course:");
		    scanf("%c");
            fgets(temp, sizeof temp, stdin);
            temp[strlen(temp)-1]= '\0';
            deleteCourse(temp, linearTable, doubleTable);
		}
			break;

		case 7:   //Compare between the two methods in terms of number of collisions occurred
		    printf("Linear Hashing Collisions in Current Table:%d\n", calculateLinearCollisions(linearTable));
		    printf("Double Hashing Collisions in Current Table:%d\n", calculateDoubleCollisions(doubleTable));
		    printf("Total Linear Hashing Collision from the start of the program: %d\n", totalLinearCollistions);
		    printf("Total Double Hashing Collision from the start of the program: %d\n", totalDoubleCOllisions);
			break;

		case 8: { //Save the double hash table to file
		    saveToFile(out, doubleTable);
		    printf("Saved To File!!!\n");
		}
			break;

		case 9:  //exit the program
			exit(0);
			break;

		default:
			printf("Enter a number from 1-9\n");

		}

	}



    printf("Hello world!\n");
    return 0;
}
