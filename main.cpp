#include<iostream>
#include<fstream>
#include<string>
using namespace std;
#define BLOCK_SIZE 4096
#define MAX_SIZE 50
#define BIN_ID_SIZE 27
char inputString[MAX_SIZE];

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
};

void tempHashFunc(int recID, int* bin_ID) {
	int remainder;
	for (int i = 0; i < BIN_ID_SIZE; i++) {
		if (recID <= 1) {
			bin_ID[i] =  recID;
			break;
		}
		remainder = recID % 2;
		bin_ID[i] = remainder;
		recID >> 1;
	}
	
	return;
}
int main() {
	Class_Student student;
	string s;
	int bin_ID[BIN_ID_SIZE];
	int N;

	ifstream DB("sampleData.csv");


	DB.getline(inputString, sizeof(char)*MAX_SIZE);
	N = atoi(inputString);
	cout << N<<endl;


	if (DB.is_open())cout << "opened" << endl;
	for (int i = 0; i<N && !DB.eof(); i++) {
		DB.getline(inputString, sizeof(char)*MAX_SIZE);
		student.Assign(inputString);
		cout << i <<" "<<student;

	}

	DB.close();

	system("pause");
	return 0;
}



ostream& operator<<(ostream& stream, const Class_Student& s1) {
	stream << s1.stud.studentID << " " << s1.stud.name << " " << s1.stud.score << " " << s1.stud.advisorID << endl;
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