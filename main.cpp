#include<iostream>
#include<fstream>
#include<string>
using namespace std;
#define BLOCK_SIZE 4096
#define MAX_SIZE 50
char inputString[MAX_SIZE];

struct Student {
	char name[20]; 
	unsigned studentID;
	float score; 
	unsigned advisorID;
};/*
class Students {
public:
	Student stud;
	Students() { ; }
	Students(char* str) {
		assign_Student(&stud, str);
	}
	void Assign(char* str) {
		assign_Student(&stud, str);
	}

};*/
void assign_Student(Student *stud, char *str) {
	char* context;
	char* token = strtok_s(str, ",", &context);// ',' 찾아서 NULL로 변환
	strcpy_s(stud->name, token);
	token = strtok_s(NULL, ",", &context);
	stud->studentID = atoi(token);
	token = strtok_s(NULL, ",", &context);
	stud->score = atof(token);
	token = strtok_s(NULL, ",", &context);
	stud->advisorID = atoi(token);
	return;
}

int main() {
	//Students student;
	Student s1;
	string s;
	int N;
	
//	cout << sizeof(student) << endl;
	ifstream DB("input.txt");
	

	DB.getline(inputString, sizeof(char)*MAX_SIZE);
	N = atoi(inputString);
	cout << N;

	if (DB.is_open())cout << "opened"<<endl;
	for (int i = 0; i<N && !DB.eof(); i++) {
		DB.getline(inputString, sizeof(char)*MAX_SIZE);
		assign_Student(&s1,inputString);
		cout << s1.advisorID<<s1.name<<s1.score<<s1.studentID<<endl;

	}

	DB.close();

	system("pause");
	return 0;
}
