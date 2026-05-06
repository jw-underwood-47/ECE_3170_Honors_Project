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
# Eventual idea
Even combined.c could be made substantially cleaner and more readable.  
There is a lot of repeated code, but I'm not sure how much of that should be  
made into functions to avoid performance hits.  
I would like to find a better way to handle each kind of sim than a switch  
case into each function.  
