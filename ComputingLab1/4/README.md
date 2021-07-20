#### Problem Statement:
    Code for Readers-Writers Problem with priority to the writers using shared memory and binary semaphores. 
    *   Consider 3 trains having 3 types of ticket(AC2,AC3,SC) with 10 seats each.
    *   Four Different processes, each have its own list of commands to be executed on shared train database.
    *   Each reservation request must first check the availability using a read-lock and then obtain a write-lock for making the reservation, if available. If no reservation is available, then the passenger will be waitlisted.
    *   If a passenger cancels a reservation, the first waitlisted passenger will be confirmed.
    *   Once the leader is identified, the agents may redefine their parents so as to point towards the leader

##### Input Files Format:
    Two types of commands:
    *   reserve < name > < age > < sex > < trainid > < class >
    *   cancel < pnr >

##### Tasks:
    * Write functions:
        *   get-<read/write>-lock( train-id )
        *   release-<read/write>-lock( train-id )
    *   Usefull Link: http://faculty.juniata.edu/rhodes/os/ch5d.htm (Semaphore Solution: Writers have Priority).
    *   Files : T1.c (Code) | i1.txt, i2.txt, i3.txt, i4.txt (TestCase)
