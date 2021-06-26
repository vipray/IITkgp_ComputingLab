#### Problem Statement:
    Given a connected User-Conent graph(Like Social Media). There are two types of edges
        1.  Friendship Links(Undirected) from user to user
        2.  Sharing Links(Directed) from user to the content that user has shared


##### Input Format:
    n (Number of users)
    <user id 1>: <list of users connected to user 1>
    to
    <user id n>: <list of users connected to user n>

    m (Number of contents)
    <user id 1>: <list of contents posted by user 1>
    to
    <user id n>: <list of contents posted by user n>
![Snap](screens/graphExample.png)


##### Task1:
    * Given 'userID' and varaible 'K'. Find first k recommended contents in increasing order of their distances from user 'userID' such that none of the recommended content is itself shared by 'userID'. 
    * Approach: 
        * Use Dijkastra (or BFS)
    * Files : Task1.c (Code) | a2_task1_input.txt (TestCase)