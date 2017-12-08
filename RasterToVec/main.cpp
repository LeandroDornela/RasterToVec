/*********************************************************
* Autor: Leandro Dornela Ribeiro
* Created:
* Decription:
* TODO:
*********************************************************/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <soil/SOIL.h>

#include "GLSLShader.h"
#include "Vertex.h"

#define _WIN32_WINNT 0x0500


// Resolução inicial da tela.
int WIDTH = 900;
int HEIGHT = 600;
// Array com os vertices.
vector<Vertex> vertices;
vector<Vertex> originalVertices;
vector<Vertex> colorVertices;
vector<Vertex> border;
// Array com os indices.
vector<GLuint> indices;
//vertex array and vertex buffer object IDs
GLuint vaoID;
vector<GLuint> vbos;
//projection and modelview matrices
glm::mat4  P = glm::mat4(1);
glm::mat4 MV = glm::mat4(1);
//camera transformation variables
int state = 0, oldX = 0, oldY = 0;
float rX = 0, rY = 0, dist = -700;
// Referencia para o shader;
GLSLShader shader;
// Menu
int selectedMenuItem = 0;
// Variaveis da imagem.
int imageWidth = 0;
int imageHeight = 0;
int tesMult = 0;
const int totalImages = 6;

// Parametros.
const char *image[totalImages] = { "images/batman.jpg",
"images/lenna.png",
"images/forest.jpg",
"images/peppersc.jpg",
"images/test.png",
"images/sonic.png" };

float colorPerception = 0.05;
int renderMode = 2;
int pointSize = 1;
bool paintBorder = true;
bool flatColor = false;
bool onlyBorders = true;
int imageId = 4;
bool fillAll = true;
bool process = false;


// Funcoes.
void ImageToVertex(const char *file);
void UpdateMenu();
void ConsoleConfig();
void UpdateMenu();
void UpdateUniform();
void FillColor(long long int i);
void ProcessImage();


//======================================================================================================
// START
//======================================================================================================

// Ajusta os parametros inicias do console.
void ConsoleConfig()
{
	//Get a console handle
	HWND myconsole = GetConsoleWindow();
	//Get a handle to device context
	HDC mydc = GetDC(myconsole);
	//Choose any color
	COLORREF COLOR = RGB(255, 255, 255);
	MoveWindow(myconsole, WIDTH, 0, 400, HEIGHT + 50, TRUE);
	ReleaseDC(myconsole, mydc);
}


//OpenGL initialization
void OnInit()
{
	ConsoleConfig();

	//set the polygon mode to render lines
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.8, 0.8, 0.8, 1);

	//load shader
	shader.LoadFromFile(GL_VERTEX_SHADER, "shaders/shader.vert");
	shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/shader.frag");
	//compile and link shader
	shader.CreateAndLinkProgram();
	shader.Use();
	//add shader attribute and uniforms
	shader.AddAttribute("vVertex");
	shader.AddAttribute("vColor");
	shader.AddUniform("MVP");
	shader.AddUniform("tesMult");
	shader.UnUse();

	//=====================================================================

	glutSwapBuffers();

	ImageToVertex(image[imageId]);

	originalVertices = vertices;

	std::cout << "Processando..." << std::endl;

	if (fillAll)
	{
		for (long long int i = 0; i < vertices.size(); i++)
		{
			if (!vertices[i].visited)
			{
				FillColor(i);
			}
		}
	}
	else
	{
		FillColor(440 + 350 * imageWidth);
	}

	std::cout << "Processando...ok" << std::endl;


	vertices = colorVertices;

	if (onlyBorders) vertices = border;

	//=====================================================================

	vbos.push_back(0);
	vbos.push_back(0);
	vbos.push_back(0);


	//setup plane vao and vbo stuff
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vbos[0]);//verticesid
	glGenBuffers(1, &vbos[1]);//indicesid
	glGenBuffers(1, &vbos[2]);//colorvertid



	glBindVertexArray(vaoID);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	//pass plane vertices to array buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof(std::vector<Vertex>) + (sizeof(Vertex) * vertices.size()), &vertices[0], GL_STATIC_DRAW);

	//enable vertex attrib array for position
	glEnableVertexAttribArray(shader["vVertex"]);
	glVertexAttribPointer(shader["vVertex"], 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, position));

	glEnableVertexAttribArray(shader["vColor"]);
	glVertexAttribPointer(shader["vColor"], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, color));

	//pass the plane indices to element array buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::vector<GLuint>) + (sizeof(GLuint) * indices.size()), &indices[0], GL_STATIC_DRAW);

	std::cout << "Initialization successfull" << std::endl;

	UpdateMenu();
}


//======================================================================================================
// UPDATE
//======================================================================================================

// Atualiza o console.
void UpdateMenu()
{
	glutPostRedisplay();
	system("cls");
	std::cout << "________________________________________" << endl;
	std::cout << "                SETTINGS                 " << endl;
	if (selectedMenuItem == 0) std::cout << "-";
	else std::cout << " ";
	std::cout << "Heigth: " << tesMult << endl;

	if (selectedMenuItem == 1) std::cout << "-";
	else std::cout << " ";
	std::cout << "Render Mode: " << renderMode << endl;

	if (selectedMenuItem == 2) std::cout << "-";
	else std::cout << " ";
	std::cout << "Point Size: " << pointSize << endl;

	if (selectedMenuItem == 3) std::cout << "-";
	else std::cout << " ";
	std::cout << "Paint Border: " << paintBorder << endl;

	if (selectedMenuItem == 4) std::cout << "-";
	else std::cout << " ";
	std::cout << "Flat Color: " << flatColor << endl;

	if (selectedMenuItem == 5) std::cout << "-";
	else std::cout << " ";
	std::cout << "Only Borders: " << onlyBorders << endl;

	if (selectedMenuItem == 6) std::cout << "-";
	else std::cout << " ";
	std::cout << "Color tolerance: " << colorPerception << endl;

	if (selectedMenuItem == 7) std::cout << "-";
	else std::cout << " ";
	std::cout << "Image: " << image[imageId] << endl;
	
	if (selectedMenuItem == 8) std::cout << "-";
	else std::cout << " ";
	std::cout << "Update" << endl;
	std::cout << "________________________________________" << endl;
}


// Atuaiza as uniforns.
void UpdateUniform()
{
	shader.Use();
	glUniform1i(shader("tesMult"), tesMult);
	shader.UnUse();
}


//display callback
void OnRender()
{
	//clear the colour and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set teh camera viewing transformation
	glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));
	glm::mat4 Rx = glm::rotate(T, rX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 MV = glm::rotate(Rx, rY, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MVP = P*MV;

	//bind the shader
	shader.Use();
	//set the shader uniforms
	glUniformMatrix4fv(shader("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));


	if (renderMode == 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else if (renderMode == 1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else if (renderMode == 2)
	{
		glPointSize(pointSize);
		glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
		glDrawArrays(GL_POINTS, 0, vertices.size());
	}


	//unbind the shader
	shader.UnUse();


	//swap front and back buffers to show the rendered result
	glutSwapBuffers();
}


//idle event callback
void OnIdle()
{
	glutPostRedisplay();
	UpdateUniform();
}


//resize event handler
void OnResize(int w, int h)
{
	//set the viewport size
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	//setup the projection matrix
	P = glm::perspective(45.0f, (GLfloat)w / h, 1.f, 10000.f);
}


//======================================================================================================
// CONTROLS
//======================================================================================================

//mosue click handler
void OnMouseDown(int button, int s, int x, int y)
{
	if (s == GLUT_DOWN)
	{
		oldX = x;
		oldY = y;
	}

	if (button == GLUT_MIDDLE_BUTTON)
	{
		state = 0;
	}
	else
	{
		state = 1;
	}
}


//mosue move handler
void OnMouseMove(int x, int y)
{
	if (state == 0)
	{
		dist *= (1 + (y - oldY) / 60.0f);
	}
	else
	{
		rY += (x - oldX) / 5.0f;
		rX += (y - oldY) / 5.0f;
	}
	oldX = x;
	oldY = y;

	glutPostRedisplay();
}


void OnKeyboard(unsigned char key, int x, int y)
{

}


void OnSpecialKey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		if (selectedMenuItem > 0) { selectedMenuItem--; }
		break;
	case GLUT_KEY_DOWN:
		if (selectedMenuItem < 8) { selectedMenuItem++; }
		break;
	case GLUT_KEY_LEFT:
		if (selectedMenuItem == 0) tesMult -= 10;
		if (selectedMenuItem == 1) { if (renderMode > 0) renderMode--; }
		if (selectedMenuItem == 2) { if (pointSize > 0) pointSize--; }
		if (selectedMenuItem == 3) { paintBorder = !paintBorder;}
		if (selectedMenuItem == 4) { flatColor = !flatColor;}
		if (selectedMenuItem == 5) { onlyBorders = !onlyBorders;}
		if (selectedMenuItem == 6) { colorPerception -= 0.01;}
		if (selectedMenuItem == 7) { if(imageId > 0) imageId -= 1;}
		break;
	case GLUT_KEY_RIGHT:
		if (selectedMenuItem == 0) tesMult += 10;
		if (selectedMenuItem == 1) { if (renderMode < 2) renderMode++; }
		if (selectedMenuItem == 2) pointSize++;
		if (selectedMenuItem == 3) { paintBorder = !paintBorder;}
		if (selectedMenuItem == 4) { flatColor = !flatColor;}
		if (selectedMenuItem == 5) { onlyBorders = !onlyBorders;}
		if (selectedMenuItem == 6) { colorPerception += 0.01;}
		if (selectedMenuItem == 7) { if (imageId < totalImages-1) imageId += 1;}
		if (selectedMenuItem == 8) { ProcessImage(); }
		break;
	}
	UpdateMenu();
}


//======================================================================================================
// MISC
//======================================================================================================


// Procesa a image com os parametros determinados.
void ProcessImage()
{
	std::cout << "Atualizando..." << std::endl;

	// Limpa os vetores
	colorVertices.clear();
	border.clear();
	indices.clear();
	vertices.clear();
	originalVertices.clear();

	ImageToVertex(image[imageId]);

	std::cout << "Processando..." << std::endl;

	if (fillAll)
	{
		for (long long int i = 0; i < vertices.size(); i++)
		{
			if (!vertices[i].visited)
			{
				FillColor(i);
			}
		}
	}
	else
	{
		FillColor(440 + 350 * imageWidth);
	}

	std::cout << "Processando...ok" << std::endl;

	vertices = colorVertices;

	if (onlyBorders) vertices = border;


	glBindVertexArray(vaoID);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	//pass plane vertices to array buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof(std::vector<Vertex>) + (sizeof(Vertex) * vertices.size()), &vertices[0], GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(std::vector<Vertex>) + (sizeof(Vertex) * colorVertices.size()), &colorVertices[0], GL_STATIC_DRAW);

	//enable vertex attrib array for position
	glEnableVertexAttribArray(shader["vVertex"]);
	glVertexAttribPointer(shader["vVertex"], 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, position));

	glEnableVertexAttribArray(shader["vColor"]);
	glVertexAttribPointer(shader["vColor"], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, color));

	//pass the plane indices to element array buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::vector<GLuint>) + (sizeof(GLuint) * indices.size()), &indices[0], GL_STATIC_DRAW);

	std::cout << "Atualizando...ok" << std::endl;
}


// Realiza uma busca em profundidade para encotrar pixels proximos.
void FillColor(long long int i)
{
	// Pilha de recursao com os ids dos vertices.
	std::stack<long long int> recursionStack;
	// Identificador para elementos do vetor de vertices.
	//long long int id = 440 + 350*imageWidth;
	long long int id = i;
	// Vetor temporario com os vertices no intervalo de cor.
	vector<Vertex> temp;

	// Adiciona o primeiro elemento na pilha.
	recursionStack.push(id);
	colorVertices.push_back(vertices[id]);

	// Enquanto a pilha nao estavazia.
	while (!recursionStack.empty())
	{
		// Id do primeiro elemento da pilha.
		long long int topVertexId = recursionStack.top();

		// Para cada vizinho do vertice.
		for (int j = -1; j <= 1; j++)
		{
			for (int i = -1; i <= 1; i++)
			{
				id = topVertexId + j*imageWidth + i;

				// Se nao é o proprio ponto, nao esta for da imagem e nao foi vizitado.
				if ((i != 0 || j != 0) && (id >= 0 && id < imageWidth*imageHeight) && (!vertices[id].visited))
				{
					if (!(topVertexId%imageWidth == 0 && (id == topVertexId - 1 || id == topVertexId - imageWidth - 1 || id == imageWidth + topVertexId - 1)) &&
						!((topVertexId + 1) % (imageWidth) == 0 && (id == topVertexId + 1 || id == topVertexId - imageWidth + 1 || id == imageWidth + topVertexId + 1)))
					{
						// Se o vizinho esta no intervalo de cor
						if ((abs(vertices[id].color.r - vertices[topVertexId].color.r) <= colorPerception) &&
							(abs(vertices[id].color.g - vertices[topVertexId].color.g) <= colorPerception) &&
							(abs(vertices[id].color.b - vertices[topVertexId].color.b) <= colorPerception))
						{
							// Adiciona vizinho nos vertices e vai pra ele
							//colorVertices.push_back(vertices[id]);
							temp.push_back(vertices[id]);

							vertices[id].visited = true;
							recursionStack.push(id);

							// Reinicia com o novo id.
							topVertexId = recursionStack.top();
							j = -1;
							i = -1;
						}
						else
						{
							// Adiciona vertice ao vetor de borda.
							if (!vertices[id].border && !vertices[topVertexId].border)
							{
								if (paintBorder) vertices[id].color = { 0,0,0,0 };

								vertices[id].border = true;

								border.push_back(vertices[id]);
							}
						}
					}
				}
			}
		}

		// Apos percorrer todos os vizinhos remove o id da pilha.
		recursionStack.pop();
	}

	// Adiciona os elementos do vetor temporario ao vetor de cores.
	for (long long int j = 0; j < temp.size(); j++)
	{
		if (temp[j].border && paintBorder) temp[j].color = { 0,0,0,0 };
		if (flatColor) temp[j].color = vertices[i].color;
		colorVertices.push_back(temp[j]);
	}
}


// Le a imagem e gera os pontos.
void ImageToVertex(const char *file)
{
	std::cout << "\nGerando imagem..." << std::endl;

	// Dados da imagem.
	unsigned char* image = SOIL_load_image(file, &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);

	long long int w = 0; // Id para possicao no array de dados da imagen. Incementado a cada componente de com r,g,b ou a.
	int px = 0; // Id para um pixel. Incementado a cada 4 componentes de cor rgba.
	int i = 0; // Id para posicao na linha.
	int j = 0; // Id para posicao na coluna.
	long long int s = 0; // Contador para tamanho do array. Incrementado a cada pixel.

	float inter = 1; // Intervalo de cor.
	long long int arraySize = imageWidth*imageHeight; // Total de pixels da imagem.

	glm::vec4 c; // Cor temporaria.
	glm::vec3 refColor = { (float)164 / 255, (float)100 / 255, (float)88 / 255 }; // Cor de referencia.


																				  // Percorre o array com os dados da imagem.
	while (s < arraySize)
	{
		// Atribui a c as componentes rgba.
		c.r = ((float)image[w]) / 255;
		w++;
		c.g = ((float)image[w]) / 255;
		w++;
		c.b = ((float)image[w]) / 255;
		w++;
		c.a = ((float)image[w]) / 255;
		w++;

		// Adiciona um novo vertice vazio ao array.
		vertices.push_back(Vertex());

		// Verifica se o vertice esta no intervalo de cor.
		if (c.r <= refColor.r + inter && c.r >= refColor.r - inter &&
			c.g <= refColor.r + inter && c.g >= refColor.g - inter &&
			c.b <= refColor.r + inter && c.b >= refColor.b - inter)
		{
			vertices[px].color = c;
		}
		else
		{
			//vertices[px].color = { (c.r + c.g + c.b) / 3,(c.r + c.g + c.b) / 3,(c.r + c.g + c.b) / 3, c.a };
			vertices[px].color = { 0,0,0,0 };
		}

		// Posiciona o vertice.
		vertices[px].position = glm::vec3(i - imageWidth / 2, j + imageHeight / 2, 0);
		//vertices[px].position = glm::vec3(i, j, 0);
		vertices[px].index = px;

		// Incrementa os contadores.
		px++;
		i++;
		s++;

		// Verifica se o contador chegou no final da linha.
		if (i == imageWidth)
		{
			i = 0;
			j--;
		}
	}

	// Atribui os indices dos pontos fazendo um trem muito louco.
	for (int i = 0; i < imageHeight - 1; i++)
	{
		for (int j = 0; j < imageWidth - 1; j++)
		{
			int i0 = i * (imageWidth)+j;
			int i1 = i0 + 1;
			int i2 = i0 + (imageWidth);
			int i3 = i2 + 1;

			if ((j + i) % 2)
			{
				indices.push_back(i0);
				indices.push_back(i2);
				indices.push_back(i1);
				indices.push_back(i1);
				indices.push_back(i2);
				indices.push_back(i3);
			}
			else
			{
				indices.push_back(i0);
				indices.push_back(i2);
				indices.push_back(i3);
				indices.push_back(i0);
				indices.push_back(i3);
				indices.push_back(i1);
			}
		}
	}

	std::cout << "Gerando imagem...ok" << std::endl;

}


//release all allocated resources
void OnShutdown()
{
	//Destroy shader
	shader.DeleteShaderProgram();

	//Destroy vao and vbo
	//glDeleteBuffers(1, &vboVerticesID);
	//glDeleteBuffers(1, &vboIndicesID);
	glDeleteVertexArrays(1, &vaoID);

	cout << "Shutdown successfull" << endl;
}


//======================================================================================================
// MAIN
//======================================================================================================

int main(int argc, char** argv)
{
	//freeglut initialization calls
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);

	glutCreateWindow("RasterToVec");
	glEnable(GL_ALPHA_TEST);


	//glew initialization
	glewExperimental = GL_TRUE;

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cerr << "Error: " << glewGetErrorString(err) << std::endl;
	}
	else
	{
		if (GLEW_VERSION_3_3)
		{
			std::cout << "Driver supports OpenGL 3.3\nDetails:" << std::endl;
		}
	}
	err = glGetError(); //this is to ignore INVALID ENUM error 1282


						//print information on screen
	std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;


	//opengl initialization
	OnInit();

	//callback hooks
	glutCloseFunc(OnShutdown);
	glutDisplayFunc(OnRender);
	glutReshapeFunc(OnResize);
	glutMouseFunc(OnMouseDown);
	glutMotionFunc(OnMouseMove);
	glutKeyboardFunc(OnKeyboard);
	glutSpecialFunc(OnSpecialKey);
	glutIdleFunc(OnIdle);


	//main loop call
	glutMainLoop();

	return 0;
}
