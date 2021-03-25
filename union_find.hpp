#ifndef UNION_FIND_HPP_
#define UNION_FIND_HPP_

///! Union-Find data structure / algorithm.
///@ref https://zhuanlan.zhihu.com/p/93647900
///@ref https://zhuanlan.zhihu.com/p/35314141
///@ref https://visualgo.net/zh/ufds
///@ref
/// https://github.com/labuladong/fucking-algorithm/blob/master/%E7%AE%97%E6%B3%95%E6%80%9D%E7%BB%B4%E7%B3%BB%E5%88%97/FloodFill%E7%AE%97%E6%B3%95%E8%AF%A6%E8%A7%A3%E5%8F%8A%E5%BA%94%E7%94%A8.md

///@note time compleixity of various implementation of union-find.
///@ref
/// https://leetcode-cn.com/problems/number-of-provinces/solution/jie-zhe-ge-wen-ti-ke-pu-yi-xia-bing-cha-0unne/

/// apps: 不需要给出路径的动态连通性问题：
/// (1) 元素的分组管理
/// (2) 最小生成树的Kruskal算法.
/// P.S. 如需要给出路径，可以考虑使用DFS、回溯等方法。

#include <cassert>    // assert
#include <stdexcept>  // std::invalid_argument
#include <vector>

#include "gtest/gtest.h"

///@note how to use UnionFind.
/// UnionFind is simply a data structure and it's generally used as an utility.
/// To use it, you must specify a set of rules to connect a pair of elements and
/// hence the pair of disjoint sets that contain them separately.
///
/// for e.g., given a rule:
/// bool is_same_set(int i, int j){...}
/// you then normally create an instance of UnionFind with use_rank = true
/// or false; init or init_with_rank is called accordingly.
/// the UnionFind algorithm (the so called UFDS) then proceeds with two separate
/// procedures:
///
/// (1) Union: after the initialization, each pair of elements are checked given
/// the rule. And if they are in the same disjoint set, they're connected by
/// calling union_sets or union_sets_by_rank. The union step is generally
/// independent of the find step.
///
/// (2) Find: the problem you're solving generally asks you to apply an
/// operation on a certain disjoint set. After union, all disjoint sets are
/// observable. You then simply call is_connected(int i, int j) with i or j set
/// to your target and the other set to the elements to be checked.
/// for e.g. given a grid with colored cells, and a cell painted with the target
/// color and indexed with idx. You may call is_connected with i = idx and j =
/// other_idx.
///
/// 0733_floodfill.cc shows the usage of UnionFind on solving
/// leetcode#0733. https://leetcode-cn.com/problems/flood-fill/

///@brief union find disjoint set, UFDS.
class UnionFind {
 public:
  // ctor.
  explicit UnionFind(int n, bool use_rank = false)
      : _n{n}, _n_dis_sets{n}, _use_rank{use_rank} {
    _parents.clear();
    _parents.resize(_n, 0);
    if (_use_rank) {
      _ranks.clear();
      _ranks.resize(_n, 0);
      assert(_parents.size() == _ranks.size());
      init_with_rank();
    } else
      init();
  }

  ///@note benefits of disabling copy ctor and assignment operator.
  ///@ref
  /// https://www.geeksforgeeks.org/explicitly-defaulted-deleted-functions-c-11/
  // disable copy ctor.
  UnionFind(const UnionFind&) = delete;
  // disable assignment operator.
  UnionFind& operator=(const UnionFind&) = delete;

  //@note defaulted dtor is more efficient than user-defined dtor even if you
  // only leave an empty dtor body.
  ///@note when to use private ctor and dtor.
  ///@ref https://stackoverflow.com/a/18546273
  virtual ~UnionFind() = default;

  // union interface; union two disjoint sets.
  ///@warning union is a reserved keyword.
  inline void union_sets(int i, int j) {
    // if they're not in the same set, each call of union_sets will decrement
    // _n_dis_sets by 1.
    if (!_use_rank)
      union_sets_without_rank(i, j);
    else
      union_sets_by_rank(i, j);
  }

  // find interface; check connectivity.
  inline bool is_connected(int i, int j) {
    if (!valid_arg(i) || !valid_arg(j))
      throw std::invalid_argument("invalid index.");
    if (!_use_rank)
      return (find_rep(i) == find_rep(j));
    else
      return (find_rep_path_comp(i) == find_rep_path_comp(j));
  }

  inline const int& get_num_dis_sets() const { return _n_dis_sets; }

 private:
  inline bool valid_arg(int i) const {
    if (i >= 0 && i < _n)
      return true;
    else
      return false;
  }

  // initialization.
  void init() {
    assert(!_use_rank);
    for (int i = 0; i < _n; ++i) _parents.at(i) = i;
  }

  // find; find the representative element's index in the disjoint set that
  // contains the query element indexed with curr_idx.
  inline int find_rep(int curr_idx) {
    assert(!_use_rank);
    if (!valid_arg(curr_idx)) throw std::invalid_argument("invalid index.");
    const int& parent_idx = _parents.at(curr_idx);
    // the representative's parent is itself.
    if (parent_idx == curr_idx)
      return curr_idx;
    else {
      // else, trace back.
      /// FIXME(20210325): the recursion can be replaced with iteration using a
      /// while loop.
      // while (_parents.at(curr_idx) != curr_idx)
      //   curr_idx = _parents.at(curr_idx);
      return find_rep(parent_idx);
    }

    // In short, the above logic can be compressed within one line.
    // return (_parents.at(curr_idx) == curr_idx
    //             ? curr_idx
    //             : find_rep(_parents.at(curr_idx)));
  }

  inline void union_sets_without_rank(int i, int j) {
    assert(!_use_rank);
    // find i and j's representative elements.
    // the representive element is the root a tree formed by a disjoint set.
    int rep_i = find_rep(i), rep_j = find_rep(j);
    // union is just linking one root with the other.
    ///@warning this logic could handle the case that rep_i == rep_j.
    /// however, we choose to explicitly handle it separately.
    if (rep_i == rep_j) return;
    _parents.at(rep_i) = rep_j;
    --_n_dis_sets;
    assert(_n_dis_sets > 0);
  }

  /// UnionFind with path compression and union-by-rank.
  ///@warning path compression and union-by-rank can be adopted separately.

  void init_with_rank() {
    assert(_use_rank);
    for (int i = 0; i < _n; ++i) {
      _parents.at(i) = i;
      // initial rank is set to 0.
      ///@warning whether set to 0 or 1 or else does not matter actually.
      _ranks.at(i) = 0;
    }
  }

  // find_rep with path compression.
  inline int find_rep_path_comp(int curr_idx) {
    assert(_use_rank);
    if (!valid_arg(curr_idx)) throw std::invalid_argument("invalid index.");
    int& parent_idx = _parents.at(curr_idx);
    // the representative's parent is itself.
    if (parent_idx == curr_idx)
      return curr_idx;
    else {
      // else, trace back.
      int rep_idx = find_rep_path_comp(parent_idx);
      // path compression, i.e. set parent_idx as rep_idx.
      parent_idx = rep_idx;
      return rep_idx;
    }

    // In short, the above logic can be compressed within one line.
    // return (_parents.at(curr_idx) == curr_idx
    //             ? curr_idx
    //             : (_parents.at(curr_idx) =
    //                    find_rep_path_comp(_parents.at(curr_idx))));
  }

  // union_sets with union-by-rank.
  inline void union_sets_by_rank(int i, int j) {
    assert(_use_rank);
    // find i and j's representative elements.
    // the representive element is the root a tree formed by a disjoint set.
    int rep_i = find_rep_path_comp(i), rep_j = find_rep_path_comp(j);
    // skip if they're in the same disjoint set.
    if (rep_i == rep_j) return;
    // union by rank. the shorter tree is united to the taller tree.
    if (_ranks.at(rep_i) < _ranks.at(rep_j)) {
      // the tree rooted at rep_i is shorter than the tree rooted at rep_j.
      _parents.at(rep_i) = rep_j;
    } else {
      // the tree rooted at rep_j is shorter than the tree rooted at rep_i.
      _parents.at(rep_j) = rep_i;
      // if the two trees are equally tall before union, then the united tree's
      if (_ranks.at(rep_i) == _ranks.at(rep_j)) _ranks.at(rep_i) += 1;
      // depth is incremented by 1.
    }
    --_n_dis_sets;
    assert(_n_dis_sets > 0);
  }

  /// data members.

  const int _n;     // initial #disjoint sets.
  int _n_dis_sets;  // current #disjoint sets, i.e. connected components.
  ///! the core of UnionFind is _parents which makes a tree only with a one-dim
  /// array!
  std::vector<int> _parents;  // index: curr_idx, value: parent_idx.
  // a element's rank is the depth of the tree rooted at this element.
  const bool _use_rank = false;
  std::vector<int> _ranks;  // index: curr_idx, value: rank.
};

#endif  // UNION_FIND_HPP_