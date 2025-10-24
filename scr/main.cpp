#include "BOX.h"
#include <IGL/IGlib.h>
#include <chrono>

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

//Vector con los IDs de la beretta
std::vector<int> gBerettaIds;


//Idenficadores de los objetos de la escena
int objId = -1;

//Vector para objetos m�ltiples
std::vector<int> objIds;
//Array para los IDs por mesh
std::vector<int> localObjs;

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
	IGlib::setIdleCB(idleFunc);


	//Mainloop
	IGlib::mainLoop();
	IGlib::destroy();
	return 0;
}

void cargarOriginal()
{
	int start = objIds.size();
	if (!carga3D("../3D_assets/Beretta_M9.obj")) {
		std::cout << "Error cargando el modelo 3D" << std::endl;
	}
	else {
		//Escalado extra y mover hacia abajo un poco
		for (int i = start; i < objIds.size(); i++) {
			gBerettaIds.push_back(objIds[i]);
		}
		
	}

}


void resizeFunc(int width, int height)
{
	//Ajusta el aspect ratio al tama�o de la venta
}

//modificada para que gire sobre el propio eje Y y haga un pequeño hover hacia arriba y abajo
void idleFunc()
{
	//usamos la libreria chrono para hacer un efecto más suave, ya que trabaja a nivel de la hora del reloj
	static float angle = 0.0f;
	static auto lastTime = std::chrono::high_resolution_clock::now(); //guarda el último tiempo de giro
	auto currentTime = std::chrono::high_resolution_clock::now(); //guarda el tiempo actual
	float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count(); //calcula el aumento de tiempo
	lastTime = currentTime; //iguala el tiempo actual y anterior para repetir el bucle

	//definimos la velocidad de giro en radianes
	float rotationSpeed = glm::radians(45.0f);
	//el ángulo de rotación es la velocidad de rotación en rad/s por el incremento de tiempo en s
	angle += rotationSpeed * deltaTime;

	//cuando una vuelta completa resetea el ángulo
	if (angle > M_PI * 2.0f)
		angle -= M_PI * 2.0f;

	//Hover arriba y abajo
	float timeSeconds = std::chrono::duration<float>(currentTime.time_since_epoch()).count(); //calcula el tiempo
	float verticalOffset = sin(timeSeconds * 2.0f) * 10.0f; //se multiplica el tiempo por el sena de la frecuencia en hz y por un multiplicador para que sea un hover más o menos alto


	//se implementa al modelo de la beretta
	for (int i = 0; i < gBerettaIds.size(); i++) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));
		model = glm::rotate(model, -3.141592f / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //los ejes de giro están cambiados ya que el modelo ha sido girado para que esté apoyado sobre el mango
		model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.02f));
		model = glm::translate(model, glm::vec3(verticalOffset, 0.0f, 0.0f));
		IGlib::setModelMat(gBerettaIds[i], model);
	}
}

//funcionalidades obligatorias del teclado
void keyboardFunc(unsigned char key, int x, int y)
{
	//cuando se pulsa w la cámara se acerca, todos los métodos de movimiento de cámara funcionan igual
	std::cout << "Se ha pulsado la tecla " << key << std::endl << std::endl;
	if (key == 'w' || key == 'W') {
		//definimos una velocidad de la cámara
		float speed = 0.1f;
		//creamos un vector unitario entre el centro de la cámara y su posición
		glm::vec3 forward = glm::normalize(camaraCentro - camaraPos);
		//cambiamos el centro de la cámara y su posición con el vector y su velocidad
		camaraPos += forward * speed;
		camaraCentro += forward * speed;
		//cambiamos la matriz de vista
		glm::mat4 view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		IGlib::setViewMat(view);
	}
	//con s se aleja
	if (key == 's' || key == 'S') {
		float speed = 0.1f;
		glm::vec3 forward = glm::normalize(camaraCentro - camaraPos);
		camaraPos -= forward * speed;
		camaraCentro -= forward * speed;
		glm::mat4 view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		IGlib::setViewMat(view);
	}
	//con a se mueve a la izquierda
	if (key == 'a' || key == 'A') {
		float speed = 0.1f;
		glm::vec3 forward = glm::normalize(camaraCentro - camaraPos);
		glm::vec3 right = glm::normalize(glm::cross(forward, camaraUp));
		camaraPos -= right * speed;
		camaraCentro -= right * speed;
		glm::mat4 view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		IGlib::setViewMat(view);
	}
	//con d hacia la derecha
	if (key == 'd' || key == 'D') {
		float speed = 0.1f;
		glm::vec3 forward = glm::normalize(camaraCentro - camaraPos);
		glm::vec3 right = glm::normalize(glm::cross(forward, camaraUp));
		camaraPos += right * speed;
		camaraCentro += right * speed;
		glm::mat4 view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		IGlib::setViewMat(view);
	}
	//con la tecla l cargamos otro modelo del Ferrari
	if (key == 'l' || key == 'L') {
		int start = objIds.size();
		if (!carga3D("../3D_assets/uploads_files_2815401_Ferarri+Testarossa.obj")) {
			std::cout << "Error cargando el modelo 3D" << std::endl;
		}
		else {
			//Direccion para movimiento atrás
			glm::vec3 forward = glm::normalize(camaraCentro - camaraPos);
			//Escalado para hacerlo mas grande y posicionamiento 
			for (int i = start; i < objIds.size(); i++) {
				glm::mat4 model = glm::translate(glm::mat4(1.0f), forward * 3.0f);
				model = glm::rotate(model, -3.141592f / 12.0f, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(2.0f));
				IGlib::setModelMat(objIds[i], model);
			}

			std::cout << "Cargado modelo 3D de Ferrari Testarossa" << std::endl;
		}
	}

}

//funcionalides del ratón
void mouseFunc(int button, int state, int x, int y)
{
	//cuando se pulsa el botón derecho
	if (button == 2) {
		if (state == 0) {
			//activamos su respectivo bool y guardamos la última powsición donde fue clickeado
			std::cout << "Se ha pulsado el bot�n ";
			leftButtonDown = true;
			lastX = x;
			lastY = y;
		}
		else {
			//cuando se suelta desactivamos el bool
			leftButtonDown = false;
			std::cout << "Se ha soltado el bot�n ";
		}


	}
	if (button == 0) std::cout << "de la izquierda del rat�n " << std::endl;
	if (button == 1) std::cout << "central del rat�n " << std::endl;
	if (button == 2) std::cout << "de la derecha del rat�n " << std::endl;


	std::cout << "en la posici�n " << x << " " << y << std::endl << std::endl;
}

//método respecto al movimiento del ratón
void mouseMoveFunc(int x, int y)
{
	//cuando se presiona el click derecho
	if (leftButtonDown) {
		//calculamos el desplazamiento del ratón por una sensibilidad
		const float sensitivity = 0.005f;
		float xoffset = (lastX - x) * sensitivity;
		float yoffset = (lastY - y) * sensitivity;
		//movemos la cámara para que siga al ratón
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
		//modificamos la matriz de vista
		glm::mat4 view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		IGlib::setViewMat(view);
		

		//reseteamos las coordenadas del ratón
		lastX = x;
		lastY = y;
	}
	std::cout << "Movimiento con presi�n en la posici�n " << x << " " << y << std::endl << std::endl;
}

void mousePassMoveFunc(int x, int y)
{
	//std::cout << "Pasivo en la posici�n " << x << " " << y << std::endl << std::endl;
}
//funcionalidad de la rueda del ratón
void mouseWheelFunc(int wheel, int dir, int x, int y)
{
	//cuando scrolleamos hacia arriba
	if (dir > 0) {
		std::cout << "Se ha pulsado la rueda del rat�n hacia arriba ";
		//Zoom
		float zoomSpeed = 0.5f;
		glm::mat4 view = glm::lookAt(camaraPos, camaraCentro, camaraUp);
		glm::vec3 direccion = glm::normalize(camaraCentro - camaraPos);
		camaraPos += direccion * zoomSpeed;
		IGlib::setViewMat(view);
	}
	//cuando scrolleamos hacia abajo
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

//método de carga de objetos 3D
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
		localObjs.push_back(id);
		objId = id;

		// Escalado por mesh
		glm::mat4 modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
		IGlib::setModelMat(id, modelMat);

		free(faceArray);
		free(vertexArray);
		free(normalArray);
		free(textCoordsArray);
	}
	int start = objIds.size();

	return true;
}






