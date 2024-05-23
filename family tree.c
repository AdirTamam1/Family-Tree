// (89110, Spring 2023, Assignment #6, Adir Tamam)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Node Node;
typedef struct Person Person;
typedef struct PersonNode PersonNode;
#define EXIT_CODE '0'

// Structs

struct Node {
    PersonNode *data;
    Node *next;
};

struct Person {
    char *name;
    unsigned int age;
};

struct PersonNode {
    Person *details;

    PersonNode *firstParent, *secondParent, *mate;
    Node *children;
};

//Menu

void printMenu();

//1
char *getString(Node *headers);

void addHeaderHelper(Node *headers);

Person *newPerson(char *name, unsigned int age, Node *headers);

PersonNode *newPersonNode(Person *details, PersonNode *firstParent, PersonNode *secondParent, PersonNode *mate, Node *children, Node *headers);

Node *newNode(PersonNode *personNode, Node *headers);

void addToNode(Node *node, Node *newNode);

PersonNode *findPersonInFamily(const char *name, PersonNode *header);

PersonNode *searchByName(Node *headers, const char *name);

//2
void marriageHelper(Node *headers);

void makeMarriage(PersonNode *firstPerson, PersonNode *secondPerson);

int isValidMarriage(PersonNode *firstPerson, PersonNode *secondPerson, Node *headers);

int hasCommonAncestor(PersonNode *firstPerson, PersonNode *secondPerson, Node *headers);

//3

void newOffspringHelper(Node *headers);

//4

void printPersonAndMate(PersonNode *person);

void printOffspringsHelper(Node *headers);

void printFamily(PersonNode *personNode, int degree);
//5

void passYearsHelper(Node *headers);

int findInArray(PersonNode **array, PersonNode *personToFind, int *size);

void pushFamilyToArray(PersonNode *head, PersonNode ***array, int *size, Node *headers);

PersonNode **getAllPeople(Node *headers, int *size);

//6

void countPeople(Node *headers);

//7

void getAncesstors(PersonNode *ancestor, unsigned int degree, unsigned int reachedDegree, char ***offspringsNames, int *size, Node *headers);

void getOffspringNames(PersonNode *person, unsigned int degree, char ***offspringsNames, int *size, Node *headers);

void printCousins(Node *headers);

void sortArray(char **names, int size);

void printArray(char **names, int size);

// Deallocate

void deallocateHead(PersonNode *head);

void deallocateAll(Node *headers, int exitCode);


int main() {
    Node headers = {NULL, NULL};
    char choice;
    while (1) {
        printMenu();
        scanf("%c%*c", &choice);

        if (choice == EXIT_CODE) {
            deallocateAll(&headers, EXIT_SUCCESS);
        }
        if (choice == '1') {
            addHeaderHelper(&headers);
        } else if (choice == '2') {
            marriageHelper(&headers);
        } else if (choice == '3') {
            newOffspringHelper(&headers);
        } else if (choice == '4') {
            printOffspringsHelper(&headers);
        } else if (choice == '5') {
            passYearsHelper(&headers);
        } else if (choice == '6') {
            countPeople(&headers);
        } else if (choice == '7') {
            printCousins(&headers);
        } else
            printf("Invalid option\n");
    }
}

// Menu
void printMenu() {
    puts("Choose an option:");
    puts("0. Exit");
    puts("1. Add a head");
    puts("2. Marry two people");
    puts("3. New offspring");
    puts("4. Print family top-down");
    puts("5. Years pass");
    puts("6. Count people");
    puts("7. Print cousins");
}

//1

char *getString(Node *headers) {
    char *string = NULL;
    unsigned int i = 0;
    while (1) {
        string = (char *) realloc(string, sizeof(char) * (i + 1));

        if (string == NULL)
            deallocateAll(headers, EXIT_FAILURE);

        scanf("%c", string + i);
        if (string[i] == '\n') {
            string[i] = '\0';
            break;
        }
        i++;
    }
    return string;
}

void addHeaderHelper(Node *headers) {
    puts("Enter a name:");
    char *name = getString(headers);
    puts("Enter age:");
    unsigned int age;
    scanf("%u%*c", &age);
    // search if exist
    if (searchByName(headers, name)) {
        puts("The name is already taken");
        free(name);
        return;
    }
    // create new node
    Person *person = newPerson(name, age, headers);
    PersonNode *personNode = newPersonNode(person, NULL, NULL, NULL, NULL, headers);
    Node *node = newNode(personNode, headers);
    addToNode(headers, node);
}

Person *newPerson(char *name, unsigned int age, Node *headers) {
    Person *person = (Person *) malloc(sizeof(Person));

    if (person == NULL) {
        free(name);
        deallocateAll(headers, EXIT_FAILURE);
    }

    person->name = name;
    person->age = age;
    return person;
}

PersonNode *
newPersonNode(Person *details, PersonNode *firstParent, PersonNode *secondParent, PersonNode *mate, Node *children,
              Node *headers) {
    PersonNode *newPersonNode = (PersonNode *) malloc(sizeof(PersonNode));

    if (newPersonNode == NULL) {
        free(details->name);
        free(details);
        deallocateAll(headers, EXIT_FAILURE);
    }

    newPersonNode->details = details;
    newPersonNode->firstParent = firstParent;
    newPersonNode->secondParent = secondParent;
    newPersonNode->mate = mate;
    newPersonNode->children = children;
    return newPersonNode;
}


Node *newNode(PersonNode *personNode, Node *headers) {

    Node *person = (Node *) malloc(sizeof(Node));

    if (person == NULL) {
        free(personNode);
        deallocateAll(headers, EXIT_FAILURE);
    }

    person->data = personNode;
    person->next = NULL;
    return person;
}

void addToNode(Node *headers, Node *newNode) {
    Node *current = headers;
    if (headers->data == NULL) {
        headers->data = newNode->data;
        headers->next = NULL;
        free(newNode);
        return;
    }
    while (current->next)
        current = current->next;

    current->next = newNode;
}


PersonNode *findPersonInFamily(const char *name, PersonNode *header) {
    if (!header)
        return NULL;
    if (strcmp(name, header->details->name) == 0)
        return header;

    Node *childNode = header->children;

    while (childNode) {
        PersonNode *currentChild = childNode->data;
        PersonNode *familyMember = findPersonInFamily(name, currentChild);

        if (familyMember) {
            return familyMember;
        }
        childNode = childNode->next;
    }

    return NULL;
}

PersonNode *searchByName(Node *headers, const char *name) {
    if (headers->data == NULL)
        return NULL;
    Node *current = headers;
    while (current) {
        PersonNode *found = findPersonInFamily(name, current->data);
        if (found)
            return found;
        current = current->next;
    }
    return NULL;
}

//2

void marriageHelper(Node *headers) {
    puts("Enter the name of the first person:");
    char *firstPersonName = getString(headers);
    PersonNode *firstPerson = searchByName(headers, firstPersonName);
    free(firstPersonName);
    puts("Enter the name of the second person:");
    char *secondPersonName = getString(headers);
    PersonNode *secondPerson = searchByName(headers, secondPersonName);
    free(secondPersonName);
    // one of then not exist
    if (!firstPerson || !secondPerson) {
        puts("One of the persons does not exist");
        return;
    }
    // Invalid marriage
    if (!isValidMarriage(firstPerson, secondPerson, headers)) {
        puts("Invalid marriage");
        return;
    }
    makeMarriage(firstPerson, secondPerson);
    printf("%s and %s are now married\n", firstPerson->details->name, secondPerson->details->name);

}

void makeMarriage(PersonNode *firstPerson, PersonNode *secondPerson) {
    firstPerson->mate = secondPerson;
    secondPerson->mate = firstPerson;
}

int isValidMarriage(PersonNode *firstPerson, PersonNode *secondPerson, Node *headers) {
    if (firstPerson->details->age < 18 || secondPerson->details->age < 18)
        return 0;
    if (firstPerson->mate || secondPerson->mate)
        return 0;
    if (hasCommonAncestor(firstPerson, secondPerson, headers))
        return 0;
    return 1;

}

int hasCommonAncestor(PersonNode *firstPerson, PersonNode *secondPerson, Node *headers) {
    Node *current = headers;
    while (current) {
        if (findPersonInFamily(firstPerson->details->name, current->data) &&
            findPersonInFamily(secondPerson->details->name, current->data))
            return 1;
        current = current->next;
    }
    return 0;
}

//3
void newOffspringHelper(Node *headers) {
    puts("Enter the name of the first parent:");
    char *firstParentName = getString(headers);
    PersonNode *firstParent = searchByName(headers, firstParentName);
    free(firstParentName);
    puts("Enter the name of the second parent:");
    char *secondParentName = getString(headers);
    PersonNode *secondParent = searchByName(headers, secondParentName);
    free(secondParentName);
    puts("Enter offspring's name:");
    char *offspringName = getString(headers);
    if (!firstParent || !secondParent) {
        puts("One of the parents does not exist");
        free(offspringName);
        return;
    }
    if (firstParent->mate != secondParent) {
        puts("The parents are not married");
        free(offspringName);
        return;
    }
    if (searchByName(headers, offspringName)) {
        puts("The name is already taken");
        free(offspringName);
        return;
    }
    Person *babyPerson = newPerson(offspringName, 0, headers);
    PersonNode *babyPersonNode = newPersonNode(babyPerson, firstParent, secondParent, NULL, NULL, headers);
    Node *babyNode = newNode(babyPersonNode, headers);
    if (firstParent->children == NULL) {
        firstParent->children = babyNode;
        secondParent->children = babyNode;
    } else {
        addToNode(firstParent->children, babyNode);
        secondParent->children = firstParent->children;
    }

    printf("%s was born\n", offspringName);
}

//4

void printOffspringsHelper(Node *headers) {
    puts("Enter the name of the person:");
    char *name = getString(headers);
    PersonNode *person = searchByName(headers, name);
    free(name);
    if (!person) {
        puts("The person does not exist");
        return;
    }
    printFamily(person, 0);
}

void printPersonAndMate(PersonNode *person) {
    printf("%s (%u)", person->details->name, person->details->age);
    if (person->mate)
        printf(" - %s (%u)\n", person->mate->details->name, person->mate->details->age);
    else
        printf("\n");
}


void printFamily(PersonNode *personNode, int degree) {
    if (!personNode)
        return;
    for (int i = 0; i < degree; i++)
        printf("\t");

    printPersonAndMate(personNode);

    Node *childNode = personNode->children;
    while (childNode) {
        printFamily(childNode->data, degree + 1);
        childNode = childNode->next;
    }
}

//5

void passYearsHelper(Node *headers) {
    unsigned int years;
    puts("Enter number of years:");
    scanf("%u%*c", &years);
    if(!headers->data)
        return;
    int size = 0;
    PersonNode **allPeople = getAllPeople(headers, &size);
    for (int i = 0; i < size; i++)
        allPeople[i]->details->age += years;
    free(allPeople);

}

PersonNode **getAllPeople(Node *headers, int *size) {
    PersonNode **people = NULL;
    Node *current = headers;
    while (current) {
        pushFamilyToArray(current->data, &people, size, headers);
        current = current->next;
    }
    return people;
}


void pushFamilyToArray(PersonNode *head, PersonNode ***array, int *size, Node *headers) {

    if (!findInArray(*array, head, size)) {
        *array = (PersonNode **) realloc(*array, sizeof(PersonNode *) * (++(*size)));

        if (*array == NULL)
            deallocateAll(headers, EXIT_FAILURE);

        (*array)[*size - 1] = head;
    }
    Node *currentChild = head->children;
    while (currentChild) {
        pushFamilyToArray(currentChild->data, array, size, headers);
        currentChild = currentChild->next;
    }
}


int findInArray(PersonNode **array, PersonNode *personToFind, int *size) {
    if (array == NULL)
        return 0;
    for (int i = 0; i < *size; i++) {
        if (array[i] == personToFind)
            return 1;
    }
    return 0;

}

//6

void countPeople(Node *headers) {
    int size = 0;
    if (headers->data) {
        PersonNode **allPeople = getAllPeople(headers, &size);
        free(allPeople);
        if (size == 1)
            puts("There is one person");
        else
            printf("There are %d people\n", size);
    } else
        printf("There are %d people\n", size);
}

//7


void printCousins(Node *headers) {
    char **names = NULL;
    int size = 0;
    unsigned int degree;
    puts("Enter the name of the person:");
    char *name = getString(headers);
    puts("Enter degree:");
    scanf("%u%*c", &degree);
    PersonNode *person = searchByName(headers, name);
    free(name);

    if (!person) {
        puts("The person does not exist");
        return;
    } else {
        getAncesstors(person, degree, 0, &names, &size, headers);
        sortArray(names, size);
        puts("Cousins:");
        printArray(names, size);
        free(names);
    }
}


void getAncesstors(PersonNode *currentPerson, unsigned int degree, unsigned int reachedDegree, char ***offspringsNames,int *size, Node *headers) {

    if (!currentPerson->firstParent || degree == 0)
        getOffspringNames(currentPerson, reachedDegree, offspringsNames, size, headers);
    else {
        getAncesstors(currentPerson->firstParent, degree - 1, reachedDegree + 1, offspringsNames, size, headers);
        getAncesstors(currentPerson->secondParent, degree - 1, reachedDegree + 1, offspringsNames, size, headers);

    }
}

void getOffspringNames(PersonNode *person, unsigned int degree, char ***offspringsNames, int *size, Node *headers) {
    if (degree == 0) {
        *offspringsNames = (char **) realloc(*offspringsNames, sizeof(char *) * ((*size) + 1));
        (*size)++;

        if (*offspringsNames == NULL)
            deallocateAll(headers, EXIT_FAILURE);

        (*offspringsNames)[*size - 1] = person->details->name;
    } else {
        Node *childNode = person->children;

        while (childNode) {
            getOffspringNames(childNode->data, degree - 1, offspringsNames, size, headers);
            childNode = childNode->next;
        }
    }
}

void sortArray(char **names, int size) {
    char *tmp = NULL;

    for (int i = 0; i < size - 1; ++i)
        for (int j = 0; j < size - i - 1; ++j)
            if (strcmp(names[j], names[j + 1]) > 0) {
                tmp = names[j];
                names[j] = names[j + 1];
                names[j + 1] = tmp;
            }
}

void printArray(char **names, int size) {
    for (int i = 0; i < size; ++i) {
        if (i == size - 1 || strcmp(names[i], names[i + 1]))
            puts(names[i]);
    }

}

void deallocateHead(PersonNode *head) {

    // break the recursive call
    if (!head)
        return;

    // free Person
    if (head->details) {
        free(head->details->name);
        free(head->details);
    }

    if (head->mate) {
        head->mate->children = NULL;
        head->mate->mate = NULL;
    }

    Node *currentChild = head->children;

    while (currentChild) {
        Node *nextChild = currentChild->next;
        deallocateHead(currentChild->data);
        free(currentChild);
        currentChild = nextChild;
    }

    // Free the PersonNode structure
    free(head);
}

void deallocateAll(Node *headers, int exitCode) {
    Node *currentHead = headers;
    int skip = 0;
    while (currentHead) {
        Node *nextHead = currentHead->next;
        deallocateHead(currentHead->data);

        if (skip == 1)
            free(currentHead);

        skip = 1;
        currentHead = nextHead;
    }
    exit(exitCode);
}


