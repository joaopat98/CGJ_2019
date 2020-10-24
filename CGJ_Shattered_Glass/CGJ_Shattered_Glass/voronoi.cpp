#include "voronoi.h"

priority_queue<point, vector<point>, gt> points; // site events
priority_queue<event*, vector<event*>, gt> events; // circle events

// Bounding box coordinates.
double X0 = 0, X1 = 0, Y0 = 0, Y1 = 0;

// Array of output segments.
vector<seg*> output;

// First item in the parabolic front linked list.
arc* voronoi_root = 0;

seg::seg(point p) : start(p), end(0, 0), done(false) { output.push_back(this); }

// Set the end point and mark as "done."
void seg::finish(point p) { if (done) return; end = p; done = true; }

vector<polygon> compute(int sites, double xpos, double ypos)
{
	X0 = 0;
	X1 = 1;
	Y0 = 0;
	Y1 = 1;

	// Calculating 1% of the bounding box
	double dx = (X1 - X0 + 1) / 100.0; double dy = (Y1 - Y0 + 1) / 100.0;

	// Set the seed
	//srand(static_cast<unsigned>(time(0)));
	//srand(time(NULL)%1000 *5);

	mt19937_64 generator(time(NULL));

	//sites = (double)rand() / (RAND_MAX) * 3
	//double sigma = (double)rand() / (RAND_MAX);

	//normal_distribution<double>sigma_distribution(1, 0.25);
	//double yes = sigma_distribution(generator);
	//cout << "\n\n\sigmaaaa " << yes << endl;
	normal_distribution<double>x_distribution(xpos, 0.15);
	normal_distribution<double>y_distribution(ypos, 0.15);

	cout << "tempe " << time(NULL);

	cout << "\n\n\nsites " << sites << endl;

	vector<polygon> regions(sites);
	for (int i = 0; i < sites; i++)
	{
		point p;
		//p.first = (double)rand() / (RAND_MAX) * 3;
		//p.second = (double)rand() / (RAND_MAX) * 3;

		// Around a point
		p.first = x_distribution(generator);
		while (p.first < X0 || p.first > X1)
			p.first = x_distribution(generator);

		p.second = y_distribution(generator);
		while (p.second < Y0 || p.second > Y1)
			p.second = y_distribution(generator);


		//cout << p.first << " " << p.second << endl;
		points.push(p);
		regions[i].center = p;
	}

	// Add 1% margins to the bounding box.
	X0 -= dx; X1 += dx; Y0 -= dy; Y1 += dy;

	//vector<polygon> regions(5);
	//
	//point p1;
	//point p2;
	//point p3;
	//point p4;
	//point p5;
	//
	//p1.first = 1;
	//p1.second = 1;
	//p2.first = 2;
	//p2.second = 1;
	//p3.first = 1;
	//p3.second = 2;
	//p4.first = 2.5;
	//p4.second = 2.5;
	//p5.first = 2.5;
	//p5.second = 1.5;
	//
	//points.push(p1);
	//points.push(p2);
	//points.push(p3);
	//points.push(p4);
	//points.push(p5);
	//regions[0].center = p1;
	//regions[1].center = p2;
	//regions[2].center = p3;
	//regions[3].center = p4;
	//regions[4].center = p5;


	//vector<polygon> regions(5);
	//// {"sites":[100,100,200,100,100,200,250,250,250,150],"queries":[]}

	// DEBUG
	//while (cin >> p.first >> p.second) {
	//   // Keep track of bounding box size.
	//   if (p.first < X0) X0 = p.first;
	//   if (p.second < Y0) Y0 = p.second;
	//   if (p.first > X1) X1 = p.first;
	//   if (p.second > Y1) Y1 = p.second;
	//}


	// Process the queues; select the top element with smaller x coordinate.
	while (!points.empty())
		if (!events.empty() && (events.top()->x <= points.top().first))
			process_event();
		else
			process_point();

	// After all points are processed, do the remaining circle events.
	while (!events.empty())
		process_event();

	finish_edges(); // Clean up dangling edges.
	print_output(); // Output the voronoi diagram.

	create_polygons(regions);
	//print_polygons(regions);
	return regions;
}

void process_point()
{
	// Get the next point from the queue.
	point p = points.top();
	points.pop();

	// Add a new arc to the parabolic front.
	front_insert(p);
}

void process_event()
{
	// Get the next event from the queue.
	event* e = events.top();
	events.pop();

	if (e->valid) {
		// Start a new edge.
		seg* s = new seg(e->p);



		// Remove the associated arc from the front.
		arc* a = e->a;
		if (a->prev) {
			a->prev->next = a->next;
			a->prev->s1 = s;
		}
		if (a->next) {
			a->next->prev = a->prev;
			a->next->s0 = s;
		}

		// Finish the edges before and after a.
		if (a->s0) a->s0->finish(e->p);
		if (a->s1) a->s1->finish(e->p);

		// Recheck circle events on either side of p:
		if (a->prev) check_circle_event(a->prev, e->x);
		if (a->next) check_circle_event(a->next, e->x);
	}
	delete e;
}

void front_insert(point p)
{
	if (!voronoi_root) {
		voronoi_root = new arc(p);
		return;
	}

	// Find the current arc(s) at height p.second (if there are any).
	for (arc* i = voronoi_root; i; i = i->next) {
		point z, zz;
		if (intersect(p, i, &z)) {
			// New parabola intersects arc i.  If necessary, duplicate i.
			if (i->next && !intersect(p, i->next, &zz)) {
				i->next->prev = new arc(i->p, i, i->next);
				i->next = i->next->prev;
			}
			else i->next = new arc(i->p, i);
			i->next->s1 = i->s1;

			// Add p between i and i->next.
			i->next->prev = new arc(p, i, i->next);
			i->next = i->next->prev;

			i = i->next; // Now i points to the new arc.

			// Add new half-edges connected to i's endpoints.
			i->prev->s1 = i->s0 = new seg(z);
			i->next->s0 = i->s1 = new seg(z);

			// Check for new circle events around the new arc:
			check_circle_event(i, p.first);
			check_circle_event(i->prev, p.first);
			check_circle_event(i->next, p.first);

			return;
		}
	}

	// Special case: If p never intersects an arc, append it to the list.
	arc* i;
	for (i = voronoi_root; i->next; i = i->next); // Find the last node.

	i->next = new arc(p, i);
	// Insert segment between p and i
	point start;
	start.first = X0;
	start.second = (i->next->p.second + i->p.second) / 2;
	i->s1 = i->next->s0 = new seg(start);
}

// Look for a new circle event for arc i.
void check_circle_event(arc* i, double x0)
{
	// Invalidate any old event.
	if (i->e && i->e->x != x0)
		i->e->valid = false;
	i->e = NULL;

	if (!i->prev || !i->next)
		return;

	double x;
	point o;

	if (circle(i->prev->p, i->p, i->next->p, &x, &o) && x > x0) {
		// Create new event.
		i->e = new event(x, o, i);
		events.push(i->e);
	}
}

// Find the rightmost point on the circle through a,b,c.
bool circle(point a, point b, point c, double* x, point* o)
{
	// Check that bc is a "right turn" from ab.
	if ((b.first - a.first) * (c.second - a.second) - (c.first - a.first) * (b.second - a.second) > 0)
		return false;

	// Algorithm from O'Rourke 2ed p. 189.

	double A = b.first - a.first, B = b.second - a.second,
		C = c.first - a.first, D = c.second - a.second,
		E = A * (a.first + b.first) + B * (a.second + b.second),
		F = C * (a.first + c.first) + D * (a.second + c.second),
		G = 2 * (A * (c.second - b.second) - B * (c.first - b.first));

	if (G == 0) return false;  // Points are co-linear.

	// Point o is the center of the circle.
	o->first = (D * E - B * F) / G;
	o->second = (A * F - C * E) / G;

	// o.first plus radius equals max x coordinate.
	*x = o->first + sqrt(pow(a.first - o->first, 2) + pow(a.second - o->second, 2));
	return true;
}

// Will a new parabola at point p intersect with arc i?
bool intersect(point p, arc* i, point* result)
{
	if (i->p.first == p.first) return false;

	double a, b;
	if (i->prev) // Get the intersection of i->prev, i.
		a = intersection(i->prev->p, i->p, p.first).second;
	if (i->next) // Get the intersection of i->next, i.
		b = intersection(i->p, i->next->p, p.first).second;

	if ((!i->prev || a <= p.second) && (!i->next || p.second <= b)) {
		result->second = p.second;

		result->first = (i->p.first * i->p.first + (i->p.second - result->second) * (i->p.second - result->second) - p.first * p.first)
			/ (2 * i->p.first - 2 * p.first);

		return true;
	}
	return false;
}

// Where do two parabolas intersect?
point intersection(point p0, point p1, double l)
{
	point res, p = p0;

	double z0 = 2 * (p0.first - l);
	double z1 = 2 * (p1.first - l);

	if (p0.first == p1.first)
		res.second = (p0.second + p1.second) / 2;
	else if (p1.first == l)
		res.second = p1.second;
	else if (p0.first == l) {
		res.second = p0.second;
		p = p1;
	}
	else {
		// Use the quadratic formula.
		double a = 1 / z0 - 1 / z1;
		double b = -2 * (p0.second / z0 - p1.second / z1);
		double c = (p0.second * p0.second + p0.first * p0.first - l * l) / z0
			- (p1.second * p1.second + p1.first * p1.first - l * l) / z1;

		res.second = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
	}
	// Plug back into one of the parabola equations.
	res.first = (p.first * p.first + (p.second - res.second) * (p.second - res.second) - l * l) / (2 * p.first - 2 * l);
	return res;
}

void finish_edges()
{
	// Advance the sweep line so no parabolas can cross the bounding box.
	double l = X1 + (X1 - X0) + (Y1 - Y0);

	// Extend each remaining segment to the new parabola intersections.
	for (arc* i = voronoi_root; i->next; i = i->next)
		if (i->s1)
			i->s1->finish(intersection(i->p, i->next->p, l * 2));
}

bool equal_coordinate(point p, point q)
{
	return p.first == q.first || p.second == q.second;
}

bool belongs_to_border(point p)
{
	return p.first == X0 || p.first == X1 || p.second == Y0 || p.second == Y1;
}

bool belong_to_same_edge(point p, point q, vector <seg*> edges)
{
	for (int i = 0; i < edges.size(); i++)
	{
		if (p == edges[i]->start && q == edges[i]->end || q == edges[i]->start && p == edges[i]->end)
			return true;
	}
	return false;
}

bool is_duplicate(point p, vector <point> points)
{
	for (int i = 0; i < points.size(); i++)
	{
		if (p == points[i])
			return true;
	}
	return false;
}

point find_corner(point p, point q)
{
	point corner;
	if (p.first == X0 || q.first == X0)
	{
		corner.first = X0;
		if (p.second == Y0 || q.second == Y0)
			corner.second = Y0;
		else if (p.second == Y1 || q.second == Y1)
			corner.second = Y1;
	}

	if (p.first == X1 || q.first == X1)
	{
		corner.first = X1;
		if (p.second == Y0 || q.second == Y0)
			corner.second = Y0;
		else if (p.second == Y1 || q.second == Y1)
			corner.second = Y1;
	}
	return corner;
}


float distance(point begin, point finish)
{
	return pow((begin.first - finish.first), 2) + pow((begin.second - finish.second), 2);
}

point middle_point(point begin, point finish)
{
	point middle;
	middle.first = (begin.first + finish.first) / 2;
	middle.second = (begin.second + finish.second) / 2;
	return middle;
}

float interp_x(float x_dif, float y_dif, float target_x)
{
	return target_x * y_dif / x_dif;
}

float interp_y(float x_dif, float y_dif, float target_y)
{
	return target_y * x_dif / y_dif;
}

void fix_point(point& toFix, point& other)
{
	if (toFix.first < X0) { toFix.second = other.second + interp_x(toFix.first - other.first, toFix.second - other.second, X0 - other.first); toFix.first = X0; }
	if (toFix.first > X1) { toFix.second = other.second + interp_x(toFix.first - other.first, toFix.second - other.second, X1 - other.first); toFix.first = X1; }

	if (toFix.second < Y0) { toFix.first = other.first + interp_y(toFix.first - other.first, toFix.second - other.second, Y0 - other.second);  toFix.second = Y0; }
	if (toFix.second > Y1) { toFix.first = other.first + interp_y(toFix.first - other.first, toFix.second - other.second, Y1 - other.second);  toFix.second = Y1; }

}
void fix_edges()
{
	vector<seg*>::iterator i;

	//// DEBUG ----------------------------------------------------------
	//cout << "before" << endl;
	//for (i = output.begin(); i != output.end(); i++) {
	//	cout << (*i)->start.first << " " << (*i)->start.second << " " << (*i)->end.first << " " << (*i)->end.second << endl;
	//}
	//cout << "after" << endl;
	//// ----------------------------------------------------------------


	for (i = output.begin(); i != output.end(); i++) {
		fix_point((*i)->start, (*i)->end);
		fix_point((*i)->end, (*i)->start);
		// DEBUG
		//cout << (*i)->start.first << " " << (*i)->start.second << " " << (*i)->end.first << " " << (*i)->end.second << endl;
	}
	//DEBUG
	//cout << endl;
}

void print_output()
{
	//// DEBUG ----------------------------------------------------------
	//// Bounding box coordinates.
	// cout << "boundingbox"<< endl;
	// cout << X0 << " "<< X1 << " " << Y0 << " " << Y1 << endl;
	// cout << "------------" << endl;
	//// ----------------------------------------------------------------
	fix_edges();
}

void create_polygons(vector<polygon>& regions)
{
	point neighbor1;
	point neighbor2;


	// Check 2 Nearest Neighboring centers of an edge and adding the edges to the neighboring polygon
	for (int i = 0; i < output.size(); i++)
	{
		//discard wrongly computed edges
		if (output[i]->start == output[i]->end) continue;

		// Default Values (put to 1000000 because we consider very far away neighbors)
		// (cannot set them to max_float as we will calculate very high values squared - would result in overflow)
		neighbor1.first = 1000000;
		neighbor1.second = 1000000;
		neighbor2.first = 1000000;
		neighbor2.second = 1000000;
		// Nearest 2 Neighbors
		for (int j = 0; j < regions.size(); j++)
		{
			point m = middle_point(output[i]->start, output[i]->end);
			float d1 = distance(m, regions[j].center);
			float d2 = distance(m, neighbor1);
			float d3 = distance(m, neighbor2);
			if (d1 < d2)
			{
				neighbor2 = neighbor1;
				neighbor1 = regions[j].center;
			}
			else if (d1 < d3)
			{
				neighbor2 = regions[j].center;
			}
		}

		// Adding edge to the polygon
		for (int j = 0; j < regions.size(); j++)
		{
			if (regions[j].center == neighbor1 || regions[j].center == neighbor2)
				regions[j].edges.push_back(output[i]);
		}

	}

	//DEBUG
	//for (int i = 0; i < regions.size(); i++)
	//{
	//	cout << "CENTER: " << regions[i].center.first << " " << regions[i].center.second << endl;
	//	cout << "EDGES: " << endl;
	//	for (int j = 0; j < regions[i].edges.size(); j++)
	//	{
	//		cout << regions[i].edges[j]->start.first << " " << regions[i].edges[j]->start.second << " " << regions[i].edges[j]->end.first << " " << regions[i].edges[j]->end.second << endl;
	//	}
	//	cout << endl;
	//}
	// Adding the points (in order) to the polygon
	// The polygons will be created with CW & CCW order so we just have to insert them following the last point added
	for (int i = 0; i < regions.size(); i++)
	{
		//cout << "Region: " << i << endl;
		vec3 p1 = vec3(regions[i].center.first - regions[i].edges[0]->start.first, regions[i].center.second - regions[i].edges[0]->start.second, 0);
		vec3 p2 = vec3(regions[i].center.first - regions[i].edges[0]->end.first, regions[i].center.second - regions[i].edges[0]->end.second, 0);
		vec3 p3 = p1.cross(p2);

		// Put the first 2 points of the first edge
		if (p3.z < 0)
		{
			regions[i].points.push_back(regions[i].edges[0]->start);
			regions[i].points.push_back(regions[i].edges[0]->end);
		}
		else
		{
			regions[i].points.push_back(regions[i].edges[0]->end);
			regions[i].points.push_back(regions[i].edges[0]->start);
		}

		//cout << "Found an initial point: " << regions[i].points.front().first << " " << regions[i].points.front().second << endl;
		//cout << "Found an initial point: " << regions[i].points.back().first << " " << regions[i].points.back().second << endl;

		// Indicates if we're expecting the next point to be on a border
		bool nextOnBorder = false;
		if (belongs_to_border(regions[i].points.back())) { nextOnBorder = true; regions[i].isBorder = true; }

		// Cycle through all the number of edges to guarantee we've added all of them
		// We want to keep adding point until we have a full cycle, not counting the first iteration
		while (regions[i].points.size() == 2 ||
			(!equal_coordinate(regions[i].points.back(), regions[i].points.front()) &&
				!belong_to_same_edge(regions[i].points.back(), regions[i].points.front(), regions[i].edges))
			)
		{
			// This polygon will include at least one border
			if (nextOnBorder && belongs_to_border(regions[i].points.back()))
			{
				for (int k = 0; k < regions[i].edges.size(); k++)
				{
					// Only one border (check if there is any on a border and if its the same border,
					// avoiding repeated edges except if its the first one)
					if (belongs_to_border(regions[i].edges[k]->start) &&
						equal_coordinate(regions[i].points.back(), regions[i].edges[k]->start) &&
						(!is_duplicate(regions[i].edges[k]->start, regions[i].points) ||
							is_duplicate(regions[i].edges[k]->start, regions[i].points) &&
							regions[i].edges[k]->start == regions[i].points.front()))
					{
						regions[i].points.push_back(regions[i].edges[k]->start);
						nextOnBorder = false;
						//cout << "Found a start border: " << regions[i].points.back().first << " " << regions[i].points.back().second << endl;
						break;
					}

					if (belongs_to_border(regions[i].edges[k]->end) &&
						equal_coordinate(regions[i].points.back(), regions[i].edges[k]->end) &&
						(!is_duplicate(regions[i].edges[k]->end, regions[i].points) ||
							is_duplicate(regions[i].edges[k]->end, regions[i].points) &&
							regions[i].edges[k]->end == regions[i].points.front()))
					{
						regions[i].points.push_back(regions[i].edges[k]->end);
						nextOnBorder = false;
						//cout << "Found an end border: " << regions[i].points.back().first << " " << regions[i].points.back().second << endl;
						break;
					}

					// One corner (check if there is any on a border and avoid repeated points )
					// (Polygons with 2 corners are not treated since that case wont appear in this implementation as the cracks wont be that big)
					if (belongs_to_border(regions[i].edges[k]->start) &&
						!equal_coordinate(regions[i].points.back(), regions[i].edges[k]->start))
					{
						point corner;
						corner = find_corner(regions[i].points.back(), regions[i].edges[k]->start);

						regions[i].points.push_back(corner);
						nextOnBorder = true;
						regions[i].isBorder = true;
						
						//cout << "Found a start corner: " << regions[i].points.back().first << " " << regions[i].points.back().second << endl;
						break;
					}

					if (belongs_to_border(regions[i].edges[k]->end) &&
						!equal_coordinate(regions[i].points.back(), regions[i].edges[k]->end))
					{
						point corner;
						corner = find_corner(regions[i].points.back(), regions[i].edges[k]->end);

						regions[i].points.push_back(corner);
						nextOnBorder = true;
						regions[i].isBorder = true;
						//cout << "Found an end corner: " << regions[i].points.back().first << " " << regions[i].points.back().second << endl;
						break;
					}
				}
			}

			// Find next edge while avoiding adding duplicates
			else
			{
				for (int k = 0; k < regions[i].edges.size(); k++)
				{
					if (regions[i].points.back() == regions[i].edges[k]->start && !is_duplicate(regions[i].edges[k]->end, regions[i].points))
					{
						regions[i].points.push_back(regions[i].edges[k]->end);
						if (belongs_to_border(regions[i].points.back())) { nextOnBorder = true; regions[i].isBorder = true;}
						//cout << "Found a new start point: " << regions[i].points.back().first << " " << regions[i].points.back().second << endl;
						break;
					}
					if (regions[i].points.back() == regions[i].edges[k]->end && !is_duplicate(regions[i].edges[k]->start, regions[i].points))
					{
						regions[i].points.push_back(regions[i].edges[k]->start);
						if (belongs_to_border(regions[i].points.back())) { nextOnBorder = true; regions[i].isBorder = true; }
						//cout << "Found a new end point: " << regions[i].points.back().first << " " << regions[i].points.back().second << endl;
						break;
					}
				}
			}

		}

	}
}


void print_polygons(vector <polygon>& regions)
{
	cout << endl;
	cout << "POLYGONS: ----------------------------------------------------------------------------";
	cout << endl;

	for (int i = 0; i < regions.size(); i++) {
		cout << "Center: " << regions[i].center.first << " " << regions[i].center.second << endl;
		cout << "Points:";
		cout << endl;
		for (int j = 0; j < regions[i].points.size(); j++)
			cout << regions[i].points[j].first << " " << regions[i].points[j].second << endl;
		cout << "--------------------------------------------" << endl;
	}
}

void createFragments(Program program, float scale, float thickness, vec2 impact, vector<Mesh>* meshes, vector<vec3>* centers,vector<bool>* isBorder) {
	auto polygons = compute(200, impact.x, impact.y);
	for (auto polygon : polygons) {
		int numVerts = polygon.points.size();

		vec3 center = { 0,0,0 };
		for (auto point : polygon.points) {
			center += vec3{ ((float)point.first + 0.02f) / 1.04f, ((float)point.second + 0.02f) / 1.04f, 0 };
		}
		center /= polygon.points.size();
		center *= scale;
		center.z = thickness / 2;

		centers->push_back(center);

		vector<vec3> vertices_redundant;
		vector<vec3> vertices;

		for (auto point : polygon.points) {
			vertices_redundant.push_back(vec3{ ((float)point.first + 0.02f) / 1.04f * scale,((float)point.second + 0.02f) / 1.04f * scale,0 });
		}

		for (int i = 0; i < numVerts; i++)
		{
			vec3 cur = vertices_redundant[i];
			vec3 prev = vertices_redundant[(i - 1) % numVerts];
			vec3 next = vertices_redundant[(i + 1) % numVerts];
			if ((cur - prev).normalize() != (next - cur).normalize())
				vertices.push_back(cur - center);
		}

		numVerts = vertices.size();

		for (int i = 0; i < numVerts; i++) {
			vertices.push_back(vertices[i] + (AXIS3_Z * thickness));
		}
		vector<vec2> texCoords = { {0,0} };
		vector<vec3> normals = { {0,0,1},{0,0,-1} };
		for (int i = 0; i < numVerts; i++)
		{
			vec3 next = vertices[(i + 1) % numVerts] - vertices[i];
			vec3 down = vertices[i + numVerts] - vertices[i];
			normals.push_back(down.cross(next).normalize());
		}

		vector<Face> faces;
		Face top, bottom;
		for (int i = 0; i < numVerts; i++)
		{
			top.vertices.push_back(i);
			bottom.vertices.push_back(numVerts * 2 - i - 1);
			top.texCoords.push_back(0);
			bottom.texCoords.push_back(0);
			top.normals.push_back(1);
			bottom.normals.push_back(0);
		}
		faces.push_back(top);
		faces.push_back(bottom);
		for (int i = 0; i < numVerts; i++)
		{
			Face f;
			f.vertices = { i, i + numVerts, (i + 1) % numVerts + numVerts, (i + 1) % numVerts };
			for (int k = 0; k < 4; k++) f.texCoords.push_back(0);
			for (int k = 0; k < 4; k++) f.normals.push_back(i + 2);
			faces.push_back(f);
		}
		meshes->push_back(Mesh::fromFaces(program, vertices, texCoords, normals, faces));
		isBorder->push_back(polygon.isBorder);
	}
}