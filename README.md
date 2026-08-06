# ECE_3170_Honors_Project
usage:  
```./sim.exe algo [error_type] [num_iterations] [bit_error_rate]```  
Error types:  
0 -> affected bits set to zero  
1 -> affected bits set to one  
2 -> affected bits inverted  
Zero is no encoding, other even numbers are repetition with assorted error  
models, odd numbers are hammond with assorted error models (see #defines at  
top of program).  
combined.c is the only one to support the error type argument (the same  
arguments otherwise work with sim and sim_zero).  
Arguments are strictly positional, for the time being.  
# UPDATE:
I have done some work on refactoring the code to use fewer global variables.  
The struct_based folder (and, at least sometimes, AI-related refactoring
subfolders) contain my efforts to replace the globals with structs.  
The most recent update to the code (as of 8/6/2026) contains attempts to
reduce the number of nearly-identical functions; the simulation behavior
seems to be identical to the earlier versions but I have not completely
verified this.  
