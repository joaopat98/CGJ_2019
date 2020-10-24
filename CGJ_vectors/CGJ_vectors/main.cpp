#include <iostream>

#include "vectors.h"

using namespace std;

bool test_colinear(vec3 v1, vec3 v2) {
	vec3 v1n = v1.normalize();
	vec3 v2n = v2.normalize();
	return v1n == v2n || v1n == -v2n;
}

void compute(vec3 view, vec3 up) {

	cout << "view = " << view.to_string();
	cout << "up = " << up.to_string();

	if (test_colinear(view, up))
	{
		cout << "cannot compute, view and up are colinear!" << endl;
		return;
	}
	vec3 v = view.normalize();
	vec3 w = up.cross(v).normalize();
	vec3 u = v.cross(w);
	cout << "u = " << u.to_string();
	cout << "v = " << v.to_string();
	cout << "w = " << w.to_string();
}

int main() {
	/*
	vec3 v1 = vec3(1, 2, 3);
	cout << "v1 = " << v1.to_string();
	//copy
	vec3 v2 = vec3(v1);
	cout << "v2 = v1 = " << v2.to_string();
	//sum
	vec3 v3 = v1 + v2;
	cout << "v3 = v1 + v2 = " << v3.to_string();
	//subtraction
	vec3 v4 = v3 - v1;
	cout << "v4 = v3 - v1 = " << v4.to_string();
	//negative
	vec3 v5 = -v4;
	cout << "v5 = -v4 = " << v5.to_string();
	//multiplication
	vec3 v6 = v5 * 4;
	vec3 v7 = 4 * v5;
	cout << "v6 = v5 * 4 = " << v6.to_string();
	cout << "v7 = 4 * v5 = " << v7.to_string();
	//division
	vec3 v8 = v6 / 4;
	cout << "v8 = v6 / 4 = " << v8.to_string();
	//equality
	cout << "v6 == v7 = " << ((v6 == v7)? "true" : "false") << endl;
	//length
	cout << "|v8| = " << v8.length() << endl;
	//dot product
	vec3 v9 = vec3(1, 1, 1);
	cout << "v9 = " << v9.to_string();
	cout << "v9.v8 = " << v9.dot(v8) << endl;
	//cross product
	vec3 v10 = vec3(1, -1, 1);
	cout << "v10 = " << v10.to_string();
	vec3 v11 = v9.cross(v10);
	cout << "v11 = v9 x v10 = " << v11.to_string();
	*/
	
	vec3 vecs[] = { {1,0,0},{0,2,0},{0,0,3} };
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			compute(vecs[i], vecs[j]);
			cout << "\n";
		}
	}
	getchar();
}