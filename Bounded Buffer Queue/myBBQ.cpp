//info
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <vector>

using namespace std;
const int MAX = 10;			//maximum size of the queue
const int NUM_THREADS = 10;		//maximum size of the producer and consumer thread counts
//const bool DEBUG = true;		//set debugging to on to show more data
const bool DEBUG = false;		//set debugging to off 
mutex mtx;		//a mutex for showing cout stuff seperate from the thread locks

class myBBQ {

	//synchronization variables
	mutex mtxObj; //mutual exclusion object that synchronizes access (a lock)
	condition_variable empty;//this CV makes thread wait during empty condition 
	condition_variable full;//this CV makes thread wait during full condition

	//state variables
	int Qitems[MAX];		//the simulated queue structure as an array
	int front;				//
	int tail;
	int itemCount;

public:
	myBBQ() { itemCount = 0; }//Constructor initializes queue to empty
	~myBBQ() {};
	void insert(int item, int threadnum, int tp);
	void remove(int* item, int threadnum, int tc);
	void printQueue();
	int getItem(int index) { return Qitems[index]; }
};

//print contents of the queue
void myBBQ::printQueue() {

	cout << "Queue contents: [";
	for (int i = 0; i < MAX; i++) {
		cout << " " << getItem(i);
	}
	cout << " ]" << endl;
}

//Directly from textbook 
//Insert an item, returns true/false depending on success fails if queue full 
void myBBQ::insert(int item, int thisThreadNumber, int tp) {
	
	unique_lock<mutex> lock(mtxObj);

	if (DEBUG) cout << "\nIn insert function working on item " << Qitems[itemCount];
	if (DEBUG) cout << " at position " << itemCount << " in the queue.\n";
	if (DEBUG) printQueue();

	while (itemCount == (MAX-1)) {
		cout << "Waiting to produce by thread (P" << thisThreadNumber << ")\n";
		full.wait(lock);
	}

	Qitems[itemCount] = item;//change to a q
	itemCount++;
	cout << "Item ID # " << item << " produced by (P" << thisThreadNumber << ")\n";
	empty.notify_one();
	lock.unlock();
}

//Remove an item, returns true/false depending on success fails if queue empty 
void myBBQ::remove(int* item, int thisThreadNumber, int tc) {
	
	unique_lock<mutex> lock(mtxObj);

	if (DEBUG) cout << "\nIn remove function working on item " << Qitems[itemCount];
	if (DEBUG) cout << " at position " << itemCount << " in the queue.\n";
	if (DEBUG) printQueue();

	while (itemCount == 0) {
		cout << "Waiting to consume by thread (C" << thisThreadNumber << ")\n"; 
		empty.wait(lock);
	}
	
	itemCount--;
	*item = Qitems[itemCount];
	cout << "Item ID # " << *item << " consumed by (C" << thisThreadNumber << ")\n";
	full.notify_one();
	lock.unlock();
}

//prints thread ID then produces forever
void producerFunc(myBBQ* Q, int thisThreadNumber, int tp, int item) {
	
	thread::id thisThreadID = this_thread::get_id();
	cout << "\nProducer thread (P" << thisThreadNumber;
	cout << ") has been created. ID is " << thisThreadID << ".\n";

	while (true) {
		this_thread::sleep_for(chrono::milliseconds(tp));//rand() % tp
		Q->insert(item, thisThreadNumber, tp);
	}
}

//prints thread ID then consumes forever
void consumerFunc(myBBQ* Q, int thisThreadNumber, int tc, int item) {

	thread::id thisThreadID = this_thread::get_id();
	cout << "\nConsumer thread (C" << thisThreadNumber;
	cout << ") has been created. ID is " << thisThreadID << ".\n";

	while (true) {
		this_thread::sleep_for(chrono::milliseconds(tc));//rand() % tc
		Q->remove(&item, thisThreadNumber, tc);
	}
}

//join all threads
void joinAllThreads(vector<thread>& ps, vector<thread>& cs) {
	//joins all threads from producers vector
	for (auto& th : ps) th.join();

	//joins all threads from consumers vector
	for (auto& th : cs) th.join();
}

//create queue, producer and consumer threads
//sleeping is used to keep cout statements in line
void runBBQ(vector<thread>& ps, vector<thread>& cs, int init_tp, int init_tc) {
	
	//create the queue
	myBBQ Q;
	
	//make some items
	int items[MAX];
	for (int i = 0; i <= MAX; i++){
		items[i] = i * 100;
	}

	int itemC = 0;

	//create 10 producer threads and create 10 consumer threads
	for (int i = 1; i <= NUM_THREADS; i++) { 
		if (itemC == MAX) {
			itemC = 0;
		}

		ps.push_back(thread(producerFunc, &Q, i, init_tp, items[itemC]));
		cs.push_back(thread(consumerFunc, &Q, i, init_tc, items[itemC]));
		itemC++;
	}

	while(true){}//create an infinite loop

	//join all threads within producer and consumer vectors
	joinAllThreads(ps, cs);//never runs

}

int main(int argc, char* argv[]) {

	//set initial timers
	int init_tp = atoi(argv[1]);//initial sleep time range limit for producing threads
	int init_tc = atoi(argv[2]);//initial sleep time range limit for consuming threads

	//display initial times
	cout << "Running main function...\nTP: " 
		<< init_tp << "    TC: " << init_tc << endl;
	
	//create random seed
	srand(time(0));

	//create vectors to hold threads
	vector<thread> producers;//a vector filled with producers //change to q?
	vector<thread> consumers;//a vector filled with consumers //change to q?

	//cout << init_tp << endl;
	//cout << (rand() % init_tp) << endl;
	
	//this_thread::sleep_for(chrono::seconds(rand() % init_tp));

	//run the bounded buffer queue
	runBBQ(producers, consumers, init_tp, init_tc);
	
	return 0;
}
