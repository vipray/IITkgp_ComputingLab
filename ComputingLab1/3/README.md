#### Problem Statement:
    Code the leader election algorithm(a common problem in distributed computing). 
    *   Consider each node as an Agent with unique ID and Agents are connected in a tree structure where each edge between two agents represents an bidirection way to communicate.
    *   Each node will be a new process and edges will be the pipes use to communicate between the processes.
    *   The algorithm elects the agent having the minimum identifier as the leader. At the end, each agent is aware of the leader, including the leader itself.
    *   Once the leader is identified, the agents may redefine their parents so as to point towards the leader

##### Input Format:
   The input consists of a list of edges of the form < node, parent > .

##### Tasks:
    *   Run Election Algorithm, find the leader then each process J, must write the identity of the leader K, using the following statement:
        "I am agent J and my leader is K"    
    *   Now Let Leader found the sum of all the Agent's IDs.
    *   Finally Leader ask every other Agent to gets terminated. Before termination each Agents announces its termination. Also every parent wait for all of its child to terminate first.
    *   Approach:
            *   Use well define algorithm to find out the leader.
    *   Files : Task123.c (Code) | A5_input.txt (TestCase)
