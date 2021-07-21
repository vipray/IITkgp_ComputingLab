#### Problem Statement:
    Given Two Matrices A(r1 x c1) and B(r2 x c2). Write multi-threaded programs for matrix-multiplication to obtain Matrix C(r1 x c2).

##### Imp1:
    *   Create r1*c2 threads and assign one position of Matrix C to each thread excusively.
    *   Files : Imp1.c (Code) | input.txt, input1.txt, input2.txt, input3.txt (TestCase)

##### Imp2:
    *   Create threads pool having threads equall to the cores in your system.
    *   Each thread will compute one element of Matrix C at a time.
    *   After completion, it again check if there is any other element left to be computed
        *   If yes, then it will start computing that element.. and so on
        *   If no, then it will terminate.
    *   Thus each thread 't' will compute(exclusively) some 'kt' elements of Matrix C.  
    *   Files : Imp2.c (Code) | input.txt, input1.txt, input2.txt, input3.txt (TestCase)

##### Imp3:
    *   Same as Imp2 but using condition varaible.
    *   Create threads pool having threads equall to the cores in your system.
    *   Each thread will compute one element of Matrix C at a time.
    *   After completion, it will wait for main() threads signal to continue.
    *   main() thread use condition varaible to signal one of the waiting thread.
    *   Thread, on receiving signal, resumes and check if there is element to be computed
        *   If yes, then it will start computing that element.. and so on
        *   If no, then it will terminate.
    *   Thus each thread 't' will compute(exclusively) some 'kt' elements of Matrix C.
    *   Files : Imp3.c (Code) | input.txt, input1.txt, input2.txt, input3.txt (TestCase)

##### Note: 
        *   When we signal a condition varaible, it does not take care of fact that whether there  is any thread waiting for this singal or not. 
        *   This fact might let the code go into deadlock. 
        *   So, It is better to make sure( may be using 'sleep()') that there should be some thread waiting for the signal, otherwise your signal get wasted.

