///! leetcode#0733: 图像渲染
/// https://leetcode-cn.com/problems/flood-fill/

///@note flood fill algorithm in image processing.
///@ref https://cloud.tencent.com/developer/article/1013350

#include <vector>

#include "gtest/gtest.h"
#include "union_find.hpp"  // UnionFind.

typedef std::vector<std::vector<int>> Image;

/// sol 1: union-find.
///@param sr start row index.
///@param sc start col index.
Image floodfill(const Image& _img, int sr, int sc, int new_color) {
  // copy the original image.
  Image img{_img};  // vector copy ctor.
  // trivial cases.
  if (img.empty() || img.front().empty()) return Image{};
  int n_rows = (int)img.size(), n_cols = (int)img.front().size();
  int ori_color = -1;  // original color.
  if (sr >= 0 && sr < n_rows && sc >= 0 && sc < n_cols) ori_color = img[sr][sc];
  // color ranges from 0 to 65535(2^16 - 1), i.e. 16bit image.
  constexpr int min_color = 0, max_color = 65535;
  if (ori_color < min_color || ori_color > max_color)
    throw std::invalid_argument("invalid input.");

  // create an UnionFind utility instance. Initially, each cell is seen as a
  // disjoint set.
  UnionFind uf(n_rows * n_cols, true);  // use_rank = false.

  // union: traverse the image and unite adjacent cells if they have the same
  // color.
  for (int j = 0; j < n_rows; ++j) {
    for (int i = 0; i < n_cols; ++i) {
      const int& color = img[j][i];
      //@warning since we choose to start the traversal from the left-top
      // corner, the unexplored cells will be either right or bottom.
      // try to unite right cell.
      if (i + 1 < n_cols && img[j][i + 1] == color) {
        ///@warning UnionFind maintains a one-dim array, so you have to flatten
        /// the index to linear index.
        // uf.union_sets(i + j * n_cols, (i + 1) + j * n_cols);
        uf.union_sets_by_rank(i + j * n_cols, (i + 1) + j * n_cols);
      }
      // try to unite bottom cell.
      if (j + 1 < n_rows && img[j + 1][i] == color) {
        // uf.union_sets(i + j * n_cols, i + (j + 1) * n_cols);
        uf.union_sets_by_rank(i + j * n_cols, i + (j + 1) * n_cols);
      }
    }
  }

  // find: with the ori_color as the target color, find all cells in the source
  // disjoint set that having the target color and render them with new_color.
  for (int j = 0; j < n_rows; ++j) {
    for (int i = 0; i < n_cols; ++i) {
      int& color = img[j][i];
      if (color == ori_color &&
          uf.is_connected(i + j * n_cols, sc + sr * n_cols))
        color = new_color;
    }
  }

  ///@note use of std::move considered harmful, since it can prevent elision.
  ///@ref https://stackoverflow.com/a/14856553
  // return std::move(img);
  return img;
}

///@test
// test cases.
Image tc0{{1, 1, 1}, {1, 1, 0}, {1, 0, 1}};

TEST(FloodfillTest, SanityCheck) {
  Image img = floodfill(tc0, 1, 1, 2);
  bool is_same = true;
  Image target{{2, 2, 2}, {2, 2, 0}, {2, 0, 1}};
  if (img.size() != target.size() ||
      img.front().size() != target.front().size())
    is_same = false;
  int n_rows = (int)img.size(), n_cols = (int)img.front().size();
  for (int j = 0; j < n_rows; ++j)
    for (int i = 0; i < n_cols; ++i)
      if (img[j][i] != target[j][i]) is_same = false;
  EXPECT_TRUE(is_same);
}
