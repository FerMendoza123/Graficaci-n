#include <vector>
#include "Vertex.hpp"
//Clase cara en la cual se guardaran los indices de los vertices que la conforman
using namespace std;

class Face
{
	//Use la clase vector porque es dinamica

	public: 
		vector<int> idVertices;
		bool visible;
		Vertex normal;
		//Vertex fuenteLuz;
		float I;
	public: 
		Face();
		void insertId(int id);
};
