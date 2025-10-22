#include "BOX.h"
#include <IGL/IGlib.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#define M_PI 3.14159265358979323846
#include <vector>
//assimp libs
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


//Variables globales para camara
bool leftButtonDown = false;
float lastX = 0.0f;
float lastY = 0.0f;


//Idenficadores de los objetos de la escena
int objId = -1;

//Vector para objetos m�ltiples
std::vector<int> objIds;

//Declaraci�n de CB
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);
void mouseMoveFunc(int x, int y);
void mousePassMoveFunc(int x, int y);
void mouseWheelFunc(int wheel, int dir, int x, int y);


//Variables de carga de objetos
const aiScene* sc; //Escena con objetos 3D cargados en formato interno de assimp

//Funcion propia carga de objetos
bool carga3D(const std::string& pFile);
void cargarOriginal();
//Variables para c�mara
glm::vec3 camaraPos = glm::vec3(5.0f, 2.0f, 5.0f);
glm::vec3 camaraCentro = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camaraUp = glm::vec3(0.0f, 1.0f, 0.0f);


int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)
	if (!IGlib::init("../shaders_P1/shader.v9.vert", "../shaders_P1/shader.v9.frag"))
		return -1;


	
	//Se ajusta la c�mara
	//Si no se da valor se cojen valores por defecto
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		





	glm::mat4 proj = glm::mat4(1.0);
	float f = 1.0 / tan(3.141592 / 6.0);
	float far = 50.0;
	float near = 0.1;

	proj[0].x = f;
	proj[1].y = f;
	proj[2].z = (far + near) / (near - far);
	proj[2].w = -1.0;
	proj[3].z = (2.0 * far * near) / (near - far);
	proj[3].w = 0.0;


	IGlib::setProjMat(proj);
	IGlib::setViewMat(view);
	
	cargarOriginal();


	




	//Incluir texturas aqu�.
	//IGlib::addColorTex(objId, "../img/color.png");




	//CBs
	IGlib::setResizeCB(resizeFunc);
	IGlib::setKeyboardCB(keyboardFunc);
	IGlib::setMouseCB(mouseFunc);
	IGlib::setMouseMoveCB(mouseMoveFunc);
	IGlib::setMousePassiveMoveCB(mousePassMoveFunc);
	IGlib::setMouseWheelMoveCB(mouseWheelFunc);


	//Mainloop
	IGlib::mainLoop();
	IGlib::destroy();
	return 0;
}

void cargarOriginal()
{
	size_t start = objIds.size();
	if (!carga3D("../3D_assets/Beretta_M9.obj")) {
		std::cout << "Error cargando el modelo 3D" << std::endl;
	}
	else {
		glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
		for (size_t i = start; i < objIds.size(); ++i)
			IGlib::setModelMat(objIds[i], s);
	}

}


void resizeFunc(int width, int height)
{
	//Ajusta el aspect ratio al tama�o de la venta
}

/*void idleFunc()
{
	static float angle = 0.0f;
	angle = (angle < 3.141592f * 2.0f) ? angle + 0.01f : 0.0f;
	glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	modelMat = glm::rotate(modelMat, angle, glm::vec3(0.0f, 1.0f, 0.0f));

	IGlib::setModelMat(objId, modelMat);
}*/

void keyboardFunc(unsigned char key, int x, int y)
{
	std::cout << "Se ha pulsado la tecla " << key << std::endl << std::endl;
	if (key == 'w' || key == 'W') {
		float speed = 0.1f;
		glm::vec3 forward = glm::normalize(camaraCentro - camaraPos);
		camaraPos += forward * speed;
		camaraCentro += forward * speed;
		glm::mat4 view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		IGlib::setViewMat(view);
	}
	if (key == 's' || key == 'S') {
		float speed = 0.1f;
		glm::vec3 forward = glm::normalize(camaraCentro - camaraPos);
		camaraPos -= forward * speed;
		camaraCentro -= forward * speed;
		glm::mat4 view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		IGlib::setViewMat(view);
	}
	if (key == 'a' || key == 'A') {
		float speed = 0.1f;
		glm::vec3 forward = glm::normalize(camaraCentro - camaraPos);
		glm::vec3 right = glm::normalize(glm::cross(forward, camaraUp));
		camaraPos -= right * speed;
		camaraCentro -= right * speed;
		glm::mat4 view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		IGlib::setViewMat(view);
	}
	if (key == 'd' || key == 'D') {
		float speed = 0.1f;
		glm::vec3 forward = glm::normalize(camaraCentro - camaraPos);
		glm::vec3 right = glm::normalize(glm::cross(forward, camaraUp));
		camaraPos += right * speed;
		camaraCentro += right * speed;
		glm::mat4 view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		IGlib::setViewMat(view);
	}
	if (key == 'l' || key == 'L') {
		size_t start = objIds.size();
		if (!carga3D("../3D_assets/Mustang 13.04.2021 Compressed.obj")) {
			std::cout << "Error cargando el modelo 3D" << std::endl;
		}
		else {
			glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
			for (size_t i = start; i < objIds.size(); ++i)
				IGlib::setModelMat(objIds[i], s);
		}
	}

}

void mouseFunc(int button, int state, int x, int y)
{
	if (button == 2) {
		if (state == 0) {
			std::cout << "Se ha pulsado el bot�n ";
			leftButtonDown = true;
			lastX = x;
			lastY = y;
		}
		else {
			leftButtonDown = false;
			std::cout << "Se ha soltado el bot�n ";
		}


	}
	if (button == 0) std::cout << "de la izquierda del rat�n " << std::endl;
	if (button == 1) std::cout << "central del rat�n " << std::endl;
	if (button == 2) std::cout << "de la derecha del rat�n " << std::endl;


	std::cout << "en la posici�n " << x << " " << y << std::endl << std::endl;
}


void mouseMoveFunc(int x, int y)
{
	if (leftButtonDown) {
		const float sensitivity = 0.005f;
		float xoffset = (lastX - x) * sensitivity;
		float yoffset = (lastY - y) * sensitivity; 
		glm::vec3 direction = camaraCentro - camaraPos;
		float radius = glm::length(direction);
		float theta = atan2(direction.z, direction.x);
		float phi = acos(direction.y / radius);
		theta += xoffset;
		phi += yoffset;
		//Limitar phi para evitar que la c�mara se voltee
		if (phi <= 0.1f)
			phi = 0.1f;
		if (phi >= M_PI - 0.1f)
			phi = M_PI - 0.1f;
		direction.x = radius * sin(phi) * cos(theta);
		direction.y = radius * cos(phi);
		direction.z = radius * sin(phi) * sin(theta);
		camaraCentro = camaraPos + direction;
		glm::mat4 view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		IGlib::setViewMat(view);
		

		lastX = x;
		lastY = y;
	}
	std::cout << "Movimiento con presi�n en la posici�n " << x << " " << y << std::endl << std::endl;
}

void mousePassMoveFunc(int x, int y)
{
	//std::cout << "Pasivo en la posici�n " << x << " " << y << std::endl << std::endl;
}
void mouseWheelFunc(int wheel, int dir, int x, int y)
{
	if (dir > 0) {
		std::cout << "Se ha pulsado la rueda del rat�n hacia arriba ";
		//Zoom
		float zoomSpeed = 0.5f;
		glm::mat4 view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		glm::vec3 direccion = glm::normalize(camaraCentro - camaraPos);
		camaraPos += direccion * zoomSpeed;
		IGlib::setViewMat(view);
	}
	else {
		std::cout << "Se ha pulsado la rueda del rat�n hacia abajo ";
		float zoomSpeed = 0.5f;
		glm::mat4 view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		glm::vec3 direccion = glm::normalize(camaraCentro - camaraPos);
		camaraPos -= direccion * zoomSpeed;
		IGlib::setViewMat(view);
	}
	std::cout << "en la posici�n " << x << " " << y << std::endl << std::endl;
}

bool carga3D(const std::string& pFile) {
	Assimp::Importer importer;
	sc = importer.ReadFile(pFile,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	if (!sc) {
		return false;
	}
	std::cout << "Se han cargado " << sc->mNumMeshes << " meshes" << std::endl;

	for (unsigned int m = 0; m < sc->mNumMeshes; ++m) {
		const aiMesh* mesh = sc->mMeshes[m];

		unsigned int* faceArray = nullptr;
		float* vertexArray = nullptr;
		float* normalArray = nullptr;
		float* textCoordsArray = nullptr;

		unsigned int faceIndex = 0, vertexIndex = 0, normalIndex = 0;

		if (mesh->HasFaces()) {
			faceArray = (unsigned int*)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
			for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
				const aiFace* face = &mesh->mFaces[t];
				memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
				faceIndex += 3;
			}
		}

		if (mesh->HasPositions()) {
			vertexArray = (float*)malloc(sizeof(float) * mesh->mNumVertices * 3);
			for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {
				const aiVector3D* v = &mesh->mVertices[k];
				memcpy(&vertexArray[vertexIndex], &float(v->x), sizeof(float));
				memcpy(&vertexArray[vertexIndex + 1], &float(v->y), sizeof(float));
				memcpy(&vertexArray[vertexIndex + 2], &float(v->z), sizeof(float));
				vertexIndex += 3;
			}
		}

		if (mesh->HasNormals()) {
			normalArray = (float*)malloc(sizeof(float) * mesh->mNumVertices * 3);
			for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {
				const aiVector3D* n = &mesh->mNormals[k];
				memcpy(&normalArray[normalIndex], &float(n->x), sizeof(float));
				memcpy(&normalArray[normalIndex + 1], &float(n->y), sizeof(float));
				memcpy(&normalArray[normalIndex + 2], &float(n->z), sizeof(float));
				normalIndex += 3;
			}
		}

		if (mesh->HasTextureCoords(0)) {
			textCoordsArray = (float*)malloc(sizeof(float) * 2 * mesh->mNumVertices);
			for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {
				const aiVector3D* t = &mesh->mTextureCoords[0][k];
				memcpy(&textCoordsArray[k * 2], &float(t->x), sizeof(float));
				memcpy(&textCoordsArray[k * 2 + 1], &float(t->y), sizeof(float));
			}
		}

		// Crear UN solo objeto por mesh (UVs opcionales)
		int id = IGlib::createObj(mesh->mNumFaces, mesh->mNumVertices, faceArray,
			vertexArray, 0, normalArray, textCoordsArray, 0);

		objIds.push_back(id);
		objId = id;

		// Escalado por mesh
		//glm::mat4 modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
		//IGlib::setModelMat(id, modelMat);

		free(faceArray);
		free(vertexArray);
		free(normalArray);
		free(textCoordsArray);
	}
	size_t start = objIds.size();

	return true;
}






