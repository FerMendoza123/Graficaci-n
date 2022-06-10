#include <vector>
#include <string>
#include "Face.hpp"

using namespace std;

class Object
{
	//Contiene dos vectores porque necesitamos que guarde la informacion de varias caras y varios vertices
	//Necesitamos saber cual es el primer vertice de este objeto
	//Ya que la numeracion de los vertices es compartida por los objetos
	//del archivo
	private: 
		int firstVertex;
		//Necesitamos saber si ya fue asignado el primer vertice para no 
		//volver a asignarlo
		bool firstVertexFlag;

		//Vector de vertices
		vector<Vertex> vertices;

		Vertex centro;
		//Vector de caras
		vector<Face> faces;
		
		float Kd;

		float Ks;		

		int n;

	public: 
		//Nombre del objeto
		string name;
		
		bool visible;
		Object();
		
		//Metodo de modificacion
		void setName(string n);
		
		//Metodo de insertar caras
		void insertFace(Face face);
		
		//Metodo de insertar vertice
		void insertVertex(Vertex ver);
		
		//Metodo que asignara el primer vertice si no ha sido asignado
		void setFirstVertex(int fV);
		
		//Metodo de acceso que nos dice si ya tiene caras y verices para ser insertado en el vector
		bool ready();

		//Metodo para calcular las normales de las caras del objeto
		void calculateNormals();

		//Metodo para calcular la visibilidad de las caras del objeto
		void determineVisibility(Vertex PRP);

		//Metodo para calcular los vectores necesarios para el color de la cara
		void determineColors(Vertex source, Vertex PRP, float sourceIntensity,float ambientLight);

		//Calcula el centro de la figura
		void calculateCenter();

		//Funcion que regresa el centro de un objeto
		Vertex getCenter();

		//Metodo para dibujar la figura
		void draw();
		
		//Matodo para rotar el objeto 1 grado
		void posRotateX();
		
		//Matodo para rotar el objeto 1 grado
		void posRotateY();
		
		//Matodo para rotar el objeto 1 grado
		void posRotateZ();
		
		//Matodo para rotar el objeto 1 grado
		void negRotateX();
		
		//Matodo para rotar el objeto 1 grado
		void negRotateY();
		
		//Matodo para rotar el objeto 1 grado
		void negRotateZ();

		void rotate(int tipe);
		
		void traslatePointsOrigin(Vertex d);

		void traslatePointsBack(Vertex d);
		//Es una función que traslada los puntos a su misma posición mas el incremento
		void traslate(Vertex d);

		void setKd(float value)
		{
			Kd = value;
		}

		void setKs(float value)
		{
			Ks = value;
		} 

		void setn(int value)
		{
			n = value;
		}
};
