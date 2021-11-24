#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;
const bool DEBUG = false;
const int MAX = 10; //maximum size of the queue

class TSQueue {//from textbook
	//synchronization variables
	mutex mtxObj; //mutual exclusion object that synchronizes access (a lock)


	//state variables
	int items[MAX];
	int front;
	int nextEmpty;

public:
	TSQueue();
	~TSQueue() { if (DEBUG) cout << "Destructor ran"; }
	bool tryInsert(int item);
	bool tryRemove(int *item);
};

//Constructor initializes queue variables to starting values 
TSQueue::TSQueue() {
	front = 0;				//starts at zero
	nextEmpty = 0;			//starts at zero
							//lock starts free
}

//Directly from textbook 
//Insert an item, returns true/false depending on success fails if queue full 
bool TSQueue::tryInsert(int item) {
	bool success = false;
	//unique_lock<mutex> mLock(m);
	mtxObj.lock();
	if ((nextEmpty - front) < MAX) {
		items[nextEmpty % MAX] = item;//won't need this
		nextEmpty++;
		success = true;
	}
	mtxObj.unlock();
	return success;
}

//Directly from textbook 
//Remove an item, returns true/false depending on success fails if queue empty 
bool TSQueue::tryRemove(int item) {
	bool success = false;
	//unique_lock<mutex> mLock(m);
	mtxObj.lock();
	if ((nextEmpty - front) < MAX) {
		*item = items[front % MAX];//won't need this
		front++;
		success = true;
	}
	mtxObj.unlock();
	return success;
}

int main(int argc, char *argv[]) {
	//argc is number of argv's
	//argv[0] is filename
	//argv[1..x] are the inputs
	//cout << "lets go " << argc << argv[1] << argv[2] << argv[3];
	cout << "lets go " << argc << argv[1] << argv[2] << argv[3];


}

//--semephores--
//lock (m(mutex))
//slotsEmpty (how many empty slots in buffer)(start = 10)
//slotsFull (how many full slots in buffer)(start = 0)

//--producer--
//while true(run forever?)
	//wait until buffer has empty slot(empty > 0) then decrement empty
	//acquire lock
	//add data to buffer	
	//release lock
	//increment full

//--consumer--
//while true(run forever?)
	//wait until buffer has full slot(full > 0) then decrement full
	//acquire lock
	//remove data from buffer	
	//release lock
	//increment empty
