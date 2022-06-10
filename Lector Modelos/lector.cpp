#include <fstream>
#include <iostream>
#include <iomanip>
#include <GL/glut.h>
#include "Object.hpp"
#include <ctime>
#include <cstdlib>
#include <cmath>

//Frequency for the animation
//#define REFRESH_FREQ 30

//int refresh_Freq=REFRESH_FREQ;
int refresh_Freq=56;
int timer;
bool start;
bool pause;
int roundN=1;
void reset();

int camera;
bool cameraFeatures;

//Variables usadas para iluminacion
//Coordenadas de la fuente de iluminación
//Vertex fuenteLuz = Vertex(200,100,30);
Vertex fuenteLuz = Vertex(200,50,30);

//luz de ambiente
float I_A = 1;
float K_A = 0.1;
float ambientLight = I_A * K_A;
//luces difusa y especular
float Il=1; //Intesidad de la fuente de luz



using namespace std;

//Vector de objetos necesario para almacenar el archivo leido
vector<Object> objetos;

vector<Vertex> vertices;

//Indices de los objetos
int airplaneIndex;
int parachuteIndex;

//Variables necesarias para la rutina de movimientos
bool curveMov;
bool lineMov;

bool firstValueBezier;
bool firstValueLine;
//Variables necesarias para controlar DDA
Vertex startPt,currentPtDDA,finPt,incremPt;
int steps,stepCount;

//Variables necesarias para controlar bezier
float t;
float t_3; 
float t_2;
float _3t_3;
float _3t_2;
float _3t;

float expr1;
float expr2;
float expr3;
float expr4;

float incremBz;

Vertex p1,p2,p3,p4;
Vertex currentPtBez;
Vertex currentPtCam;

int rotations;
int currentRotations;
bool positiveRotation;
int grados;

Vertex PRP;
Vertex PRP2;
bool prp2Set;

void callNextTimer(int value);
//Función que controla DDA, recibe el apuntador del objeto sobre el que actuara
void lineDDA();
//Funcion para movimiento bezier de un objeto, recibe el apuntador del objeto sobre el que actuara
void bezier();
//deg son los grados a rotar, 
//void rotateRutine();

void Timer_2(int val);
//void Timer_3(int val);

void putPixel(float x,float y,float z);

class Reader
{
	private: 
		ifstream file;
	
	public:
		Reader()
		{
		}
		
		bool readFile(string fileName,vector<Object>*collection)
		{
			bool startLine=true;
		   //Con esta cadena leeremos el nombre de el Objeto
			string objName;
			//Se utilizará para hacer verificaciones como el caracter de inicio de linea (o, v, f)y en ocaciones el de fin de linea
			char c;
			//Con estas variables leeremos las coordenadas de un vertice, despues de eso podremos instanciar un objeto de Vertex
			float x,y,z;
			int vertexCount=0;
			//Con esta variable podremos leer todos los indices de vertices para la clase Face
			int idVer;
			Face auxFace;
			//Auqui podremos almacenar nuestras caras y vertices para luego insertarlo en el vector de la clase Objetos
			Object auxObj;
			file.open(fileName);
			if(!file.is_open()) //Verificacion de apertura
			{
			//Si no se pudo retornamos el resultado
				return false;
			}
			
			ifstream reader;
			reader.open(fileName);
			//Verificacion de apertura
			if(!reader.is_open())
			{
				cout<< "No se encontró el archivo especificado";
				exit(1);
			}		
			//Si no se salio podemos continuar con todo
			
			/*
	Con la estructura vector tenemos que insertar los elementos, es decir, tenemos que instanciar un objeto del tipo de elementos del vector, siendo así ya que nuestro vector principal es contenedor de objetos de clase Objeto tendríamos que crear un Objeto e insertarlo, sin embargo dentro de el objeto tenemos un vector de tipo Vertex, y otro de tipo Face, y dentro de la clase Face tenemos un vector de enteros,así que tendremos que crear uno a uno los elementos de clase Face e insertarlos en el vector de clase Face de un Objeto aucciliar, de igual forma tendremos que crear uno a uno los elementos de clase Vertex e insertarlos en el vector de clase Vertex del Objeto auxiliar. 
			*/
			
		
			
			
			//Se lee mientras no se termine el archivo
			while(!reader.eof())
			{
				//Se obtienen caracteres
				reader.get(c);
				//Solo cuando es principio de linea puede ser algo que nos interese
				if(startLine)
				{
					//Las tres opciones son una cara, un vertice o una nueva figura
					switch(c)
					{
						case 'f':
						{
						//Si es una cara instanciamos un nuevo aucciliar
							auxFace = Face();
							//Leemos los vertices hasta que llegue al fin de linea
							do
							{
								reader>>idVer;
								reader.get(c);
								auxFace.insertId(idVer);
							}while(c!='\r'&&c!='\n');
							//Se inserta en el objeto auxiliar
							auxObj.insertFace(auxFace);
						}
						break;
						case 'o':
						{
						//Si es una figura verificamos si hay una anterior
							if(auxObj.ready())
							{
							//Si es así la insertamos en el vector de objetos
								 collection->push_back(auxObj);
							}
							//Instanciamos un nuevo auxiliar
							auxObj = Object();
							//Leemos el nombre
							reader >>objName;
							//Y lo asignamos
							auxObj.setName(objName);
						}
						break;
						case 'v':
						{
						//Si es un vertice leemos las coordenadas
							reader >>x>>y>>z;
							//Instanciamos el vertice
							Vertex auxVer = Vertex(x,y,z);
							//Enviamos el numero de vertice actual para que lo asigne si es que es el primero del objeto
							auxObj.setFirstVertex(vertexCount++);
							//insertamos el vertice en el objeto auxiliar
							auxObj.insertVertex(auxVer);
						}
						break;
					}
				}
				//Si se leyo el primer caracter dejará de ser inicio de linea
				if(startLine)
					startLine= false;
				//Si el caracter es fin de linea el seguiente será inicio de linea
				if(c=='\n')
				{
					startLine=true;
				}
			}
			//Se inserta el ultimo auxiliar usado
			if(auxObj.ready())
			{
				collection->push_back(auxObj);
			}	
			
			file.close();
			//Regresamos el resultado
			return true;
		}
};


void display (void)
{
	//Borrado de pantalla
	glClear (GL_COLOR_BUFFER_BIT);
	//Se establece el color de dibujado en blanco
	glColor3f (5.0/255, 94.0/255, 5.0/255);
	//Se dibujan los ejes coordenados
	
	//Con esto se dibuja el plano

	glBegin(GL_POLYGON);
	//for (GLfloat i = -100; i <= 300; i += 30) {
		glVertex3f(10, 0, 100); 
		glVertex3f(10, 0, -100);
		glVertex3f(300, 0, -100); 
		glVertex3f(300, 0, 100);
	//}
	glEnd();
	//Con esto se dibujan los puntos por donde pasa el avion
	
	glColor3f (0.0, 0.0, 0.0);
	for (int i=0;i<vertices.size();i++)
		putPixel(vertices[i].x,vertices[i].y,vertices[i].z);
	
	//Se dibuja el eje x
	for(float y=0;y<=300;y+=0.01){putPixel(y,0,0);}
	
	//Se dibujan los objetos que estén disponibles
	for(int i=0;i<objetos.size();i++)
	{
		if(objetos[i].visible)
		{
			if(camera==1)
				objetos[i].determineColors(fuenteLuz,PRP,Il,ambientLight);
			else
				objetos[i].determineColors(fuenteLuz,PRP2,Il,ambientLight);
			objetos[i].draw();
		}
	}	
	//cout<<camera<<"\n";
	//Se intercambia el buffer
	glutSwapBuffers();
}





//Este timer es para el ascenso del avión utilizando una curva bezier
/*void Timer_1(int val)
{
	//Se inicia un movimiento de curva, ya que la bandera cambia cuando entra por ultimo a bezier simpre entrará aquí cuando se encuentre realizando una curva
	if(curveMov)
	{
		//Si es la primera vez entonces asigna los puntos que usará el algoritmo bezier
		if(firstValueBezier)
		{
			Vertex airplaneCenter = objetos[airplaneIndex].getCenter();
			p1=airplaneCenter;
			p1.z=0;
			p2=Vertex(airplaneCenter.getX()+40,airplaneCenter.getY()+5,0);
			p3=Vertex(airplaneCenter.getX()+40,airplaneCenter.getY()+50,0);
			p4=Vertex(airplaneCenter.getX()+100,airplaneCenter.getY()+50,0);
			start=false;
		}
		//Ejecuta una iteración de vecier
		bezier();
		objetos[airplaneIndex].traslate(currentPtBez);
		//glutPostRedisplay();
		//glutTimerFunc(refresh_Freq, Timer_1, val);
		callNextTimer(val);
	}
	else
	{
		firstValueLine = true;
		lineMov = true;
		timer=2;
		callNextTimer(val);
		//glutTimerFunc(0, Timer_2, val);
	}
}*/

void Timer_1(int val)
{
	if(lineMov)
	{
		if(firstValueLine)
		{
			startPt = objetos[airplaneIndex].getCenter();
			start=false;
			finPt = Vertex(150,startPt.getY(),0);
		}
		
		lineDDA();

		if(camera==2)
		{
			glTranslatef(PRP2.x,PRP2.y,PRP2.z);
		}
		Vertex airplaneCenter = objetos[airplaneIndex].getCenter();

		PRP2.x=airplaneCenter.x-20;
		PRP2.y=airplaneCenter.y+10;
		PRP2.z=0;

		if(camera==2)
			glTranslatef(-PRP2.x,-PRP2.y,-PRP2.z);


		objetos[airplaneIndex].traslate(currentPtDDA);
		//glutPostRedisplay();
		//glutTimerFunc(refresh_Freq,Timer_2,val);
		callNextTimer(val);
	}
	else
	{
		//glTranslatef(PRP.x,PRP.y,PRP.z);
		//glRotatef(-90,0,1,0);
		//PRP.x=270;
		//PRP.y=PRP.z=0;
		//glTranslatef(-PRP.x,-PRP.y,-PRP.z);
		//glTranslatef(-270,0,0);

		firstValueLine = true;
		firstValueBezier = true;
		lineMov = true;
		curveMov = true;
		//glutTimerFunc(0,Timer_3,val);
		timer=2;
		callNextTimer(val);
	}
}

void Timer_2(int val)
{
	//cout<<"timer: "<<timer<<endl;
	if(lineMov)
	{
		if(firstValueLine)
		{
			startPt = finPt;
			//cout<<"x:"<<startPt.x<<"y: "<<startPt.y<<"z: "<<startPt.z<<endl;
			finPt = Vertex(350,startPt.getY(),0);
		}
		lineDDA();
		//cout<<"linemov\n";
		objetos[airplaneIndex].traslate(currentPtDDA);
	}
	if (curveMov)
	{

		if(firstValueBezier)
		{
			
			Vertex center = objetos[airplaneIndex].getCenter();
			p1=startPt;
			p2=Vertex(startPt.getX()+(float)(900/(refresh_Freq*2))*center.getY()/50,44*center.getY()/50,0);
			p3=Vertex(startPt.getX()+(float)(1800/(refresh_Freq*2))*center.getY()/50,24*center.getY()/50,0);
			p4=Vertex(startPt.getX()+(float)(2700/(refresh_Freq*2))*center.getY()/50,0,0);

			//glTranslatef(PRP.x,PRP.y,PRP.z);
			//glRotatef(-90,0,1,0);
			//glTranslatef(-(p4.x+20),0,0);
			//fuenteLuz=p4;
			//fuenteLuz.x+=20;
			
			objetos[parachuteIndex].visible = true;
			rotations=60;
			positiveRotation = true;
			currentRotations=rotations;
			for(int i=0;i<rotations-10;i++)
				objetos[parachuteIndex].negRotateZ();
			//incremBz = incremBz*(float)refresh_Freq/20;
		}

		int i;
		if(rotations>0)
		{
				
			for (i = 0; i < 2; i++)
			{
				if(positiveRotation)
					objetos[parachuteIndex].posRotateZ();	
				else
					objetos[parachuteIndex].negRotateZ();
			}

			currentRotations -= 2;
			if(currentRotations <= 0)
			{
				if(positiveRotation)
					rotations-=15;
				currentRotations = rotations;
				positiveRotation = !positiveRotation;

			}
		}

		bezier();


		if(camera==2)
		{
			glTranslatef(PRP2.x,PRP2.y,PRP2.z);
		}
		Vertex parachuteCenter = objetos[parachuteIndex].getCenter();

		PRP2.x=parachuteCenter.x-20;
		PRP2.y=parachuteCenter.y+10;
		PRP2.z=0;

		if(camera==2)
			glTranslatef(-PRP2.x,-PRP2.y,-PRP2.z);

		//currentPtBez.x=currentPtDDA.x;
		objetos[parachuteIndex].traslate(currentPtBez);
	}

	if(!curveMov&&!lineMov&&roundN<4)
		reset();
	//glutPostRedisplay();
	//glutTimerFunc(refresh_Freq,Timer_3,val);
	callNextTimer(val);
}

void lineDDA()
{
	//Inicialización de las variables necesarias para DDA
	if(firstValueLine)
	{
		//Selección de dimensión principal para el algoritmo
		//(Con esta se define el numero de pasos que se realizará)
		int dx = (finPt.x-startPt.x);
		int dy = (finPt.y-startPt.y);
		int dz = (finPt.z-startPt.z);
		
		if(abs(dx) > abs(dy)){
			steps = abs(dx);
		}
		else{
			steps = abs(dy);
		}
		
		if(abs(dz) > steps){
			steps = abs(dz);
		}
		//Asignación del punto actual
		currentPtDDA.x = startPt.x;
		currentPtDDA.y = startPt.y;
		currentPtDDA.z = startPt.z;
		
		//Calculo de los valores de incremento con respecto al numero de pasos
		incremPt.x = float(dx)/float(steps);
		incremPt.y = float(dy)/float(steps);
		incremPt.z = float(dz)/float(steps);

		//Cambio de valor de la bandera
		firstValueLine = false;
		//Asignación de numero de pasos a 0
		stepCount = 0;
		//cout<<"firstValueLine\n";
	}
	else
	{
		//cout<<steps<<"timer: "<<timer<<"\n";
		//Si no se uan dado todos los pasos
		if(stepCount<steps)
		{
			//Calculo del nuevo punto actual
			currentPtDDA.x += incremPt.x;
			currentPtDDA.y += incremPt.y;
			currentPtDDA.z += incremPt.z;
			//Vertex n = Vertex(currentPtDDA.x,currentPtDDA.y,currentPtDDA.z);
			//Inserción del punto al vector de trayectoria
			//vertices.push_back(n);
			//Aumento de la cuenta de pasos
			stepCount ++;
		}
		else
		{
			//Si terminó indicamos que el movimiento de esta linea finalizó
			lineMov=false;
		}
	}
}

//Función que calcula los puntos para la grafica bezier
void bezier()
{
	//Si es el primer valor
	if(firstValueBezier)
	{
		//Cambia la bandera para saber que ya entró aquí
		firstValueBezier = false;
		//Pone el punto actual al punto inicial de bezier
		currentPtBez = p1;//Vertex(p1.x,p1.y,p1.z);
		//Variable de incremento de biezier
		t = 0;
	}
	else
	{
		//Mientras la variable de control no llegue al final entonces continua
		if(t<=1)
		{
			//Precalculamos los valores necesarios para  esta iteración
			t_2 = t*t;
			t_3 = t_2*t;
			_3t = 3*t;
			_3t_2 = 3*t_2;
			_3t_3 = 3*t_3;
			
			expr1 = 1 - _3t + _3t_2 -t_3;
			expr2 = _3t - 2*_3t_2 + _3t_3;
			expr3 = _3t_2 - _3t_3;
			expr4 = t_3;

			//Calculamos el nuevo punto actual
			currentPtBez.x = expr1*p1.x + expr2*p2.x + expr3*p3.x + expr4*p4.x;
			currentPtBez.y = expr1*p1.y + expr2*p2.y + expr3*p3.y + expr4*p4.y;
			currentPtBez.z = expr1*p1.z + expr2*p2.z + expr3*p3.z + expr4*p4.z;
			Vertex n = Vertex(currentPtBez.x,currentPtBez.y,currentPtBez.z);
			//Introdcimos el punto al vector de puntos (para redibujar siempre la trayectoria)
			vertices.push_back(n);
			//Incrementamos la variable de control
			t += incremBz;
		}
		else
		{
			//Indicamos que el movimento de esta curva terminó
			curveMov = false;
		}
	}	
}

//Función que dibuja un pixel en las coordenadas especificadas
void putPixel(float x,float y,float z)
{
	glBegin(GL_POINTS);
	glVertex3f(x,y,z);
	glEnd();
}

void init (void) 
{
	PRP.x=155;
	PRP.y=50;
	PRP.z=160;
   camera=1;
   //Color con el cual se limpiara la pantalla
   glClearColor (17.0/255, 90.0/255, 130.0/255, 0);
   ambientLight = (17+90+130)/(3*255);
   //glClearColor(ambientLight,ambientLight,ambientLight,ambientLight);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   //glOrtho(20.0, -20.0, -20.0, 20.0, -20.0, 60.0);
   
   //Función para visualización en perspectiva
   glFrustum(-1, 1, -1, 1, 1, 300);
   
   //Aquí se leen los archivos, si no se pudo abrir alguno despliega un mensaje
   	Reader reader = Reader();
	if(!reader.readFile("airplane.obj",&objetos))
	{
		printf("No se pudo abrir el archivo\n");
		exit(0);
	}
	if(!reader.readFile("parachute.obj",&objetos))
	{
		printf("No se pudo abrir el archivo\n");
		exit(0);
	}
	/*reader.readFile("cube.obj",&objetos);
	objetos[0].visible=true;
	airplaneIndex=0;*/
	//En este for se asigna para cada objeto sus caracteristicas iniciales
	for(int i=0;i<objetos.size();i++)
	{
		//Al inicio el paracaidas no sera visible
		if(objetos[i].name.compare("parachute")==0)
		{
			//Guardado del indice del paracaidas
			parachuteIndex = i;
			objetos[i].visible=false;
			objetos[i].setn(3);
			objetos[i].setKd(0.8);
			objetos[i].setKs(1);
			for (int j = 0; j < 90; j++)
			{
				objetos[i].negRotateY();
			}
		}
		//El avion será visible 
		if(objetos[i].name.compare("airplane")==0)
		{
			//Guardado del indice del avion
			airplaneIndex = i;
			objetos[i].visible=true;
			objetos[i].setn(1);
			objetos[i].setKd(0.7);
			objetos[i].setKs(1);
			//Hay que rotar el avión para que su frente sea +X
			for (int j = 0; j < 90; j++)
			{
				objetos[i].negRotateY();
			}
		}
	}
	//Se rota un poco la escena para que se pueda visualizar mejor

	//Se traslada la escena a un punto en donde la camara la capte mejor
	
	
	//glRotatef(30,1,0,0);
	//Se inicializan las variables globales para las funciones de movimiento
	//curveMov = true;
	reset();
}

void reset()
{
	timer=1;
	pause=true;
    start=true;
    lineMov = true;
	firstValueLine=true;
	Vertex airplaneCenter = objetos[airplaneIndex].getCenter();
	airplaneCenter.x=0;
	objetos[airplaneIndex].traslate(airplaneCenter);
	
	refresh_Freq-=13;
	objetos[airplaneIndex].traslate(Vertex(0,airplaneCenter.getY(),0));
	objetos[parachuteIndex].visible=false;
	objetos[airplaneIndex].visible=true;
	//callNextTimer(0);

	
	if(camera==1&&roundN==1)
	{
		//glRotatef(-90,0,1,0);
		glTranslatef(-PRP.x,-PRP.y,-PRP.z);

	}
	//else
	if(camera==2)
	{
		glTranslatef(PRP2.x,PRP2.y,PRP2.z);
	}
	PRP2=airplaneCenter;
	PRP2.x-=20;
	PRP2.y+=10;
	PRP2.z=0;
	//}

	if(camera==2)
	{
		glTranslatef(-PRP2.x,-PRP2.y,-PRP2.z);
	}
	roundN++;
	callNextTimer(0);
}

void keyboard (unsigned char key, int x, int y)
{
	switch(key)
	{
		case's':
			if(start)
			{
				Vertex airplaneCenter = objetos[airplaneIndex].getCenter();
				if(airplaneCenter.y>10)
				{
					airplaneCenter.y-=10;
					if(camera==2)
						glTranslatef(0,10,0);
					PRP2.x=airplaneCenter.x-20;
					PRP2.y=airplaneCenter.y+10;
					PRP2.z=0;

					objetos[airplaneIndex].traslate(airplaneCenter);
				}
			}
			break;
		case'w':
			if(start)
			{
				
				Vertex airplaneCenter = objetos[airplaneIndex].getCenter();
				if(airplaneCenter.y<80)
				{
					airplaneCenter.y+=10;
					if(camera==2)
						glTranslatef(0,-10,0);
					PRP2.x=airplaneCenter.x-20;
					PRP2.y=airplaneCenter.y+10;
					PRP2.z=0;

					objetos[airplaneIndex].traslate(airplaneCenter);
				}
			}
			break;
		case'p':
			pause=!pause;
			if(start)
			{
				Vertex airplaneCenter = objetos[airplaneIndex].getCenter();
				incremBz=0.007*(refresh_Freq/20.0)*(50.0/airplaneCenter.getY());
			}
			break;
		case 'c':
			if(pause)
			{
				if(camera==1)
				{
					glTranslatef(PRP.x,PRP.y,PRP.z);
					glRotatef(90,0,1,0);
					camera=2;
					glTranslatef(-PRP2.x,-PRP2.y,-PRP2.z);
				}
				else
				{
					
					glTranslatef(PRP2.x,PRP2.y,PRP2.z);
					camera=1;
					glRotatef(-90,0,1,0);
					glTranslatef(-PRP.x,-PRP.y,-PRP.z);
				}
			}
			break;
		default:break;
	}

	callNextTimer(0);
}

void callNextTimer(int value)
{
	glutPostRedisplay();	
	if(!pause)
	{
		switch(timer)
		{
			case 1:
				glutTimerFunc(refresh_Freq,Timer_1,value);
				break;
			case 2:
				glutTimerFunc(refresh_Freq,Timer_2,value);
				break;
			/*case 3:
				glutTimerFunc(refresh_Freq,Timer_3,value);
				break;*/
			default: break;
		}
	}
}


int main (int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   //glutInitWindowSize (1024, 640); 
   glutInitWindowSize (1200, 700); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Lector");
   //Inicialización de todas las caracteristicas necesarias
   init ();
   glutDisplayFunc(display); 
   glutKeyboardFunc(keyboard);
   //glutTimerFunc(0, Timer_1, 0);
   //callNextTimer(0);
   glutMainLoop();
   return 0;
}
