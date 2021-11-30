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
	int itemCount;			//the number of active slots in the queue
	int timesWaitToProd;  //the number of times that the queue hit 100% capacity
	int timesWaitToCons;  //the number of times that the queue hit 0% capacity
	int tpGlobal;
	int tcGlobal;

public:
	//Constructor initializes queue to empty
	myBBQ() { itemCount = 0; timesWaitToCons = 0; timesWaitToProd = 0; }
	~myBBQ() {};
	void insert(int item, int threadnum);
	void remove(int* item, int threadnum);
	void printQueueAndCounters();
	void printQueue();
	int getItem(int index) { return Qitems[index]; }
	void setItem(int index, int val) { Qitems[index] = val; }
	void initializeQAndTimes(int tpi, int tci);
	int getItemCountOfQ() { return itemCount; }
};

//initialize slots in queue to starting values
void myBBQ::initializeQAndTimes(int tpi, int tci) {
	for (int i = 0; i < MAX; i++) {
		setItem(i, -1);
	}
	tpGlobal = tpi;
	tcGlobal = tci;
}

//print contents of the queue
void myBBQ::printQueue() {

	cout << "Queue contents: [";
	for (int i = 0; i < MAX; i++) {
		cout << " " << getItem(i);
	}
	cout << " ] " << endl;
}

//print contents of the queue and number of waiting times
void myBBQ::printQueueAndCounters() {

	cout << "Queue contents: [";
	for (int i = 0; i < MAX; i++) {
		cout << " " << getItem(i);
	}
	cout << " ] \n#times waited to produce//consume: " << timesWaitToProd;
	cout << "//" << timesWaitToCons << endl;
}

//Directly from textbook 
//Insert an item, returns true/false depending on success fails if queue full 
void myBBQ::insert(int item, int thisThreadNumber) {
	
	unique_lock<mutex> lock(mtxObj);

	if (DEBUG) cout << "\nIn insert function working on item " << Qitems[itemCount];
	if (DEBUG) cout << " at position " << itemCount << " in the queue.\n";
	if (DEBUG) printQueueAndCounters();

	while (itemCount == MAX) {
		cout << "Waiting to produce by thread (P" << thisThreadNumber << ")\n";
		timesWaitToProd++;
		printQueueAndCounters();
		full.wait(lock);
	}

	Qitems[itemCount] = item;
	itemCount++;
	cout << "Item ID # " << item << " produced by (P" << thisThreadNumber << ")\n";
	printQueue();
	
	cout << "\ntp: " << tpGlobal << "   ";
	if (itemCount <= (.25 * (MAX - 1)) && tpGlobal > 1) {
		tpGlobal = tpGlobal - 1;
		cout << " (low) changed to " << tpGlobal << endl;
	}
	else if (itemCount >= (.75 * (MAX - 1)) && tpGlobal > 1) {
		tpGlobal = tpGlobal + 1;
		cout << " (high) changed to " << tpGlobal << endl;
	}
	
	this_thread::sleep_for(chrono::milliseconds(rand() % tpGlobal));
	
	empty.notify_one();
	lock.unlock();
}

//Remove an item, returns true/false depending on success fails if queue empty 
void myBBQ::remove(int* item, int thisThreadNumber) {
	
	unique_lock<mutex> lock(mtxObj);

	if (DEBUG) cout << "\nIn remove function working on item " << Qitems[itemCount];
	if (DEBUG) cout << " at position " << itemCount << " in the queue.\n";
	if (DEBUG) printQueueAndCounters();

	while (itemCount == 0) {
		cout << "Waiting to consume by thread (C" << thisThreadNumber << ")\n"; 
		timesWaitToCons++;
		printQueueAndCounters();
		empty.wait(lock);
	}
	
	itemCount--;
	*item = Qitems[itemCount];
	Qitems[itemCount] = -1;
	cout << "Item ID # " << *item << " consumed by (C" << thisThreadNumber << ")\n";
	printQueue();

	cout << "\ntp: " << tpGlobal << "   ";
	if (itemCount <= (.25 * (MAX - 1)) && tpGlobal > 1) {
		tpGlobal = tpGlobal - 1;
		cout << " (low) changed to " << tpGlobal << endl;
	}
	else if (itemCount >= (.75 * (MAX - 1)) && tpGlobal > 1) {
		tpGlobal = tpGlobal + 1;
		cout << " (high) changed to " << tpGlobal << endl;
	}

	this_thread::sleep_for(chrono::milliseconds(rand() % tcGlobal));

	full.notify_one();
	lock.unlock();
}

//prints thread ID then produces forever
void producerFunc(myBBQ* Q, int thisThreadNumber, int item) {

	thread::id thisThreadID = this_thread::get_id();
	cout << "\nProducer thread (P" << thisThreadNumber;
	cout << ") has been created. ID is " << thisThreadID << "...";
	this_thread::sleep_for(chrono::seconds(1));

	while (true) {
		Q->insert(item, thisThreadNumber);
	}
}

//prints thread ID then consumes forever
void consumerFunc(myBBQ* Q, int thisThreadNumber, int item) {
	
	thread::id thisThreadID = this_thread::get_id();
	cout << "\nConsumer thread (C" << thisThreadNumber;
	cout << ") has been created. ID is " << thisThreadID << "...";
	this_thread::sleep_for(chrono::seconds(1));//wait

	while (true) {
		Q->remove(&item, thisThreadNumber);
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
void runBBQ(vector<thread>& ps, vector<thread>& cs, int init_tp, int init_tc) {
	
	//create the queue
	myBBQ Q;
	
	//initialize queue to null values [ -1, -1, ..., -1 ]
	Q.initializeQAndTimes(init_tp, init_tc);

	//print starting values
	Q.printQueueAndCounters();
	this_thread::sleep_for(chrono::seconds(5));//wait 5 seconds to view
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

		ps.push_back(thread(producerFunc, &Q, i, items[itemC]));
		cs.push_back(thread(consumerFunc, &Q, i, items[itemC]));
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
	vector<thread> producers;//a vector filled with producers
	vector<thread> consumers;//a vector filled with consumers

	//run the bounded buffer queue
	runBBQ(producers, consumers, init_tp, init_tc);
	
	return 0;
}
