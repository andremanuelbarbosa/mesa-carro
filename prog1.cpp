//**************************************************
// Bibliotecas nessarias ao programa
//**************************************************
#include "stdafx.h"
#include "glui.h"
#include "RGBpixmap.h"
#include <math.h>
#include <windows.h>
//**************************************************


//**************************************************
// Definicao da dimensao e posicao da janela
//**************************************************
#define DIMX 500
#define DIMY 500
#define INITIALPOS_X 100
#define INITIALPOS_Y 100
//**************************************************


//**************************************************
// Definicao de listas a usar no programa
//**************************************************
#define MESA_LIST 1
#define CHAO_LIST 2
#define PAREDE1_LIST 3
#define PAREDE2_LIST 4
#define CARRO_LIST 5
#define QUADRO_LIST 6
//**************************************************


//**************************************************
// Definicao de variaveis auxiliares
//**************************************************
#define VLENGTH 3
#define PI 3.14159265
#define POINTS_CIRCLE 24
//**************************************************


//**************************************************
// Variaveis LIVE
//**************************************************
int wireframe = 0;
int segments = 8;
//**************************************************

//**************************************************
// Variaveis necessarias para as janelas
//**************************************************
int main_window;
GLUI *window1, *window2;
//**************************************************


//**************************************************
// Variaveis necessarias para o movimento do carro
//**************************************************
double pos_centro_rotacao[3][3] = {1.0, 1.0, 0.0,
								   2.0, 1.0,-1.0,
								   0.0, 1.0, 1.0};
double pos_inicial_carro[3] = {1.0,1.0,1.5};
double pos_nova_carro[3] = {1.0,1.0,1.5};
double raio_mov_carro[3] = {3.0,1.23,1.6};
bool mov = false;
int circulo = 0;
SYSTEMTIME initial_time;
SYSTEMTIME final_time;
double beta = (2 * PI) / (POINTS_CIRCLE * 2);
int cont = 0;
int cont_rot = 0;
int aux = 0;
double rot_inicial = -45.0;
//**************************************************


float xy_aspect;

float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float obj_pos[] = { 0.0, 0.0, 0.0 };

RGBpixmap pix[3];

GLUI_Button *mov_button;
GLUI_Panel *panel;
GLUI_RadioGroup *group;
int mov_type;


//**************************************************
// Variaveis usadas para as definicoes dos materiais
//**************************************************

	// Material do topo e da base do candeeiro
	float material1_shininess[] = {20.0}; 
	float material1_specular[]  = {0.8,0.8,0.8,1.0};
	float material1_diffuse[]   = {1.0,0.0,0.0,1.0};

	// Material do pe do canddeiro
	float material2_shininess[] = {1.0}; 
	float material2_specular[]  = {0.4,0.4,0.4,1.0};
	float material2_diffuse[]   = {0.3,0.3,0.3,1.0};

	// Material da esfera
	float material3_shininess[] = {1.0}; 
	float material3_specular[]  = {0.4,0.4,0.4,1.0};
	float material3_diffuse[]   = {0.0,1.0,0.0,1.0};

	// Material da mesa
	float material4_shininess[] = {10.0};
	float material4_specular[]  = { 0.8, 0.8, 0.8,1.0};
	float material4_diffuse[]   = {0.56,0.40,0.03,1.0};

	// Material das paredes e do chao
	float material5_shininess[] = {10.0};
	float material5_specular[]  = {0.8,0.8,0.8,1.0};
	float material5_diffuse[]   = {1.0,1.0,1.0,1.0};

//**************************************************


//**************************************************
// Funcao que normaliza um vector pela unidade de medida
//**************************************************
void normalise(GLdouble *vec)
{
	GLdouble length = 0.0;
	int i;
	
	for (i = 0; i < VLENGTH; i++)
		length += vec[i] * vec[i]; 

	length = (GLdouble) sqrt((double)length); 

	for (i = 0; i < VLENGTH; i++)
		vec[i] = vec[i] / length; 
}
//**************************************************


//**************************************************
// Funcao que calcula a normal de um poligono
//**************************************************
void calculaNormal(GLdouble *vec1, GLdouble *vec2, GLdouble *vec3, GLdouble *vec4, GLdouble *normal)
{
	normal[0] = ((vec1[1] - vec2[1]) * (vec1[2] + vec2[2])) + 
				((vec2[1] - vec3[1]) * (vec2[2] + vec3[2])) +
				((vec3[1] - vec4[1]) * (vec3[2] + vec4[2])) +
				((vec4[1] - vec1[1]) * (vec4[2] + vec1[2]));

	normal[1] = ((vec1[2] - vec2[2]) * (vec1[0] + vec2[0])) + 
				((vec2[2] - vec3[2]) * (vec2[0] + vec3[0])) +
				((vec3[2] - vec4[2]) * (vec3[0] + vec4[0])) +
				((vec4[2] - vec1[2]) * (vec4[0] + vec1[0]));

	normal[2] = ((vec1[0] - vec2[0]) * (vec1[1] + vec2[1])) + 
				((vec2[0] - vec3[0]) * (vec2[1] + vec3[1])) +
				((vec3[0] - vec4[0]) * (vec3[1] + vec4[1])) +
				((vec4[0] - vec1[0]) * (vec4[1] + vec1[1]));

	normalise(normal);
}
//**************************************************


//**************************************************
// Funcao que representa o objecto carro
//**************************************************
void carro(GLdouble comprimento)
{
	GLdouble c = comprimento,
			 h = 0.33 * c,
			 l = 0.39 * c;

	GLdouble normal[VLENGTH];

	float material_shininess[] = {20.0};					/* How shiny is the object (specular exponent)  */
	float material_specular[] =  {0.8, 0.8, 0.8, 1.0};	/* specular reflection. */
	float material_diffuse[] =   {0.56, 0.40, 0.03, 1.0};	/* diffuse reflection. */

	// define as caracteristicas do materialerial (dos materiais seguintes, i.e. ate nova alteracao
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);


	//**************************************************
	// Pneus
	//**************************************************

	GLdouble raio_out = 0.58 * h;
	GLdouble raio_in = (raio_out * 3) / 4;
	GLdouble raio_eixo = raio_in / 3;

	GLdouble drc = 0.1;

	GLint i;
	GLdouble alpha = (2 * PI) / POINTS_CIRCLE;

	//**************************************************
	// Pneu Trás Direito
	//**************************************************
	GLdouble ptd_centro_1[3] = {(-c + ((0.32 * c) + drc + (raio_out / 2))),-h,l};
	GLdouble ptd_centro_2[3] = {(-c + ((0.32 * c) + drc + (raio_out / 2))),-h,(l * 4) / 5};
	GLdouble ptd_in_1[POINTS_CIRCLE][3];
	GLdouble ptd_in_2[POINTS_CIRCLE][3];
	GLdouble ptd_out_1[POINTS_CIRCLE][3];
	GLdouble ptd_out_2[POINTS_CIRCLE][3];

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		ptd_in_1[i][0] = ptd_centro_1[0] + (raio_in * cos(i * alpha));
		ptd_in_1[i][1] = ptd_centro_1[1] + (raio_in * sin(i * alpha));
		ptd_in_1[i][2] = ptd_centro_1[2];

		ptd_in_2[i][0] = ptd_centro_2[0] + (raio_in * cos(i * alpha));
		ptd_in_2[i][1] = ptd_centro_2[1] + (raio_in * sin(i * alpha));
		ptd_in_2[i][2] = ptd_centro_2[2];

		ptd_out_1[i][0] = ptd_centro_1[0] + (raio_out * cos(i * alpha));
		ptd_out_1[i][1] = ptd_centro_1[1] + (raio_out * sin(i * alpha));
		ptd_out_1[i][2] = ptd_centro_1[2];

		ptd_out_2[i][0] = ptd_centro_2[0] + (raio_out * cos(i * alpha));
		ptd_out_2[i][1] = ptd_centro_2[1] + (raio_out * sin(i * alpha));
		ptd_out_2[i][2] = ptd_centro_2[2];
	}

	material_diffuse[0] = 0.0;
	material_diffuse[1] = 0.0;
	material_diffuse[2] = 0.0;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(ptd_out_1[i]);
	glEnd();

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(ptd_out_2[i]);
	glEnd();

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		glBegin(GL_POLYGON);
			if(i < (POINTS_CIRCLE - 1))
			{
				calculaNormal(ptd_out_1[i],ptd_out_2[i],ptd_out_2[i + 1],ptd_out_1[i + 1],normal);
				glNormal3dv(normal);
				glVertex3dv(ptd_out_1[i]);
				glVertex3dv(ptd_out_2[i]);
				glVertex3dv(ptd_out_2[i + 1]);
				glVertex3dv(ptd_out_1[i + 1]);
			}
			else
			{
				calculaNormal(ptd_out_1[i],ptd_out_2[i],ptd_out_2[0],ptd_out_1[0],normal);
				glNormal3dv(normal);
				glVertex3dv(ptd_out_1[i]);
				glVertex3dv(ptd_out_2[i]);
				glVertex3dv(ptd_out_2[0]);
				glVertex3dv(ptd_out_1[0]);
			}
		glEnd();
	}

	material_diffuse[0] = 0.5;
	material_diffuse[1] = 0.5;
	material_diffuse[2] = 0.5;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(ptd_in_1[i]);
	glEnd();

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(ptd_in_2[i]);
	glEnd();

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		glBegin(GL_POLYGON);
			if(i < (POINTS_CIRCLE - 1))
			{
				calculaNormal(ptd_in_1[i],ptd_in_2[i],ptd_in_2[i + 1],ptd_in_1[i + 1],normal);
				glNormal3dv(normal);
				glVertex3dv(ptd_in_1[i]);
				glVertex3dv(ptd_in_2[i]);
				glVertex3dv(ptd_in_2[i + 1]);
				glVertex3dv(ptd_in_1[i + 1]);
			}
			else
			{
				calculaNormal(ptd_in_1[i],ptd_in_2[i],ptd_in_2[0],ptd_in_1[0],normal);
				glNormal3dv(normal);
				glVertex3dv(ptd_in_1[i]);
				glVertex3dv(ptd_in_2[i]);
				glVertex3dv(ptd_in_2[0]);
				glVertex3dv(ptd_in_1[0]);
			}
		glEnd();
	}

	//**************************************************
	// Pneu Trás Esquerdo
	//**************************************************
	GLdouble pte_centro_1[3] = {(-c + ((0.32 * c) + drc + (raio_out / 2))),-h,-((l * 4) / 5)};
	GLdouble pte_centro_2[3] = {(-c + ((0.32 * c) + drc + (raio_out / 2))),-h,-l};
	GLdouble pte_in_1[POINTS_CIRCLE][3];
	GLdouble pte_in_2[POINTS_CIRCLE][3];
	GLdouble pte_out_1[POINTS_CIRCLE][3];
	GLdouble pte_out_2[POINTS_CIRCLE][3];

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		pte_in_1[i][0] = pte_centro_1[0] + (raio_in * cos(i * alpha));
		pte_in_1[i][1] = pte_centro_1[1] + (raio_in * sin(i * alpha));
		pte_in_1[i][2] = pte_centro_1[2];

		pte_in_2[i][0] = pte_centro_2[0] + (raio_in * cos(i * alpha));
		pte_in_2[i][1] = pte_centro_2[1] + (raio_in * sin(i * alpha));
		pte_in_2[i][2] = pte_centro_2[2];

		pte_out_1[i][0] = pte_centro_1[0] + (raio_out * cos(i * alpha));
		pte_out_1[i][1] = pte_centro_1[1] + (raio_out * sin(i * alpha));
		pte_out_1[i][2] = pte_centro_1[2];

		pte_out_2[i][0] = pte_centro_2[0] + (raio_out * cos(i * alpha));
		pte_out_2[i][1] = pte_centro_2[1] + (raio_out * sin(i * alpha));
		pte_out_2[i][2] = pte_centro_2[2];
	}

	material_diffuse[0] = 0.0;
	material_diffuse[1] = 0.0;
	material_diffuse[2] = 0.0;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(pte_out_1[i]);
	glEnd();

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(pte_out_2[i]);
	glEnd();

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		glBegin(GL_POLYGON);
			if(i < (POINTS_CIRCLE - 1))
			{
				calculaNormal(pte_out_1[i],pte_out_2[i],pte_out_2[i + 1],pte_out_1[i + 1],normal);
				glNormal3dv(normal);
				glVertex3dv(pte_out_1[i]);
				glVertex3dv(pte_out_2[i]);
				glVertex3dv(pte_out_2[i + 1]);
				glVertex3dv(pte_out_1[i + 1]);
			}
			else
			{
				calculaNormal(pte_out_1[i],pte_out_2[i],pte_out_2[0],pte_out_1[0],normal);
				glNormal3dv(normal);
				glVertex3dv(pte_out_1[i]);
				glVertex3dv(pte_out_2[i]);
				glVertex3dv(pte_out_2[0]);
				glVertex3dv(pte_out_1[0]);
			}
		glEnd();
	}

	material_diffuse[0] = 0.5;
	material_diffuse[1] = 0.5;
	material_diffuse[2] = 0.5;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(pte_in_1[i]);
	glEnd();

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(pte_in_2[i]);
	glEnd();

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		glBegin(GL_POLYGON);
			if(i < (POINTS_CIRCLE - 1))
			{
				calculaNormal(pte_in_1[i],pte_in_2[i],pte_in_2[i + 1],pte_in_1[i + 1],normal);
				glNormal3dv(normal);
				glVertex3dv(pte_in_1[i]);
				glVertex3dv(pte_in_2[i]);
				glVertex3dv(pte_in_2[i + 1]);
				glVertex3dv(pte_in_1[i + 1]);
			}
			else
			{
				calculaNormal(pte_in_1[i],pte_in_2[i],pte_in_2[0],pte_in_1[0],normal);
				glNormal3dv(normal);
				glVertex3dv(pte_in_1[i]);
				glVertex3dv(pte_in_2[i]);
				glVertex3dv(pte_in_2[0]);
				glVertex3dv(pte_in_1[0]);
			}
		glEnd();
	}

	//**************************************************
	// Pneu Frente Direito
	//**************************************************
	GLdouble pfd_centro_1[3] = {(c - ((0.32 * c) + drc + (raio_out / 2))),-h,l};
	GLdouble pfd_centro_2[3] = {(c - ((0.32 * c) + drc + (raio_out / 2))),-h,(l * 4) / 5};
	GLdouble pfd_in_1[POINTS_CIRCLE][3];
	GLdouble pfd_in_2[POINTS_CIRCLE][3];
	GLdouble pfd_out_1[POINTS_CIRCLE][3];
	GLdouble pfd_out_2[POINTS_CIRCLE][3];

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		pfd_in_1[i][0] = pfd_centro_1[0] + (raio_in * cos(i * alpha));
		pfd_in_1[i][1] = pfd_centro_1[1] + (raio_in * sin(i * alpha));
		pfd_in_1[i][2] = pfd_centro_1[2];

		pfd_in_2[i][0] = pfd_centro_2[0] + (raio_in * cos(i * alpha));
		pfd_in_2[i][1] = pfd_centro_2[1] + (raio_in * sin(i * alpha));
		pfd_in_2[i][2] = pfd_centro_2[2];

		pfd_out_1[i][0] = pfd_centro_1[0] + (raio_out * cos(i * alpha));
		pfd_out_1[i][1] = pfd_centro_1[1] + (raio_out * sin(i * alpha));
		pfd_out_1[i][2] = pfd_centro_1[2];

		pfd_out_2[i][0] = pfd_centro_2[0] + (raio_out * cos(i * alpha));
		pfd_out_2[i][1] = pfd_centro_2[1] + (raio_out * sin(i * alpha));
		pfd_out_2[i][2] = pfd_centro_2[2];
	}

	material_diffuse[0] = 0.0;
	material_diffuse[1] = 0.0;
	material_diffuse[2] = 0.0;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(pfd_out_1[i]);
	glEnd();

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(pfd_out_2[i]);
	glEnd();

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		glBegin(GL_POLYGON);
			if(i < (POINTS_CIRCLE - 1))
			{
				calculaNormal(pfd_out_1[i],pfd_out_2[i],pfd_out_2[i + 1],pfd_out_1[i + 1],normal);
				glNormal3dv(normal);
				glVertex3dv(pfd_out_1[i]);
				glVertex3dv(pfd_out_2[i]);
				glVertex3dv(pfd_out_2[i + 1]);
				glVertex3dv(pfd_out_1[i + 1]);
			}
			else
			{
				calculaNormal(pfd_out_1[i],pfd_out_2[i],pfd_out_2[0],pfd_out_1[0],normal);
				glNormal3dv(normal);
				glVertex3dv(pfd_out_1[i]);
				glVertex3dv(pfd_out_2[i]);
				glVertex3dv(pfd_out_2[0]);
				glVertex3dv(pfd_out_1[0]);
			}
		glEnd();
	}

	material_diffuse[0] = 0.5;
	material_diffuse[1] = 0.5;
	material_diffuse[2] = 0.5;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(pfd_in_1[i]);
	glEnd();

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(pfd_in_2[i]);
	glEnd();

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		glBegin(GL_POLYGON);
			if(i < (POINTS_CIRCLE - 1))
			{
				calculaNormal(pfd_in_1[i],pfd_in_2[i],pfd_in_2[i + 1],pfd_in_1[i + 1],normal);
				glNormal3dv(normal);
				glVertex3dv(pfd_in_1[i]);
				glVertex3dv(pfd_in_2[i]);
				glVertex3dv(pfd_in_2[i + 1]);
				glVertex3dv(pfd_in_1[i + 1]);
			}
			else
			{
				calculaNormal(pfd_in_1[i],pfd_in_2[i],pfd_in_2[0],pfd_in_1[0],normal);
				glNormal3dv(normal);
				glVertex3dv(pfd_in_1[i]);
				glVertex3dv(pfd_in_2[i]);
				glVertex3dv(pfd_in_2[0]);
				glVertex3dv(pfd_in_1[0]);
			}
		glEnd();
	}

	//**************************************************
	// Pneu Frente Esquerdo
	//**************************************************
	GLdouble pfe_centro_1[3] = {(c - ((0.32 * c) + drc + (raio_out / 2))),-h,-((l * 4) / 5)};
	GLdouble pfe_centro_2[3] = {(c - ((0.32 * c) + drc + (raio_out / 2))),-h,-l};
	GLdouble pfe_in_1[POINTS_CIRCLE][3];
	GLdouble pfe_in_2[POINTS_CIRCLE][3];
	GLdouble pfe_out_1[POINTS_CIRCLE][3];
	GLdouble pfe_out_2[POINTS_CIRCLE][3];

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		pfe_in_1[i][0] = pfe_centro_1[0] + (raio_in * cos(i * alpha));
		pfe_in_1[i][1] = pfe_centro_1[1] + (raio_in * sin(i * alpha));
		pfe_in_1[i][2] = pfe_centro_1[2];

		pfe_in_2[i][0] = pfe_centro_2[0] + (raio_in * cos(i * alpha));
		pfe_in_2[i][1] = pfe_centro_2[1] + (raio_in * sin(i * alpha));
		pfe_in_2[i][2] = pfe_centro_2[2];

		pfe_out_1[i][0] = pfe_centro_1[0] + (raio_out * cos(i * alpha));
		pfe_out_1[i][1] = pfe_centro_1[1] + (raio_out * sin(i * alpha));
		pfe_out_1[i][2] = pfe_centro_1[2];

		pfe_out_2[i][0] = pfe_centro_2[0] + (raio_out * cos(i * alpha));
		pfe_out_2[i][1] = pfe_centro_2[1] + (raio_out * sin(i * alpha));
		pfe_out_2[i][2] = pfe_centro_2[2];
	}

	material_diffuse[0] = 0.0;
	material_diffuse[1] = 0.0;
	material_diffuse[2] = 0.0;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(pfe_out_1[i]);
	glEnd();

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(pfe_out_2[i]);
	glEnd();

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		glBegin(GL_POLYGON);
			if(i < (POINTS_CIRCLE - 1))
			{
				calculaNormal(pfe_out_1[i],pfe_out_2[i],pfe_out_2[i + 1],pfe_out_1[i + 1],normal);
				glNormal3dv(normal);
				glVertex3dv(pfe_out_1[i]);
				glVertex3dv(pfe_out_2[i]);
				glVertex3dv(pfe_out_2[i + 1]);
				glVertex3dv(pfe_out_1[i + 1]);
			}
			else
			{
				calculaNormal(pfe_out_1[i],pfe_out_2[i],pfe_out_2[0],pfe_out_1[0],normal);
				glNormal3dv(normal);
				glVertex3dv(pfe_out_1[i]);
				glVertex3dv(pfe_out_2[i]);
				glVertex3dv(pfe_out_2[0]);
				glVertex3dv(pfe_out_1[0]);
			}
		glEnd();
	}

	material_diffuse[0] = 0.5;
	material_diffuse[1] = 0.5;
	material_diffuse[2] = 0.5;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(pfe_in_1[i]);
	glEnd();

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(pfe_in_2[i]);
	glEnd();

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		glBegin(GL_POLYGON);
			if(i < (POINTS_CIRCLE - 1))
			{
				calculaNormal(pfe_in_1[i],pfe_in_2[i],pfe_in_2[i + 1],pfe_in_1[i + 1],normal);
				glNormal3dv(normal);
				glVertex3dv(pfe_in_1[i]);
				glVertex3dv(pfe_in_2[i]);
				glVertex3dv(pfe_in_2[i + 1]);
				glVertex3dv(pfe_in_1[i + 1]);
			}
			else
			{
				calculaNormal(pfe_in_1[i],pfe_in_2[i],pfe_in_2[0],pfe_in_1[0],normal);
				glNormal3dv(normal);
				glVertex3dv(pfe_in_1[i]);
				glVertex3dv(pfe_in_2[i]);
				glVertex3dv(pfe_in_2[0]);
				glVertex3dv(pfe_in_1[0]);
			}
		glEnd();
	}

	//**************************************************
	// Eixo Traseiro
	//**************************************************
	GLdouble et_1[POINTS_CIRCLE][3];
	GLdouble et_2[POINTS_CIRCLE][3];
	
	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		et_1[i][0] = ptd_centro_1[0] + (raio_eixo * cos(i * alpha));
		et_1[i][1] = ptd_centro_1[1] + (raio_eixo * sin(i * alpha));
		et_1[i][2] = ptd_centro_1[2];

		et_2[i][0] = pte_centro_1[0] + (raio_eixo * cos(i * alpha));
		et_2[i][1] = pte_centro_1[1] + (raio_eixo * sin(i * alpha));
		et_2[i][2] = pte_centro_1[2];
	}

	material_diffuse[0] = 0.0;
	material_diffuse[1] = 0.0;
	material_diffuse[2] = 0.0;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(et_1[i]);
	glEnd();

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(et_2[i]);
	glEnd();

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		glBegin(GL_POLYGON);
			if(i < (POINTS_CIRCLE - 1))
			{
				calculaNormal(et_1[i],et_2[i],et_2[i + 1],et_1[i + 1],normal);
				glNormal3dv(normal);
				glVertex3dv(et_1[i]);
				glVertex3dv(et_2[i]);
				glVertex3dv(et_2[i + 1]);
				glVertex3dv(et_1[i + 1]);
			}
			else
			{
				calculaNormal(et_1[i],et_2[i],et_2[0],et_1[0],normal);
				glNormal3dv(normal);
				glVertex3dv(et_1[i]);
				glVertex3dv(et_2[i]);
				glVertex3dv(et_2[0]);
				glVertex3dv(et_1[0]);
			}
		glEnd();
	}

	//**************************************************
	// Eixo Frente
	//**************************************************
	GLdouble ef_1[POINTS_CIRCLE][3];
	GLdouble ef_2[POINTS_CIRCLE][3];
	
	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		ef_1[i][0] = pfd_centro_1[0] + (raio_eixo * cos(i * alpha));
		ef_1[i][1] = pfd_centro_1[1] + (raio_eixo * sin(i * alpha));
		ef_1[i][2] = pfd_centro_1[2];

		ef_2[i][0] = pfe_centro_1[0] + (raio_eixo * cos(i * alpha));
		ef_2[i][1] = pfe_centro_1[1] + (raio_eixo * sin(i * alpha));
		ef_2[i][2] = pfe_centro_1[2];
	}

	material_diffuse[0] = 0.0;
	material_diffuse[1] = 0.0;
	material_diffuse[2] = 0.0;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(ef_1[i]);
	glEnd();

	glBegin(GL_POLYGON);
		for(i = 0; i < POINTS_CIRCLE; i++)
			glVertex3dv(ef_2[i]);
	glEnd();

	for(i = 0; i < POINTS_CIRCLE; i++)
	{
		glBegin(GL_POLYGON);
			if(i < (POINTS_CIRCLE - 1))
			{
				calculaNormal(ef_1[i],ef_2[i],ef_2[i + 1],ef_1[i + 1],normal);
				glNormal3dv(normal);
				glVertex3dv(ef_1[i]);
				glVertex3dv(ef_2[i]);
				glVertex3dv(ef_2[i + 1]);
				glVertex3dv(ef_1[i + 1]);
			}
			else
			{
				calculaNormal(ef_1[i],ef_2[i],ef_2[0],ef_1[0],normal);
				glNormal3dv(normal);
				glVertex3dv(ef_1[i]);
				glVertex3dv(ef_2[i]);
				glVertex3dv(ef_2[0]);
				glVertex3dv(ef_1[0]);
			}
		glEnd();
	}

	//**************************************************
	// Chao
	//**************************************************
	GLdouble ch_1[POINTS_CIRCLE + 4][3];
	GLdouble ch_2[POINTS_CIRCLE + 4][3];

	ch_1[0][0] = -c;
	ch_1[0][1] = -h;
	ch_1[0][2] = l;

	ch_1[POINTS_CIRCLE + 3][0] = c;
	ch_1[POINTS_CIRCLE + 3][1] = -h;
	ch_1[POINTS_CIRCLE + 3][2] = l;

	ch_2[0][0] = -c;
	ch_2[0][1] = -h;
	ch_2[0][2] = -l;

	ch_2[POINTS_CIRCLE + 3][0] = c;
	ch_2[POINTS_CIRCLE + 3][1] = -h;
	ch_2[POINTS_CIRCLE + 3][2] = -l;

	int aux = 12;

	for(i = 1; i < ((POINTS_CIRCLE / 2) + 2); i++)
	{
		ch_1[i][0] = ptd_centro_1[0] + ((raio_out + drc) * cos(aux * alpha));
		ch_1[i][1] = ptd_centro_1[1] + ((raio_out + drc) * sin(aux * alpha));
		ch_1[i][2] = ptd_centro_1[2];

		ch_1[i + (POINTS_CIRCLE / 2) + 1][0] = pfd_centro_1[0] + ((raio_out + drc) * cos(aux * alpha));
		ch_1[i + (POINTS_CIRCLE / 2) + 1][1] = pfd_centro_1[1] + ((raio_out + drc) * sin(aux * alpha));
		ch_1[i + (POINTS_CIRCLE / 2) + 1][2] = pfd_centro_1[2];

		ch_2[i][0] = pte_centro_2[0] + ((raio_out + drc) * cos(aux * alpha));
		ch_2[i][1] = pte_centro_2[1] + ((raio_out + drc) * sin(aux * alpha));
		ch_2[i][2] = pte_centro_2[2];

		ch_2[i + (POINTS_CIRCLE / 2) + 1][0] = pfe_centro_2[0] + ((raio_out + drc) * cos(aux * alpha));
		ch_2[i + (POINTS_CIRCLE / 2) + 1][1] = pfe_centro_2[1] + ((raio_out + drc) * sin(aux * alpha));
		ch_2[i + (POINTS_CIRCLE / 2) + 1][2] = pfe_centro_2[2];

		aux--;
	}

	material_diffuse[0] = 0.0;
	material_diffuse[1] = 0.0;
	material_diffuse[2] = 0.0;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	for(i = 0; i < (POINTS_CIRCLE + 3); i++)
	{
		calculaNormal(ch_1[i],ch_2[i],ch_2[i + 1],ch_1[i + 1],normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glVertex3dv(ch_1[i]);
			glVertex3dv(ch_2[i]);
			glVertex3dv(ch_2[i + 1]);
			glVertex3dv(ch_1[i + 1]);
		glEnd();
	}

	//**************************************************
	// Cobertura
	//**************************************************
	GLdouble cb_1[6][3];
	GLdouble cb_2[6][3];

	cb_1[0][0] = -c;
	cb_1[0][1] = h / 3;
	cb_1[0][2] = l;

	cb_1[1][0] = ch_1[1][0];
	cb_1[1][1] = h / 3;
	cb_1[1][2] = (l * 9) / 10;

	cb_1[2][0] = ptd_centro_1[0];
	cb_1[2][1] = h;
	cb_1[2][2] = (l * 4) / 5;
	
	cb_1[4][0] = ch_1[2 + (POINTS_CIRCLE / 2)][0];
	cb_1[4][1] = h / 3;
	cb_1[4][2] = (l * 9) / 10;

	cb_1[3][0] = cb_1[4][0] - (cb_1[2][0] - cb_1[1][0]);
	cb_1[3][1] = h;
	cb_1[3][2] = (l * 4) / 5;

	cb_1[5][0] = c;
	cb_1[5][1] = -(h / 4);
	cb_1[5][2] = l;

	cb_2[0][0] = -c;
	cb_2[0][1] = h / 3;
	cb_2[0][2] = -l;

	cb_2[1][0] = ch_1[1][0];
	cb_2[1][1] = h / 3;
	cb_2[1][2] = -((l * 9) / 10);

	cb_2[2][0] = ptd_centro_1[0];
	cb_2[2][1] = h;
	cb_2[2][2] = -((l * 4) / 5);
	
	cb_2[4][0] = ch_1[2 + (POINTS_CIRCLE / 2)][0];
	cb_2[4][1] = h / 3;
	cb_2[4][2] = -((l * 9) / 10);

	cb_2[3][0] = cb_1[4][0] - (cb_1[2][0] - cb_1[1][0]);
	cb_2[3][1] = h;
	cb_2[3][2] = -((l * 4) / 5);

	cb_2[5][0] = c;
	cb_2[5][1] = -(h / 4);
	cb_2[5][2] = -l;

	material_diffuse[0] = 0.0;
	material_diffuse[1] = 0.0;
	material_diffuse[2] = 1.0;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);

	calculaNormal(ch_1[0],cb_1[0],cb_2[0],ch_2[0],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(ch_1[0]);
		glVertex3dv(cb_1[0]);
		glVertex3dv(cb_2[0]);
		glVertex3dv(ch_2[0]);
	glEnd();

	for(i = 0; i < 5; i++)
	{
		calculaNormal(cb_1[i],cb_1[i + 1],cb_2[i + 1],cb_2[i],normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glVertex3dv(cb_1[i]);
			glVertex3dv(cb_1[i + 1]);
			glVertex3dv(cb_2[i + 1]);
			glVertex3dv(cb_2[i]);
		glEnd();
	}

	calculaNormal(ch_1[3 + POINTS_CIRCLE],cb_1[5],cb_2[5],ch_2[3 + POINTS_CIRCLE],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(ch_1[3 + POINTS_CIRCLE]);
		glVertex3dv(cb_1[5]);
		glVertex3dv(cb_2[5]);
		glVertex3dv(ch_2[3 + POINTS_CIRCLE]);
	glEnd();

	//**************************************************
	// Parte Lateral Direita
	//**************************************************
	GLdouble ld[(POINTS_CIRCLE * 2) + 4 + 4 + 4][3];

	ld[0][0] = ch_1[0][0];
	ld[0][1] = ch_1[0][1];
	ld[0][2] = ch_1[0][2];

	ld[1][0] = cb_1[0][0];
	ld[1][1] = cb_1[0][1];
	ld[1][2] = cb_1[0][2];

	double m = (((-h/4)-(h/3))/(c - ch_1[2 + (POINTS_CIRCLE / 2)][0]));

	for(i = 0; i < ((POINTS_CIRCLE / 2) + 1); i++)
	{
		ld[(i * 2) + 2][0] = ch_1[i + 1][0];
		ld[(i * 2) + 2][1] = ch_1[i + 1][1];
		ld[(i * 2) + 2][2] = ch_1[i + 1][2];

		ld[(i * 2) + 3][0] = ch_1[i + 1][0];
		ld[(i * 2) + 3][1] = h / 3;
		ld[(i * 2) + 3][2] = cb_1[1][2];

		ld[(i * 2) + 4 + POINTS_CIRCLE][0] = ch_1[i + (POINTS_CIRCLE / 2) + 2][0];
		ld[(i * 2) + 4 + POINTS_CIRCLE][1] = ch_1[i + (POINTS_CIRCLE / 2) + 2][1];
		ld[(i * 2) + 4 + POINTS_CIRCLE][2] = ch_1[i + (POINTS_CIRCLE / 2) + 2][2];

		ld[(i * 2) + 5 + POINTS_CIRCLE][0] = ch_1[i + (POINTS_CIRCLE / 2) + 2][0];
		ld[(i * 2) + 5 + POINTS_CIRCLE][1] = (m * ch_1[i + (POINTS_CIRCLE / 2) + 2][0]) + ((h/3) - (m * ch_1[2 + (POINTS_CIRCLE / 2)][0]));
		ld[(i * 2) + 5 + POINTS_CIRCLE][2] = cb_1[1][2];
	}

	ld[(POINTS_CIRCLE * 2) + 4 + 2][0] = ch_1[POINTS_CIRCLE + 3][0];
	ld[(POINTS_CIRCLE * 2) + 4 + 2][1] = ch_1[POINTS_CIRCLE + 3][1];
	ld[(POINTS_CIRCLE * 2) + 4 + 2][2] = ch_1[POINTS_CIRCLE + 3][2];

	ld[(POINTS_CIRCLE * 2) + 4 + 3][0] = cb_1[5][0];
	ld[(POINTS_CIRCLE * 2) + 4 + 3][1] = cb_1[5][1];
	ld[(POINTS_CIRCLE * 2) + 4 + 3][2] = cb_1[5][2];

	ld[(POINTS_CIRCLE * 2) + 4 + 4][0] = cb_1[2][0];
	ld[(POINTS_CIRCLE * 2) + 4 + 4][1] = cb_1[2][1];
	ld[(POINTS_CIRCLE * 2) + 4 + 4][2] = cb_1[2][2];

	ld[(POINTS_CIRCLE * 2) + 4 + 5][0] = cb_1[1][0];
	ld[(POINTS_CIRCLE * 2) + 4 + 5][1] = cb_1[1][1];
	ld[(POINTS_CIRCLE * 2) + 4 + 5][2] = cb_1[1][2];

	ld[(POINTS_CIRCLE * 2) + 4 + 6][0] = cb_1[4][0];
	ld[(POINTS_CIRCLE * 2) + 4 + 6][1] = cb_1[4][1];
	ld[(POINTS_CIRCLE * 2) + 4 + 6][2] = cb_1[4][2];

	ld[(POINTS_CIRCLE * 2) + 4 + 7][0] = cb_1[3][0];
	ld[(POINTS_CIRCLE * 2) + 4 + 7][1] = cb_1[3][1];
	ld[(POINTS_CIRCLE * 2) + 4 + 7][2] = cb_1[3][2];

	material_diffuse[0] = 0.0;
	material_diffuse[1] = 0.0;
	material_diffuse[2] = 1.0;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	calculaNormal(ld[0],ld[2],ld[3],ld[1],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(ld[0]);
		glVertex3dv(ld[2]);
		glVertex3dv(ld[3]);
		glVertex3dv(ld[1]);
	glEnd();

	for(i = 0; i < (POINTS_CIRCLE / 2); i++)
	{
		calculaNormal(ld[(i * 2) + 2],ld[(i * 2) + 4],ld[(i * 2) + 5],ld[(i * 2) + 3],normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glVertex3dv(ld[(i * 2) + 2]);
			glVertex3dv(ld[(i * 2) + 4]);
			glVertex3dv(ld[(i * 2) + 5]);
			glVertex3dv(ld[(i * 2) + 3]);
		glEnd();
	}

	calculaNormal(ld[POINTS_CIRCLE + 2],ld[POINTS_CIRCLE + 4],ld[POINTS_CIRCLE + 5],
		ld[POINTS_CIRCLE + 3],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(ld[POINTS_CIRCLE + 2]);
		glVertex3dv(ld[POINTS_CIRCLE + 4]);
		glVertex3dv(ld[POINTS_CIRCLE + 5]);
		glVertex3dv(ld[POINTS_CIRCLE + 3]);
	glEnd();

	for(i = 0; i < (POINTS_CIRCLE / 2); i++)
	{
		calculaNormal(ld[(i * 2) + POINTS_CIRCLE + 4],ld[(i * 2) + POINTS_CIRCLE + 6],
			ld[(i * 2) + POINTS_CIRCLE + 7],ld[(i * 2) + POINTS_CIRCLE + 5],normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glVertex3dv(ld[(i * 2) + POINTS_CIRCLE + 4]);
			glVertex3dv(ld[(i * 2) + POINTS_CIRCLE + 6]);
			glVertex3dv(ld[(i * 2) + POINTS_CIRCLE + 7]);
			glVertex3dv(ld[(i * 2) + POINTS_CIRCLE + 5]);
		glEnd();
	}

	calculaNormal(ld[(POINTS_CIRCLE * 2) + 4],ld[(POINTS_CIRCLE * 2) + 6],
		ld[(POINTS_CIRCLE * 2) + 7],ld[(POINTS_CIRCLE * 2) + 5],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(ld[(POINTS_CIRCLE * 2) + 4]);
		glVertex3dv(ld[(POINTS_CIRCLE * 2) + 6]);
		glVertex3dv(ld[(POINTS_CIRCLE * 2) + 7]);
		glVertex3dv(ld[(POINTS_CIRCLE * 2) + 5]);
	glEnd();

	calculaNormal(ld[(POINTS_CIRCLE * 2) + 4 + 4],ld[(POINTS_CIRCLE * 2) + 4 + 5],
		ld[(POINTS_CIRCLE * 2) + 4 + 6],ld[(POINTS_CIRCLE * 2) + 4 + 7],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(ld[(POINTS_CIRCLE * 2) + 4 + 4]);
		glVertex3dv(ld[(POINTS_CIRCLE * 2) + 4 + 5]);
		glVertex3dv(ld[(POINTS_CIRCLE * 2) + 4 + 6]);
		glVertex3dv(ld[(POINTS_CIRCLE * 2) + 4 + 7]);
	glEnd();


	//**************************************************
	// Parte Lateral Esquerda
	//**************************************************
	GLdouble le[(POINTS_CIRCLE * 2) + 4 + 4 + 4][3];

	le[0][0] = ch_2[0][0];
	le[0][1] = ch_2[0][1];
	le[0][2] = ch_2[0][2];

	le[1][0] = cb_2[0][0];
	le[1][1] = cb_2[0][1];
	le[1][2] = cb_2[0][2];

	m = (((-h/4)-(h/3))/(c - ch_2[2 + (POINTS_CIRCLE / 2)][0]));

	for(i = 0; i < ((POINTS_CIRCLE / 2) + 1); i++)
	{
		le[(i * 2) + 2][0] = ch_2[i + 1][0];
		le[(i * 2) + 2][1] = ch_2[i + 1][1];
		le[(i * 2) + 2][2] = ch_2[i + 1][2];

		le[(i * 2) + 3][0] = ch_2[i + 1][0];
		le[(i * 2) + 3][1] = h / 3;
		le[(i * 2) + 3][2] = cb_2[1][2];

		le[(i * 2) + 4 + POINTS_CIRCLE][0] = ch_2[i + (POINTS_CIRCLE / 2) + 2][0];
		le[(i * 2) + 4 + POINTS_CIRCLE][1] = ch_2[i + (POINTS_CIRCLE / 2) + 2][1];
		le[(i * 2) + 4 + POINTS_CIRCLE][2] = ch_2[i + (POINTS_CIRCLE / 2) + 2][2];

		le[(i * 2) + 5 + POINTS_CIRCLE][0] = ch_2[i + (POINTS_CIRCLE / 2) + 2][0];
		le[(i * 2) + 5 + POINTS_CIRCLE][1] = (m * ch_2[i + (POINTS_CIRCLE / 2) + 2][0]) + ((h/3) - (m * ch_2[2 + (POINTS_CIRCLE / 2)][0]));
		le[(i * 2) + 5 + POINTS_CIRCLE][2] = cb_2[1][2];
	}

	le[(POINTS_CIRCLE * 2) + 4 + 2][0] = ch_2[POINTS_CIRCLE + 3][0];
	le[(POINTS_CIRCLE * 2) + 4 + 2][1] = ch_2[POINTS_CIRCLE + 3][1];
	le[(POINTS_CIRCLE * 2) + 4 + 2][2] = ch_2[POINTS_CIRCLE + 3][2];

	le[(POINTS_CIRCLE * 2) + 4 + 3][0] = cb_2[5][0];
	le[(POINTS_CIRCLE * 2) + 4 + 3][1] = cb_2[5][1];
	le[(POINTS_CIRCLE * 2) + 4 + 3][2] = cb_2[5][2];

	le[(POINTS_CIRCLE * 2) + 4 + 4][0] = cb_2[2][0];
	le[(POINTS_CIRCLE * 2) + 4 + 4][1] = cb_2[2][1];
	le[(POINTS_CIRCLE * 2) + 4 + 4][2] = cb_2[2][2];

	le[(POINTS_CIRCLE * 2) + 4 + 5][0] = cb_2[1][0];
	le[(POINTS_CIRCLE * 2) + 4 + 5][1] = cb_2[1][1];
	le[(POINTS_CIRCLE * 2) + 4 + 5][2] = cb_2[1][2];

	le[(POINTS_CIRCLE * 2) + 4 + 6][0] = cb_2[4][0];
	le[(POINTS_CIRCLE * 2) + 4 + 6][1] = cb_2[4][1];
	le[(POINTS_CIRCLE * 2) + 4 + 6][2] = cb_2[4][2];

	le[(POINTS_CIRCLE * 2) + 4 + 7][0] = cb_2[3][0];
	le[(POINTS_CIRCLE * 2) + 4 + 7][1] = cb_2[3][1];
	le[(POINTS_CIRCLE * 2) + 4 + 7][2] = cb_2[3][2];

	calculaNormal(le[0],le[2],le[3],le[1],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(le[0]);
		glVertex3dv(le[2]);
		glVertex3dv(le[3]);
		glVertex3dv(le[1]);
	glEnd();

	for(i = 0; i < (POINTS_CIRCLE / 2); i++)
	{
		calculaNormal(le[(i * 2) + 2],le[(i * 2) + 4],le[(i * 2) + 5],le[(i * 2) + 3],normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glVertex3dv(le[(i * 2) + 2]);
			glVertex3dv(le[(i * 2) + 4]);
			glVertex3dv(le[(i * 2) + 5]);
			glVertex3dv(le[(i * 2) + 3]);
		glEnd();
	}

	calculaNormal(le[POINTS_CIRCLE + 2],le[POINTS_CIRCLE + 4],le[POINTS_CIRCLE + 5],
		le[POINTS_CIRCLE + 3],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(le[POINTS_CIRCLE + 2]);
		glVertex3dv(le[POINTS_CIRCLE + 4]);
		glVertex3dv(le[POINTS_CIRCLE + 5]);
		glVertex3dv(le[POINTS_CIRCLE + 3]);
	glEnd();

	for(i = 0; i < (POINTS_CIRCLE / 2); i++)
	{
		calculaNormal(le[(i * 2) + POINTS_CIRCLE + 4],le[(i * 2) + POINTS_CIRCLE + 6],
			le[(i * 2) + POINTS_CIRCLE + 7],le[(i * 2) + POINTS_CIRCLE + 5],normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glVertex3dv(le[(i * 2) + POINTS_CIRCLE + 4]);
			glVertex3dv(le[(i * 2) + POINTS_CIRCLE + 6]);
			glVertex3dv(le[(i * 2) + POINTS_CIRCLE + 7]);
			glVertex3dv(le[(i * 2) + POINTS_CIRCLE + 5]);
		glEnd();
	}

	calculaNormal(le[(POINTS_CIRCLE * 2) + 4],le[(POINTS_CIRCLE * 2) + 6],
		le[(POINTS_CIRCLE * 2) + 7],le[(POINTS_CIRCLE * 2) + 5],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(le[(POINTS_CIRCLE * 2) + 4]);
		glVertex3dv(le[(POINTS_CIRCLE * 2) + 6]);
		glVertex3dv(le[(POINTS_CIRCLE * 2) + 7]);
		glVertex3dv(le[(POINTS_CIRCLE * 2) + 5]);
	glEnd();

	calculaNormal(le[(POINTS_CIRCLE * 2) + 4 + 4],le[(POINTS_CIRCLE * 2) + 4 + 5],
		le[(POINTS_CIRCLE * 2) + 4 + 6],le[(POINTS_CIRCLE * 2) + 4 + 7],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(le[(POINTS_CIRCLE * 2) + 4 + 4]);
		glVertex3dv(le[(POINTS_CIRCLE * 2) + 4 + 5]);
		glVertex3dv(le[(POINTS_CIRCLE * 2) + 4 + 6]);
		glVertex3dv(le[(POINTS_CIRCLE * 2) + 4 + 7]);
	glEnd();

	//**************************************************
	// Vidros
	//**************************************************
	GLdouble vtd[4][3];
	GLdouble vte[4][3];
	GLdouble vfd[4][3];
	GLdouble vfe[4][3];
	GLdouble vt[4][3];
	GLdouble vf[4][3];

	vtd[0][0] = cb_1[2][0];
	vtd[0][1] = cb_1[2][1] - 0.05;
	vtd[0][2] = cb_1[2][2] + 0.05;

	vtd[1][0] = cb_1[1][0] + 0.05;
	vtd[1][1] = cb_1[1][1];
	vtd[1][2] = cb_1[1][2] + 0.05;

	vtd[2][0] = cb_1[2][0] + (0.35* c) - 0.05;
	vtd[2][1] = cb_1[1][1];
	vtd[2][2] = cb_1[1][2] + 0.05;

	vtd[3][0] = cb_1[2][0] + (0.35* c) - 0.05;
	vtd[3][1] = cb_1[2][1] - 0.05;
	vtd[3][2] = cb_1[2][2] + 0.05;

	vte[0][0] = cb_2[2][0];
	vte[0][1] = cb_2[2][1] - 0.05;
	vte[0][2] = cb_2[2][2] - 0.05;

	vte[1][0] = cb_2[1][0] + 0.05;
	vte[1][1] = cb_2[1][1];
	vte[1][2] = cb_2[1][2] - 0.05;

	vte[2][0] = cb_2[2][0] + (0.35 * c) - 0.05;
	vte[2][1] = cb_2[1][1];
	vte[2][2] = cb_2[1][2] - 0.05;

	vte[3][0] = cb_2[2][0] + (0.35 * c) - 0.05;
	vte[3][1] = cb_2[2][1] - 0.05;
	vte[3][2] = cb_2[2][2] - 0.05;

	vfd[0][0] = cb_1[4][0] - 0.05;
	vfd[0][1] = cb_1[4][1];
	vfd[0][2] = cb_1[4][2] + 0.05;

	vfd[1][0] = cb_1[3][0];
	vfd[1][1] = cb_1[3][1] - 0.05;
	vfd[1][2] = cb_1[3][2] + 0.05;

	vfd[2][0] = cb_1[2][0] + (0.35 * c) + 0.05;
	vfd[2][1] = cb_1[3][1] - 0.05;
	vfd[2][2] = cb_1[3][2] + 0.05;

	vfd[3][0] = cb_1[2][0] + (0.35 * c) + 0.05;
	vfd[3][1] = cb_1[4][1];
	vfd[3][2] = cb_1[4][2] + 0.05;

	vfe[0][0] = cb_2[4][0] - 0.05;
	vfe[0][1] = cb_2[4][1];
	vfe[0][2] = cb_2[4][2] - 0.05;

	vfe[1][0] = cb_2[3][0];
	vfe[1][1] = cb_2[3][1] - 0.05;
	vfe[1][2] = cb_2[3][2] - 0.05;

	vfe[2][0] = cb_2[2][0] + (0.35 * c) + 0.05;
	vfe[2][1] = cb_2[3][1] - 0.05;
	vfe[2][2] = cb_2[3][2] - 0.05;

	vfe[3][0] = cb_2[2][0] + (0.35 * c) + 0.05;
	vfe[3][1] = cb_2[4][1];
	vfe[3][2] = cb_2[4][2] - 0.05;

	vf[0][0] = cb_1[3][0] + 0.05;
	vf[0][1] = cb_1[3][1] - 0.05;
	vf[0][2] = cb_1[3][2] - 0.05;

	vf[1][0] = cb_1[4][0] + 0.05;
	vf[1][1] = cb_1[4][1];
	vf[1][2] = cb_1[4][2] - 0.05;

	vf[2][0] = cb_2[4][0] + 0.05;
	vf[2][1] = cb_2[4][1];
	vf[2][2] = cb_2[4][2] + 0.05;

	vf[3][0] = cb_2[3][0] + 0.05;
	vf[3][1] = cb_2[3][1] - 0.05;
	vf[3][2] = cb_2[3][2] + 0.05;

	vt[0][0] = cb_1[2][0] - 0.05;
	vt[0][1] = cb_1[2][1] - 0.05;
	vt[0][2] = cb_1[2][2] - 0.05;

	vt[1][0] = cb_1[1][0] - 0.05;
	vt[1][1] = cb_1[1][1];
	vt[1][2] = cb_1[1][2] - 0.05;

	vt[2][0] = cb_2[1][0] - 0.05;
	vt[2][1] = cb_2[1][1];
	vt[2][2] = cb_2[1][2] + 0.05;

	vt[3][0] = cb_2[2][0] - 0.05;
	vt[3][1] = cb_2[2][1] - 0.05;
	vt[3][2] = cb_2[2][2] + 0.05;

	material_diffuse[0] = 0.5;
	material_diffuse[1] = 0.5;
	material_diffuse[2] = 0.5;
	material_diffuse[3] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	calculaNormal(vtd[0],vtd[1],vtd[2],vtd[3],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(vtd[0]);
		glVertex3dv(vtd[1]);
		glVertex3dv(vtd[2]);
		glVertex3dv(vtd[3]);
	glEnd();

	calculaNormal(vte[0],vte[1],vte[2],vte[3],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(vte[0]);
		glVertex3dv(vte[1]);
		glVertex3dv(vte[2]);
		glVertex3dv(vte[3]);
	glEnd();

	calculaNormal(vfd[0],vfd[1],vfd[2],vfd[3],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(vfd[0]);
		glVertex3dv(vfd[1]);
		glVertex3dv(vfd[2]);
		glVertex3dv(vfd[3]);
	glEnd();

	calculaNormal(vfe[0],vfe[1],vfe[2],vfe[3],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(vfe[0]);
		glVertex3dv(vfe[1]);
		glVertex3dv(vfe[2]);
		glVertex3dv(vfe[3]);
	glEnd();

	calculaNormal(vf[0],vf[1],vf[2],vf[3],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(vf[0]);
		glVertex3dv(vf[1]);
		glVertex3dv(vf[2]);
		glVertex3dv(vf[3]);
	glEnd();

	calculaNormal(vt[0],vt[1],vt[2],vt[3],normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(vt[0]);
		glVertex3dv(vt[1]);
		glVertex3dv(vt[2]);
		glVertex3dv(vt[3]);
	glEnd();
}


//**************************************************
// Funcao que representa o objecto paralelo
//**************************************************
void paralelo(GLdouble dimx, GLdouble dimy, GLdouble dimz)
{
	GLdouble dx = dimx / 2,
			 dy = dimy / 2,
			 dz = dimz / 2;
	
	GLdouble v1[3] = { dx,-dy, dz};
	GLdouble v2[3] = { dx,-dy,-dz};
	GLdouble v3[3] = { dx, dy, dz};
	GLdouble v4[3] = { dx, dy,-dz};
	GLdouble v5[3] = {-dx,-dy, dz};
	GLdouble v6[3] = {-dx, dy, dz};
	GLdouble v7[3] = {-dx, dy,-dz};
	GLdouble v8[3] = {-dx,-dy,-dz};

	GLdouble normal[VLENGTH];

	// Frente com coordenadas de textura
	calculaNormal(v1,v3,v6,v5,normal);
	glBegin(GL_POLYGON);
	    glNormal3dv(normal);
		glTexCoord2f(1.0,0.0); glVertex3dv(v1);
		glTexCoord2f(1.0,1.0); glVertex3dv(v3);
		glTexCoord2f(0.0,1.0); glVertex3dv(v6);
		glTexCoord2f(0.0,0.0); glVertex3dv(v5);
	glEnd();

	// Tras com coordenadas de textura
	calculaNormal(v8,v7,v4,v2,normal);
	glBegin(GL_POLYGON);
	    glNormal3dv(normal);
		glTexCoord2f(0.0,0.0); glVertex3dv(v8);
		glTexCoord2f(1.0,0.0); glVertex3dv(v7);
		glTexCoord2f(1.0,1.0); glVertex3dv(v4);
		glTexCoord2f(0.0,1.0); glVertex3dv(v2);
	glEnd();

	// Lateral esquerda com coordenadas de textura
	calculaNormal(v2,v4,v3,v1,normal);
	glBegin(GL_POLYGON);
 	    glNormal3dv(normal);
		glTexCoord2f(0.0,0.0); glVertex3dv(v2);
		glTexCoord2f(1.0,0.0); glVertex3dv(v4);
		glTexCoord2f(1.0,1.0); glVertex3dv(v3);
		glTexCoord2f(0.0,1.0); glVertex3dv(v1);
	glEnd();

	// Lateral direita com coordenadas de textura
	calculaNormal(v5,v6,v7,v8,normal);
	glBegin(GL_POLYGON);
	    glNormal3dv(normal);
		glTexCoord2f(0.0,0.0); glVertex3dv(v5);
		glTexCoord2f(1.0,0.0); glVertex3dv(v6);
		glTexCoord2f(1.0,1.0); glVertex3dv(v7);
		glTexCoord2f(0.0,1.0); glVertex3dv(v8);
	glEnd();

	// Base com coordenadas de textura
	calculaNormal(v1,v5,v8,v2,normal);
	glBegin(GL_POLYGON);
	    glNormal3dv(normal);
		glTexCoord2f(0.0,0.0); glTexCoord2f(0.0,0.0);glVertex3dv(v1);
		glTexCoord2f(1.0,0.0); glVertex3dv(v5);
		glTexCoord2f(1.0,1.0); glVertex3dv(v8);
		glTexCoord2f(0.0,1.0); glVertex3dv(v2);
	glEnd();

	// Topo com coordenadas de textura
	calculaNormal(v3,v4,v7,v6,normal);
	glBegin(GL_POLYGON);
	    glNormal3dv(normal); 
		glTexCoord2f(0.0,0.0); glVertex3dv(v3);
		glTexCoord2f(1.0,0.0); glVertex3dv(v4);
		glTexCoord2f(1.0,1.0); glVertex3dv(v7);
		glTexCoord2f(0.0,1.0); glVertex3dv(v6);
	glEnd();
}
//**************************************************


//**************************************************
// Funcao que faz o movimento simples do carro
//**************************************************
void movimento_simples(void)
{
	if(cont == (POINTS_CIRCLE * 2))
		cont = 0;

	if(((final_time.wMilliseconds - initial_time.wMilliseconds) > 10) ||
		((final_time.wMilliseconds < initial_time.wMilliseconds) && 
			((initial_time.wMilliseconds - final_time.wMilliseconds) < 990)))
	{
		pos_nova_carro[0] = pos_centro_rotacao[0][0] + (raio_mov_carro[0] * sin(cont * beta));
		pos_nova_carro[1] = pos_centro_rotacao[0][1];
		pos_nova_carro[2] = pos_centro_rotacao[0][2] + (raio_mov_carro[0] * cos(cont * beta));
		cont++;
		GetLocalTime(&initial_time);
	}
	glTranslated(pos_nova_carro[0],pos_nova_carro[1],pos_nova_carro[2]);
	glRotated((((beta * cont) * 180) / PI),0.0,1.0,0.0);
}
//**************************************************


//**************************************************
// Funcao que faz o movimento composto do carro
//**************************************************
void movimento_composto(void)
{
	if(circulo == 1)
	{
		if(cont == (POINTS_CIRCLE * 2))
			cont = 0;

		if((cont == (POINTS_CIRCLE * 2) - (POINTS_CIRCLE / 4)) && (aux == 1))
		{
			circulo = 2;
			cont = (POINTS_CIRCLE / 4) * 3;
			aux = 0;
		}
	}

	if(circulo == 2)
	{
		if(cont == 0)
			cont = (POINTS_CIRCLE * 2);

		if((cont == ((POINTS_CIRCLE / 4) * 3)) && (aux == 1))
		{
			circulo = 1;
			cont = (POINTS_CIRCLE * 2) - (POINTS_CIRCLE / 4);
			aux = 0;
		}
	}

	if(circulo == 0)
	{
		circulo = 1;
		cont = (POINTS_CIRCLE * 2) - (POINTS_CIRCLE / 4);
	}

	aux = 1;

	if(cont_rot == (POINTS_CIRCLE * 2))
		cont_rot = 0;

	if(((final_time.wMilliseconds - initial_time.wMilliseconds) > 10) ||
		((final_time.wMilliseconds < initial_time.wMilliseconds) && 
			((initial_time.wMilliseconds - final_time.wMilliseconds) < 990)))
	{
		if(circulo == 1)
		{
			pos_nova_carro[0] = pos_centro_rotacao[1][0] + (raio_mov_carro[1] * sin(cont * beta));
			pos_nova_carro[1] = pos_centro_rotacao[1][1];
			pos_nova_carro[2] = pos_centro_rotacao[1][2] + (raio_mov_carro[1] * cos(cont * beta));
			cont++;
		}
		else if(circulo == 2)
		{
			pos_nova_carro[0] = pos_centro_rotacao[2][0] + (raio_mov_carro[2] * sin(cont * beta));
			pos_nova_carro[1] = pos_centro_rotacao[2][1];
			pos_nova_carro[2] = pos_centro_rotacao[2][2] + (raio_mov_carro[2] * cos(cont * beta));
			cont--;
		}

		cont_rot++;
		glTranslated(pos_nova_carro[0],pos_nova_carro[1],pos_nova_carro[2]);

		if(circulo == 1)
			glRotated(rot_inicial + (((beta * cont_rot) * 180) / PI),0.0,1.0,0.0);
		else if(circulo == 2)
			glRotated(rot_inicial - (((beta * cont_rot) * 180) / PI),0.0,1.0,0.0);

		GetLocalTime(&initial_time);
	}
}
//**************************************************


//**************************************************
// Funcao que apresenta os objectos no ecra
//**************************************************
void display(void)
{
	//**************************************************
	// Inicializacao de variaveis e definicoes
	//**************************************************

		GLUquadric* glQ;
		   
		glQ = gluNewQuadric();
		gluQuadricOrientation(glQ, GLU_OUTSIDE);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-xy_aspect * 0.04,xy_aspect * 0.04,-0.04,0.04,0.1,50.0);

		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();
		glTranslated(0.0,0.0,-25.0);
		glRotated(20.0,1.0,0.0,0.0);
		glRotated(-45.0,0.0,1.0,0.0);
		glTranslatef(obj_pos[0],obj_pos[1],-obj_pos[2]); 

		glMultMatrixf(view_rotate);

	//**************************************************


	//**************************************************
	// Construcao da mesa
	//**************************************************

		// Definicao do material da mesa
		glMaterialfv(GL_FRONT,GL_SHININESS,material4_shininess);
		glMaterialfv(GL_FRONT,GL_SPECULAR,material4_specular);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,material4_diffuse);

		// Tampo da mesa
		glPushMatrix();
		glCallList(MESA_LIST);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glCallList(MESA_LIST);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glPopMatrix();

		// Pes da mesa
		glPushMatrix();
		glTranslated(4.0,0.0,3.0);
		glRotated(90.0,1.0,0.0,0.0);
		gluCylinder(glQ,0.5,0.5,4.0,8,10);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-4.0,0.0,3.0);
		glRotated(90.0,1.0,0.0,0.0);
		gluCylinder(glQ,0.5,0.5,4.0,8,10);
		glPopMatrix();

		glPushMatrix();
		glTranslated(4.0,0.0,-3.0);
		glRotated(90.0,1.0,0.0,0.0);
		gluCylinder(glQ,0.5,0.5,4.0,8,10);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-4.0,0.0,-3.0);
		glRotated(90.0,1.0,0.0,0.0);
		gluCylinder(glQ,0.5,0.5,4.0,8,10);
		glPopMatrix();

	//**************************************************


	//**************************************************
	// Construcao do candeeiro
	//**************************************************

		// Definicao do material do pe do candeeiro
		glMaterialfv(GL_FRONT,GL_SHININESS,material2_shininess);
		glMaterialfv(GL_FRONT,GL_SPECULAR,material2_specular);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,material2_diffuse);

		// Pe do candeeiro
		glPushMatrix();
		glTranslated(-3.5,0.0,-2.0);
		glRotated(-90.0,1.0,0.0,0.0 );
		gluCylinder(glQ,0.2,0.2,4.0,8,10);
		glPopMatrix();

		// Definicao do material do topo e da base do candeeiro
		glMaterialfv(GL_FRONT,GL_SHININESS,material1_shininess);
		glMaterialfv(GL_FRONT,GL_SPECULAR,material1_specular);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,material1_diffuse);

		// Topo do candeeiro
		glPushMatrix();
		glTranslated(-3.5,3.0,-2.0);
		glRotated(-90.0,1.0,0.0,0.0);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		gluCylinder(glQ,1.0,0.4,2.0,16,10);
		glPopMatrix();

		// Base do candeeiro
		glPushMatrix();
		glTranslated(-3.5,0.5,-2.0);
		glRotated(-90.0,1.0,0.0,0.0);
		gluCylinder(glQ,1.0,0.2,0.5,16,10);
		glPolygonMode(GL_FRONT,GL_FILL);
		glPopMatrix();

	//**************************************************


	//**************************************************
	// Construcao da esfera
	//**************************************************

		// Definicao do material da esfera
		glMaterialfv(GL_FRONT,GL_SHININESS,material3_shininess);
		glMaterialfv(GL_FRONT,GL_SPECULAR,material3_specular);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,material3_diffuse);

		// Esfera
		glPushMatrix();
		glTranslated(2.0,1.0,-1.0);
		gluSphere(glQ,0.5,16,10);
		glPopMatrix();

	//**************************************************


	//**************************************************
	// Construcao do chao
	//**************************************************

		// Definicao do material das paredes e do chao
		glMaterialfv(GL_FRONT,GL_SHININESS,material5_shininess);
		glMaterialfv(GL_FRONT,GL_SPECULAR,material5_specular);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,material5_diffuse);

		// Chao
		glPushMatrix();
		glTranslated(0.0,-4.0,0.0);
		glCallList(CHAO_LIST);							
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);		
		glCallList(CHAO_LIST);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glPopMatrix();

	//**************************************************


	//**************************************************
	// Construcao das paredes
	//**************************************************

		// Parede 1
		glPushMatrix();
		glTranslated(-10.0,0.9,0.0);
		glCallList(PAREDE1_LIST);							
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);		
		glCallList(PAREDE1_LIST);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glPopMatrix();

		// Parede 2
		glPushMatrix();
		glTranslated(0.0,0.9,-8.0);
		glCallList(PAREDE2_LIST);							
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);		
		glCallList(PAREDE2_LIST);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glPopMatrix();

	//**************************************************


	//**************************************************
	// Construcao do quadro
	//**************************************************

		// Quadro
		glPushMatrix();
		glTranslated(0.0,3.0,-7.9);
		glCallList(QUADRO_LIST);							
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);		
		glCallList(QUADRO_LIST);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glPopMatrix();

	//**************************************************


	//**************************************************
	// Construcao do carro
	//**************************************************

		GetLocalTime(&final_time);

		glPushMatrix();

		if(mov)
		{
			if(mov_type)
				movimento_composto();
			else
				movimento_simples();
		}
		else
		{
			glTranslated(pos_centro_rotacao[0][0],pos_centro_rotacao[0][1],pos_centro_rotacao[0][2]);
			glRotated(-45.0,0.0,1.0,0.0);
		}

		glCallList(CARRO_LIST);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);	
		glCallList(CARRO_LIST);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glPopMatrix();

	//**************************************************


	//**************************************************
	// Construcao do bule
	//**************************************************

		glPushMatrix();
		glColor3f(0.5,0.5,0.5);
		glTranslated(0.0,1.05,1.0);
		glutSolidTeapot(0.7);
		glPopMatrix();

	//**************************************************

	
	glutSwapBuffers();
	glFlush();
}
//**************************************************


//**************************************************
// Funcao que controla o rato
//**************************************************
void processMouse(int button,int state,int x,int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{	 
	}
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
	}
	
	glutPostRedisplay();
}
//**************************************************


//**************************************************
// Funcao que controla o movimento do rato
//**************************************************
void processMouseMoved(int x,int y)
{
	glutPostRedisplay();				
}
//**************************************************


//**************************************************
// Funcao que processa o movimento passivo do rato
//**************************************************
void processPassiveMouseMoved(int x,int y)
{
	glutPostRedisplay();				
}
//**************************************************


//**************************************************
// Funcao que actualiza o viewport
//**************************************************
void reshape(int w,int h)
{
	int tx, ty, tw, th;

	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);
	xy_aspect = (float) tw / (float) th;

	glutPostRedisplay();
}
//**************************************************


//**************************************************
// Funcao que controla o teclado
//**************************************************
void keyboard(unsigned char key, int x, int y)
{
   switch(key) 
   {
      case 27:
		 exit(0);
         break;
   }
}
//**************************************************


//**************************************************
// Funcao 
//**************************************************
void myGlutIdle(void)
{
  if(glutGetWindow() != main_window) 
    glutSetWindow(main_window);

  glutPostRedisplay();

  /****************************************************************/
  /*            This demonstrates GLUI::sync_live()               */
  /*   We change the value of a variable that is 'live' to some   */
  /*   control.  We then call sync_live, and the control          */
  /*   associated with that variable is automatically updated     */
  /*   with the new value.  This frees the programmer from having */
  /*   to always remember which variables are used by controls -  */
  /*   simply change whatever variables are necessary, then sync  */
  /*   the live ones all at once with a single call to sync_live  */
  /****************************************************************/

  //window1->sync_live();

}
//**************************************************


//**************************************************
// Funcao que inicializa os componentes
//**************************************************
void inicializacao(void)
{
	//**************************************************
	// Inicializacao de variavies e definicoes
	//**************************************************

		// Variaveis para as luzes e para o ambiente
		float ambient[]   = { 0.5, 0.5, 0.5,1.0};
		float diffuse0[]  = { 1.0, 1.0, 1.0,0.5};
		float diffuse1[]  = { 1.0, 1.0, 1.0,0.5};
		float diffuse2[]  = { 1.0, 1.0, 1.0,0.5};
		float diffuse3[]  = { 1.0, 1.0, 1.0,0.5};
		float position0[] = {50.0, 0.0, 0.0,0.0};
		float position1[] = { 0.0,50.0, 0.0,0.0};
		float position2[] = { 0.0, 0.0,50.0,0.0};
		float position3[] = {-3.5, 3.0,-2.0,0.0};
		float lmodel_ambient[] = {0.0,0.0,0.0,1.0};

		// Definicoes necessarias ao programa
		glFrontFace(GL_CCW);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT,lmodel_ambient); 
		glClearColor(1.0,1.0,1.0,1.0);

		// Definicoes das luzes e activacao das mesmas
		glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
		glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse0);
		glLightfv(GL_LIGHT0,GL_POSITION,position0);
		glLightfv(GL_LIGHT1,GL_AMBIENT,ambient);
		glLightfv(GL_LIGHT1,GL_DIFFUSE,diffuse1);
		glLightfv(GL_LIGHT1,GL_POSITION,position1);
		glLightfv(GL_LIGHT2,GL_AMBIENT,ambient);
		glLightfv(GL_LIGHT2,GL_DIFFUSE,diffuse2);
		glLightfv(GL_LIGHT2,GL_POSITION,position2);
		glLightfv(GL_LIGHT3,GL_AMBIENT,ambient);
		glLightfv(GL_LIGHT3,GL_DIFFUSE,diffuse3);
		glLightfv(GL_LIGHT3,GL_POSITION,position3);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);

		// Outras definicoes
		glShadeModel(GL_SMOOTH);
		glPolygonMode(GL_FRONT,GL_FILL);

	//**************************************************


	//**************************************************
	// Inicializacao das texturas
	//**************************************************
	
		// Textura padrao
		pix[0].makeCheckBoard();
		pix[0].setTexture(2001);

		// Textura "textura.bmp"
		pix[1].readBMPFile("paredes.bmp"); 
		pix[1].setTexture(2002);

		// Textura "quadro.bmp"
		pix[2].readBMPFile("entrada.bmp");
		pix[2].setTexture(2003);

	//**************************************************


	//**************************************************
	// Contrucao das listas a usar no desenho
	//**************************************************

		// Lista da mesa
		glNewList(MESA_LIST,GL_COMPILE);
			paralelo(10.0,1.0,8.0);
		glEndList();

		// Lista do chao
		glNewList(CHAO_LIST,GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D,2002);
			glEnable(GL_TEXTURE_2D);
			paralelo(20.0,0.2,16.0);
			glDisable(GL_TEXTURE_2D);
		glEndList();

		// Lista da parede 1
		glNewList(PAREDE1_LIST,GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D, 2002);
			glEnable(GL_TEXTURE_2D);
			paralelo(0.2,10.0,16.0);
			glDisable(GL_TEXTURE_2D);
		glEndList();

		// Lista da parede 2
		glNewList(PAREDE2_LIST,GL_COMPILE);	
			glBindTexture(GL_TEXTURE_2D, 2002);
			glEnable(GL_TEXTURE_2D);
			paralelo(20.0,10.0,0.2);
			glDisable(GL_TEXTURE_2D);
		glEndList();

		// Lista do carro
		glNewList(CARRO_LIST,GL_COMPILE);
			carro(1.0);
		glEndList();

		// Lista do quadro
		glNewList(QUADRO_LIST,GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D, 2003);
			glEnable(GL_TEXTURE_2D);
			paralelo(5.0,5.0,0.1);
			glDisable(GL_TEXTURE_2D);
		glEndList();

	//**************************************************


	//**************************************************
	// Outras inicializacoes
	//**************************************************

		GetLocalTime(&initial_time);

	//**************************************************
}
//**************************************************


//**************************************************
// Funcao que move/para o carro
//**************************************************
void move_carro()
{
	if(mov)
	{
		mov = false;
		circulo = 0;
		cont = 0;
		mov_button->set_name("GO!");
		panel->enable();
	}
	else
	{
		mov = true;
		mov_button->set_name("STOP!");
		panel->disable();

	}
}

//**************************************************
// Funcao que liga/desliga a luz do candeeiro
//**************************************************
void luz_candeeiro()
{
	if(glIsEnabled(GL_LIGHT3))
		glDisable(GL_LIGHT3);
	else
		glEnable(GL_LIGHT3);
}
//**************************************************


//**************************************************
// Funcao main
//**************************************************
int main(int argc, char* argv[])
{
	//**************************************************
	// Inicializacoes do glut para a janela
	//**************************************************
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(DIMX,DIMY);
	glutInitWindowPosition(INITIALPOS_X,INITIALPOS_Y);
	main_window = glutCreateWindow(argv[0]);
	//**************************************************
 
	//**************************************************
	// Inicializacoes do glut para o tratamento de eventos
	//**************************************************
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseMoved);
	glutPassiveMotionFunc(processPassiveMouseMoved);
	//**************************************************
   
	//**************************************************
	// Contrucao dos menus
	//**************************************************

		int a = 0, b = 0;

		// Menu que contem as opcoes wireframe e segments
		window1 = GLUI_Master.create_glui("GLUI",0,INITIALPOS_X + DIMX + 10,INITIALPOS_Y);
		window1->add_checkbox("Wireframe",&wireframe);
		GLUI_Spinner *segment_spinner = 
			window1->add_spinner("Segments:",GLUI_SPINNER_INT,&segments);
		segment_spinner->set_int_limits(3,60); 
		window1->set_main_gfx_window(main_window);

		// Menu que contem as opcoes de rotacao, translacao e luz do candeeiro
		window2 = GLUI_Master.create_glui_subwindow(main_window,GLUI_SUBWINDOW_BOTTOM);
		window2->set_main_gfx_window(main_window);
		GLUI_Rotation *view_rot = window2->add_rotation("Rotacao",view_rotate);
		view_rot->set_spin(1.0);
		window2->add_column(false);
		GLUI_Translation *trans_z = 
			window2->add_translation("Zoom",GLUI_TRANSLATION_Z,&obj_pos[2]);
		trans_z->set_speed(.02);
		window2->add_column(true);
		mov_button = window2->add_button("GO!",0,(GLUI_Update_CB)move_carro);
		panel = window2->add_panel("",GLUI_PANEL_EMBOSSED);
		group = window2->add_radiogroup_to_panel(panel,&mov_type);
		window2->add_radiobutton_to_group(group,"Simples");
		window2->add_radiobutton_to_group(group,"Composto");
		window2->add_column(true);
		window2->add_button("Candeeiro",0,(GLUI_Update_CB)luz_candeeiro);

	//**************************************************


	GLUI_Master.set_glutIdleFunc(myGlutIdle);
	inicializacao();
	glutMainLoop();

	return 0;
}
