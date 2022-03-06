/*	Eric Smrkovsky 
	A Bounded Buffer Queue
	A Producer Consumer Problem
	A multi-threaded program
	CSCI 114
	CSU Fresno
	Fall 2021

	In this project I implemented a bounded buffer queue(BBQ).
	DEMO of program execution: 

	Using a c++11 compiler.
	Run the program in a Cygwin terminal with the following command:
			* g++ -std=c++11 myBBQ.cpp -o BBQ ; ./BBQ.exe TP TC

	The program will run forever until the user stops the program
	by pressing ctrl-c

	user also can press q-enter to stop the program and display data needed
	for graphs in the report.


	I use locks and conditional variables to synchronize access of the queue. 
	20 threads are created with 10 producers and 10 consumers.

	/////////////IMPORTANT/////////////
	//////The size of the queue MAX is a global variable that can be changed 
	//////at the time of testing. 
	
	//////You can change the program to debug mode by 
	//////changing the DEBUG global variable to true. 
	
	//////You can change the program to execute in milliseconds rather 
	//////than seconds by changing MILLI to true 

	//////You can set the program by changing 

	This program displays the following:
		•	Thread creation with thread ID number
		•	Item ID # produced by thread number #
		•	Item ID # consumed by thread number #
		•	Waiting to produce by thread number #
		•	Waiting to consume by thread number #

	-- TP represents the initial sleep time range limit for producing threads
	-- TC represents the initial sleep time range limit for consuming threads.
	
	This program has the following constraints:
	Producers control their speed of producing by changing the sleeping time 
	range between two consecutive produce operations. 
	Consumers sleeping time range remains constant throughout program execution 
	but consumers stop consuming when the queue is empty.
	The sleep time between calls to producers and consumers is a random value
	within a range (0, T).
	Producers dynamically change speed of "producing" by gradually slowing down
	producing when the queue is over 75% occupancy and stops producing when
	queue is 100% full. 
	Producers also gradually accelerate the producing when the queue is below
	25% occupancy and reaches twice the initial sleeping time range when 
	buffer is empty.
*/

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <vector>

using namespace std;

const int MAX = 75;			//maximum size of the queue
const int NUM_THREADS = 10;		//maximum size of the producer and consumer thread counts
const bool DEBUG = true;		//set debugging to on to show more data
//const bool DEBUG = false;		//set debugging to off 
const bool MILLI = true;		//set time to milliseconds
//const bool MILLI = false;		//set time to seconds
//bool FIFTEEN_SECONDS_STOP = true;	//set program to stop after 15 seconds and display data
bool FIFTEEN_SECONDS_STOP = false;	//set program to run indefinitly or when user stops
mutex mtx;		//a mutex for showing cout stuff seperate from the thread locks

class myBBQ {

	//synchronization variables
	mutex mtxObj; //mutual exclusion object that synchronizes access (a lock)
	condition_variable empty;//this CV makes thread wait during empty condition 
	condition_variable full;//this CV makes thread wait during full condition

	//state variables
	int Qitems[MAX];		//the simulated queue structure as an array
	int front;				
	int tail;
	int itemCount;		//the number of active slots in the queue
	int timesWaitToProd;//the number of times that the queue hit 100% capacity
	int timesWaitToCons;//the number of times that the queue hit 0% capacity
	int tpGlobal;		//tp value that is altered throughout the program
	int tcGlobal;		//tc value that is altered throughout the program
	int tpInit;			//initial tp given by user during program execution
	int tcInit;			//initial tc given by user during program execution

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
	int getNumOfTimesEmpty() { return timesWaitToCons; }
	int getNumOfTimesFull() { return timesWaitToProd; }
};

//initialize slots in queue to starting values and tp/tc values 
void myBBQ::initializeQAndTimes(int tpi, int tci) {
	for (int i = 0; i < MAX; i++)
		setItem(i, -1);
	
	tpInit = tpi;
	tcInit = tci;
	tpGlobal = tpi;
	tcGlobal = tci;

}

//print contents of the queue
void myBBQ::printQueue() {

	cout << "Queue contents: [";
	for (int i = 0; i < MAX; i++) 
		cout << " " << getItem(i);
	
	cout << " ]\n\n";
}

//print contents of the queue and number of waiting times
void myBBQ::printQueueAndCounters() {

	cout << "Queue contents: [";
	for (int i = 0; i < MAX; i++) 
		cout << " " << getItem(i);
	
	cout << " ] \n#times waited to produce//consume: " << timesWaitToProd;
	cout << "//" << timesWaitToCons << endl << endl;
}

//Insert an item into the queue while following buffer timing restraints  
void myBBQ::insert(int item, int thisThreadNumber) {
	
	//acquire the lock
	unique_lock<mutex> lock(mtxObj);

	//debugging
	if (itemCount > 0) {
		if (DEBUG) cout << "\nIn insert function working on item " << Qitems[itemCount - 1];
		if (DEBUG) cout << " at position " << itemCount - 1 << " in the queue.\n";
		if (DEBUG) printQueueAndCounters();
	}
	else {
		if (DEBUG) cout << "\nIn remove function with an empty queue\n";
		if (DEBUG) printQueueAndCounters();
	}

	if(DEBUG) cout << "tp: " << tpGlobal << "   ";//only insert
	//tp reaches twice the initial sleeping time range when buffer is empty
	if (itemCount == 0) {
		tpGlobal = (tpInit / 2);
		if (DEBUG) cout << " (Qempty) changed to " << tpGlobal;
		if (DEBUG) cout << "  ItemCount: " << itemCount << endl;
	}
	//gradually accelerate producing when the queue is below 25% occupancy
	else if (itemCount <= (.25 * MAX) && tpGlobal > 1) {
		tpGlobal = tpGlobal - 1;
		if (DEBUG) cout << " (low) changed to " << tpGlobal;
		if (DEBUG) cout << "  ItemCount: " << itemCount << endl;
	}
	//gradually slow down producing when the queue is over 75% occupancy
	else if (itemCount >= (.75 * MAX) && tpGlobal >= 1) {
		tpGlobal = tpGlobal + 1;
		if (DEBUG) cout << " (high) changed to " << tpGlobal;
		if (DEBUG) cout << "  ItemCount: " << itemCount << endl;
	}

	//stops producing when	queue is 100% full
	while (itemCount == MAX) {
		cout << "Waiting to produce by thread (P" << thisThreadNumber << ")\n";
		timesWaitToProd++;
		if (DEBUG) printQueueAndCounters();
		full.wait(lock);//set conditional variable to wait because queue is full
	}

	Qitems[itemCount] = item; //put item in queue
	itemCount++;			  //need to increase item counter

	//debugging
	if (DEBUG) {
		cout << "Item ID # " << item << " produced by (P";
		cout << thisThreadNumber << ")\n";
		printQueue();
	}
	
	//set conditional variable to reflect that this 
	//thread is now available for execution
	empty.notify_one();
	
	//free the lock
	lock.unlock();
	
	//The sleep time between calls to producers is a random value
	//within a range(0, tp).
	if(MILLI)//decide on seconds or milliseconds at top of program
		this_thread::sleep_for(chrono::milliseconds(rand() % tpGlobal));
	else
		this_thread::sleep_for(chrono::seconds(rand() % tpGlobal));
}

//Remove an item from the queue while following buffer timing restraints  
void myBBQ::remove(int* item, int thisThreadNumber) {
	
	//acquire the lock
	unique_lock<mutex> lock(mtxObj);

	//debugging
	if (itemCount > 0) {
		if (DEBUG) cout << "\nIn remove function working on item " << Qitems[itemCount-1];
		if (DEBUG) cout << " at position " << itemCount - 1 << " in the queue.\n";
		if (DEBUG) printQueueAndCounters();
	}
	else {
		if (DEBUG) cout << "\nIn remove function with an empty queue\n";
		if (DEBUG) printQueueAndCounters();
	}

	//stops consuming when queue is empty
	while (itemCount == 0) {
		cout << "Waiting to consume by thread (C" << thisThreadNumber << ")\n"; 
		timesWaitToCons++;
		if (DEBUG) printQueueAndCounters();
		empty.wait(lock);//set conditional variable to wait because queue is empty
	}
	
	itemCount--; //decrease counter cause we are removing an item from queue
	*item = Qitems[itemCount];//remove item from queue (doesn't go anywhere)
	Qitems[itemCount] = -1;//reset queue slot to -1 
	cout << "Item ID # " << *item << " consumed by (C" << thisThreadNumber << ")\n";
	if (DEBUG) printQueue();

	//set conditional variable to reflect that this 
	//thread is now available for execution
	full.notify_one();

	//free the lock
	lock.unlock();

	//The sleep time between calls to consumers is a random value
	//within a range(0, tc).
	if (MILLI)//decide on seconds or milliseconds at top of program
		this_thread::sleep_for(chrono::milliseconds(rand() % tcGlobal));
	else
		this_thread::sleep_for(chrono::seconds(rand() % tcGlobal));
}

//producer function prints thread ID then produces forever
void producerFunc(myBBQ* Q, int thisThreadNumber, int item) {

	//get the thread ID
	thread::id thisThreadID = this_thread::get_id();

	//display thread creation
	cout << "\nProducer thread (P" << thisThreadNumber;
	cout << ") has been created. ID is " << thisThreadID << "...";
	
	//wait long enough for all creations to print
	this_thread::sleep_for(chrono::milliseconds(50));

	//constantly try to insert from the queue until program termination by user
	while (true) {
		Q->insert(item, thisThreadNumber);
	}
}

//consumer function prints thread ID then consumes forever
void consumerFunc(myBBQ* Q, int thisThreadNumber, int item) {
	
	//get the thread ID
	thread::id thisThreadID = this_thread::get_id();
	
	//display thread creation
	cout << "\nConsumer thread (C" << thisThreadNumber;
	cout << ") has been created. ID is " << thisThreadID << "...";
	
	//wait long enough for all creations to print
	this_thread::sleep_for(chrono::milliseconds(50));

	//constantly try to remove from the queue until program termination by user
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
	this_thread::sleep_for(chrono::seconds(1));//wait 1 second to view
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
		//push producer and consumer threads into appropriate vectors
		ps.push_back(thread(producerFunc, &Q, i, items[itemC]));
		cs.push_back(thread(consumerFunc, &Q, i, items[itemC]));
		itemC++;
	}

	//stop program and display data after one minute 
	if (FIFTEEN_SECONDS_STOP) {
		this_thread::sleep_for(chrono::seconds(15));
		cout << "\n\n\nRESULTS AFTER 15 SECONDS \n\n\n";
		cout << "Inital tp/tc: " << init_tp << "/" << init_tc << endl;
		cout << "Times full/empty: " << Q.getNumOfTimesFull() << "/";
		cout << Q.getNumOfTimesEmpty() << endl;
		exit(0);
	}

	char key;

	//while loop will terminate if user presses q otherwise it is
	//an infinite loop
	while(true){
		//cin >> key;
		//switch (key)
		//{
		//	//press 'q' to terminate program and print data
		//case 'q':
		//	cout << "\n\n\nRESULTS \n\n\n";
		//	cout << "Inital tp/tc: " << init_tp << "/" << init_tc << endl;
		//	cout << "Times empty/full: " << Q.getNumOfTimesEmpty() << "/";
		//	cout << Q.getNumOfTimesFull() << endl;
		//	exit(0);
		//	break;
		//}
	}

	//join all threads within producer and consumer vectors
	joinAllThreads(ps, cs);//never runs

}

//main program execution
int main(int argc, char* argv[]) {

	//set initial timers
	int init_tp = atoi(argv[1]);//initial sleep time range limit for producing threads
	int init_tc = atoi(argv[2]);//initial sleep time range limit for consuming threads

	//display initial times
	if (DEBUG) {
		cout << "Running main function...\nTP: ";
		cout << init_tp << "    TC: " << init_tc << endl;
	}

	//create random seed
	srand(time(0));

	//create vectors to hold threads
	vector<thread> producers;//a vector filled with producers
	vector<thread> consumers;//a vector filled with consumers

	//run the bounded buffer queue
	runBBQ(producers, consumers, init_tp, init_tc);
	
	return 0;
}
