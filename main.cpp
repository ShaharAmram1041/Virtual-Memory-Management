#include <iostream>
#include "sim_mem.h"

using namespace std;
char main_memory[MEMORY_SIZE];

int main(){
    //test - two processes. write AND read to/from swap.
    cout<<"=================== test ==================="<<endl;
    sim_mem mem_sm5((char*)"file1", (char*)"file2" , (char*)"swap_file" ,5, 5, 5, 5, 4, 5,2);

    mem_sm5.store(2, 7, 'X');  //bring 'BBXBB' to frame 0
    mem_sm5.load (1, 7);       //bring 'bbbbb' to frame 1
    mem_sm5.store(1, 15, 'X'); //bring 'X0000' to frame 2
    mem_sm5.load(1, 10);       //bring '00000' to frame 3

    //MEMORY IS FULL, frames 0, 2, 3 are dirty

    mem_sm5.load (1, 0);      //bring 'aaaaa' to frame 0. write 'BBXBB' to swap
    mem_sm5.load (2, 11);     //bring '00000' to frame 1. no write to swap
    mem_sm5.load (2, 0);      //bring 'AAAAA' to frame 2. write 'X0000' to swap

    mem_sm5.load(2, 7);       //bring 'BBXBB' to frame 3 **FROM SWAP**
    mem_sm5.store(1, 15, 'X'); //bring 'X0000' to frame 0 **FROM SWAP**

    mem_sm5.print_memory();
    mem_sm5.print_swap();
    mem_sm5.print_page_table();

    return 0;
}


