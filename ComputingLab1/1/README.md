#### Problem Statement:
    Given Paths(a sequence of vertices) of different users. Find the longest common subpath among them.

##### Task1:
    * Given two such paths. Find the longest common sub-path between them USING Dynamic Programming.
    * Approach: 
        * Let paths be u1, u2..., un and v1, v2..., vm.
        * if ui = vj , then DP(i, j) = DP(i − 1, j − 1) + 1 
        * else L(i, j) = 0.
    * Files : Task1.c (Code) | task1_tc1.txt (TestCase)

##### Task2:
    * Given two such paths. Find the longest common sub-path between them USING Rabin and Karp algorithm with Rolling Hashing.
    * Approach:
        1.  Find Smallest path len (smallestPathLen)
        2.  As we need to find the longest Common subpath, we have to check for all length from
            "smallestPathLen" to 1. We can apply binary search over the length.
                a.  First we will go for 'mid' length and check if there is any common subPath 
                    of length 'mid'. 
                b.  If yes then we will look whether there is any common subPath 
                    bigger than 'mid' for that we will search for lengths 'mid+1' to 'smallestPathLen'.
                c.  else we will search for lengths '1' to 'mid-1'.
        
        3.  For each length 'l'
                a.  First we insert all subPaths of 1st path in the HashTable
                b.  Then from 2nd path, we will check if there is any 
                    subPath of current path which is already present in the HashTable
                c.  As we need a subPath to be common in all the Paths. So that every Hash entry, 
                    we keep a count of matches till now.
                    So take a senario, we are checking for 3rd path and we found a match in the hash table
                    but suppose if that subPath (in hash table) didn't get matched with any subPath of 2nd path, then this match is useless.
    * Files : Task2.c (Code) | task2_tc1.txt (TestCase)

##### Task3:
    * Given 'K' such paths. Find the longest common sub-path among them USING Rabin and Karp algorithm with Rolling Hashing.
    * Files : Task3.c (Code) | task3_tc200.txt (TestCase)