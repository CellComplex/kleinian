/* kleinian_group.cc */

/* basic object, with all the data and functions; the Kleinian group is a *global* variable,
	so that the GL functions (which *must* take void as an argument) can use the data */

class kleinian_group{

	public:
		vector<mat > GENERATORS;		// semigroup generators
		automaton AUTOMATON;			// automaton defining a combing
		
		vector<vertex > ELEMENTS;		// group elements
		vector<triangle > TRIANGLES;	// orbit classes of triangles
		vector<vec > COLORS;			// colors of triangle orbits
		
		vector<triangle > DRAW_TRIANGLES;	// actual Euclidean triangles in R^3 to draw
		vector<vec > DRAW_NORMALS;			// normals to actual Euclidean triangles
		vector<vec > DRAW_COLORS;			// colors to actual Euclidean triangles in R^3 to draw
		
		vec	BACKGROUND_COLOR;				// background for GLUT animation
		
		vector<triangle > draw_triangles;
		bool draw_triangles_generated;
		bool do_prune;	// should be "true" unless you *know* the automaton defines a combing

		mat CAMERA;
		
		string MODE;	// could be "GLUT", "X" or "command"
		
		void generate_dialog();		// dialog to generate triangles from group
		void spine_example();		// hardcoded example: super-ideal triangle group
		void torus_example();		// hardcoded example: once-punctured torus group
		void napoleon_example();	// hardcoded example: hyperbolic napoleon theorem
		void clever_prune_vertices(int dep);	// needs to be more clever
		void prune_vertices(int dep);	// removes redundant vertices of depth dep
		void generate_to_depth(int n);	// generate elements out to depth n using combing
		void list_elements();	// output to cout list of group elements

		void generate_triangles();	
		void fancy_generate_triangles();	// try to get curvy Poincare triangles by subdivision
		bool generate_fancy;
		dbl mesh;	// mesh size for fancy triangles
		void sort_triangles();		// sort by height - for .eps output
		
		void draw_X();		// draw to X windows
		void draw_glut();	// draw to GLUT
		void draw_eps();	// draw to eps
		
		void read_triangles_from_file(ifstream &);		// read triangle data from file
		void read_group_from_file(ifstream &);			// read group data from file
		void write_triangles_to_file();		// write triangle data to file
		void write_group_to_file();			// write group data to file

		void X_user_interface();			// top-level user interaction routine
};

void kleinian_group::generate_dialog(){		// dialog to generate triangles from group
	int depth;
	char c;
	
	depth=12;	// default

	cout << "enter depth to generate to\n";
	cout << "(should be < 13 unless group is elementary\n";
	cout << "or has very small fundamental domain): ";
	cin >> depth;
	
	cout << "generating triangles \n";
	generate_to_depth(depth);
	
	cout << "fancy curvilinear triangles (y/n)?: ";
	cin >> c;
	if(c=='y'){
		generate_fancy=true;
		mesh = 0.05;	// default
		cout << "mesh size to subdivide triangles to\n";
		cout << "(should be between 0.02 and 0.2): ";
		cin >> mesh;
	} else {
		generate_fancy=false;
	};
	cout << "entering GLUT mode \n";
	MODE="GLUT";
};

void kleinian_group::spine_example(){	// this is a hardcoded example; 
	GENERATORS.clear();
	
	int i;
	mat a,b,A,B;
	
	cout << "generating example group\n";
	
	a=build_mat(0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 1.0,0.0,0.0,0.0, 0.0,0.0,0.0,1.0); // alpha
	b=build_mat(0.0,0.0,-1.0,0.0, 1.0,0.0,0.0,0.0, 0.0,-1.0,0.0,0.0, 0.0,0.0,0.0,1.0); // beta
	A=a*a;	// alpha^-1
	B=b*b;	// beta^-1
	
	// 11 nontrivial elements of A_4
	
	GENERATORS.push_back(a);	// gen 0
	GENERATORS.push_back(A);	// gen 1
	GENERATORS.push_back(b);	// gen 2
	GENERATORS.push_back(B);	// gen 3
	GENERATORS.push_back(a*b);	// gen 4
	GENERATORS.push_back(a*B);	// gen 5
	GENERATORS.push_back(b*a);	// gen 6
	GENERATORS.push_back(b*A);	// gen 7
	GENERATORS.push_back(A*b);	// gen 8
	GENERATORS.push_back(B*a);	// gen 9
	GENERATORS.push_back(a*b*a);	// gen 10
		
	dbl len;
	dbl ord;	
	mat x,y,z,X,Y,Z,m,M;
	cout << "enter order (should be an integer, at least 6): ";
	cin >> ord;
	len=acosh((cos(PI/ord)/sqrt(3.0))/(sin(PI/ord)*sqrt(2.0)/sqrt(3.0)));
	
	x=build_mat(0,2,TWOPI/ord);
	y=build_mat(0,1,PI/4.0);
	z=build_mat(2,3,len);
	X=build_mat(2,0,TWOPI/ord);
	Y=build_mat(1,0,PI/4.0);
	Z=build_mat(2,3,-1.0*len);
	
	m=y*z*x*Y*Z;
	M=y*z*X*Y*Z;
	
	// rotation of order ord
	
	GENERATORS.push_back(m);
	GENERATORS.push_back(M);

	AUTOMATON.clear();
	vector< pair<int,int> > V;

	// state 0
	V.clear();	
	for(i=0;i<13;i++){
		V.push_back(make_pair(i,0));
	};
	AUTOMATON.push_back(V);
	
	TRIANGLES.clear();
	triangle T;
	T.v[0]=z*build_vec(0.0,0.0,0.0,1.0);
	T.v[1]=build_vec(0.0,0.0,0.0,1.0);
	T.v[2]=m*build_vec(0.0,0.0,0.0,1.0);
	TRIANGLES.push_back(T);
	
	COLORS.clear();
//	COLORS.push_back(build_vec(0.0,0.39215686,0.0,0.0));			// dark green
	COLORS.push_back(build_vec(0.19607843,0.8039215686,0.19607843,0.0));		// lime green
//	COLORS.push_back(build_vec(0.8,0.8,0.8,0.0));			// light gray

//	CAMERA=id_mat();		// camera skew angle
	CAMERA=build_mat(2,3,-0.5)*build_mat(0,2,0.7);	// alternate camera angle
	
	BACKGROUND_COLOR=build_vec(0.9, 0.9, 0.99, 1.0);	// light blue
	
	draw_triangles_generated=false;
	do_prune=true;
};

void kleinian_group::torus_example(){	// this is a hardcoded example; 
	GENERATORS.clear();
	
	mat a,b,A,B;
	dbl twist,shear;
	twist=0.0;	// default
	shear=0.0;	// default
	cout << "generating torus group\n";
	cout << "enter twist and shear parameters\n";
	cout << "(two numbers separated by a space; e.g. 0 0): ";
	cin >> twist >> shear;
	
	a=build_mat(0,3,0.54930614)*build_mat(0,2,-1.0*twist)*build_mat(1,3,-1.0*shear)*build_mat(0,3,0.54930614)*
		build_mat(0,1,1.04719755)*build_mat(0,3,0.54930614)*build_mat(0,2,twist)*build_mat(1,3,shear)*
		build_mat(0,3,0.54930614)*build_mat(0,1,-1.04719755);

	A=build_mat(0,1,1.04719755)*build_mat(0,3,-0.54930614)*build_mat(0,2,-1.0*twist)*build_mat(1,3,-1.0*shear)*
		build_mat(0,3,-0.54930614)*build_mat(0,1,-1.04719755)*build_mat(0,3,-0.54930614)*
		build_mat(0,2,twist)*build_mat(1,3,shear)*build_mat(0,3,-0.54930614);
		
	b=build_mat(0,3,0.54930614)*build_mat(0,2,-1.0*twist)*build_mat(1,3,-1.0*shear)*build_mat(0,3,0.54930614)*
		build_mat(0,1,-1.04719755)*build_mat(0,3,0.54930614)*build_mat(0,2,0.0)*
		build_mat(0,3,0.54930614)*build_mat(0,1,1.04719755);

	B=build_mat(0,1,-1.04719755)*build_mat(0,3,-0.54930614)*build_mat(0,2,0.0)*
		build_mat(0,3,-0.54930614)*build_mat(0,1,1.04719755)*build_mat(0,3,-0.54930614)*
		build_mat(0,2,twist)*build_mat(1,3,shear)*build_mat(0,3,-0.54930614);
	
	GENERATORS.push_back(a);	// gen 0
	GENERATORS.push_back(A);	// gen 1
	GENERATORS.push_back(b);	// gen 2
	GENERATORS.push_back(B);	// gen 3
	
	AUTOMATON.clear();
	vector< pair<int,int> > V;

	// state 0
	V.clear();	
	V.push_back(make_pair(0,1));
	V.push_back(make_pair(1,2));
	V.push_back(make_pair(2,3));
	V.push_back(make_pair(3,4));
	AUTOMATON.push_back(V);
	
	// state 1
	V.clear();	
	V.push_back(make_pair(0,1));
	V.push_back(make_pair(2,3));
	V.push_back(make_pair(3,4));
	AUTOMATON.push_back(V);
	
	// state 2
	V.clear();	
	V.push_back(make_pair(1,2));
	V.push_back(make_pair(2,3));
	V.push_back(make_pair(3,4));
	AUTOMATON.push_back(V);
	
	// state 3
	V.clear();	
	V.push_back(make_pair(0,1));
	V.push_back(make_pair(1,2));
	V.push_back(make_pair(2,3));
	AUTOMATON.push_back(V);
	
	// state 4
	V.clear();	
	V.push_back(make_pair(0,1));
	V.push_back(make_pair(1,2));
	V.push_back(make_pair(3,4));
	AUTOMATON.push_back(V);
	
	TRIANGLES.clear();
	triangle T;
	
	T.v[0]=build_vec(42.8451,-74.2032,0,85.6903);
	T.v[1]=build_vec(42.8451,74.2032,0,85.6903);
	T.v[2]=build_vec(-85.6844,0.00336936,0,85.6903);
	TRIANGLES.push_back(T);

	T.v[0]=build_vec(42.8451,-74.2032,0,85.6903);
	T.v[1]=build_vec(-85.6844,0.00336936,0,85.6903);
	T.v[2]=build_vec(-128.533,-222.62,0,257.063);
	TRIANGLES.push_back(T);
	
	COLORS.clear();
	COLORS.push_back(build_vec(1.0,0.8,0.05,0.0));			// yellow
	COLORS.push_back(build_vec(0.345,0.235,0.196,0.0));		// brown
	
	CAMERA=id_mat();
	//build_mat(2,3,-0.5)*build_mat(0,2,0.7);	// camera skew angle
	
	BACKGROUND_COLOR=build_vec(0.9, 0.9, 0.99, 1.0);	// light blue
	
	draw_triangles_generated=false;
	do_prune=false;
};

void kleinian_group::napoleon_example(){	// this is a hardcoded example; 
	GENERATORS.clear();
	
	dbl a,b,c;
	dbl alpha,beta,gamma,lambda,mu,nu;
	/*
	int order;
	dbl angle, target_angle;
	bool finished;
	char x;
	
	
	cout << "generating hyperbolic napoleon group\n";
	cout << "enter order (integer > 1):\n";
	cin >> order;
	cout << "enter edge lengths a and b\n";
	cout << "(two numbers separated by a space; e.g. 1 1): ";
	cin >> a >> b;
	c=(a+b)/2.0;	// initial guess
	target_angle=TWOPI/((dbl) order);
		cout.precision(15);
	
	finished=false;
	
	while(finished==false){
		lambda=acos(cosh(a)/(cosh(a)+1.0));
		mu=acos(cosh(b)/(cosh(b)+1.0));
		nu=acos(cosh(c)/(cosh(c)+1.0));
		alpha=acos((cosh(b)*cosh(c)-cosh(a))/(sinh(b)*sinh(c)));
		beta=acos((cosh(a)*cosh(c)-cosh(b))/(sinh(a)*sinh(c)));
		gamma=acos((cosh(a)*cosh(b)-cosh(c))/(sinh(a)*sinh(b)));
		angle=alpha+beta+gamma+lambda+mu+nu;

		cout << "c = " << c << "\n";
		cout << "angle " << angle << "\n";
		cout << "target angle " << target_angle << "\n";
		cout << "close enough? ";
		cin >> x;
		if(x=='y'){
			finished=true;
		} else {
			cout << "enter new value for c ";
			cin >> c;
		};
	};
	
	cout << "side lengths \n";
	cout << " a " << a << "\n";
	cout << " b " << b << "\n";
	cout << " c " << c << "\n";
	cout << "angles \n";
	cout << " alpha " << alpha << "\n";
	cout << " beta " << beta << "\n";
	cout << " gamma " << gamma << "\n";
	
	cout.flush();
	
	assert(1==0);
	
	*/	

	a=3.0;
	b=4.0;
	c=1.9749025;
	lambda=acos(cosh(a)/(cosh(a)+1.0));
	mu=acos(cosh(b)/(cosh(b)+1.0));
	nu=acos(cosh(c)/(cosh(c)+1.0));
	alpha=acos(((cosh(b)*cosh(c))-cosh(a))/(sinh(b)*sinh(c)));
	beta=acos(((cosh(a)*cosh(c))-cosh(b))/(sinh(a)*sinh(c)));
	gamma=acos(((cosh(a)*cosh(b))-cosh(c))/(sinh(a)*sinh(b)));	
	
	mat X,Y,Z;
	X=build_mat(0,3,a)*build_mat(0,1,lambda-PI);
	Y=build_mat(0,3,a)*build_mat(0,1,PI-beta)*build_mat(0,3,c)*build_mat(0,1,nu-PI)*build_mat(1,0,PI-beta)*build_mat(0,3,-1.0*a);
	Z=build_mat(0,1,gamma-PI)*build_mat(0,3,-1.0*b)*build_mat(0,3,b)*build_mat(0,1,mu-PI)*build_mat(0,3,b)*build_mat(1,0,gamma-PI);
//	Y=build_mat(0,3,3.0)*build_mat(1,0,PI-beta)*build_mat(0,3,2.0264)*build_mat(0,1,PI-nu)*build_mat(0,1,PI-beta)*build_mat(0,3,-3.0);
//	Z=build_mat(1,0,PI-gamma)*build_mat(0,3,-4.0)*build_mat(0,3,4.0)*build_mat(0,1,PI-mu)*build_mat(0,3,4.0)*build_mat(0,1,PI-gamma);
	
	GENERATORS.push_back(X);	// gen 0
	GENERATORS.push_back(Y);	// gen 1
	GENERATORS.push_back(Z);	// gen 2
	
	AUTOMATON.clear();
	vector< pair<int,int> > V;

	// state 0
	V.clear();	
	V.push_back(make_pair(0,0));
	V.push_back(make_pair(1,0));
	V.push_back(make_pair(2,0));
	AUTOMATON.push_back(V);
	
	TRIANGLES.clear();
	triangle T;
	
	vec W;
	W=build_vec(0.0,0.0,0.0,1.0);
	
	T.v[0]=W;
	T.v[1]=build_mat(0,3,a)*W;
	T.v[2]=build_mat(0,1,gamma)*build_mat(0,3,b)*W;
	TRIANGLES.push_back(T);
	T.v[0]=W;
	T.v[1]=build_mat(0,3,a*0.5)*W;
	T.v[2]=build_mat(1,0,lambda)*build_mat(0,3,a*0.5)*W;
	TRIANGLES.push_back(T);
	T.v[0]=W;
	T.v[1]=build_mat(0,1,gamma)*build_mat(0,3,b*0.5)*W;
	T.v[2]=build_mat(0,1,gamma+mu)*build_mat(0,3,b*0.5)*W;
	TRIANGLES.push_back(T);
	T.v[0]=build_mat(0,3,a)*W;
	T.v[1]=build_mat(0,3,a)*build_mat(0,1,PI-beta)*build_mat(0,3,c*0.5)*W;
	T.v[2]=build_mat(0,3,a)*build_mat(0,1,PI-beta-nu)*build_mat(0,3,c*0.5)*W;
	TRIANGLES.push_back(T);

	COLORS.clear();
	COLORS.push_back(build_vec(0.9333333,0.9098039,0.66666666,0.0));	// pale goldenrod
	COLORS.push_back(build_vec(0.5,0.0,0.0,0.0));			// red
	COLORS.push_back(build_vec(0.0,0.5,0.0,0.0));			// green
	COLORS.push_back(build_vec(0.0,0.0,0.5,0.0));			// blue


//	CAMERA=id_mat();
	CAMERA=build_mat(0,3,-1.6)*build_mat(0,1,0.5*lambda);
	//build_mat(2,3,-0.5)*build_mat(0,2,0.7);	// camera skew angle
	
	BACKGROUND_COLOR=build_vec(0.9, 0.9, 0.99, 1.0);	// light blue
	
	draw_triangles_generated=false;
	do_prune=true;
};


void kleinian_group::prune_vertices(int dep){
	int i,j;
	for(i=0;i< (int) ELEMENTS.size();i++){
		for(j=0;j<i;j++){
			if(ELEMENTS[i].d==dep){	// don't prune the same vertex twice
				if(norm(ELEMENTS[i].M+(-1.0*ELEMENTS[j].M))<0.01){	// if i and j are too close
					ELEMENTS.erase(ELEMENTS.begin()+i);	// prune vertex i
					i--;
				};
			};
		};
	};
};

void kleinian_group::clever_prune_vertices(int dep){
	int i;
	cout << "sorting vertices \n";
	ELEMENTS=sort(ELEMENTS);
	cout << "pruning vertices \n";
	vector<vertex > V;
	V.clear();
	V.push_back(ELEMENTS[0]);
	for(i=1;i< (int) ELEMENTS.size();i++){
		if(norm(ELEMENTS[i].M+(-1.0*ELEMENTS[i-1].M))>=0.01){	// if i and i-1 are not too close
			V.push_back(ELEMENTS[i]);
		};
	};
	ELEMENTS=V;
};

void kleinian_group::generate_to_depth(int n){
	int i,j,dep;
	int s,t;
	vertex X;
	edge E;
	mat M;

	ELEMENTS.clear();	// initialize;
	
	for(dep=0;dep<=n;dep++){	
		if(dep==0){	// base case for induction
			X.M=id_mat();	// base matrix is identity
			X.s=0;			// base state is 0
			X.d=0;			// base depth is 0
			ELEMENTS.push_back(X);
		} else {	
			for(i=0;i< (int) ELEMENTS.size();i++){	// for each element in the list,
				if(ELEMENTS[i].d==dep-1){	// is this a terminal vertex?
					s=ELEMENTS[i].s;	// s is state
					for(j=0;j<(int) AUTOMATON[s].size();j++){
						M=GENERATORS[AUTOMATON[s][j].first];	// transition matrix
						t=AUTOMATON[s][j].second;		// end state
						X.M=ELEMENTS[i].M*M;		// new vertex location
						X.s=t;						// new state
						X.d=dep;					// new depth
						ELEMENTS.push_back(X);		// add vertex to collection
					};
				};
			};
		};
		cout << "depth " << dep << ";  " << (int) ELEMENTS.size() << " elements\n"; 
		if(do_prune){
			clever_prune_vertices(dep);	// we'll see
			cout << "after pruning,  " << (int) ELEMENTS.size() << " elements\n"; 
		};
	};
	return;
};

void kleinian_group::list_elements(){
	int i;
	cout << (int) ELEMENTS.size() << " elements \n";
	for(i=0;i<(int) ELEMENTS.size();i++){
		cout << "element " << i << "\n";
		write(ELEMENTS[i].M);	// write matrix
		cout << "state:" << ELEMENTS[i].s << "  depth:" << ELEMENTS[i].d << "\n"; 
	};
	return;
};