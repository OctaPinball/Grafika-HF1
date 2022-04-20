//=============================================================================================
// Mintaprogram: Zöld háromszög. Ervenyes 2019. osztol.
//
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat, BOM kihuzando.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni a printf-et kiveve
// - Mashonnan atvett programresszleteket forrasmegjeloles nelkul felhasznalni es
// - felesleges programsorokat a beadott programban hagyni!!!!!!! 
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak 
// A keretben nem szereplo GLUT fuggvenyek tiltottak.
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Scholtz Bálint András
// Neptun : A8O5M2
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================
#include "framework.h"

int const MAX_RANDOM_POS = 500;
int const MAX_RANDOM_CHARGE = 10;
int const MAX_RANDOM_MASS = 5;
int const nv = 100;
float const RADIUS_CONST = 0.03f;
float const LINK_WIDTH = 1;
float const K =	 0.002f;
float const MINIMAL_DISTANCE = 0.00005f;
float const RO = 1;
float const CD = 0.47f;
float const SURFACE = 1;
float const DELTA_T = 0.05f;


struct Data {
	vec2 position;
	vec3 color;
};

struct TranslatedData {
	vec3 position;
	vec3 color;
};

struct Atom {
	Data d;
	int mass;
	int charge;
};

void drawCircle(Atom atom, vec2 center);

struct Link {
	Atom* start;
	Atom* end;
};

void drawLine(Link link, vec2 center);

class Camera {
	vec2 center;
	vec2 windowSize;
	
public:
	Camera() : center(0,0),windowSize(600,600) {}
	~Camera(){}
	void Zoom(float f) { windowSize = windowSize * f; }
	void Pan(vec2 v) { center = center + v; }

	mat4 V() { return TranslateMatrix(vec3(center.x, center.y, -1)); }
	mat4 P() { return ScaleMatrix(vec2(2 / windowSize.x, 2 / windowSize.y)); }
};

class Molecule {
public: 
	std::vector<Atom> atoms;
	std::vector<Link> links;
	vec2 velocity;
	float rotation;
	vec2 center;
	int totalCharge;
	int totalMass;
	int atomNumber;

	Molecule() {
		
		totalCharge = 0;
		totalMass = 0;
		int n = rand() % 7 + 2;
		center = vec2{ 0,0 };
		velocity = vec2{ 0,0 };
		rotation = 0;
		atomNumber = n;

		//Atoms
		for (int i = 0; i < n - 1; i++)
		{
			float x = (float)(rand() % MAX_RANDOM_POS * 2 - MAX_RANDOM_POS) / 500;
			float y = (float)(rand() % MAX_RANDOM_POS * 2 - MAX_RANDOM_POS) / 500;

			int tCharge = rand() % (MAX_RANDOM_CHARGE * 2) - MAX_RANDOM_CHARGE;
			if (totalCharge + tCharge > MAX_RANDOM_CHARGE || totalCharge + tCharge < -MAX_RANDOM_CHARGE)
				tCharge = -tCharge;
			totalCharge += tCharge;

			float blue = 0;
			float red = 0;
			if (tCharge > 0)
			{
				red = (float)((float)tCharge / (float)MAX_RANDOM_CHARGE);
			}
			else
			{
				blue = (float)(( -(float)tCharge) / (float)MAX_RANDOM_CHARGE);
			}
			
			int tMass = rand() % MAX_RANDOM_MASS + 1;
			totalMass += tMass;
			
			atoms.push_back(Atom{ Data{vec2(x,y),vec3(red,0,blue)},tMass,tCharge });
			
		}
		
		//Last atom
		float x = (float)(rand() % MAX_RANDOM_POS * 2 - MAX_RANDOM_POS) / 500;
		float y = (float)(rand() % MAX_RANDOM_POS * 2 - MAX_RANDOM_POS) / 500;
		int tCharge = -totalCharge;
		float blue = 0;
		float red = 0;
		if (tCharge > 0)
		{
			red = (float)(tCharge / MAX_RANDOM_CHARGE);
		}
		else
		{
			blue = (float)((-tCharge) / MAX_RANDOM_CHARGE);
		}
		int tMass = 1;
		totalMass += tMass;
		totalCharge += tCharge;
		atoms.push_back(Atom{ Data{vec2(x,y),vec3(red,0,blue)},tMass,tCharge });

		center = vec2{ 0,0 };
		//Center
		for (int i = 0; i < this->atomNumber; i++)
		{
			center = center + (atoms[i].d.position * (float)atoms[i].mass);
		}

		center = center / (float)totalMass;

		

		//Reference from center
		for (int i = 0; i < this->atomNumber; i++)
		{
			atoms[i].d.position = atoms[i].d.position - center;
		}

		//Create links
		for (int i = 1; i < n+1 ; i++)
		{
			int r = 1;
			if(i != 1)	
				r = rand() % (i - 1);
			links.push_back(Link{ &atoms[i-1], &atoms[r] });
		}
	}

	void drawCenter(vec2 c) {
		Atom centerD = Atom{ Data{ vec2(0,0), vec3{1,1,1}}, 1,1};
		drawCircle(centerD, c);
	}
	
	void drawAtom() {
		for (int i = 0; i < this->atomNumber; i++)
		{
			drawCircle(atoms[i], this->center);
		}

	}
	
	void drawLink() {
		for (int i = 0; i < this->atomNumber; i++)
		{
			drawLine(links[i], this->center);
		}
	}

	void drawMolecule() {
		this->drawLink();
		this->drawAtom();
	}

	void transform() {
		this->center = this->center + this->velocity;
		for (int i = 0; i < this->atomNumber; i++)
		{
			float alpha = this->rotation;

			this->atoms[i].d.position = vec2(cos(alpha) * this->atoms[i].d.position.x - sin(alpha) * this->atoms[i].d.position.y, sin(alpha) * this->atoms[i].d.position.x + cos(alpha) * this->atoms[i].d.position.y);
		}
	}
	~Molecule() {}
};

Molecule* m1 = nullptr;
Molecule* m2 = nullptr;

TranslatedData* translateArray(Data* array, int l) {
	TranslatedData* d = (TranslatedData*)malloc(sizeof(TranslatedData) * l);
	for (int i = 0; i < l; i++)
	{
		float x = array[i].position.x;
		float y = array[i].position.y;
		float z = sqrt(x * x + y * y + 1);
		d[i].position = vec3(x * 600 / (z + 1), y * 600 / (z + 1), z * 600);
		d[i].color = array->color;
	}
	return d;
}

void draw(Data* array, int lenght, int type) {
	TranslatedData* d = translateArray(array, lenght);
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TranslatedData) * lenght, d, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TranslatedData), (void*)offsetof(TranslatedData, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TranslatedData), (void*)offsetof(TranslatedData, color));
	glDrawArrays(type, 0, lenght);
	glDeleteBuffers(1, &vbo);
	free(d);
}


void drawCircle(Atom atom, vec2 center) {
	int i;
	float radius = atom.mass * RADIUS_CONST;
	Data* d = (Data*)malloc(sizeof(Data) * (nv + 1));
	for (i = 0; i < nv; i++)
	{
		float fi = i * 2 * M_PI / nv;
		d[i] = Data{ vec2(cos(fi) * radius, sin(fi) * radius ) + atom.d.position + center, atom.d.color
	};
	}
	draw(d, i, GL_TRIANGLE_FAN);
	free(d);
}

inline vec2 norm1(const vec2 &v) { return vec2{ v.y,-v.x };}

inline vec2 norm2(const vec2 &v) { return vec2{ -v.y,v.x };}


void drawLine(Link link, vec2 center) {
	Data* d = (Data*)malloc(sizeof(Data) * 2);
	d[0].position = link.end->d.position + center;
	d[1].position = link.start->d.position + center;
	for (int i = 0; i < 2; i++)
	{
		d[i].color = vec3{ 1, 1, 1 };
	}
	draw(d, 2, GL_LINES);
	free(d);
}



void calculateForces() {
	float m1RForce = 0;
	vec2 m1MForce = vec2{ 0,0 };
	float m2RForce = 0;
	vec2 m2MForce = vec2{ 0,0 };
	for (Atom atom1 : m1->atoms)
	{
		for (Atom atom2 : m2->atoms)
		{
			vec2 temp;
			vec2 v = atom2.d.position + m2->center - (atom1.d.position + m1->center);
			float r = length(v);
			float Force = 0;
			if (r < MINIMAL_DISTANCE)
				r = MINIMAL_DISTANCE;
			if((atom1.charge <= 0 && atom2.charge <= 0) || (atom1.charge >= 0 && atom2.charge >= 0))
				Force = -(K * atom1.charge * atom2.charge / (r * r)); //Taszító erõ
			else
				Force = (K * atom1.charge * atom2.charge / (r * r)); //Vonzó erõ

			v = v * Force;
			temp = v * normalize(atom1.d.position);
			m1MForce = m1MForce + ((temp.x + temp.y) * normalize(atom1.d.position));
			vec2 v1Normal = norm2(atom1.d.position);
			temp = v * normalize(v1Normal);
			m1RForce += (temp.x + temp.y) * length(atom1.d.position);

			v = atom1.d.position + m1->center - (atom2.d.position + m2->center);
			v = v * Force;
			temp = v * normalize(atom2.d.position);
			m2MForce = m2MForce + ((temp.x + temp.y) * normalize(atom1.d.position));
			vec2 v2Normal = norm2(atom2.d.position);
			temp = v * normalize(v2Normal);
			m2RForce += (temp.x + temp.y) * length(atom2.d.position);


		}
	}

	m1->velocity = m1MForce * DELTA_T / m1->totalMass;
	m2->velocity = m2MForce * DELTA_T / m2->totalMass;

	float m1MDrag = CD * SURFACE * RO * length(m1->velocity) * length(m1->velocity) / 2;
	float m2MDrag = CD * SURFACE * RO * length(m2->velocity) * length(m2->velocity) / 2;

	vec2 m1MTotal = m1MForce - m1MDrag;
	vec2 m2MTotal = m2MForce - m2MDrag;

	m1->velocity = m1MTotal * DELTA_T / m1->totalMass;
	m2->velocity = m2MTotal * DELTA_T / m2->totalMass;

	m1->rotation = m1RForce * DELTA_T;
	m2->rotation = m2RForce * DELTA_T;

	float m1RDrag = CD * SURFACE * RO * m1->rotation * m1->rotation / 2;
	float m2RDrag = CD * SURFACE * RO * m2->rotation * m2->rotation / 2;

	float m1RTotal = m1RForce - m1RDrag;
	float m2RTotal = m2RForce - m2RDrag;

	m1->rotation = m1RTotal * DELTA_T;
	m2->rotation = m2RTotal * DELTA_T;
}












// vertex shader in GLSL: It is a Raw string (C++11) since it contains new line characters
const char * const vertexSource = R"(
	#version 330				// Shader 3.3
	precision highp float;		// normal floats, makes no difference on desktop computers

	uniform mat4 MVP;			// uniform variable, the Model-View-Projection transformation matrix
	layout(location = 0) in vec3 vp;	// Varying input: vp = vertex position is expected in attrib array 0
	layout(location = 1) in vec3 vc;
	out vec3 color;

	void main() {
		//float z = sqrt(vp.x * vp.x + vp.y * vp.y + 1);
		gl_Position = vec4(vp, 1) * MVP;		// transform vp from modeling space to normalized device space
		color = vc;
	}
)";

// fragment shader in GLSL
const char * const fragmentSource = R"(
	#version 330			// Shader 3.3
	precision highp float;	// normal floats, makes no difference on desktop computers
	
	in vec3 color;		// uniform variable, the color of the primitive
	out vec4 outColor;		// computed color of the current pixel

	void main() {
		outColor = vec4(color, 1.0);	// computed color is the color of the primitive
	}
)";

GPUProgram gpuProgram; // vertex and fragment shaders
unsigned int vao;	   // virtual world on the GPU

Camera camera = Camera();



// Initialization, create an OpenGL context
void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);

	glGenVertexArrays(1, &vao);	// get 1 vao id
	glBindVertexArray(vao);		// make it active



	// create program for the GPU
	gpuProgram.create(vertexSource, fragmentSource, "outColor");
}

// Window has become invalid: Redraw

void onDisplay() {
	glClearColor(0.3f, 0.3f, 0.3f, 0);     // background color
	glClear(GL_COLOR_BUFFER_BIT); // clear frame buffer

	gpuProgram.setUniform(camera.V() * camera.P(), "MVP");

	glBindVertexArray(vao);
	if (m1 != nullptr)
		m1->drawMolecule();
	if (m2 != nullptr)
		m2->drawMolecule();
	

	glutSwapBuffers(); // exchange buffers for double buffering
}

// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY) {
	switch (key)
	{
	case ' ':
		m1->~Molecule();
		m2->~Molecule();
		m1 = new Molecule();
		m2 = new Molecule();
		break;
	case 'e':
		camera.Pan(vec2(0, -10));
		glutPostRedisplay();
		break;
	case 's':
		camera.Pan(vec2(-10, 0));
		glutPostRedisplay();
		break;
	case 'x':
		camera.Pan(vec2(0, 10));
		glutPostRedisplay();
		break;
	case 'd':
		camera.Pan(vec2(10, 0));
		glutPostRedisplay();
		break;
	case 'w':
		camera.Zoom(0.9f);
		glutPostRedisplay();
		break;
	case 'r':
		camera.Zoom(1.1f);
		glutPostRedisplay();
		break;
	default:
		break;
	}

}

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {	// pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
	// Convert to normalized device space
	float cX = 2.0f * pX / windowWidth - 1;	// flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;
	printf("Mouse moved to (%3.2f, %3.2f)\n", cX, cY);
}

// Mouse click event
void onMouse(int button, int state, int pX, int pY) { // pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
	// Convert to normalized device space
	float cX = 2.0f * pX / windowWidth - 1;	// flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;

	char * buttonStat;
	switch (state) {
	case GLUT_DOWN: buttonStat = "pressed"; break;
	case GLUT_UP:   buttonStat = "released"; break;
	}

	switch (button) {
	case GLUT_LEFT_BUTTON:   printf("Left button %s at (%3.2f, %3.2f)\n", buttonStat, cX, cY);   break;
	case GLUT_MIDDLE_BUTTON: printf("Middle button %s at (%3.2f, %3.2f)\n", buttonStat, cX, cY); break;
	case GLUT_RIGHT_BUTTON:  printf("Right button %s at (%3.2f, %3.2f)\n", buttonStat, cX, cY);  break;
	}
}

long lastTime = 0;
// Idle event indicating that some time elapsed: do animation here
void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME); // elapsed time since the start of the program
	long ut = 10;
	while (lastTime < time)
	{
		if (m1 != nullptr && m2 != nullptr)
		{
			calculateForces();
			m1->transform();
			m2->transform();
			lastTime += ut;
		}
		else
		{
			m1 = new Molecule();
			m2 = new Molecule();
		}
	glutPostRedisplay();
	}
}
