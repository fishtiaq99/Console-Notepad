#include <iostream>
#include <Windows.h>
#include <fstream>
using namespace std;

const int MAIN_AREA_WIDTH = 100;
const int MAIN_AREA_HEIGHT = 20;
const int SUGGESTIONS_HEIGHT = 10;
const int SEARCH_WIDTH = 20;

void gotoxy(int x, int y) {
    COORD c = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
struct Variables {
    bool running = true;
    int NumOfLinePointers = 0;
    const int max = 5;
}variables;

class Node {
public:
    char character;
    Node* left, * right, * up, * down;
    int positionX, positionY;
    bool backSlashN;
    Node(char ch) : character(ch), left(nullptr), right(nullptr), up(nullptr), down(nullptr), backSlashN(false) {}
};

class PointersNode {
public:
    PointersNode* up;
    Node* holdline;
    PointersNode* down;
}HeadPointers, TailPointers;

class undoImplementation {
public:
    static int count;
    int x;
    int y;
    undoImplementation* next;

    undoImplementation(int xx, int yy) {
        x = xx;
        y = yy;
        count++;
    }
    void addLetter(int xx, int yy) {
        if (undoHead == nullptr) {
            undoHead = new undoImplementation(xx, yy);
            return;
        }
        if (count >= variables.max) {
            undoImplementation* temp = undoHead;
            while (temp->next && temp->next->next) {
                temp = temp->next;
            }
            delete temp->next;
            temp->next = nullptr;
            count--;
        }
        undoImplementation* newNode = new undoImplementation(xx, yy);
        newNode->next = undoHead;
        undoHead = newNode;
    }
}*undoHead;


int undoImplementation::count = 0;

class redoImplementation {
public:
    static int count;
    int x;
    int y;
    Node* holdword;
    redoImplementation* next;

    redoImplementation(int xx, int yy) {
        count++;
        x = xx;
        y = yy;
    }
    redoImplementation* getPointer(int xx, int yy) {
        if (redoHead == nullptr) {
            redoHead = new redoImplementation(xx, yy);
            return redoHead;
        }
        redoImplementation* temp = redoHead;
        if (count >= variables.max) {
            count--;
            while (temp->next->next) {
                temp = temp->next;
            }
            temp->next = nullptr;
        }
        redoImplementation* newNode = new redoImplementation(xx, yy);
        newNode->next = redoHead;
        redoHead = newNode;
        return redoHead;
    }
    void addRedoWord(Node* currnode, int xx, int yy) {
        redoImplementation* temp = getPointer(xx, yy);
        temp->holdword = currnode;
    }

}*redoHead;

int redoImplementation::count = 0;

class Notepad {
private:
    PointersNode* head;
    PointersNode* tail;
    Node* cursor;
    int cursorX, cursorY;

public:
    Notepad() {
        head = &HeadPointers;
        tail = &TailPointers;
        cursor = head->holdline;
        cursorX = 0;
        cursorY = 0;
        variables.NumOfLinePointers++;
    }
    void display() {

        system("CLS");
        formNodeConnections();
        drawLayout();
        //cout << "\033[1;31m" << "Highlighted Text" << "\033[0m" << endl;
        PointersNode* currentLine = head;
        PointersNode* currentLineEnd = tail;
        Node* currentNode = head->holdline;
        int lineNumber = 0;
        int characterNumber = 0;
        while (currentLine) {
            while (currentNode) {
                if (currentNode->left) {
                    if (currentNode->left->right == currentLineEnd->holdline) {
                        gotoxy(characterNumber, lineNumber);
                        if (currentNode->character == '*') { cout << " "; }
                        else { cout << currentNode->character; }
                        currentNode = currentNode->right;
                        break;
                    }
                }
                gotoxy(characterNumber, lineNumber);
                if (currentNode->character == '*') {
                    cursorX = 0;
                    //gotoxy(cursorX, cursorY);
                    cout << " ";
                }
                else {
                    cout << currentNode->character;
                }
                currentNode = currentNode->right;
                characterNumber++;
            }
            currentLine = currentLine->down;
            currentLineEnd = currentLineEnd->down;
            lineNumber++;
            characterNumber = 0;
        }
        if (cursor->backSlashN) {
            cursorX = 0;
        }
        gotoxy(cursorX, cursorY);
        formNodeConnections();
    }
    void formNodeConnections() {
        if (head->down == nullptr) { return; }
        PointersNode* upperline = head;
        PointersNode* upperLineEnd = tail;
        PointersNode* lowerline = head->down;
        PointersNode* lowerLineEnd = tail->down;
        Node* currentUpperNode = upperline->holdline;
        Node* currentLowerNode = lowerline->holdline;
        while (lowerline) {
            currentUpperNode = upperline->holdline;
            currentLowerNode = lowerline->holdline;
            int x = 600;
            while (x--) {
                if (currentUpperNode && currentLowerNode) {
                    currentUpperNode->down = currentLowerNode;
                    currentLowerNode->up = currentUpperNode;
                }
                if ((currentUpperNode == upperLineEnd->holdline) || (currentLowerNode == lowerLineEnd->holdline)) { break; }
                if (currentUpperNode && currentLowerNode) {
                    currentUpperNode = currentUpperNode->right;
                    currentLowerNode = currentLowerNode->right;
                }
            }
            upperline = upperline->down;
            lowerline = lowerline->down;
            upperLineEnd = upperLineEnd->down;
            lowerLineEnd = lowerLineEnd->down;
        }
    }
    bool ccheck = false;
    void fixAlignment() {
        Node* holdcursor = cursor;
        PointersNode* tempHead = head->down;
        PointersNode* tempTail = tail->down;
        Node* tempNode = head->holdline;
        int count = 0;
        while (tempHead) {
            tempHead->holdline = nullptr;
            tempHead = tempHead->down;
        }
        while (tempTail) {
            tempTail->holdline = nullptr;
            tempTail = tempTail->down;
        }
        tempHead = head;
        tempTail = tail;
        while (tempNode) {
            tempNode->down = nullptr;
            tempNode->up = nullptr;
            if (tempNode->backSlashN) {
                count = 0;
                if (tempHead) {
                    tempHead->holdline = tempNode;
                    tempHead = tempHead->down;
                    if (tempNode->left) {
                        tempTail->holdline = tempNode->left;
                        tempTail = tempTail->down;
                    }
                }
                else {
                    PointersNode* newHead = new PointersNode();
                    PointersNode* newTail = new PointersNode();
                    tempHead = head;
                    tempTail = tail;
                    while (tempHead->down) {
                        tempHead = tempHead->down;
                    }
                    while (tempTail->down) {
                        tempTail = tempTail->down;
                    }
                    tempHead->down = newHead;
                    newHead->up = tempHead;
                    tempTail->down = newTail;
                    newTail->up = tempTail;
                    tempHead = newHead;
                    tempTail->holdline = tempNode->left;
                    tempTail = tempTail->down;
                    tempHead->holdline = tempNode;
                }
            }
            else if (count >= MAIN_AREA_WIDTH - 1) {
                count = 0;
                if (tempTail)
                    tempTail->holdline = tempNode;
                while ((tempTail->holdline->character != ' ')) {
                    tempTail->holdline = tempTail->holdline->left;
                }
                tempNode = tempTail->holdline->right;
                tempTail = tempTail->down;
                if (tempNode) {
                    if (tempHead) {
                        tempHead->holdline = tempNode;
                        tempHead = tempHead->down;
                    }
                    else {
                        PointersNode* newHead = new PointersNode();
                        PointersNode* newTail = new PointersNode();
                        tempHead = head;
                        tempTail = tail;
                        while (tempHead->down) {
                            tempHead = tempHead->down;
                        }
                        while (tempTail->down) {
                            tempTail = tempTail->down;
                        }
                        tempHead->down = newHead;
                        newHead->up = tempHead;
                        tempTail->down = newTail;
                        newTail->up = tempTail;

                        tempHead = newHead;
                        tempTail->holdline = tempNode->left;
                        tempTail = tempTail->down;;
                        tempHead->holdline = tempNode;
                    }
                }
                cursorX = cursor->positionX + 1;
            }
            count++;
            if (tempNode) {
                tempNode = tempNode->right;
            }
            if (cursor)
                cursorX = cursor->positionX + 1;
        }
        tempTail = tail;
        while (tempTail->down) {
            tempTail = tempTail->down;
        }
        tempNode = head->holdline;
        while (tempNode->right) {
            tempNode = tempNode->right;
        }
        tempTail->holdline = tempNode;
        count = 0;
        formNodeConnections();
        fixNumbering();
        countLines();
    }
    void fixNumbering() {
        Node* tempNode = head->holdline;
        PointersNode* tempHead = head;
        PointersNode* tempTail = tail;
        int x = 0;
        int y = 0;
        while (tempNode) {

            tempNode->positionX = x++;
            tempNode->positionY = y;

            if (tempNode && tempTail)
                if (tempNode == tempTail->holdline) {
                    tempHead = tempHead->down;
                    tempTail = tempTail->down;
                    x = 0;
                    y++;
                }
            tempNode = tempNode->right;
        }
    }
    void countLines() {
        int count = 0;
        PointersNode* tempHead = head;
        while (tempHead) {
            if (tempHead->holdline) {
                count++;
            }
            tempHead = tempHead->down;
        }
        variables.NumOfLinePointers = count;
    }
    void undo() {
        if (undoHead == nullptr) { return; }
        int x = undoHead->x;
        int y = undoHead->y;
        undoImplementation* deletenode = undoHead;
        undoHead = undoHead->next;
        undoHead->count--;
        Node* TempNode = head->holdline;
        Node* start = nullptr;

        while ((TempNode->right != nullptr)) {
            TempNode = TempNode->right;
            if ((TempNode->right->positionX == x) && (TempNode->right->positionY == y)) {
                break;
            }
        }
        start = TempNode;
        TempNode = TempNode->right;
        while ((TempNode->character != ' ') && (TempNode->backSlashN != true)) {
            TempNode = TempNode->right;
        }
        start = start->left;
        Node* passOn = start->right;

        start->right->left = nullptr;
        start->right = TempNode;
        start->right->left->right = nullptr;
        TempNode->left = start;
        redoHead->addRedoWord(passOn, start->positionX, start->positionY);
        start = nullptr;
        TempNode = nullptr;
        countLines();
        fixAlignment();
        formNodeConnections();
        fixNumbering();
        display();
    }

    void redo() {
        if (redoHead == nullptr) { return; }
        int x = redoHead->x;
        int y = redoHead->y;

        Node* Redoword = redoHead->holdword;

        Node* endRedoword = redoHead->holdword;
        while (endRedoword->right) {
            endRedoword = endRedoword->right;
        }
        redoHead = redoHead->next;
        redoHead->count--;
        Node* TempNode = head->holdline;
        int a = 0;
        while ((TempNode->positionX != x) || (TempNode->positionY != y)) {
            a++;
            TempNode = TempNode->right;
        }
        Node* tempnode2 = TempNode->right;
        TempNode->right = Redoword;
        Redoword->left = TempNode;
        tempnode2->left = endRedoword;
        endRedoword->right = tempnode2;
        TempNode = TempNode->right->right;

        countLines();
        fixAlignment();
        formNodeConnections();
        fixNumbering();
        undoHead->addLetter(TempNode->positionX, TempNode->positionY);
        display();
    }

    void insertCharacter(char ch) {
        fixAlignment();
        formNodeConnections();
        countLines();
        PointersNode* temphead = head;
        PointersNode* temptail = tail;
        for (int i = 0; i < cursorY; i++) {
            temphead = temphead->down;
            temptail = temptail->down;
        }
        if ((ch == '\n')) {
            if (variables.NumOfLinePointers >= MAIN_AREA_HEIGHT - 1) { return; }
            if (cursor->right) {
                if (cursorX == 0 && cursorY == 0) {
                    Node* newNode = new Node('*');
                    PointersNode* newHead = new PointersNode();
                    newHead->up = temphead;
                    PointersNode* newTail = new PointersNode();
                    newTail->up = temptail;
                    newNode->backSlashN = true;
                    newNode->right = head->holdline;
                    newNode->right->left = newNode;
                    head->holdline = newNode;
                    cursor = head->holdline;
                    cursorX++;
                    fixAlignment();
                }
                if (cursorX == 0) {
                    Node* newNode = new Node('*');
                    newNode->backSlashN = true;
                    cursor->left->right = newNode;
                    newNode->left = cursor->left;
                    newNode->right = cursor;
                    cursor->left = newNode;
                    cursor = cursor->left;
                }
                else if (cursor->right->backSlashN == false) {
                    cursor->right->backSlashN = true;
                    cursor = cursor->right;
                }
                else {
                    Node* newNode = new Node('*');
                    newNode->backSlashN = true;
                    cursor->right->left = newNode;
                    newNode->right = cursor->right;
                    newNode->left = cursor;
                    cursor->right = newNode;
                    cursor = cursor->right;
                }
            }
            else if (1) {
                PointersNode* newHead = new PointersNode();
                newHead->up = temphead;
                PointersNode* newTail = new PointersNode();
                newTail->up = temptail;
                if (temphead->down) {
                    newHead->down = temphead->down;
                    temphead->down = newHead;
                    newHead->down->up = newHead;

                    newTail->down = temptail->down;
                    temptail->down = newTail;
                }
                else {
                    temphead->down = newHead;
                    newHead->up = temphead;

                    temptail->down = newTail;
                    newTail->up = temptail;
                }
                Node* newNode = new Node('*');
                newNode->backSlashN = true;
                newHead->holdline = newNode;
                newTail->holdline = newNode;
                cursor->right = newNode;
                cursor->right->left = cursor;
                cursor = newNode;
                newNode->positionX = 0;
                newNode->positionY = cursorY + 1;
            }
            cursorX = 0;
            cursorY++;
            fixAlignment();
        }
        else if (1) {
            if (cursor && (cursor->character == '*')) {
                if (!(cursor == temphead->holdline)) { return; }
                cursor->backSlashN = true;
                cursor->character = ch;
                cursorX = cursorX++;
            }
            else if (cursorX == 0 && cursorY == 0) {
                Node* newNode = new Node(ch);
                newNode->backSlashN = true;
                newNode->right = cursor;
                cursor->left = newNode;
                head->holdline = newNode;
                cursor->backSlashN = false;
                cursor = cursor->left;
                cursor->backSlashN = true;
                cursorX++;
            }
            else if (cursorX == 0) {
                Node* newNode = new Node(ch);
                newNode->backSlashN = true;
                cursor->backSlashN = false;
                newNode->right = cursor;
                newNode->left = cursor->left;
                cursor->left = newNode;
                cursor->left->left->right = newNode;
                cursor = cursor->left;
                cursorX++;
            }
            else if (cursor && (cursorX > cursor->positionX)) {
                Node* newNode = new Node(ch);
                newNode->positionX = cursorX;
                newNode->positionY = cursorY;
                Node* tempCursor = cursor->right;
                if (tempCursor) {
                    cursor->right = newNode;
                    newNode->left = cursor;
                    newNode->right = tempCursor;
                    tempCursor->left = newNode;
                }
                newNode->left = cursor;
                cursor->right = newNode;
                cursor = newNode;
                temptail->holdline = newNode;
                cursorX++;

            }
            if (cursorX == MAIN_AREA_WIDTH) {
                cursorX = 0;
                cursorY++;
            }
        }
        cursorX = 10;
        gotoxy(cursorX, cursorY);
        countLines();
        fixAlignment();
        formNodeConnections();
        fixNumbering();
        if ((cursor->left == nullptr) || ((cursor->left) && (cursor->left->character == ' ') && (cursor->character != ' ')) || (cursor->backSlashN && cursor->character != '*')) {
            undoHead->addLetter(cursor->positionX, cursor->positionY);
        }
        display();
    }
    void deleteCharacter() {
        ccheck = true;
        if (cursorX == 0 && cursorY == 0 && (cursor->right == nullptr)) {
            cursor->character = '*';
        }
        else if (cursorX == 1 && cursor->right) {
            Node* toDelete = cursor;
            if (toDelete->right) {
                toDelete->left->right = toDelete->right;
                toDelete->right->left = toDelete->left;
                cursor = cursor->left;
                cursor->right->backSlashN = true;
            }
        }
        else if (cursorX == 0 && cursorY == 0) {
            return;
        }
        else if ((cursor->positionX == 0 && cursor->character == '*') || (cursor->character != '*')) {
            Node* toDelete = cursor;
            if (toDelete->right) {
                toDelete->left->right = toDelete->right;
                toDelete->right->left = toDelete->left;
                cursor = cursor->left;
            }
            else {
                cursor = cursor->left;
                cursor->right = nullptr;
                PointersNode* tempTail = tail;
                while (tempTail->down) {
                    tempTail = tempTail->down;
                }
                tempTail->holdline = cursor;
            }
            toDelete->right = nullptr;
            toDelete->left = nullptr;
            delete toDelete;
        }
        else if (cursor->positionX == 0 && cursor->character != '*') {
            cursor->backSlashN = false;
        }
        countLines();
        fixAlignment();
        formNodeConnections();
        fixNumbering();
        display();
        if (cursorY > 0) {
            cursorY = cursor->positionY;
            gotoxy(cursorX, cursorY);
        }
    }
    void moveUp() {
        if (cursor && cursor->up && cursorY > 0) {
            cursor = cursor->up;
            cursorY = cursor->positionY;
            cursorX = cursor->positionX + 1;
            gotoxy(cursorX, cursorY);
            return;
        }
        if (cursorY > 0) {
            PointersNode* temphead = head;
            PointersNode* temptail = tail;

            for (int i = 0; i < cursorY - 1; i++) {
                temphead = temphead->down;
                temptail = temptail->down;
            }
            Node* currentNode = temphead->holdline;
            while (currentNode != temptail->holdline) {
                currentNode = currentNode->right;
            }
            cursor = currentNode;
            cursorX = cursor->positionX + 1;
            cursorY = cursor->positionY;
            gotoxy(cursorX, cursorY);

        }
    }
    void moveDown() {
        if (cursor && cursor->down) {
            cursor = cursor->down;
            cursorX = cursor->positionX + 1;
            cursorY = cursor->positionY;
            gotoxy(cursorX, cursorY);
            return;
        }
        else if (cursorY < MAIN_AREA_HEIGHT - 1) {
            PointersNode* temphead = head;
            PointersNode* temptail = tail;
            for (int i = 0; i < cursorY; i++) {
                temphead = temphead->down;
                temptail = temptail->down;
            }
            if (!temphead->down) { return; }
            temphead = temphead->down;
            temptail = temptail->down;
            Node* currentNode = temphead->holdline;
            while (currentNode && (currentNode != temptail->holdline)) {
                currentNode = currentNode->right;
            }
            cursor = currentNode;
            if (cursor) {
                cursorX = cursor->positionX + 1;
                cursorY = cursor->positionY;
                gotoxy(cursorX, cursorY);
            }

        }
    }
    void moveLeft() {
        if (cursorX == 1) {
            cursorX = 0;
            gotoxy(cursorX, cursorY);
        }
        else if (cursor && cursor->left) {
            cursor = cursor->left;
            cursorX = cursor->positionX + 1;
            cursorY = cursor->positionY;
            gotoxy(cursorX, cursorY);
        }
    }

    void moveRight() {
        if (cursor && cursor->right) {
            if (cursor->left) {
                if ((cursor->left->positionY != cursor->positionY) && (cursor->positionY != cursor->right->positionY) && cursorX == 0) {
                    cursorX = 1;
                    gotoxy(cursorX, cursorY);
                    return;
                }
            }
            if (cursor->positionY != cursor->right->positionY) {
                cursor = cursor->right;
                cursorX = 0;
                cursorY = cursor->positionY;
                gotoxy(cursorX, cursorY);
                return;
            }
            if (cursorX == 0) {
                cursorX++;
                gotoxy(cursorX, cursorY);
                return;
            }
            cursor = cursor->right;
            cursorX = cursor->positionX + 1;
            cursorY = cursor->positionY;
            gotoxy(cursorX, cursorY);
        }
    }
    void drawLayout() {
        for (int i = 0; i < MAIN_AREA_WIDTH; i += 1) {
            gotoxy(i, MAIN_AREA_HEIGHT);
            cout << "-";
        }
        for (int i = MAIN_AREA_HEIGHT; i < MAIN_AREA_HEIGHT + SUGGESTIONS_HEIGHT; i += 1) {
            gotoxy(MAIN_AREA_WIDTH, i);
            cout << "|";
        }
        for (int i = 0; i < MAIN_AREA_HEIGHT + SUGGESTIONS_HEIGHT; i += 1) {
            gotoxy(MAIN_AREA_WIDTH, i);
            cout << "|";
        }
    }
    void saveToFile(const string& filename) {
        ofstream outFile(filename);
        cout << "\nFile saved successfully.\n";
        PointersNode* currentLine = head;
        PointersNode* currentLineEnd = tail;
        Node* currentNode = head->holdline;

        while (currentLine) {
            while (1) {
                if (currentNode && currentNode->left) {
                    if (currentNode->left->right == currentLineEnd->holdline) {
                        if (currentNode->character == '*') {
                        }
                        else {
                            outFile << currentNode->character;
                        }
                        currentNode = currentNode->right;
                        outFile << '\n';
                        break;
                    }
                }
                if (currentNode && (currentNode->character == '*')) {
                    outFile << " ";
                }
                else {
                    if (currentNode) {
                        outFile << currentNode->character;
                    }
                }
                currentNode = currentNode->right;
            }
            currentLine = currentLine->down;
            currentLineEnd = currentLineEnd->down;
        }
        outFile.close();
    }
    void loadFromFile(const string& filename) {
        system("cls");
        ifstream inFile(filename);
        if (!inFile) {
            ofstream newFile(filename);
            newFile.close();
        }
        else {
            head->holdline = new Node('*');
            cursor = head->holdline;
            char ch;
            while (inFile.get(ch)) {
                insertCharacter(ch);
            }
            inFile.close();
        }
    }
    void createNewFile() {
        string filename;
        cout << "enter the name of the file you want to create:";
        cin >> filename;
        ofstream newFile(filename);
        newFile.close();
    }
    void menu() {
        int choice;
        string filename;

        do {
            cout << endl << "Menu:" << endl;
            cout << "1. Create New File" << endl;
            cout << "2. Load Existing File" << endl;
            cout << "3. Exit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            switch (choice) {
            case 1:
                createNewFile();
                head->holdline = new Node('*');
                cursor = head->holdline;
                cursorX = 0;
                cursorY = 0;
                system("cls");
                drawLayout();
                return;
                break;
            case 2:
                cout << "Enter the file name to load: ";
                cin >> filename;
                loadFromFile(filename);
                system("cls");
                drawLayout();
                return;
                break;
            case 3:
                variables.running = false;
                return;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
            }
        } while (choice != 3);
    }
};
int main() {
    Notepad notepad;
    notepad.menu();
    notepad.drawLayout();
    HANDLE rhnd = GetStdHandle(STD_INPUT_HANDLE);
    DWORD Events = 0, EventsRead = 0;
    while (variables.running) {
        string filename;
        GetNumberOfConsoleInputEvents(rhnd, &Events);
        if (Events != 0) {
            INPUT_RECORD eventBuffer[200];
            ReadConsoleInput(rhnd, eventBuffer, Events, &EventsRead);
            for (DWORD i = 0; i < EventsRead; i += 1) {
                if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown) {
                    switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode) {
                    case VK_UP:
                        notepad.moveUp();
                        break;
                    case VK_DOWN:
                        notepad.moveDown();
                        break;
                    case VK_LEFT:
                        notepad.moveLeft();
                        break;
                    case VK_RIGHT:
                        notepad.moveRight();
                        break;
                    case VK_BACK:
                        notepad.deleteCharacter();
                        break;
                    case VK_ESCAPE:
                        notepad.display();
                        system("cls");
                        cout << "Do you want to save the current file before exiting? (y/n): ";
                        char saveBeforeExit;
                        cin >> saveBeforeExit;
                        if (saveBeforeExit == 'y' || saveBeforeExit == 'Y') {
                            cout << "Enter the file name to save: ";
                            cin >> filename;
                            notepad.saveToFile(filename);
                        }
                        variables.running = false;
                        break;
                    case VK_RETURN:
                        notepad.insertCharacter('\n');
                        break;
                    case 'Z':
                        if (eventBuffer[i].Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED ||
                            eventBuffer[i].Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED) {
                            notepad.undo();
                            break;
                        }
                    case 'Y':
                        if (eventBuffer[i].Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED ||
                            eventBuffer[i].Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED) {
                            notepad.redo();
                            break;
                        }
                    default:
                        char ch = eventBuffer[i].Event.KeyEvent.uChar.AsciiChar;
                        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch == ' ')) {
                            notepad.insertCharacter(ch);
                        }
                        notepad.formNodeConnections();
                        notepad.display();
                        break;
                    }
                }
            }
        }
    }
    return 0;
}