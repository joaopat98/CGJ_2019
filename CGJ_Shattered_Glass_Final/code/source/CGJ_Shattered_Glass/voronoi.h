#ifndef VORONOI_H
#define VORONOI_H
#include <iostream>
#include <queue>
#include <set>
#include <math.h>
#include <limits>
#include <algorithm>
#include <time.h>
#include <random>
#include "engine.h"

using namespace std;

// Notation for working with points
typedef pair<double, double> point;

// Arc, event, segment & polygon datatypes
struct center;
struct arc;
struct seg;

struct event {
	double x;
	point p;
	arc* a;
	bool valid;

	event(double xx, point pp, arc* aa)
		: x(xx), p(pp), a(aa), valid(true) {}
};

struct arc {
	point p;
	arc* prev, * next;
	event* e;

	seg* s0, * s1;

	arc(point pp, arc* a = 0, arc* b = 0)
		: p(pp), prev(a), next(b), e(0), s0(0), s1(0) {}
};



struct seg {
	point start, end;
	bool done;

	seg(point p);
	void finish(point p);
};

//centers of polygons
struct polygon
{
	point center;
	vector <point> points;
	vector <seg*> edges;
	bool isBorder = false;
};



// Function declarations
void process_point();
void process_event();
void front_insert(point  p);

bool circle(point a, point b, point c, double* x, point* o);
void check_circle_event(arc* i, double x0);

bool intersect(point p, arc* i, point* result = 0);
point intersection(point p0, point p1, double l);

void finish_edges();
void print_output();
void fix_edges();
void create_polygons(vector<polygon>& regions);
void print_polygons(vector<polygon>& regions);

vector<polygon> compute(int sites, double xpos, double ypos);

void createFragments(Program program, float scale, float thickness, vec2 impact, vector<Mesh>* meshes, vector<vec3>* centers, vector<bool>* isBorder);

// "Greater than" comparison, for reverse sorting in priority queue.
struct gt {
	bool operator()(point a, point b) { return a.first == b.first ? a.second > b.second : a.first > b.first; }
	bool operator()(event* a, event* b) { return a->x > b->x; }
};

#endif VORONOI_H
