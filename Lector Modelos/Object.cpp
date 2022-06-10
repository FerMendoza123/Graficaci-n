#include "Object.hpp"
#include <GL/glut.h>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;
/*
void imprimeVertice(int i,int f,Vertex v);

void imprimeVertice(int i,int f,Vertex v)
{
	cout<<endl<<"vertice "<<f<<" de la cara "<<i<<": "<<v.x<<" "<<v.y<<" "<<v.z<<" ";
}*/

Object::Object()
{
//Indicamos que no se ha asignado el primer vertice de la figura
	firstVertexFlag=false;
	firstVertex=0;
}

void Object::setName(string n)
{
	name=n;
}

void Object::insertFace(Face face)
{
	faces.push_back(face);
}

void Object::insertVertex(Vertex ver)
{
	vertices.push_back(ver);
}

void Object::setFirstVertex(int fV)
{
//Chequeo con bandera
	if(!firstVertexFlag)
	{
		firstVertex=fV;
		firstVertexFlag=true;
	}
}

bool Object::ready()
{
	if(!vertices.empty()&&!faces.empty())
		return true;
	return false;
}

void Object::draw()
{
	//Dibujado de caras
	for(int j=0;j<faces.size();j++)
	{
		if(faces[j].visible)
		{

			glColor3f (faces[j].I, faces[j].I, faces[j].I);
			glBegin(GL_POLYGON);
			//Todas las cordenadas de los vertices
			//Aquí es donde se usa first vertex 
			//vertices[faces[j].idVertices[k]-1-firstVertex].getX() += xi;
			for(int k=0;k<faces[j].idVertices.size();k++)
			{	glVertex3f(vertices[faces[j].idVertices[k]-1-firstVertex].getX(),
				vertices[faces[j].idVertices[k]-1-firstVertex].getY(),
				vertices[faces[j].idVertices[k]-1-firstVertex].getZ());
			}
			glEnd();
		}
	}
}

void Object::determineVisibility(Vertex PRP)
{
	calculateNormals();
	Vertex auxVector;
	Vertex V;
	float magnitude;

	float visible;
	for (int f = 0; f<faces.size(); f++)
    {
    	auxVector.x = vertices[faces[f].idVertices[0]-1-firstVertex].x;
		auxVector.y = vertices[faces[f].idVertices[0]-1-firstVertex].y;
		auxVector.z = vertices[faces[f].idVertices[0]-1-firstVertex].z;

    	V.x = PRP.x-auxVector.x;
		V.y = PRP.y-auxVector.y;
		V.z = PRP.z-auxVector.z;

		magnitude = V.x*V.x + V.y*V.y + V.z*V.z;

		V.x /= magnitude;
		V.y /= magnitude;
		V.z /= magnitude;


    	//imprimeVertice(f,faces[f].normal);
    	visible = (faces[f].normal.x*V.x) +
    				(faces[f].normal.y*V.y) +
    				(faces[f].normal.z*V.z);
    	if(visible<0){
    		faces[f].visible=false;
    		//cout<<endl<<"cara "<<f<<" no visible";
    	}
    	else{
    		faces[f].visible=true;
    		//cout<<endl<<"cara "<<f<<" visible";
    	}
    }	
}

void Object::calculateNormals()
{
	Vertex R1, R2;
	float magnitud=0;
	for(int f = 0;f < faces.size(); f++)
	{
		R1.x = vertices[faces[f].idVertices[1]-1-firstVertex].x -
			    vertices[faces[f].idVertices[0]-1-firstVertex].x;
		R1.y = vertices[faces[f].idVertices[1]-1-firstVertex].y -
				vertices[faces[f].idVertices[0]-1-firstVertex].y;
		R1.z = vertices[faces[f].idVertices[1]-1-firstVertex].z -
				vertices[faces[f].idVertices[0]-1-firstVertex].z;
		
		/*
		imprimeVertice(f,3,vertices[faces[f].idVertices[2]-1-firstVertex]);
		imprimeVertice(f,2,vertices[faces[f].idVertices[1]-1-firstVertex]);
		imprimeVertice(f,1,vertices[faces[f].idVertices[0]-1-firstVertex]);
		*/
		R2.x = vertices[faces[f].idVertices[2]-1-firstVertex].x -
				vertices[faces[f].idVertices[0]-1-firstVertex].x;
		R2.y = vertices[faces[f].idVertices[2]-1-firstVertex].y -
				vertices[faces[f].idVertices[0]-1-firstVertex].y;
		R2.z = vertices[faces[f].idVertices[2]-1-firstVertex].z -
				vertices[faces[f].idVertices[0]-1-firstVertex].z;
		/*
		imprimeVertice(f,1,R1);
		imprimeVertice(f,2,R2);
		*/

		//Vector normal
		faces[f].normal.x = (R1.y*R2.z) - (R1.z*R2.y);
		faces[f].normal.y = (R1.z*R2.x) - (R1.x*R2.z);
		faces[f].normal.z = (R1.x*R2.y) - (R1.y*R2.x);
		//Obtencion de vector normal unitario
		magnitud = faces[f].normal.x*faces[f].normal.x + faces[f].normal.y*faces[f].normal.y + faces[f].normal.z*faces[f].normal.z;
		magnitud = sqrt(magnitud);
		faces[f].normal.x /= magnitud;
		faces[f].normal.y /= magnitud;
		faces[f].normal.z /= magnitud;
		//imprimeVertice(f,3,faces[f].normal);
		//cout<<endl;
	}
}

void Object::determineColors(Vertex source, Vertex PRP, float sourceIntensity,float ambientLight)
{
	determineVisibility(PRP);
	//Con este vertice voy a tomar el primer vertice de cada cara para 
	//restarselo al vector de la fuente de luz y obtener el vector para la cara
	Vertex auxVector;
	float magnitude;

	Vertex L, R,V;

	float factorR, NxL, RxV;
	for(int f=0;f<faces.size();f++)
	{
		if(faces[f].visible)
		{
			
			auxVector.x = vertices[faces[f].idVertices[0]-1-firstVertex].x;
			auxVector.y = vertices[faces[f].idVertices[0]-1-firstVertex].y;
			auxVector.z = vertices[faces[f].idVertices[0]-1-firstVertex].z;

			//Calculo de vector de la cara hacia la fuente de luz
			L.x = source.x-auxVector.x;
			L.y = source.y-auxVector.y;
			L.z = source.z-auxVector.z;
			
			magnitude = L.x*L.x + L.y*L.y + L.z*L.z;

			magnitude = sqrt(magnitude);

			L.x /= magnitude;
			L.y /= magnitude;
			L.z /= magnitude;


			//Calculo del vector de la cara hacia el PRP
			
			V.x = PRP.x-auxVector.x;
			V.y = PRP.y-auxVector.y;
			V.z = PRP.z-auxVector.z;

			magnitude = V.x*V.x + V.y*V.y + V.z*V.z;

			V.x /= magnitude;
			V.y /= magnitude;
			V.z /= magnitude;


			NxL = faces[f].normal.x*L.x + faces[f].normal.y*L.y + faces[f].normal.z*L.z;

			factorR = NxL*2;
			R.x = factorR * faces[f].normal.x - L.x;
			R.y = factorR * faces[f].normal.y - L.y;
			R.z = factorR * faces[f].normal.z - L.z;

			RxV = R.x*V.x + R.y*V.y + R.z*V.z;


			faces[f].I = ambientLight + sourceIntensity*(Kd*NxL + Ks*pow(RxV,n));
		}
	}



	
	
	
}

void Object::calculateCenter()
{
	int i;
	float xp=0,yp=0,zp=0;
	for(i=0;i<vertices.size();i++)
	{
		xp += vertices[i].x;
		yp += vertices[i].y;
		zp += vertices[i].z;
	}
	xp/=i;
	yp/=i;
	zp/=i;
	
	centro = Vertex(xp,yp,zp);
}

Vertex Object::getCenter()
{
	return centro;
}
void Object::posRotateX()
{	
	
	rotate(1);
}	

void Object::posRotateY()
{
	rotate(2);
}

void Object::posRotateZ()
{
	rotate(3);
}

void Object::negRotateX()
{	
	
	rotate(4);
}	

void Object::negRotateY()
{
	rotate(5);
}

void Object::negRotateZ()
{
	rotate(6);
}
//A esta función se le tiene que pasar el pivote
void Object::traslatePointsOrigin(Vertex d)
{
	for(int i=0;i<vertices.size();i++)
	{
		vertices[i].x-=d.x;
		vertices[i].y-=d.y;
		vertices[i].z-=d.z;
	}
}
//A esta funcion se le tiene que pasar el punto original del pivote
void Object::traslatePointsBack(Vertex d)
{
	for(int i=0;i<vertices.size();i++)
	{
		vertices[i].x+=d.x;
		vertices[i].y+=d.y;
		vertices[i].z+=d.z;
	}
}

void Object::traslate(Vertex d)
{
	
	calculateCenter();
	traslatePointsOrigin(centro);

	for(int i=0;i<vertices.size();i++)
	{
		vertices[i].x+=d.x;
		vertices[i].y+=d.y;
		vertices[i].z+=d.z;
	}
	calculateCenter();
}	
		
void Object::rotate(int tipe)
{
	calculateCenter();
	traslatePointsOrigin(centro);
	
	float grad=3.1416/180;
	if(tipe>3)
	{
		grad = -grad;
		tipe /= 2;
	}

	for(int j=0;j<vertices.size();j++)
	{	
		float x,y,z; 
		x = vertices[j].x;
		y = vertices[j].y;
		z = vertices[j].z;
		
		switch(tipe)
		{
			case 1:
				vertices[j].y = cos(grad)*y - sin(grad)*z;
				vertices[j].z = sin(grad)*y + cos(grad)*z;
				break;
			case 2:
				vertices[j].x = cos(grad)*x + sin(grad)*z;
				vertices[j].z = cos(grad)*z - sin(grad)*x;
				break;
			case 3:
				vertices[j].x = cos(grad)*x - sin(grad)*y;
				vertices[j].y = sin(grad)*x + cos(grad)*y;
				break;
		}
		///cout<<endl<<vertices[j].x<<" "<<vertices[j].y<<" "<<vertices[j].z<<" ";	
	}
	traslatePointsBack(centro);
}
