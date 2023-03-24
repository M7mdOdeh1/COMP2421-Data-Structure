#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct AVLnode *AVLNode;

typedef struct{
    char name[50];
    int creditHours;
    char code[30];
    char department[50];
    char topic[100][50];
    int noOfTopics;
}Course;

struct AVLnode
{
    Course Element;
    AVLNode Left;
    AVLNode Right;
    int Height;
};

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

int Height( AVLNode T )
{
    if( T == NULL )
        return -1;
    else
        return T->Height;
}

int Max( int left, int right )
{
    if(left > right)
        return left;
    else
        return right;
}

//rotate between T2 and its left child
AVLNode SingleRotateWithLeft( AVLNode T2 ){
    AVLNode  T1;

    T1 = T2->Left;
    T2->Left = T1->Right;
    T1->Right = T2;

    T2->Height = Max( Height( T2->Left ), Height( T2->Right ) ) + 1;
    T1->Height = Max( Height( T1->Left ), T2->Height ) + 1;

    return T1; //the new root
}

//Rotate between T1 and its right child
AVLNode SingleRotateWithRight(AVLNode T1) {
	AVLNode T2;

	T2 = T1->Right;
	T1->Right = T2->Left;
	T2->Left = T1;

	T1->Height = Max(Height(T1->Left), Height(T1->Right)) + 1;
	T2->Height = Max(Height(T2->Right), T1->Height) + 1;

	return T2; // the new root
}

//left-right double rotation
AVLNode DoubleRotateWithLeft(AVLNode T3) {
	// Rotate between T1 and T2
	T3->Left = SingleRotateWithRight(T3->Left);

	// Rotate between T3 and T2
	return SingleRotateWithLeft(T3);
}

//right-left double rotation
AVLNode DoubleRotateWithRight(AVLNode T1) {
	// Rotate between T3 and T2
	T1->Right = SingleRotateWithLeft(T1->Right);

	// Rotate between T1 and T2
	return SingleRotateWithRight(T1);
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

AVLNode Insert(Course course, AVLNode T){
    if( T==NULL )
    {                         //create node
        T = malloc( sizeof( struct AVLnode ) );
        if( T == NULL )
            printf( "Out of space!\n" );
        else
        {
           /* copying course info to the AVL node */
           copyCourse(&T->Element, course);


            T->Height = 0;
            T->Left = T->Right = NULL;
        }
    }
    else if( strcasecmp(course.code, T->Element.code) < 0 )
    {
        T->Left = Insert( course, T->Left );
        
    }
    else if( strcasecmp(course.code, T->Element.code) > 0 )
    {
        T->Right = Insert( course, T->Right );
    }
    else
        printf("%s is already exist\n", course.code);

    return T;
}

AVLNode readCoursesFile(AVLNode T) { //

	FILE *courses;
	courses = fopen("courses.txt", "r");
	if(courses == NULL)
        printf("File not Found\n");

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
        T = Insert(course, T); //add the course to the tree
        i++;
    }

    printf("File Loaded\n");
    fclose(courses);
    return T;
}

AVLNode readFromUser(AVLNode T){ //add new course from the user and insert it to the tree
    Course c;
    c.noOfTopics=0;
    char temp[300], *token;
    //read course info from the user
    printf("Enter Course name, credit hours, code, department(each separated by comma):\n");
   // scanf("%s%d%s%s", &c.name, &c.creditHours, &c.code, &c.department);
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
    printf("Course Added!!!\n");

    return Insert(c, T);

}

AVLNode findCourseByCode(char code[], AVLNode T){  //find a course with its code
    if(T != NULL){
        if(strcasecmp(code, T->Element.code) > 0) //go right
            findCourseByCode(code, T->Right);
        else if(strcasecmp(code, T->Element.code) < 0) //go left
            findCourseByCode(code, T->Left);
        else   //element found
            return T;
    }
    else
        return NULL;
}

AVLNode findMin(AVLNode T){
    if(T == NULL)
        return T;
     if(T->Left == NULL)
        return T;
    else
        return findMin(T->Left);
}

int getBalance(AVLNode T){ //find the balance of the node
    if(T==NULL)
        return 0;
    return Height(T->Left)-Height(T->Right);
}

AVLNode balanceNode(AVLNode T){ // check the balance of the node
    if (!T)
        return T;

    //right single rotation case
    int balance = getBalance(T);
    if (balance > 1 && getBalance(T->Left) >= 0)   /* rotateReight */
        return SingleRotateWithRight(T);

    // left single rotation Case
    if (balance < -1 && getBalance(T->Right) <= 0) /* rotateLeft */
        return SingleRotateWithLeft(T);

    //right left double rotation case
    if (balance > 1 && getBalance(T->Left) < 0)    /* rotateLeftReight */
    {

       return DoubleRotateWithLeft(T);
    }

    // right left double rotation Case
    if (balance < -1 && getBalance(T->Right) > 0)  /* rotateReightLeft */
    {
       return DoubleRotateWithRight(T);
    }
    return T;
}

AVLNode Delete(char code[], AVLNode T){ //delete a node from the tree them rearrange the tree by checking the balance and return the new root
    if(T == NULL){
       // printf("Course Not Found\n");
        return NULL;
    }
    if(strcasecmp(code, T->Element.code) > 0) //go right
        T->Right = Delete(code, T->Right);
    else if(strcasecmp(code, T->Element.code) < 0) //go left
        T->Left = Delete(code, T->Left);

    // the node to be deleted
    else if(T->Right !=NULL && T->Left != NULL){//if the node has two children

            AVLNode temp = findMin(T->Right);
            copyCourse(&T->Element, temp->Element);
            T->Right = Delete(T->Element.code, T->Right);
        }
    else{ //one or no child
        AVLNode temp = T;
        /*if there is one child then T point to it then delete temp
            and if there is no child the T will point to NUll then delete temp*/
        if(T->Right == NULL)
            T = T->Left;
        else if(T->Left == NULL)
            T = T->Right;

        free(temp);
    }


    //To handle if there was only one node
    if(T==NULL)
        return NULL;

    //update the Height of the node
    T->Height = 1 + Max(Height(T->Left), Height(T->Right));

    T = balanceNode(T);

    return T;
}

AVLNode updateCourse(AVLNode T, AVLNode root){ //update a course then return the new root
    char temp[100],*token, newCode[100];
    int isCodeChanged=0; //to know if the code changed to rearrange the tree

    if(root == NULL)
        return NULL;

    if(T != NULL){
        printf("Update the course info (take the field empty if you don't want to change it)\n");
        printf("Course name: ");
        scanf("%c");
        fgets(temp, sizeof temp, stdin);//read a whole line from stdin
        if(temp[0]!='\n') //check if the input is null and if it null the field will not change
            strcpy(T->Element.name, trim(temp));

        printf("Credit Hours: ");
        fgets(temp, sizeof temp, stdin);
        if(temp[0]!='\n')
            T->Element.creditHours = atoi(temp);

        printf("Course Code: ");
        fgets(temp, sizeof temp, stdin);
        if(temp[0]!='\n'){
            strcpy(newCode, temp);
            isCodeChanged =1;
        }

        printf("Department: ");
        fgets(temp, sizeof temp, stdin);
        if(temp[0]!='\n')
            strcpy(T->Element.department, temp);

            /*split topics and put it in array*/
        printf("Topics(separated by commas without spaces): ");
        fgets(temp, sizeof temp, stdin);
        if(temp[0]!='\n'){
            T->Element.noOfTopics=0;
            token = strtok(temp, ",");
            for(int i = 0; token != NULL; i++ ){
                strcpy(T->Element.topic[i], trim(token));
                T->Element.noOfTopics++;
                token = strtok(NULL, ",");
            }

        }
        if(isCodeChanged){ //check if code is changed to rearrange the tree
            Course c;
            copyCourse(&c, T->Element);
            strcpy(c.code, newCode);
            root = Delete(T->Element.code, root);  //delete the course
            root = Insert(c, root);                //then insert it again in the tree
        }
    }
    else
        printf("Course Not Found\n");

    return root;
}
//print all courses in lexicographic order that belong to the same department
void printSameDepartment(char department[], AVLNode T){
    if(T != NULL){
        printSameDepartment(department, T->Left);
        if(strcasecmp(T->Element.department, department)==0)
            printNode(T);
        printSameDepartment(department, T->Right);

    }
}

AVLNode deleteCourseStartWithChar(char ch[], AVLNode T, AVLNode root){ // delete all courses started with a specific character
    if(T != NULL){
        deleteCourseStartWithChar(ch, T->Left, root);  //go left
        deleteCourseStartWithChar(ch, T->Right, root);   //go right
        if(tolower(T->Element.code[0])== tolower(ch[0]))  //check if the course start with the character then delete it
            root = Delete(T->Element.code, root);
    }
    return root;
}

AVLNode deleteCourseBelongToDepartment(char department[], AVLNode T, AVLNode root){ // delete all courses that belong to a given department
    if(T != NULL){
        deleteCourseBelongToDepartment(department, T->Left, root);  //go left
        deleteCourseBelongToDepartment(department, T->Right, root);   //go right
        if(strcasecmp(T->Element.department, department)== 0)  //check if the course belong to the department
            root = Delete(T->Element.code, root);
    }
    return root;
}


void printNode(AVLNode T){ //print a single node
    if(T != NULL){
        //print course info
        printf("%c Course name:%s \t Credit Hours:%d \t Code:%s \t Department:%s    Topics:",254 , T->Element.name,
                T->Element.creditHours, T->Element.code, T->Element.department);
        for(int i=0; i<T->Element.noOfTopics; i++){  //print topics
            printf("%s", T->Element.topic[i]);
            if(i != T->Element.noOfTopics-1)
                printf(", ");
        }
        printf("\n");
    }
    else
        printf("Course not Found!!!\n");

}

//print Courses Info in lexicographic order according to course code
void printInOrder(AVLNode T){
    if(T != NULL){
        printInOrder(T->Left);
        printNode(T);
        printInOrder(T->Right);
    }

}

void saveToFile(FILE* out, AVLNode T){ //save all courses to a file in lexicographic order
    if(T != NULL){
        saveToFile(out, T->Left); //go left

        Course c ;
        copyCourse(&c, T->Element);
        fprintf(out, "%s:%d#%s#%s/", c.name, c.creditHours, c.code, c.department);
        for(int i=0; i<c.noOfTopics; i++){
            fprintf(out, "%s", c.topic[i]);
            if(i != c.noOfTopics-1)
                fprintf(out, ", ");
        }
        fprintf(out, "\n");

        saveToFile(out, T->Right); //go right
    }


}

int main() {
    FILE *out;
	AVLNode T = NULL;
	int menu;

	while (1) {
		printf("=============================== Menu =========================================\n");
		printf("1- Load courses file \t 2- Add a course \t 3- Find a course and upadate it  \n4- Print courses info in lexicographic order\t"
               "5- Print all topics associated with a course \n6- Print courses in lexicographic order of the same department\n7-Delete a Course\t"
               "8- Delete all courses start with a given letter \n9-Delete all courses belong to given department \n"
               "10- Save data to File \t 11- Exit the program\n");
		printf("==============================================================================\n");
		printf("Enter a number: ");

		scanf("%d", &menu);

		switch (menu) {
		case 1:  //read courses file
			T = readCoursesFile(T);
			break;
		case 2:  //add a course
		    printf("Add a Course:\n");
		    T = readFromUser(T);
			break;
		case 3: {   //find and update a course
		    char s[50];
		    printf("Update a Course:\n");
		    printf("Enter the course code you want to update:");
		    scanf("%s", s);
		    AVLNode P = findCourseByCode(s, T);
		    if(P != NULL ){
              printNode(P);
		      T = updateCourse(P,T);
		      printf("Course Updated!!!\n");
		    }
		    else
                printf("Course not Found!!!\n");

            }
		    break;

		case 4:  //print all course in order
		    printf("=============================== Courses =========================================\n");
		    if(T != NULL)
			  printInOrder(T);
            else
              printf("There is no data Loaded\n");
		    printf("=================================================================================\n");
			break;
		case 5:{          //print topics associated with a course
		    char s[50];
		    printf("Enter the course code:");
		    scanf("%c");
		    fgets(s, sizeof s, stdin);
            s[strlen(s)-1]= '\0'; // to make the last char null instead of \n (new line)

		    AVLNode P = findCourseByCode(s, T);
		    if(P!= NULL){
                printf("Topics: ");
		    for(int i =0; i < P->Element.noOfTopics; i++){
                printf("%s", P->Element.topic);
                if(i != P->Element.noOfTopics-1)
                   printf(", ");
		    }
		    printf("\n");
		    }
		    else printf("Not found\n");


		}
			break;

		case 6:{   //print all course belong to the same department
            char s[100];
		    printf("Enter the department:");
		    scanf("%c");
		    fgets(s, sizeof s, stdin);
		    s[strlen(s)-1]= '\0'; // to make the last char null instead of \n (new line)
            if(T != NULL)
                printSameDepartment(s, T);
            else
                printf("There is no data loaded\n");

		}

			break;

		case 7: {  //delete a course
		    char s[50];
		    printf("Enter the course code you want to DELETE:");
            scanf("%s", s);
		    AVLNode P = findCourseByCode(s, T);
		    if(P==NULL)
                printf("Course Not Found\n");
            else
		        T = Delete(s, T);

		}
			break;

		case 8: { //delete courses start with a specific letter
		    char ch[20];
		    printf("Enter a character:");
		    scanf("%s", ch);
		    T = deleteCourseStartWithChar(ch, T, T);

		}
			break;

		case 9:{  //delete all courses belong to a given department
		    char s[100];
		    printf("Enter the department:");
		    scanf("%c");
		    fgets(s, sizeof s, stdin);
		    s[strlen(s)-1]= '\0'; // to make the last char null instead of \n (new line)
		    T = deleteCourseBelongToDepartment(s, T, T);
		}
		    break;

		case 10:  //save all courses to a file
		    out= fopen("offered_courses.txt", "w");
		    saveToFile(out, T);
		    printf("Saved to File\n");
			break;

		case 11:  //exit the program
			exit(0);
			break;

		default:
			printf("Enter a number from 1-11\n");

		}

	}

	return 0;
}



