# union_find
My implemention of union-find data structure.
### dependency 
google-test: `sudo apt-get install libgtest-dev`.
### how to 
UnionFind is simply a data structure and it's generally used as an utility.

To use it, you must specify a set of rules to connect a pair of elements and hence the pair of disjoint sets that contain them separately.

for e.g., given a rule: `bool is_same_set(int i, int j){...}`
you then normally create an instance of UnionFind with use_rank = true or false; init or init_with_rank is called accordingly.
the UnionFind algorithm (the so called UFDS) then proceeds with two separate procedures:

(1) Union: after the initialization, each pair of elements are checked given
the rule. And if they are in the same disjoint set, they're connected by
calling union_sets or union_sets_by_rank. The union step is generally
independent of the find step.

(2) Find: the problem you're solving generally asks you to apply an
operation on a certain disjoint set. After union, all disjoint sets are
observable. You then simply call is_connected(int i, int j) with i or j set
to your target and the other set to the elements to be checked.

for e.g. given a grid with colored cells, and a cell painted with the target
color and indexed with idx. You may call is_connected with i = idx and j =
other_idx.

0733_floodfill.cc shows the usage of UnionFind on solving
leetcode#0733. https://leetcode-cn.com/problems/flood-fill/
