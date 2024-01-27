#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <gl/GL.h>
#include <gl/glut.h>
#include <iostream>
#include <cmath>
#include <time.h>
#define M_PI       3.14159265358979323846 

#define G 6.674 * pow(10,-11)
#define Msun  1.989 * pow(10,30)

#define TIME_SLOW 10000000000
#define ZOOM_SLOW 10

// Typ definiujacy punkt o 3 wspolrzednych

//położenie obserwatora
static GLfloat viewer[] = { 0.0, 0.0, 0.0 };
static GLfloat viewerViewPoint[] = { 0.0, 0.0, 0.0 };

GLfloat timeSpeed = 10;

int cameraMode = 1;
int zoom = 30;


// zmienna definiująca tryb obrotu obiekt/obserwator
bool mode = true;
// przelicznik pikseli na stopnie
static GLfloat pix2angle;
// stan klawiszy myszy
// 0 - nie naciśnięto żadnego klawisza
// 1 - naciśnięty został lewy klawisz
static GLint status = 0;

// inicjalizacja położenia obserwatora
static GLfloat theta_x = 0.0;   // kąt obrotu obiektu
static GLfloat theta_y = 30.0;   // kąt obrotu obiektu

static GLfloat theta_x1 = 180;   // kąt obrotu obiektu
static GLfloat theta_y1 = 0.0;   // kąt obrotu obiektu

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy
static int delta_x = 0;        // różnica pomiędzy pozycją bieżącą i poprzednią kursora myszy
static int y_pos_old = 0;
static int delta_y = 0;

GLfloat delta_pos_x = 0.0;
GLfloat delta_pos_y = 0.0;
GLfloat delta_pos_z = 0.0;

struct Planet {
	GLfloat distance_from_sun;
	GLfloat rotation_time;
	GLfloat radius;
	GLfloat orbit_speed;
	GLfloat rotation_speed;
	GLfloat rotation_angle;
	GLfloat angle; // Kąt obrotu wokół Słońca
	GLuint textureId;
};

GLfloat calcOrbitSpeed(GLfloat distance){
	GLfloat T = sqrt(4 * M_PI * M_PI / (G * Msun) * pow(distance,3));
	GLfloat speed = 1 / T * timeSpeed / TIME_SLOW;
	return speed;
}

// Przykład planet
Planet planets[] = {
	{ 5.0f, 0.1f, 0.5f, 0.10f, 0.1f, 0.02f, 0.0f }, // Merkury
	{ 10.0f, 0.1f, 1.0f, 0.06f, 0.05f, 0.02f, 0.0f }, // Wenus
	{ 15.0f, 0.1f, 1.5f, 0.03f, 0.03f, 0.02f, 0.0f }, // Ziemia
	{ 20.0f, 0.1f, 1.0f, 0.02f, 0.02f, 0.02f, 0.0f }, // Mars
	{ 25.0f, 0.1f, 2.0f, 0.01f, 0.01f, 0.02f, 0.0f }, // Jupiter
	{ 30.0f, 0.1f, 1.8f, 0.01f, 0.01f, 0.02f, 0.0f }, // Saturn
	{ 35.0f, 0.1f, 1.7f, 0.01f, 0.01f, 0.02f, 0.0f }, // Uranus
	{ 40.0f,  0.1f, 1.4f, 0.01f, 0.01f, 0.02f, 0.0f }, // Neptune
};


GLuint textureSUN;


GLfloat calcViewerX(GLfloat theta_x, GLfloat theta_y, GLfloat zoomZ)
{
	return zoomZ * cos(theta_y * M_PI / 180) * cos(theta_x * M_PI / 180);
}

GLfloat calcViewerY(GLfloat theta_x, GLfloat theta_y, GLfloat zoomZ)
{
	return zoomZ * sin(theta_x * M_PI / 180);
}

GLfloat calcViewerZ(GLfloat theta_x, GLfloat theta_y, GLfloat zoomZ)
{
	return zoomZ * sin(theta_y * M_PI / 180) * cos(theta_x * M_PI / 180);
}

GLfloat floatMod(GLfloat number, GLfloat mod)
{
	while (number < 0) { number += mod; }
	while (number > mod) { number -= mod; }
	return number;
}

GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{
#pragma pack(1)           
	typedef struct
	{
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}TGAHEADER;
#pragma pack(8)
	FILE* pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte* pbitsperpixel = NULL;
	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;
	pFile = fopen(FileName, "rb");
	if (pFile == NULL)
		return NULL;
	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);
	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;
	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;
	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;
	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
	if (pbitsperpixel == NULL)
		return NULL;
	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}
	switch (sDepth)
	{
	case 3:
		*ImFormat = GL_BGR_EXT;
		*ImComponents = GL_RGB8;
		break;
	case 4:
		*ImFormat = GL_BGRA_EXT;
		*ImComponents = GL_RGBA8;
		break;
	case 1:
		*ImFormat = GL_LUMINANCE;
		*ImComponents = GL_LUMINANCE8;
		break;
	};
	fclose(pFile);
	return pbitsperpixel;
}


/*************************************************************************************/
// Funkcja "monitoruje" położenie kursora myszy i ustawia wartości odpowiednich
// zmiennych globalnych

float sensitivity = 0.1f; // Możesz dostosować tę wartość do swoich potrzeb


void Motion(GLsizei x, GLsizei y) {
	delta_x = x - x_pos_old;
	x_pos_old = x;
	delta_y = y - y_pos_old;
	y_pos_old = y;

	/*
	if (status == 1) { // Jeśli lewy przycisk myszy jest wciśnięty
		theta_x += delta_x * pix2angle * sensitivity;
		theta_y += delta_y * pix2angle * sensitivity;
		delta_x = 0;
		delta_y = 0;
	}

	if (status == 2) { // Jeśli prawy przycisk myszy jest wciśnięty
		// Tutaj logika do obsługi przybliżania
		if (delta_y > 0) {
			viewer[2] += 0.1;
			viewerViewPoint[2] += 0.25;
		}
		else {
			viewer[2] -= 0.1;
			viewerViewPoint[2] -= 0.25;
		}
		delta_y = 0; // Reset delta_y do zera
	}

	*/

	// kamera swobodna
	if (cameraMode == 1) {

		if (status == 1)
		{
			theta_y += delta_y * pix2angle;
			if (theta_y > 90 && theta_y < 270)
			{
				theta_x -= delta_x * pix2angle;
			}
			else
			{
				theta_x += delta_x * pix2angle;
			}

			if (theta_y >= 90 && theta_y < 180) {
				theta_y = 89.9;
			}
			if (theta_y <= 270 && theta_y > 180) {
				theta_y = 270.1;
			}

			theta_x = floatMod(theta_x, 360);
			theta_y = floatMod(theta_y, 360);
		}

		if
			(status == 2)
		{
			zoom += delta_y / ZOOM_SLOW;
		}


		viewer[0] = calcViewerX(theta_y, theta_x, zoom);
		viewer[1] = calcViewerY(theta_y, theta_x, zoom); 
		viewer[2] = calcViewerZ(theta_y, theta_x, zoom); 

		viewerViewPoint[0] = 0;
		viewerViewPoint[1] = 0;
		viewerViewPoint[2] = 0;
	}
	else {

		if (status == 1)
		{
			theta_y1 += delta_y * pix2angle;
			theta_x1 -= delta_x * pix2angle;

			if (theta_y1 > 90) {
				theta_y1 = 90;
			}
			if (theta_y1 < -90) {
				theta_y1 = -90;
			}

			viewerViewPoint[0] = viewer[0] + cos(theta_x1 * M_PI / 180);
			viewerViewPoint[1] = viewer[1] + sin(theta_y1 * M_PI / 180);
			viewerViewPoint[2] = viewer[2] + sin(theta_x1 * M_PI / 180);
		}

		if (status == 2) {

			delta_pos_x = (viewerViewPoint[0] - viewer[0]) / 10 * delta_y * pix2angle;
			delta_pos_y = (viewerViewPoint[1] - viewer[1]) / 10 * delta_y * pix2angle;
			delta_pos_z = (viewerViewPoint[2] - viewer[2]) / 10 * delta_y * pix2angle;

			viewer[0] += delta_pos_x;
			viewer[1] += delta_pos_y;
			viewer[2] += delta_pos_z;

			viewerViewPoint[0] += delta_pos_x;
			viewerViewPoint[1] += delta_pos_y;
			viewerViewPoint[2] += delta_pos_z;
		}
	}

	glutPostRedisplay();
}



void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;
		y_pos_old = y;
		status = 1;
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;
		y_pos_old = y;
		status = 2;
	}
	else {
		status = 0;
	}

	Motion(x_pos_old, y_pos_old);

}

/*************************************************************************************/
void drawTexturedSphere(GLuint textureID, float radius) {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLUquadricObj* sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricTexture(sphere, GL_TRUE);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluSphere(sphere, radius, 50, 50);
	gluDeleteQuadric(sphere);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawOrbit(float radius) {
	GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess = { 20.0f };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
	int num_segments = 100; // Ilość segmentów użyta do narysowania okręgu
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < num_segments; i++) {
		angle = 2.0f * M_PI * float(i) / float(num_segments); // 360 stopni podzielone na ilość segmentów
		glVertex3f(cos(angle) * radius, 0.0f, sin(angle) * radius); // Użyj osi Y i Z
	}
	glEnd();
}


void drawSun() {
	drawTexturedSphere(textureSUN, 2.5f); // Użyj odpowiedniej wartości promienia dla Słońca
}

void drawPlanet(Planet& planet) {
	glColor3f(1.0f, 1.0f, 1.0f);
	drawOrbit(planet.distance_from_sun);

	GLfloat mat_diffuse[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat mat_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat mat_shininess = { 0.0 };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


	glPushMatrix();
	glRotatef(planet.angle, 0.0f, 1.0f, 0.0f); // Obrót wokół Słońca
	glTranslatef(planet.distance_from_sun, 0.0f, 0.0f); // Ustawienie odległości od Słońca
	glRotatef(planet.rotation_angle, 0.0f, 1.0f, 0.0f); // Obrót wokół własnej osi
	glRotatef(90.0f, 1.0, 0.0, 0.0); // Obrót planety do odpowiedniej orientacji
	drawTexturedSphere(planet.textureId, planet.radius); // Użyj odpowiedniej wartości promienia dla planety
	glPopMatrix();
}

void update() {
	// Aktualizacja pozycji planet
	for (int i = 0; i < sizeof(planets) / sizeof(Planet); ++i) {
		planets[i].angle += planets[i].orbit_speed;
		if (planets[i].angle > 360.0f) {
			planets[i].angle -= 360.0f;
		}
		planets[i].rotation_angle += planets[i].rotation_speed* timeSpeed;
		if (planets[i].rotation_angle > 360.0f) {
			planets[i].rotation_angle -= 360.0f;
		}
	}

	glutPostRedisplay();
}


void Draw() {
	glDisable(GL_LIGHTING);
	drawSun();
	glEnable(GL_LIGHTING);
	// Rysowanie planet
	for (int i = 0; i < sizeof(planets) / sizeof(Planet); ++i) {
		drawPlanet(planets[i]);
	}

}


/*************************************************************************************/

// Funkcja określająca co ma być rysowane (zawsze wywoływana gdy trzeba
// przerysować scenę)



void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym

	glLoadIdentity();

	gluLookAt(viewer[0], viewer[1], viewer[2], viewerViewPoint[0], viewerViewPoint[1], viewerViewPoint[2], 0.0, 1.0, 0.0);
	// Zdefiniowanie położenia obserwatora

	glColor3f(1.0f, 1.0f, 1.0f);

	Draw();
	glFlush();

	glutSwapBuffers();
}

void loadTexturesPlanets() {
	GLbyte* pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	// Załaduj teksturę Słońca
	pBytes = LoadTGAImage("sun.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	glGenTextures(1, &textureSUN);
	glBindTexture(GL_TEXTURE_2D, textureSUN);
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	free(pBytes);

	// Załaduj teksturę Ziemi
	pBytes = LoadTGAImage("mercury.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	glGenTextures(1, &planets[0].textureId);
	glBindTexture(GL_TEXTURE_2D, planets[0].textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	free(pBytes);

	pBytes = LoadTGAImage("venus.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	glGenTextures(1, &planets[1].textureId);
	glBindTexture(GL_TEXTURE_2D, planets[1].textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	free(pBytes);

	pBytes = LoadTGAImage("earth.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	glGenTextures(1, &planets[2].textureId);
	glBindTexture(GL_TEXTURE_2D, planets[2].textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	free(pBytes);

	pBytes = LoadTGAImage("mars.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	glGenTextures(1, &planets[3].textureId);
	glBindTexture(GL_TEXTURE_2D, planets[3].textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	free(pBytes);

	pBytes = LoadTGAImage("jupiter.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	glGenTextures(1, &planets[4].textureId);
	glBindTexture(GL_TEXTURE_2D, planets[4].textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	free(pBytes);

	pBytes = LoadTGAImage("saturn.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	glGenTextures(1, &planets[5].textureId);
	glBindTexture(GL_TEXTURE_2D, planets[5].textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	free(pBytes);

	pBytes = LoadTGAImage("uranus.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	glGenTextures(1, &planets[6].textureId);
	glBindTexture(GL_TEXTURE_2D, planets[6].textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	free(pBytes);

	pBytes = LoadTGAImage("neptune.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	glGenTextures(1, &planets[7].textureId);
	glBindTexture(GL_TEXTURE_2D, planets[7].textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	free(pBytes);

}


void keys(unsigned char key, int x, int y)
{
	if (key == 'i') {
		timeSpeed += 1.0f;
		for (int i = 0; i < sizeof(planets) / sizeof(Planet); ++i) {
			planets[i].orbit_speed = calcOrbitSpeed(planets[i].distance_from_sun);
		}
	}
	if (key == 'd') {
		timeSpeed -= 1.0f;
		if(timeSpeed < 0){
			timeSpeed = 0;
		}
		for (int i = 0; i < sizeof(planets) / sizeof(Planet); ++i) {
			planets[i].orbit_speed = calcOrbitSpeed(planets[i].distance_from_sun);
		}
	}
	if (key == 'm') {
		if (cameraMode == 1) {
			cameraMode = 2;
		}
		else {
			cameraMode = 1;
		}
		Motion(x_pos_old, y_pos_old);
	}

	if (key == '[') {
		zoom += 1.0f;
		Motion(x_pos_old, y_pos_old);
	}
	if (key == ']') {
		zoom -= 1.0f;
		if (zoom <= 0) {
			zoom = 0.1;
		}
		Motion(x_pos_old, y_pos_old);
	}

	RenderScene(); // przerysowywanie obrazu sceny
}
/*************************************************************************************/

// Funkcja ustalająca stan renderowania



void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszczący (wypełnienia okna) ustawiono na czarny

	gluLookAt(viewer[0], viewer[1], viewer[2], viewerViewPoint[0], viewerViewPoint[1], viewerViewPoint[2], 0.0, 1.0, 0.0);

	//  Definicja materiału z jakiego zrobiony jest czajnik
	//  i definicja źródła światła

	// Definicja materiału z jakiego zrobiony jest czajnik

	GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat mat_specular[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat mat_shininess = { 20.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
	/*************************************************************************************/
	// Definicja źródła światła nr 1


	GLfloat att_constant = { 1.0 };
	// składowa stała ds dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_linear = { 0.05 };
	// składowa liniowa dl dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_quadratic = { 0.001 };




	GLfloat sun_light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat sun_light_diffuse[] = { 0.8f, 0.8f, 1.0f, 1.0f };
	GLfloat sun_light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat sun_light_position[] = { 0.0f, 0.0f, 0.0f,4 * M_PI };

	glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);



	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);



	glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);      // Włączenie światła nr 0 (Słońce)
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);  // Włączenie mechanizmu z-bufora

	loadTexturesPlanets();

	for (int i = 0; i < sizeof(planets) / sizeof(Planet); ++i) {
		planets[i].orbit_speed = calcOrbitSpeed(planets[i].distance_from_sun);
	}

	Motion(0, 0);
}

/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są
// przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

	glMatrixMode(GL_PROJECTION);
	// Przełączenie macierzy bieżącej na macierz projekcji
	pix2angle = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie
	glLoadIdentity();
	// Czyszcznie macierzy bieżącej

	gluPerspective(100, 1.0, 1.0, 100.0);
	// Ustawienie parametrów dla rzutu perspektywicznego


	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielkości okna okna widoku (viewport) w zależności
	// relacji pomiędzy wysokością i szerokością okna

	glMatrixMode(GL_MODELVIEW);
	// Przełączenie macierzy bieżącej na macierz widoku modelu  

	glLoadIdentity();
	// Czyszczenie macierzy bieżącej

}

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w modeie konsoli


void init() {

	std::cout << "Grafika komputerowa i komunikacja czlowiek-komputer\nMini Projekt - Uklad Sloneczny\nMichal Kazmierczak\n263924\n\n";
}

int main(void)
{
	init();
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(800, 800);

	glutCreateWindow("Mini Projekt - uklad sloneczny");

	glutDisplayFunc(RenderScene);
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną
	// (callback function).  Będzie ona wywoływana za każdym razem
	// gdy zajdzie potrzeba przerysowania okna


	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
	// za zmiany rozmiaru okna                       
	//glutIdleFunc(spinEgg);

	glutMouseFunc(Mouse);
	// Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy
	glutKeyboardFunc(keys);
	glutIdleFunc(update);
	MyInit();
	// Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przystąpieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	// Włączenie mechanizmu usuwania niewidocznych elementów sceny

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT

	return 0;
}

/*************************************************************************************/