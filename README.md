![‏‏צילום מסך (4)](https://user-images.githubusercontent.com/126160304/221202753-560e6b6b-e751-46c8-8088-7ab46a400c71.png)
# Virtual-Memory-Management

Authored by Shahar Amram
<br>

### ***This program is a simulation of CPU accesses memory. Using the paging mechanism that allows programs to be run when only part of it is in memory. The program memory is divided into pages which are loaded into memory as needed.***




### ==Description==<br>
program contain 4 files:

(notice: we need to provide to the program 1 or 2 files that representing the processes, without them the program will not work).<br>


***main.cpp***:

The main program.
in the main we use the constractor, and create object type of 'sim_mem'.
include the 'sim_mem.h', running the program with all the functions.
we simulation memory program that do store and load to specific process, and specific address, than store or return the value in that address. include activate the page table, and the swap file if we need.
at the end, printing the memory,swap file, and the page-table.<br>


***sim_mem.h***:

the file contain the class 'sim_mem', with all its attributes.
In addition, the file contain the struct 'page descriptor' that represting the page table that save all the information of the pages.
in the end the file conatin the function that we used them in the 'sim_mem.cpp' file.<br>


***sim_mem.cpp***:

The contents of the work.
the file contain all the functions that we used them to the the work.
activate the constactor of the object, opening the file descriptors,create/open swap file, allocate memory in page table, and initialize the main memory to be in '0'.
the initialize of the page_table(V = 0,P = 0/1(0 in text zone),D = 0,frame = -1, swapindex = -1).<br>

Two main functions:<br>

* store - get address,number of process and char.in this function we want to store the char that we get in the addrres, that mean we take the page of the address and put it in the main memory, and there store the char. in the function we check if the page is already in main memory or found in swap or in the file process itself.notice: if we try to get to text zone by the given address we do nothing and error output.

* load()- get address, process id. in this function we want to return the char that found in this specific address. we check where the page is found. if it already in the main memory or swap or in the process file.
in the function we do global check where is the page and what the area of the address is.<br>

in both functions we activate all the attributes of the page table(V,D,frame,swapindex).
In addition, some more functions in the file: the destractor -free all the allocates memories that were made, and closing the open files, print memory-printing the main memory,print swap-printing the swap file,print page table-printing the page table.<br>


***swap_file.txt***:

file creating during the program. (if already exists, activate)
saving the pages that we clear from the main memory.<br>



### ==Program DATABASE==

1.del_mem = type of int, variable saving the index of the main memory that we want to swapout(by using the theory of FIFO).<br>
2.swapfile_fd = int,save the file descriptor of the swap file.<br>
3.program_fd[2] = saving the file descriptor of the given files.<br>
4.struct page_descriptor = struct visualizes the page table with all the attributes.<br>
5.page_table = pointer to the page table we allocate during the program.<br>
6.main memory = array size of 200, visualizes the RAM.<br>
7.file_name1[],file_name2[] = input of files that visualizes the processes in the simulation.<br>


### Functions:
1.store - function type void, get address,process id, and char. store the char in the given address.<br>
2.load - function type char, get address, and process id. returning the char that found in the address.<br>
3.open - built in function opening file for read\write.<br>
4.write - built in function open file descriptor to write.<br>
5.read - built in function read file descriptor to buffer by size of numbers.<br>
6.lseek - built in function places the cursor of the file descriptor.  <br>
7.fullMem - private function clear the first process in the main memory to swap, or if its type of text page delete.<br>
8.freeFrame - private function find the first free index in the main memory.<br>
9.memset - built in function initalize array in wanting char,by size of numbers.<br>
10.print_memory - function printing the main memory.<br>
11.print_swap - function printing the swap file.<br>
12.print_page_table - function printing the page table.<br>
13.~destractor - function free the allocate memory, closing the opening files.<br>



### ==Program Files==<br>

* main.cpp - the file contain the main, using all the functions of the class.<br>
* sim_mem.h - the header file, contain all the function that we use, and the attributes of the class plus the struct page descriptor with all of his attributes.<br>
* sim_mem.cpp - the file that creates all the functions,opening the given files,initialize all the class attributes,initialize main memory,etc.<br>
* swap_file.txt - file that save all the swap pages, create or open during the program.<br>
* exe_file_name1[],exe_file_name2[] - files input to the program represent the processes in the program.<br>


### ==How to compile?==<br>

compile: g++ main.cpp sim_mem.cpp -o test<br>
run: ./test


### ==Input:==<br>

there is no iput, but to check the program use the load and store functions in main.cpp file.

### ==Output:==<br>

printing of the main memory,page table and the swap file.<br>
try to notice each of your load/store, and check if its exit correctly as you wanted.



