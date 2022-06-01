/* todome - search for TODOs FIXMEs BUGs and NOTEs ... in source code
   Copyright (C) 2021-2021 Bishr Ghalil.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bishr Ghalil */


#ifndef BINSTREE_H_
#define BINSTREE_H_

#include "dna_string.h"
#include "tag.h"

typedef struct TREE_NODE {
  STRING *line;
  TAG *tag;
  struct TREE_NODE *right;
  struct TREE_NODE *left;
} TREE_NODE;

typedef struct BINSTREE {
  TREE_NODE *root;
  size_t nelem;
} BINSTREE;

TREE_NODE *tnode_new(TAG *tag, STRING *line) {
  TREE_NODE *node = (TREE_NODE *)malloc(sizeof(*node));
  node->line = line;
  node->tag = tag;
  node->right = NULL;
  node->left = NULL;

  return node;
}

BINSTREE *binstree_new() {
  BINSTREE *tree = (BINSTREE *)malloc(sizeof(*tree));
  tree->root = NULL;
  tree->nelem = 1;
  return tree;
}

void tnode_insert(TREE_NODE *a, TREE_NODE *b) {
  switch (tag_cmp(a->tag, b->tag)) {
  case 1:
  case 0:
    if (a->left) {
      tnode_insert(a->left, b);
    }

    else {
      a->left = b;
    }
    break;

  case -1:
    if (a->right) {
      tnode_insert(a->right, b);
    }

    else {
      a->right = b;
    }
    break;
  }
}

void tnode_maxprint(TREE_NODE *node) {
  if (!node)
    return;
  if (node->right) {
    tnode_maxprint(node->right);
  }

  printf("%s %s\n", tag_data(node->tag), node->line->data);

  if (node->left) {
    tnode_maxprint(node->left);
  }
}

void binstree_insert(BINSTREE *tree, TREE_NODE *node) {
  if (!tree->root) {
    tree->root = node;
  } else {
    TREE_NODE *root = tree->root;
    tnode_insert(root, node);
  }
  tree->nelem++;
}

void binstree_print(BINSTREE *tree) {
  TREE_NODE *root = tree->root;
  tnode_maxprint(root);
}

void tnode_free(TREE_NODE *node) {
  if (node->right) {
    tnode_free(node->right);
  }

  if (node->left) {
    tnode_free(node->left);
  }

  string_free(node->line);
  tag_free(node->tag);
  free(node);
}

void binstree_free(BINSTREE *tree) {
  if (tree->root) {
    tnode_free(tree->root);
  }
  free(tree);
}

#endif
