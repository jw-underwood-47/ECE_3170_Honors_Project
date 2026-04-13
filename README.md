# ECE_3170_Honors_Project
usage:  
```./sim.exe algo [num_iterations] [bit_error_rate]```  
Zero is no encoding, other even numbers are repetition with assorted error  
models, odd numbers are hammond with assorted error models (see #defines at  
top of program).  sim_zero is basically the same, but bit errors only go 1->0  
iter.sh and iter_zero.sh run through all error models (within that program)  
and send the results to separate, labelled files in a results directory  
of the form results* where * is the first command line argument to the script  
tweaking the number of iterations and bit error values should be pretty easy  

# Eventual idea
While it won't be till the end of this (spring 2026 semester), at earliest, I  
would rather like to get all of this into one, cleaner library since sim_zero  
and sim do very similar things and those functions could probably be combined  
in some way... but I also need to deal with coursework and other school stuff  
