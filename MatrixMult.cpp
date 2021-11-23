//Eric Smrkovsky--9/30/2021
//CSci 114--practice2--Q4.5
#include <iostream>
#include <thread>

using namespace std;

//Make two 4x4 global 2d arrays like Fig 4.19
const int rows = 4;
const int cols = 4;
bool debug = false;

//Make a two dimensional array (Matrix A)
int a[rows][cols] = {{0, 1, 2, 3} 
			 , {4, 5, 6, 7}
			 , {8, 9, 10, 11}
			 , {12, 13, 14, 15}};
	
//Make a two dimensional array (Matrix B)
int b[rows][cols] = {{0, 1, 2, 3} 
			 , {4, 5, 6, 7}
			 , {8, 9, 10, 11}
			 , {12, 13, 14, 15}};

//Make a empty two dimensional array (Matrix C) 
int c[rows][cols];  //For output	

//Takes in a function as input and a bool. 
//Set 'bool debug' on line 9 to true for debugging 	
void multiplyAddAndPrint(int colToMul, int debug){
	int i_loc = rows;	//from global var for number of rows in array	
	int j_loc = cols;	//from global var for number of columns in array
	int total = 0;		//accumulator adds products after multiplication
	
	//outer for traverses columns of Array A  	
	for(int i = 0; i < i_loc; i++){	
		//inner for traverses rows of the of Array B in column 'i' 
		for(int j = 0; j < j_loc; j++){
			total += (a[colToMul][j] * b[j][i]);//multiplyA(i,k)B(k,j) fromQ4.5
			
			//simply debugging to show values being multiplied
			if(debug){
				cout << ":" << a[colToMul][j] << "x" << b[j][i];
				cout << " = " << total << endl;
			}
		}	
		c[colToMul][i] = total;//Assign dotproduct in C(i,j) from Q4.5 in text
		total = 0;	//Set accumulator to 0 for next iteration of outer for loop
	}
	
	//Display row of the dot product matrix C 
	cout << endl << c[colToMul][0] << "  " << c[colToMul][1];
	cout << "  " << c[colToMul][2] << "  " << c[colToMul][3] << endl;
	
}//end function


//Main function simply runs a for loop, executes 4 threads and populates
//a 2d array with dot products 
int main()  
{	
	//New Thread will start just after the creation of new object on line 63
	//and will execute the passed callback in parallel to thread 
	//that has started it. Passes function to find dot products for answer 
	//array and the column for the thread to work on. 
	for(int col = 0; col < cols; col++){
		thread threadObj(multiplyAddAndPrint, col, debug);
		threadObj.join();//wait for thread to exit 
	}
    return 0;
}