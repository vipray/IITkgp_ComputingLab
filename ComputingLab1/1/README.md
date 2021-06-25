#### Problem Statement:
    Given Paths(a sequence of vertices) of different users. Find the longest common subpath among them.

##### Task1:
    * Given two such paths. Find the longest common sub-path between them USING Dynamic Programming.
    * Approach: 
        * Let paths be u1, u2..., un and v1, v2..., vm.
        * if ui = vj , then DP(i, j) = DP(i − 1, j − 1) + 1 
        * else L(i, j) = 0.
    * Files : Task1.c (Code) | task1_tc1.txt (TestCase)