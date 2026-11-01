# Pseudocode

EECE 7205 Mini Project: Airport Code Lookup Service with a Binary Search Tree

## Task 1 - Load the data and build the tree

### 1(a) Split one line into fields

```
SPLIT-LINE(line): main steps
1.  fields ← empty list;  field ← "";  inQuotes ← FALSE
2.  for i ← 0 to line.size − 1 do
3.      c ← line[i]
4.      if c = '"' then
5.          if inQuotes and line[i + 1] = '"' then
6.              field ← field + '"';  i ← i + 1
7.          else inQuotes ← not inQuotes
8.      else if c = ',' and not inQuotes then
9.          append field to fields;  field ← ""
10.     else field ← field + c
11. append field to fields
12. return fields
```

### 1(a) Load the records

```
LOAD(fileName, records): main steps
1.  open fileName;  if it fails then return FALSE
2.  skipped ← 0;  totalLines ← 0;  firstLine ← TRUE
3.  while getline(in, line) do
4.      if firstLine then
5.          firstLine ← FALSE
6.          if line starts with "airport_id" then continue
7.      totalLines ← totalLines + 1
8.      f ← SPLIT-LINE(line)
9.      if f.size < 9 then continue
10.     if f[4] = "\N" or f[4] = "" then skipped ← skipped + 1;  continue
11.     a ← new Airport
12.     a.name ← f[1];  a.city ← f[2];  a.country ← f[3]
13.     a.iata ← f[4];  a.icao ← f[5]
14.     a.lat ← to_double(f[6]);  a.lon ← to_double(f[7])
15.     append a to records
16. return TRUE, skipped, totalLines
```

### 1(a) Check the loader against the posted facts

```
COUNT-COUNTRIES(records): main steps
1.  seen ← empty list
2.  for i ← 0 to records.size − 1 do
3.      found ← FALSE
4.      for j ← 0 to seen.size − 1 do
5.          if seen[j] = records[i].country then found ← TRUE; break
6.      if not found then append records[i].country to seen
7.  return seen.size

COUNT-IN-COUNTRY(records, country): main steps
1.  total ← 0
2.  for i ← 0 to records.size − 1 do
3.      if records[i].country = country then total ← total + 1
4.  return total
```

### 1(b) Insert into the BST

```
INSERT(key, record): main steps
1.  create newNode with key and record
2.  if root = NULL then root ← newNode; return INSERTED
3.  cur ← root
4.  loop
5.      if key < cur.key then
6.          if cur.left = NULL then cur.left ← newNode; return INSERTED
7.          cur ← cur.left
8.      else if key > cur.key then
9.          if cur.right = NULL then cur.right ← newNode; return INSERTED
10.         cur ← cur.right
11.     else return DUPLICATE
```

```
BUILD-FROM-ARRAY(records, n): main steps
1.  for i ← 0 to n − 1 do
2.      if INSERT(records[i].iata, records[i]) = DUPLICATE then
3.          report records[i].iata as a duplicate
```

### 1(c) Height and average depth

```
COLLECT(nodes, depths): main steps
1.  if root = NULL then return
2.  append root to nodes;  append 0 to depths
3.  for i ← 0 to nodes.size − 1 do
4.      node ← nodes[i];  d ← depths[i]
5.      if node.left  ≠ NULL then append node.left  to nodes; append d + 1 to depths
6.      if node.right ≠ NULL then append node.right to nodes; append d + 1 to depths
```

```
HEIGHT(): main steps
1.  if root = NULL then return −1
2.  COLLECT(nodes, depths)
3.  maxDepth ← 0
4.  for i ← 0 to depths.size − 1 do
5.      if depths[i] > maxDepth then maxDepth ← depths[i]
6.  return maxDepth
```

```
AVERAGE-DEPTH(): main steps
1.  if n = 0 then return 0
2.  COLLECT(nodes, depths)
3.  total ← 0
4.  for i ← 0 to depths.size − 1 do
5.      total ← total + depths[i]
6.  return total / n
```

## Task 2 - Search

### 2(a) Find a code, with the search path and the comparison count

```
FIND(target, path, record): main steps
1.  path ← empty list
2.  cur ← root
3.  while cur ≠ NULL do
4.      append cur.key to path
5.      if target = cur.key then record ← cur.data; return FOUND
6.      else if target < cur.key then cur ← cur.left
7.      else cur ← cur.right
8.  return NOT FOUND
```

The number of comparisons is path.size, one per node visited.

### 2(b) Smallest and largest code

```
MIN-KEY(): main steps
1.  if root = NULL then return "(empty)"
2.  cur ← root
3.  while cur.left ≠ NULL do cur ← cur.left
4.  return cur.key

MAX-KEY(): main steps
1.  if root = NULL then return "(empty)"
2.  cur ← root
3.  while cur.right ≠ NULL do cur ← cur.right
4.  return cur.key
```

### 2(b) Pick the test cases out of the tree

```
A-LEAF-KEY(): main steps
1.  COLLECT(nodes, depths)
2.  for i ← 0 to nodes.size − 1 do
3.      if nodes[i].left = NULL and nodes[i].right = NULL then return nodes[i].key
4.  return "(none)"

DEEPEST-KEY(): main steps
1.  COLLECT(nodes, depths)
2.  best ← 0
3.  for i ← 0 to depths.size − 1 do
4.      if depths[i] > depths[best] then best ← i
5.  return nodes[best].key
```

## Task 3 - Delete

### 3(a) Remove a code, reporting which case was used

```
DELETE(node, key, caseUsed): main steps
1.  if node = NULL then return NULL
2.  if key < node.key then node.left ← DELETE(node.left, key, caseUsed)
3.  else if key > node.key then node.right ← DELETE(node.right, key, caseUsed)
4.  else
5.      if node.left = NULL and node.right = NULL then
6.          caseUsed ← "D1 leaf";  free node;  return NULL
7.      if node.left = NULL then
8.          caseUsed ← "D2 only a right child";  child ← node.right
9.          free node;  return child
10.     if node.right = NULL then
11.         caseUsed ← "D2 only a left child";  child ← node.left
12.         free node;  return child
13.     caseUsed ← "D3 two children"
14.     s ← node.right
15.     while s.left ≠ NULL do s ← s.left
16.     node.key ← s.key;  node.data ← s.data
17.     node.right ← DELETE(node.right, s.key, ignored)
18. return node
```

The record is copied along with the key in step 16, otherwise the node would
keep the successor's code but the old airport's name.

### 3(b) Pick one code for each case out of the tree

```
FIND-CASE(kind): main steps
1.  COLLECT(nodes, depths)
2.  for i ← 0 to nodes.size − 1 do
3.      hasLeft  ← nodes[i].left  ≠ NULL
4.      hasRight ← nodes[i].right ≠ NULL
5.      if kind = LEAF       and not hasLeft and not hasRight then return nodes[i].key
6.      if kind = LEFT-ONLY  and hasLeft and not hasRight     then return nodes[i].key
7.      if kind = RIGHT-ONLY and not hasLeft and hasRight     then return nodes[i].key
8.      if kind = TWO        and hasLeft and hasRight         then return nodes[i].key
9.  return "(none)"
```

### 3(b) Level order of the top three levels

```
LEVEL-ORDER-TOP(levels): main steps
1.  if root = NULL then return
2.  COLLECT(nodes, depths)
3.  for d ← 0 to levels − 1 do
4.      print "level d:"
5.      for i ← 0 to nodes.size − 1 do
6.          if depths[i] = d then print nodes[i].key
```

### 3(b) Validate after every delete

```
IS-BST(node, lo, hi): main steps
1.  if node = NULL then return TRUE
2.  if lo is set and node.key ≤ lo then return FALSE
3.  if hi is set and node.key ≥ hi then return FALSE
4.  return IS-BST(node.left, lo, node.key) and IS-BST(node.right, node.key, hi)
```

Initial call: IS-BST(root, unset, unset).

## Task 4 - Traversals and validation

### 4(a) Inorder, which must come out sorted

```
INORDER(out): main steps
1.  stack ← empty;  cur ← root
2.  while cur ≠ NULL or stack is not empty do
3.      while cur ≠ NULL do
4.          push cur;  cur ← cur.left
5.      cur ← pop
6.      append cur.key to out
7.      cur ← cur.right
```

```
IS-SORTED(out): main steps
1.  for i ← 0 to out.size − 2 do
2.      if out[i + 1] ≤ out[i] then return FALSE
3.  return TRUE
```

### 4(b) Save in preorder, load it back

Preorder is the order that rebuilds the identical tree: the root is written
first, so on reload it is inserted first and lands at the root again, and the
same holds at every subtree.

```
PREORDER(out): main steps
1.  if root = NULL then return
2.  stack ← empty;  push root
3.  while stack is not empty do
4.      node ← pop
5.      append node.key to out
6.      if node.right ≠ NULL then push node.right
7.      if node.left  ≠ NULL then push node.left
```

```
SAVE(fileName): main steps
1.  PREORDER(out)
2.  open fileName for writing
3.  for i ← 0 to out.size − 1 do
4.      write out[i] and a newline
```

```
LOAD-PREORDER(fileName, records): main steps
1.  clear the tree
2.  open fileName for reading
3.  while getline(in, code) do
4.      find the record whose iata = code
5.      INSERT(code, record)
```

### 4(b) Same shape

```
SAME-SHAPE(a, b): main steps
1.  if a.height ≠ b.height then return FALSE
2.  LEVEL-ORDER(a, la);  LEVEL-ORDER(b, lb)
3.  if la.size ≠ lb.size then return FALSE
4.  for i ← 0 to la.size − 1 do
5.      if la[i] ≠ lb[i] then return FALSE
6.  return TRUE
```

### 4(c) Validate

IS-BST is in Task 3. It is called after every insert and every delete in the
tests of Tasks 3 and 4.

## Task 5 - Range queries

### 5(a) range FROM TO, without visiting subtrees outside the range

```
RANGE(node, from, to, out, visited): main steps
1.  if node = NULL then return
2.  visited ← visited + 1
3.  if node.key > from then RANGE(node.left, from, to, out, visited)
4.  if from ≤ node.key and node.key ≤ to then append node.data to out
5.  if node.key < to then RANGE(node.right, from, to, out, visited)
```

The two tests in steps 3 and 5 are the pruning. If node.key ≤ from, every key in
the left subtree is smaller than node.key and so smaller than from, and the whole
subtree is skipped. If node.key ≥ to, the right subtree is skipped for the same
reason on the other side. Without those tests the walk would be the full tree.

Left, node, right is the inorder shape, so the matches come out sorted.

visited counts every node the function looks at, which is the cost of the query.

## Task 6 - The shape experiment

### 6(a) The balanced build

The sorted array comes from an inorder traversal of the file-order tree, since
inorder of a BST is already sorted. Taking the middle element as the root splits
the remaining keys into two halves of equal size, and doing that at every level
gives a tree of height floor(log2 n).

```
BUILD-BALANCED(a, lo, hi): main steps
1.  if lo > hi then return NULL
2.  mid ← floor((lo + hi) / 2)
3.  node ← new node with key a[mid] and record a[mid]
4.  node.left  ← BUILD-BALANCED(a, lo, mid − 1)
5.  node.right ← BUILD-BALANCED(a, mid + 1, hi)
6.  return node
```

Initial call: root ← BUILD-BALANCED(a, 0, n − 1). The recursion is only
floor(log2 6072) = 12 deep, so it is safe, unlike a recursive insert.

```
REBALANCE(): main steps
1.  INORDER-RECORDS(sorted)
2.  BUILD-BALANCED(sorted, 0, sorted.size − 1)
```

### 6(b) Shuffle

```
SHUFFLE(a): main steps
1.  for i ← a.size − 1 down to 1 do
2.      j ← random integer in 0 .. i
3.      swap a[i] and a[j]
```

### 6(c) Time 10,000 random searches

```
TIME-SEARCHES(tree, codes, howMany): main steps
1.  start ← clock
2.  for i ← 0 to howMany − 1 do
3.      pick a random code from codes
4.      FIND(code, path, record)
5.  stop ← clock
6.  return stop − start
```

The same list of random codes is used for every tree, so the four timings differ
only by the shape of the tree being searched.

### 6(d) Figures used in the explanation

```
LONGEST-INCREASING-RUN(a, startAt): main steps
1.  best ← 1;  cur ← 1;  curStart ← 0;  startAt ← 0
2.  for i ← 0 to a.size − 2 do
3.      if a[i + 1].iata > a[i].iata then
4.          cur ← cur + 1
5.          if cur > best then best ← cur;  startAt ← curStart
6.      else
7.          cur ← 1;  curStart ← i + 1
8.  return best
```

```
COUNT-PREFIX(a, letter, country, total, inCountry): main steps
1.  total ← 0;  inCountry ← 0
2.  for i ← 0 to a.size − 1 do
3.      if a[i].iata[0] = letter then
4.          total ← total + 1
5.          if a[i].country = country then inCountry ← inCountry + 1
```
