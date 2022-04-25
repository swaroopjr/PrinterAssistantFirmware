
/************************************************* 
Heap increases
Stack decreases

     ------------ Heap Base
     |                  |
     |                  |      |
     |                  |      | 
     |                  |      V
     |                  |
     ------------ Heap Limit
     ------------ Stack Limit     
     |                  |
     |                  |      ^
     |                  |      | 
     |                  |      |
     |                  |
     ------------ Stack Base

************************************************ */
unsigned int __heap_base   = 0x40005000;
unsigned int __heap_limit  = 0x40006000;
unsigned int __stack_base  = 0x40008000 - 4;
unsigned int __stack_limit = 0x40006000;





