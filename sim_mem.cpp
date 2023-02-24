#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include "sim_mem.h"
using namespace std;

int del_mem = 0; //index of the first process that we want to swapout

int freeFrame(int);//declartion function
sim_mem:: sim_mem(char exe_file_name1[], char exe_file_name2[], char swap_file_name[],int text_size,
                  int data_size, int bss_size, int heap_stack_size,
                  int num_of_pages, int page_size, int num_of_process) {
    if (exe_file_name1[0] == '\0' || exe_file_name1[0] == ' ') {//executable dont exist
        fprintf(stderr, "process dont exist");
        exit(1);
    }
    if (strcmp(exe_file_name1, "NULL") == 0 || strcmp(exe_file_name2, "NULL") == 0) {
        fprintf(stderr, "one of the processes is NULL");
        exit(1);
    }
    int text_pages;
    if (page_size == 0) {
        fprintf(stderr, "error with page size");
        exit(1);
    } else {
        text_pages = (text_size / page_size) + (text_size % page_size);
    }
    //initialize all the attributes of class
    this->text_size = text_size;
    this->data_size = data_size;
    this->bss_size = bss_size;
    this->heap_stack_size = heap_stack_size;
    this->num_of_pages = num_of_pages;
    this->page_size = page_size;
    this->num_of_proc = num_of_process;
    this->program_fd[0] = 0;
    this->program_fd[1] = 0;
    ifstream p, p1, s;
    if (num_of_process > 1) {
        if (open(exe_file_name2, O_RDONLY, 0) <= 0) {
            fprintf(stderr, "Unable to open file2");
            exit(1);
        }
        this->program_fd[1] = open(exe_file_name2, O_RDONLY, 0);
    }//initialize fd be file descriptor in index 1
    if (open(exe_file_name1, O_RDONLY, 0) <= 0) {
        fprintf(stderr, "Unable to open file1");
        exit(1);
    }
    this->program_fd[0] = open(exe_file_name1, O_RDONLY, 0);//initialize fd be file descriptor in index 0
    memset(main_memory, '0', MEMORY_SIZE);//initialize the main memory in zeros
    this->page_table = (page_descriptor **) malloc(sizeof(page_descriptor *) * num_of_process);
    if (page_table == NULL) {
        perror("malloc fail");
        exit(1);
    }
    this->page_table[0] = (page_descriptor *) malloc(sizeof(page_descriptor) * num_of_pages);
    if (page_table[0] == NULL) {
        free(this->page_table);
        perror("malloc fail");
        exit(1);
    }//initialize all the struct attributes in zero
    for (int j = 0; j < num_of_pages; j++) {
        this->page_table[0][j].V = 0;
        this->page_table[0][j].D = 0;
        this->page_table[0][j].P = 1;
        this->page_table[0][j].frame = -1;
        this->page_table[0][j].swap_index = -1;
    }
    for (int j = 0; j < text_size / page_size; j++)//change permission by the text, no permission for text
        this->page_table[0][j].P = 0;//no permission is 0

    if (num_of_process > 1) {
        this->page_table[1] = (page_descriptor *) malloc(sizeof(page_descriptor) * num_of_pages);
        if (page_table[1] == NULL) {
            free(this->page_table);
            free(this->page_table[0]);
            perror("malloc fail");
            exit(1);
        }//initialize all the struct attributes in zero
        for (int j = 0; j < num_of_pages; j++) {
            this->page_table[1][j].V = 0;
            this->page_table[1][j].D = 0;
            this->page_table[1][j].P = 1;
            this->page_table[1][j].frame = -1;
            this->page_table[1][j].swap_index = -1;
        }
        for (int j = 0; j < text_size / page_size; j++)//change permission by the text, no permission for text
            this->page_table[1][j].P = 0;//no permission is 0
    }//open or create the swapfile
    if ((this->swapfile_fd = open(swap_file_name, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR)) <= 0) {
        perror("fail open the swapfile");//failed open the swap, free the alloc memory
        free(page_table[0]);
        free(page_table);
        if (num_of_process > 1)
            free(page_table[1]);
        exit(1);
    }
    for (int i = 0; i < page_size * (num_of_pages - text_pages); i++)
        write(swapfile_fd, "0", 1);//initialize the swap file with zero
    memset(main_memory,'0',200);//initialize main memory in zero
}

sim_mem::~sim_mem() {//free all allocate memory, close the open file
    if(this->num_of_proc > 1)
        free(this->page_table[1]);
    free(this->page_table[0]);
    close(this->program_fd[0]);
    close(this->program_fd[1]);
    close(this->swapfile_fd);
    free(this->page_table);
}

char sim_mem:: load(int process_id, int address){
    //address is out of range
    if(address >page_size*num_of_pages){
        fprintf(stderr,"error with the size of address\n"
                       "try address between 0 - %d",page_size*num_of_pages);
        return '\0';}
    //split the address to page and offset
    int page = address / page_size;
    int offset = address % page_size;
    int i = process_id - 1;
    int freeIdx = freeFrame(page_size);//contain the first free index of the [RAM]
    if (this->page_table[i][page].V == 1)  //hit, the page already in [RAM]
        return main_memory[(this->page_table[i][page].frame * page_size) + offset];

    if (freeIdx + page_size > MEMORY_SIZE) //[RAM] is full, and after we use it to swap
        fullMem(i);

//this part we want to bring page to [RAM]
    freeIdx = freeFrame(page_size);
    if (this->page_table[i][page].P == 0){//if there is no access to the page,text page
        lseek(program_fd[i],address-offset,SEEK_SET);//go to the wanted index
        read(program_fd[i],&main_memory[freeIdx],page_size);//copy it to [RAM]
        this->page_table[i][page].V = 1;
        this->page_table[i][page].P = 0;
        this->page_table[i][page].D = 0;
        this->page_table[i][page].frame = freeIdx / page_size;
        this->page_table[i][page].swap_index = -1;
        return main_memory[(this->page_table[i][page].frame* page_size) + offset];//return the char from [RAM]
    }

    if (this->page_table[i][page].D == 1){//this situation: V = 0, P = 1, D = 1
        char s[page_size];// array to delete the chosen index swap
        for (int m = 0; m < page_size; m++)
            s[m] = '0';
        lseek(swapfile_fd, this->page_table[i][page].swap_index,SEEK_SET);//go to location of our page in swap file
        read(swapfile_fd, &main_memory[freeIdx], page_size);//read the page from swap to [RAM]
        lseek(swapfile_fd, this->page_table[i][page].swap_index, SEEK_SET);
        write(swapfile_fd, s, page_size);//"delete" the file in swap
        this->page_table[i][page].V = 1;
        this->page_table[i][page].D = 0;
        this->page_table[i][page].swap_index = -1;
        this->page_table[i][page].frame = freeIdx/page_size ;
        return main_memory[(freeIdx) + offset];}

    else{ // the page is data or heap/stack or bss
        if(address >= (this->data_size)-(this->text_size) && address < this->data_size){//page from data
            lseek(program_fd[i],page*page_size,SEEK_SET);
            read(program_fd[i],&main_memory[freeIdx],page_size);
            this->page_table[i][page].V = 1;
            this->page_table[i][page].D = 0;
            this->page_table[i][page].frame = freeIdx / page_size;
            this->page_table[i][page].swap_index = -1;
            return main_memory[freeIdx + offset] ;
        }
        //page from bss
        if(address >= (this->data_size) + (this->text_size)  &&  address < (this->bss_size) + (this->data_size) + (this->text_size)){
            lseek(program_fd[i],page*page_size,SEEK_SET);
            read(program_fd[i],&main_memory[freeIdx],page_size);
            this->page_table[i][page].V = 1;
            this->page_table[i][page].D = 0;
            this->page_table[i][page].frame = freeIdx / page_size;
            this->page_table[i][page].swap_index = -1;
            return main_memory[freeIdx + offset] ;
        }
        else//page from heap/stack
            return '\0';
    }}






void sim_mem::store(int process_id, int address, char value) {
    //address is out of range
    if(address >page_size*num_of_pages){
        fprintf(stderr,"error with the size of address\n"
                       "try address between 0 - %d",page_size*num_of_pages);
        return;}
    //split the address to page and offset
    int page = address / page_size;
    int offset = address % page_size;
    int i = process_id - 1;
    int freeIdx = freeFrame(page_size);//contain the first free index of the [RAM]
    if (this->page_table[i][page].P == 0){//if there is no access to the page to write
        fprintf(stderr, "this is page of text,no permission!\n") ;
        return;}

    if (this->page_table[i][page].V == 1) { //hit, the page already in [RAM]
        main_memory[(this->page_table[i][page].frame * page_size) + offset] = value;
        this->page_table[i][page].D = 1;//active the dirty bit
        return;}


    if (freeIdx + page_size > MEMORY_SIZE) {//[RAM] is full, and after we use it to swap
        fullMem(i);}

    //the page is not in the main memory [RAM], and [RAM] not full
    if (this->page_table[i][page].V == 0) {
        freeIdx = freeFrame(page_size);
        if (this->page_table[i][page].D == 1) {//page only in swap
            char s[page_size];// array to delete the chosen index swap
            for (int m = 0; m < page_size; m++)
                s[m] = '0';
            lseek(swapfile_fd, this->page_table[i][page].swap_index,SEEK_SET);//go to location of our page in swap file
            read(swapfile_fd, &main_memory[freeIdx], page_size);//read the page from swap
            lseek(swapfile_fd, this->page_table[i][page].swap_index, SEEK_SET);
            write(swapfile_fd, s, page_size);//"delete" the page in the swap
            this->page_table[i][page].V = 1;
            this->page_table[i][page].D = 0;
            this->page_table[i][page].swap_index = -1;
            this->page_table[i][page].frame = freeIdx/page_size ;
            main_memory[(freeIdx) + offset] = value;
            return;
        }
        else{//this station v = 0,p = 1,d = 0, page is in data or heap/stack
            freeIdx = freeFrame(page_size);
            if(address >= (this->text_size) && address < (this->data_size)+(this->bss_size)){//page from data
                lseek(program_fd[i], page*page_size, SEEK_SET);//go to the wanted address
                read(program_fd[i],&main_memory[freeIdx],page_size);//copy to the [RAM]
                main_memory[freeIdx + offset] = value;
                this->page_table[i][page].V = 1;
                this->page_table[i][page].D = 1;
                this->page_table[i][page].frame = freeIdx / page_size;
                this->page_table[i][page].swap_index = -1;
            }
            else{//page from heap/stack, bss
                for(int c = 0;c < page_size; c++)
                    main_memory[freeIdx+c] = '0';//put zero's in the main memory instead
                this->page_table[i][page].V = 1;
                this->page_table[i][page].D = 1;
                this->page_table[i][page].frame = freeIdx / page_size;
                this->page_table[i][page].swap_index = -1;
                main_memory[freeIdx+ offset] = value;}//active the wanted char
        }}
}


void sim_mem::print_memory(){
    int i;
    printf("\n Physical memory\n");
    for(i = 0;i < MEMORY_SIZE; i++)
        printf("[%c]\n",main_memory[i]);
}

void sim_mem::print_swap(){
    char* str =(char*) malloc(this->page_size *sizeof(char));
    int i;
    printf("\n Swap memory\n");
    lseek(swapfile_fd, 0, SEEK_SET); // go to the start of the file
    while(read(swapfile_fd, str, this->page_size) == this->page_size) {
        for(i = 0; i < page_size; i++) {
            printf("%d - [%c]\t", i, str[i]);
        }
        printf("\n");
    }
    free(str);
}

void sim_mem::print_page_table() {
    int i;
    for (int j = 0; j < num_of_proc; j++) {
        printf("\n page table of process: %d \n", j);
        printf("Valid\t Dirty\t Permission \t Frame\t Swap index\n");
        for(i = 0; i < num_of_pages; i++) {
            printf("[%d]\t[%d]\t[%d]\t[%d]\t[%d]\n",
                   page_table[j][i].V,
                   page_table[j][i].D,
                   page_table[j][i].P,
                   page_table[j][i].frame ,
                   page_table[j][i].swap_index);
        }
    }
}
int freeFrame(int page_size) {
    int i = 0,j = 0;
    bool flag = false;
    while (i < MEMORY_SIZE) {
        if (main_memory[i] == '0' ) { //find free index
            for (j = i; j < i + page_size; j++){//check space for all the size
                if (main_memory[j] != '0')
                    break;
                if(j == i+page_size-1 && main_memory[j] == '0'){
                    flag = true;
                    break;}}
        }
        if(flag)
            break;
        i+= page_size;
    }
    return i;
}

void sim_mem:: fullMem(int i){//the [RAM] is full, clear first process out to swapfile
    int swapIdx = 0;
    bool flag = false;
    char c[page_size];//keep us the result from reading function
    memset(c,'0',page_size);
    lseek(swapfile_fd, 0, SEEK_SET);
    while (read(swapfile_fd, c, page_size) == page_size) {//search for empty index in swap
        for(int m = 0; m < page_size;m++) {
            if (c[m] != '0'){
                swapIdx++;
                break;}
            if (m == page_size - 1 && c[m] == '0'){
                flag = true;
                break;}
        }
        if(flag)
            break;

    }//swapIdx is index of free in swap file
    lseek(swapfile_fd,page_size*swapIdx,SEEK_SET);//go to the free index in the swap
    write(swapfile_fd, main_memory+del_mem,page_size);//write from page in [RAM] to swap
    for(int m =0;m<page_size;m++)
        main_memory[del_mem+m] = '0';

    int j = 0;

    //change the p.t of the swapping process
    for(j = 0; j <this->num_of_pages;j++){
        if(page_table[i][j].frame ==  freeFrame(page_size)/page_size)
            break;}
    if(page_table[i][j].P == 0){
        memset(c,'0',page_size);
        lseek(swapfile_fd,page_size*swapIdx,SEEK_SET);//go to the index in the swap
        write(swapfile_fd, c,page_size);//delete page from swap
        this->page_table[i][j].V = 0;
        this->page_table[i][j].D = 0;
        this->page_table[i][j].frame = -1;
        this->page_table[i][j].swap_index = -1 ;
    }
    else{
        this->page_table[i][j].V = 0;
        this->page_table[i][j].D = 1;
        this->page_table[i][j].frame = -1;
        this->page_table[i][j].swap_index = swapIdx*page_size ;}

    del_mem += page_size; // active the index of the next delete
    if(del_mem >= MEMORY_SIZE -1)
        del_mem = 0;
}



