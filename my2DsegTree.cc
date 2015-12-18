#include <iostream>
#include <iomanip>
#include <string.h>

using namespace std;

const unsigned MAX_N = 1000;

class segTree {
	typedef struct node {
		unsigned x1, y1, x2, y2;
		int sum;
	} node;
	public:
		segTree(int **a, unsigned w, unsigned h):width((w+1)<<1), height((h+1)<<1), arr(a) {
			treeX = new node [height];
			treeY = new node* [height];
			for (unsigned i = 0; i < height; ++i) {
				treeY[i] = new node [width];
			}
			build(1, 1, 1, h - 1, w - 1);
		}
		~segTree() {
			delete [] treeX;
			for (unsigned i = 0; i < height; ++i) {
				delete [] treeY[i];
			}
			if (height > 1)
				delete [] treeY;
		}
		void build(unsigned root, unsigned x1, unsigned y1, unsigned x2, unsigned y2) {
			if (x1 > x2 || y1 > y2) return;
			treeX[root].x1 = x1;
			treeX[root].x2 = x2;
			treeX[root].y1 = y1;
			treeX[root].y2 = y2;
			if (x1 != x2) {
				unsigned m = (x1 + x2) >> 1;
				build(root<<1, x1, y1, m, y2);
				build(root<<1|1, m+1, y1, x2, y2);
			}
			buildY(root, 1, x1, y1, x2, y2);
			treeX[root].sum = treeY[root][1].sum;
		}
		void add(unsigned root, int adder,unsigned x, unsigned y) {
			if ((treeX[root].x1 <= x) && (x <= treeX[root].x2)) {
				treeX[root].sum += adder;
				addY(root, 1, adder, x, y);
				if (treeX[root].x1 == treeX[root].x2) return;
				unsigned m = (treeX[root].x1 + treeX[root].x2) >> 1;
				if (x <= m)
					add(root<<1, adder, x, y);
				else
					add(root<<1|1, adder, x, y);
			}
		}
		int query(unsigned root, unsigned x1, unsigned y1, unsigned x2, unsigned y2) {
			if ((x1 <= treeX[root].x1) && (treeX[root].x2 <= x2))
				return queryY(root, 1, x1, y1, x2, y2);
			if (treeX[root].x1 == treeX[root].x2) return 0;

			unsigned m = (treeX[root].x1 + treeX[root].x2) >> 1;
			int ret = 0;
			if (x1 <= m)
				ret += query(root<<1, x1, y1, x2, y2);
			if (m <= x2)
				ret += query(root<<1|1, x1, y1, x2, y2);

			return ret;
		}
		void dump() {
			cout << "TreeX:" << endl;
			for (unsigned i = 0; i < height; ++i) {
				cout << setw(3) << treeX[i].sum << " ";
			}
			cout << endl;
			cout << "TreeY:" << endl;
			for (unsigned i = 0; i < height; ++i) {
				for (unsigned j = 0; j < width; ++j) {
					cout << setw(3) << treeY[i][j].sum << " ";
				}
				cout << endl;
			}
			cout << endl;
		}

	private:
		segTree() {}
		void buildY(unsigned rootX, unsigned rootY, unsigned x1, unsigned y1, unsigned x2, unsigned y2) {
			if (y1 > y2) return;
			treeY[rootX][rootY].x1 = x1;
			treeY[rootX][rootY].x2 = x2;
			treeY[rootX][rootY].y1 = y1;
			treeY[rootX][rootY].y2 = y2;
			if (y1 == y2) {
				// buildX ensures that x1 != x2 && y1 == y2 happens before
				// x1 == x2 && y1 != y2
				if (x1 == x2)
					treeY[rootX][rootY].sum = arr[x1 - 1][y1 - 1];
				else {
					unsigned mx = (x1 + x2) >> 1;
					// by this time, the nodes below are ready for query
					treeY[rootX][rootY].sum = query(1, x1, y1, mx, y2) + query(1, mx+1, y1, x2, y2);
				}
				return;
			}
			unsigned m = (y1 + y2) >> 1;
			buildY(rootX, rootY<<1, x1, y1, x2, m);
			buildY(rootX, rootY<<1|1, x1, m+1, x2, y2);
			treeY[rootX][rootY].sum = treeY[rootX][rootY<<1].sum + treeY[rootX][rootY<<1|1].sum;
		}
		void addY(unsigned rootX, unsigned rootY, int adder, unsigned x, unsigned y) {
			if ((treeY[rootX][rootY].y1 <= y) && (y <= treeY[rootX][rootY].y2)) {
				treeY[rootX][rootY].sum += adder;
				if (treeY[rootX][rootY].y1 != treeY[rootX][rootY].y2) {
					unsigned m = (treeY[rootX][rootY].y1 + treeY[rootX][rootY].y2) >> 1;
					if (y <= m)
						addY(rootX, rootY<<1, adder, x, y);
					else
						addY(rootX, rootY<<1|1, adder, x, y);
				}
			}
		}
		int queryY(unsigned rootX, unsigned rootY, unsigned x1, unsigned y1, unsigned x2, unsigned y2) {
			if ((y1 <= treeY[rootX][rootY].y1) && (treeY[rootX][rootY].y2 <= y2))
				return treeY[rootX][rootY].sum;
			if (treeY[rootX][rootY].y1 == treeY[rootX][rootY].y2) return 0;

			unsigned m = (treeY[rootX][rootY].y1 + treeY[rootX][rootY].y2) >> 1;
			int ret = 0;
			if (y1 <= m)
				ret += queryY(rootX, rootY<<1, x1, y1, x2, y2);
			if (m <= y2)
				ret += queryY(rootX, rootY<<1|1, x1, y1, x2, y2);

			return ret;
		}

		unsigned width, height; // size of 2d array
		node *treeX, **treeY; // treeX divide rows, treeY divide columns
		int **arr; // pointer to orginal array
};

int main(int argc, char **argv) {
	int arr[5][5] = {
			{9, 2, 7, 0, 5},
			{1, 2, 4, 8, 9},
			{7, 6, 3, 3, 2},
			{1, 9, 9, 0, 7},
			{6, 8, 4, 5, 1},
	};
	int **a;
	a = new int* [5];
	for (unsigned i = 0; i < 5; ++i) {
		a[i] = new int [5];
		for (unsigned j = 0; j < 5; ++j) {
			a[i][j] = arr[i][j];
		}
	}

	segTree sol(a, 5, 5);
	sol.dump();
	int res = sol.query(1, 2, 2, 3, 3);
	cout << "Sum of elements in specified submatrix is " << res << endl;
	sol.add(1, -3, 2, 3);
	res = sol.query(1, 2, 2, 3, 3);
	cout << "Sum of elements in specified submatrix is " << res << endl;

	for (unsigned i = 0; i < 5; ++i) {
		delete [] a[i];
	}
	delete [] a;

	return 0;
}
