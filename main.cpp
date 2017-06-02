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
#define NAME_SIZE 20
//ID number is composited of numbers of 9, and binary format of 999999999 is 30bit, and + NULL
#define BIN_ID_SIZE 31


void tempHashFunc(int recID, char* bin_ID);
struct Student {
	char name[NAME_SIZE];
	unsigned studentID;
	float score;
	unsigned advisorID;
};
struct hashPair {	//because two demention array evoke confusion
	int address = 0;
	int prefix = 0;	//hashprefix of a block
	int size = 0;
};
const int RECORD_SIZE = sizeof(Student);
const int BLOCKING_FACTOR = BLOCK_SIZE/RECORD_SIZE  ;

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
	void operator=(const Class_Student& s1) {
		stud.advisorID = s1.stud.advisorID;
		//strcpy_s(stud.name, NAME_SIZE, s1.stud.name);
		for (int i = 0; i < NAME_SIZE; i++) {
			stud.name[i]=s1.stud.name[i];
		}
		stud.score = s1.stud.score;
		stud.studentID = s1.stud.studentID;

	}
};

class HashMap {
private:
	
	struct hashPair *map;
	int maxprefix;	//biggest hashprefix among every block 
	int used_block;
	
public:
	HashMap();
	~HashMap();
	void insert(Class_Student& record, fstream& out);
	void extendMap();
	int ctrl_overflow(int, int, int, fstream& );
	void make_outfile(char*);
};

void HashMap::make_outfile(char* filename) {
	ofstream outfile(filename, ios::out | ios::binary | ios::trunc);
	int mapsize = pow(2, maxprefix);
	outfile.seekp(0,ios::beg);
	
	outfile.write((char*)&mapsize, sizeof(mapsize));
	outfile.write((char*)map, sizeof(hashPair)*mapsize);
	outfile.close();
}

int HashMap::ctrl_overflow(int old_addr,int new_addr,int prefix, fstream& out) {
	Class_Student block[BLOCKING_FACTOR], odd[BLOCKING_FACTOR], even[BLOCKING_FACTOR];
	int odd_cnt, even_cnt;
	odd_cnt = 0;
	even_cnt = 0;

	out.seekg(old_addr , ios::beg);
	out.read((char*)block, sizeof(block));
	

	int confirm_bit = 0;
	for (int i = 0; i < BLOCKING_FACTOR; i++) {
		confirm_bit = (block[i].getID() >> prefix) % 2;
		if ( confirm_bit == 0) {
			even[even_cnt++] = block[i];
		}
		else {
			odd[odd_cnt++] = block[i];
		}
	}
	out.seekp(old_addr, ios::beg);
	out.write((char*)even, BLOCK_SIZE);
	out.seekp(new_addr, ios::beg);
	out.write((char*)odd, BLOCK_SIZE);
	

	return even_cnt;

}
void HashMap::insert(Class_Student& record, fstream& out) {
	int hash_idx = 0;
	int num_of_even = 0;
	int old_addr = 0;
	int old_prefix = 0;
	int old_size=0;
	int new_size = 0; 
	int new_addr = 0;
	int save_addr = 0;
	char bin_ID[BIN_ID_SIZE];
	tempHashFunc(record.getID(), bin_ID);
	
	for (int i = 0; i < maxprefix; i++) {
		hash_idx += (bin_ID[i] - '0')*pow(2,maxprefix-i-1);
	}
	old_addr = map[hash_idx].address;
	old_prefix = map[hash_idx].prefix;
	old_size = map[hash_idx].size;

	
	if(old_size == BLOCKING_FACTOR) {
		new_addr = BLOCK_SIZE*used_block;
		num_of_even = ctrl_overflow( old_addr, new_addr, old_prefix, out);
		used_block++;

		if (maxprefix == old_prefix) {			//need extend Map.
			maxprefix++;
			extendMap();
										//after extendmap, update hash index.
			
			int temp_idx = hash_idx << 1;					//shift orign index, and add one more bit
			hash_idx = temp_idx + (bin_ID[maxprefix] - '0');//
			map[temp_idx].address = old_addr;
			map[temp_idx].prefix = old_prefix + 1;
			map[temp_idx].size = num_of_even;
			map[temp_idx + 1].address = new_addr;
			map[temp_idx + 1].prefix = old_prefix + 1;
			map[temp_idx + 1].size = BLOCKING_FACTOR - num_of_even;
			map[hash_idx].size += 1;							//because appended record
			new_size = map[hash_idx].size;
			save_addr = map[hash_idx].address;
			
		}
		else {					//if dont need to extend Map. 포인터가 중복해서 같은 블럭을 참조할 수 있음
			int start_idx = 0;
			int mid_idx = 0;
			int end_idx = 0;
			for (int i = 0; i < old_prefix; i++) {
				start_idx += (bin_ID[i] - '0')*pow(2, maxprefix - i - 1);
			}
			mid_idx = start_idx + pow(2, maxprefix - old_prefix - 1);
			end_idx = start_idx + pow(2, maxprefix - old_prefix);
			
			for (int i = start_idx; i < mid_idx; i++) {
				map[i].address = old_addr;
				map[i].prefix = old_prefix + 1;
				map[i].size = num_of_even;
			}
			for (int i = mid_idx; i < end_idx; i++) {
				map[i].address = new_addr;
				map[i].prefix = old_prefix + 1;
				map[i].size = BLOCKING_FACTOR - num_of_even;
			}

			if (bin_ID[old_prefix + 1] == '0') {
				new_size = num_of_even + 1;
				save_addr = old_addr;
				for (int i = start_idx; i < mid_idx; i++) {
					map[i].size += 1;
				}
			}
			else {
				save_addr = new_addr;
				new_size = BLOCKING_FACTOR - num_of_even + 1;
				for (int i = mid_idx; i < end_idx; i++) {
					map[i].size += 1;
				}
			}
		}
	}
	else {
		if (maxprefix == old_prefix) {
			map[hash_idx].size = old_size + 1;
		}
		else {
			int start_idx = 0;
			int end_idx = 0;
			for (int i = 0; i < old_prefix; i++) {
				start_idx += (bin_ID[i] - '0')*pow(2, maxprefix - i - 1);
			}
			end_idx = start_idx + pow(2, maxprefix - old_prefix);
			for (int i = start_idx; i < end_idx; i++) {
				map[i].size += 1;
			}
		}
		
		new_size = old_size + 1;
		save_addr = old_addr;
	}
	//
	out.seekp(save_addr + RECORD_SIZE*(new_size - 1), ios::beg);
	out.write((char*)&record, sizeof(record));
}
void HashMap::extendMap() {
	struct hashPair *newmap,*temp;
	int mapsize = pow(2, maxprefix);
	newmap = new hashPair[mapsize];

	for (int i = 0; i < mapsize; i++) {
			newmap[i].address = map[i / 2].address;
			newmap[i].prefix = map[i / 2].prefix;
			newmap[i].size = map[i / 2].size;	
	}

	temp = map;
	map = newmap;
	
	delete[] temp;
	return;
}

int main() {
	char inputString[MAX_SIZE];
	char bin_ID[BIN_ID_SIZE];
	char filename_hash[] = "Students.hash";
	HashMap hash_map;
	Class_Student student;
	int N;	//number of students 
	fstream out("Student.DB", ios::in | ios::out | ios::binary | ios::trunc);
	ifstream inputData("sampleData.csv");

	inputData.getline(inputString, sizeof(char)*MAX_SIZE);
	N = atoi(inputString);
	cout << N<<endl;


	if (inputData.is_open())cout << "opened" << endl;
	int test = 0;
	for (int i = 0; i<N && !inputData.eof(); i++) {
		inputData.getline(inputString, sizeof(char)*MAX_SIZE);
		student.Assign(inputString);
		tempHashFunc(student.getID(), bin_ID);
		hash_map.insert(student, out);
		cout << test++ << endl;
	}

	inputData.close();
	out.close();
	hash_map.make_outfile(filename_hash);


	///////////////////////

	//char filename_hash[] = "Students.hash";
	char filename_out[] = "Students_score.idx";
	hashPair *table;
	int num_of_idx = 0;
	ifstream infile(filename_hash, ios::in | ios::binary);
	//ofstream outfile(filename_out, ios::out | ios::binary | ios::trunc);
	infile.seekg(0, ios::beg);
	infile.read((char*)&num_of_idx, sizeof(num_of_idx));
	cout << num_of_idx << endl;
	table = new hashPair[num_of_idx];


	infile.read((char*)table, sizeof(hashPair)*num_of_idx);
	int sum = 0;
	for (int i = 0; i <num_of_idx; i++) {
		cout << table[i].address << " " << table[i].prefix << " " << table[i].size << endl;
		sum += table[i].size;
	}
	cout << sum << endl;

	infile.close();
	//outfile.close();
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
	for (int i = 0; i < NAME_SIZE-1; i++)stud->name[i] = token[i];	//if data have the name larger than 20, drop left.
	stud->name[NAME_SIZE - 1] = NULL;
	token = strtok_s(NULL, ",", &context);
	stud->studentID = atoi(token);
	token = strtok_s(NULL, ",", &context);
	stud->score = atof(token);
	token = strtok_s(NULL, ",", &context);
	stud->advisorID = atoi(token);
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
HashMap::HashMap() {
	maxprefix = 0;
	used_block = 1;
	map = new hashPair;
	map->address = 0;
	map->prefix = 0;
	map->size = 0;
}
HashMap::~HashMap() {
	
	delete[] map;
}
