/*	File Struct Project 2 - dinamic hash and B+tree
	this project started 2017.05.19
	last update 2017.05.25 
	in progress
*/

#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
using namespace std;
#define BLOCK_SIZE 4096
#define MAX_SIZE 50
//ID number is composited of numbers of 9, and binary format of 999999999 is 30bit, and + NULL
#define BIN_ID_SIZE 31

void tempHashFunc(int recID, char* bin_ID);
struct Student {
	char name[20];
	unsigned studentID;
	float score;
	unsigned advisorID;
};

class Class_Student {
private:
	friend void assign_Student(Student *, char *);
	friend ostream& operator<<(ostream& , const Class_Student& );
	Student stud;
public:
	Class_Student() { }	//default constructor
	
	//constructor and assign method for input a record 
	Class_Student(char* str) { assign_Student(&stud, str);}
	void Assign(char* str) { assign_Student(&stud, str);}

	int getID() { return stud.studentID; }
};
class HashMap {
private:
	struct Pair {	//because two demention array evoke confusion
		int address;
		int prefix;	//hashprefix of a block
	};
	Pair *map;
	int mapsize;	//2^maxprefix (it is not nessesary maybe..)
	int maxprefix;	//biggest hashprefix among every block 
	
public:
	HashMap():mapsize(0),maxprefix(0){}
	~HashMap() { if(!mapsize)delete map; }
	void insert(int);
};
void HashMap::insert(int recID) {
	int mid_address = 0;
	char bin_ID[BIN_ID_SIZE];
	tempHashFunc(recID, bin_ID);
	for (int i = 0; i < maxprefix; i++) {
		mid_address += (bin_ID[i]-'0')*pow(2,i);
	}
	/*	now seek the real adrress by using file pointer.
		TODO
		overflow control
		some work , if maxprefix > pair.prefix 
	*/
}

int main() {
	char inputString[MAX_SIZE];
	char bin_ID[BIN_ID_SIZE];
	Class_Student student;
	int N;	//number of students 

	ifstream DB("sampleData.csv");
	

	DB.getline(inputString, sizeof(char)*MAX_SIZE);
	N = atoi(inputString);
	cout << N<<endl;


	if (DB.is_open())cout << "opened" << endl;
	for (int i = 0; i<N && !DB.eof(); i++) {
		DB.getline(inputString, sizeof(char)*MAX_SIZE);
		student.Assign(inputString);
		tempHashFunc(student.getID(), bin_ID);
		cout << i <<" "<<student<<" "<<bin_ID<<endl;
	}

	DB.close();

	system("pause");
	return 0;
}



ostream& operator<<(ostream& stream, const Class_Student& s1) {
	stream << s1.stud.studentID << " " << s1.stud.name << " " << s1.stud.score << " " << s1.stud.advisorID;
	return stream;
}

void assign_Student(Student *stud, char *str) {		//input record to struct Student 
	char* context;
	char* token = strtok_s(str, ",", &context);// ',' 찾아서 NULL로 변환
	strcpy_s(stud->name, sizeof(char)*MAX_SIZE, token);
	token = strtok_s(NULL, ",", &context);
	stud->studentID = atoi(token);
	token = strtok_s(NULL, ",", &context);
	stud->score = atof(token);
	token = strtok_s(NULL, ",", &context);
	stud->advisorID = atoi(token);
	return;
}

void tempHashFunc(int recID, char* bin_ID) {
	int remainder;									//convert (int)studentID to binary format
	bin_ID[BIN_ID_SIZE - 1] = NULL;					//least significant bit is bin_ID[0]
	for (int i = 0; i < BIN_ID_SIZE - 1; i++) {		//if 24 -> 11000(2) -> "00011 + 000..."
		bin_ID[i] = '0';
	}
	for (int i = 0; i < BIN_ID_SIZE - 1; i++) {
		if (recID <= 1) {
			bin_ID[i] = recID + '0';
			break;
		}
		remainder = recID % 2;
		bin_ID[i] = remainder + '0';
		recID = recID >> 1;
	}

	return;
}