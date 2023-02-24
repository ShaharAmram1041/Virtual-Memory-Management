#ifndef EX5_1_SIM_MEM_H
#define EX5_1_SIM_MEM_H
#define MEMORY_SIZE 200
extern char main_memory[MEMORY_SIZE];

typedef struct page_descriptor {
    int V; //valid
    int D; //dirty
    int P; //permission
    int frame; //the number of a frame if in case it is page-mapped
    int swap_index;// where the page is located in the swap file.
} page_descriptor;

class sim_mem {
    int swapfile_fd;
    int program_fd[2];
    int text_size;
    int data_size;
    int bss_size;
    int heap_stack_size;
    int num_of_pages;
    int page_size;
    int num_of_proc;
    page_descriptor **page_table;       //pointer to page table

public:
    sim_mem(char exe_file_name1[], char exe_file_name2[], char swap_file_name[],int text_size,
                     int data_size, int bss_size, int heap_stack_size,
                     int num_of_pages, int page_size, int num_of_process);

    ~sim_mem();

    char load(int process_id, int address);

    void store(int process_id, int address, char value);

    void print_memory();

    void print_swap();

    void print_page_table();

    void fullMem(int);//function declaration

};
#endif //EX5_1_SIM_MEM_H
