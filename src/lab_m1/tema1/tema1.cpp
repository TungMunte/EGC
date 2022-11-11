#include "lab_m1/tema1/Tema1.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <time.h>	

#include "lab_m1/tema1/transform.h"
#include "lab_m1/tema1/support.h"

using namespace std;
using namespace m1;

Tema1::Tema1()
{
	glm::ivec2 resolution = window->GetResolution();
	angularStep = 0.0f;
	angularDemo = 0.0f;
	duckAppear = true;
	speed = 0.0f;
	startPosition = support::position(resolution.x);
	angleOfDuck = support::angle();
	upLeft = false;
	upRight = false;
	downLeft = false;
	downRight = false;
	if (angleOfDuck > 0)
	{
		upLeft = true;
	}
	if (angleOfDuck < 0)
	{
		upRight = true;
	}
	x = startPosition ;
	y = 0;
	duckGetShot = false;
	duckSurvival = 0.0f;
	countMiss = 0;
	countScore = 0.0f;
	countBullet = 3;
	countLife = 3;
	countDuckGetShot = 0;
	increaseSpeed = 0.0f;
}

Tema1::~Tema1()
{
}

void Tema1::Init()
{
	auto camera = GetSceneCamera();
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	logicSpace.x = 0;       // logic x
	logicSpace.y = 0;       // logic y
	logicSpace.width = 4;   // logic width
	logicSpace.height = 4;  // logic height

	std::vector<VertexFormat> vertices0 =
	{
		VertexFormat(glm::vec3(0, 0.3f, 0), glm::vec3(1, 0, 1)),
		VertexFormat(glm::vec3(-0.1f, 0, 0), glm::vec3(1, 0, 1)),
		VertexFormat(glm::vec3(0.1f, 0, 0), glm::vec3(1, 0, 1))
	};
	const std::vector<unsigned int> indices0 = { 0, 1, 2 };
	Mesh* mouth = Create("mouth", indices0, vertices0);
	AddMeshToList(mouth);

	std::vector<VertexFormat> vertices1 =
	{
		VertexFormat(glm::vec3(0, 1.2f, 0), glm::vec3(1, 0, 0)),
		VertexFormat(glm::vec3(-0.3f, 0, 0), glm::vec3(1, 0, 0)),
		VertexFormat(glm::vec3(0.3f, 0, 0), glm::vec3(1, 0, 0))
	};
	const std::vector<unsigned int> indices1 = { 0, 1, 2 };
	Mesh* body = Create("body", indices1, vertices1);
	AddMeshToList(body);

	std::vector<VertexFormat> vertices2 =
	{
		VertexFormat(glm::vec3(0, 0, 0), glm::vec3(1, 1, 0)),
		VertexFormat(glm::vec3(-0.5, 0, 0), glm::vec3(1, 1, 0)),
		VertexFormat(glm::vec3(-0.25f, 0.2f, 0), glm::vec3(1, 1, 0))
	};
	const std::vector<unsigned int> indices2 = { 0, 1, 2 };
	Mesh* left_wing = Create("left wing", indices2, vertices2);
	AddMeshToList(left_wing);

	std::vector<VertexFormat> vertices3 =
	{
		VertexFormat(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)),
		VertexFormat(glm::vec3(0.5f, 0, 0), glm::vec3(1, 1, 1)),
		VertexFormat(glm::vec3(0.25f, 0.2f, 0), glm::vec3(1, 1, 1))
	};
	const std::vector<unsigned int> indices3 = { 0, 1, 2 };
	Mesh* right_wing = Create("right wing", indices3, vertices3);
	AddMeshToList(right_wing);

	Mesh* head = Circle("head", glm::vec3(0, 1, 0));
	AddMeshToList(head);

	Mesh* life = Circle("life", glm::vec3(1, 0, 0));
	AddMeshToList(life);

	glm::vec3 corner = glm::vec3(0.0f, 0.0f, 0.0f);
	Mesh* square1 = Square("square", corner, glm::vec3(0, 1, 0));
	AddMeshToList(square1);

	Mesh* score = Square("score", corner, glm::vec3(0, 0, 1), true);
	AddMeshToList(score);
}

glm::mat3 Tema1::VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
	float sx, sy, tx, ty;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	tx = viewSpace.x - sx * logicSpace.x;
	ty = viewSpace.y - sy * logicSpace.y;

	return glm::transpose(glm::mat3(
		sx, 0.0f, tx,
		0.0f, sy, ty,
		0.0f, 0.0f, 1.0f));
}

glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	return glm::transpose(glm::mat3(
		smin, 0.0f, tx,
		0.0f, smin, ty,
		0.0f, 0.0f, 1.0f));
}

void Tema1::SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
	GetSceneCamera()->Update();
}

Mesh* m1::Tema1::Create(const std::string & name, const std::vector<unsigned int> & indices, const std::vector<VertexFormat> & vertices)
{
	Mesh* square = new Mesh(name);
	square->InitFromData(vertices, indices);
	return square;
}

Mesh* m1::Tema1::Circle(const std::string & name, glm::vec3 color)
{
	int triangleAmount = 40;
	GLfloat x = .0f;
	GLfloat y = .0f;
	GLfloat radius = .4f;
	GLfloat twicePi = 2.0f * M_PI;

	std::vector<VertexFormat> vertices = { VertexFormat(glm::vec3(0,0,0), color) };
	std::vector<unsigned int> indices = { 0 };

	for (unsigned int i = 0; i < 180; i++)
	{
		vertices.push_back(VertexFormat(glm::vec3(x + (radius * cos(i * twicePi / triangleAmount)), y + (radius * sin(i * twicePi / triangleAmount)), 0), color));
		indices.push_back(i);
		indices.push_back(0);
	}

	Mesh * square = new Mesh(name);
	square->InitFromData(vertices, indices);
	return square;
}

Mesh* m1::Tema1::Square(const std::string& name, glm::vec3 leftBottomCorner, glm::vec3 color, bool fill){
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(0.5f, 0, 0), color),
		VertexFormat(corner + glm::vec3(0.5f, 1.0f, 0), color),
		VertexFormat(corner + glm::vec3(0, 1.0f, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 1, 2, 3 };

	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// Draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}

void Tema1::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Tema1::duck(float deltaTimeSeconds) {
	glm::ivec2 resolution = window->GetResolution();


	// wall left
	if (x <= 0 && upLeft == true) {
		upLeft = false;
		upRight = true;
		angleOfDuck = -angleOfDuck;
	}
	if (x <= 0 && downLeft == true) {
		downLeft = false;
		downRight = true;
		angleOfDuck = -angleOfDuck;
	}
	// wall right
	if (x >= 8 * resolution.x / 10 && upRight == true) {
		upRight = false;
		upLeft = true;
		angleOfDuck = -angleOfDuck;
	}
	if (x >= 8 * resolution.x / 10 && downRight == true) {
		downRight = false;
		downLeft = true;
		angleOfDuck = -angleOfDuck;
	}
	// wall up
	if (y >= 8 * resolution.y / 10 && upLeft == true) {
		upLeft = false;
		downLeft = true;
		angleOfDuck = M_PI - angleOfDuck;
	}
	if (y >= 8 * resolution.y / 10 && upRight == true) {
		upRight = false;
		downRight = true;
		angleOfDuck = M_PI - angleOfDuck;
	}
	// wall down
	if (y <= 0 && downLeft == true) {
		downLeft = false;
		upLeft = true;
		angleOfDuck = M_PI - angleOfDuck;
	}
	if (y <= 0 && downRight == true) {
		downRight = false;
		upRight = true;
		angleOfDuck = M_PI - angleOfDuck;
	}


	if (upLeft == true)
	{
		if (sin(angleOfDuck) > 0)
		{
			x = x - (300 + increaseSpeed) * sin(angleOfDuck) * deltaTimeSeconds;
		}
		if (sin(angleOfDuck) < 0)
		{
			x = x + (300 + increaseSpeed) * sin(angleOfDuck) * deltaTimeSeconds;
		}
		if (cos(angleOfDuck) > 0)
		{
			y = y + (300 + increaseSpeed) * cos(angleOfDuck) * deltaTimeSeconds;
		}
		if (cos(angleOfDuck) < 0)
		{
			y = y - (300 + increaseSpeed) * cos(angleOfDuck) * deltaTimeSeconds;
		}
	}
	if (upRight == true)
	{
		if (sin(angleOfDuck) > 0)
		{
			x = x + (300 + increaseSpeed) * sin(angleOfDuck) * deltaTimeSeconds;
		}
		if (sin(angleOfDuck) < 0)
		{
			x = x - (300 + increaseSpeed) * sin(angleOfDuck) * deltaTimeSeconds;
		}
		if (cos(angleOfDuck) > 0)
		{
			y = y + (300 + increaseSpeed) * cos(angleOfDuck) * deltaTimeSeconds;
		}
		if (cos(angleOfDuck) < 0)
		{
			y = y - (300 + increaseSpeed) * cos(angleOfDuck) * deltaTimeSeconds;
		}
	}
	if (downLeft == true)
	{
		if (sin(angleOfDuck) > 0)
		{
			x = x - (300 + increaseSpeed) * sin(angleOfDuck) * deltaTimeSeconds;
		}
		if (sin(angleOfDuck) < 0)
		{
			x = x + (300 + increaseSpeed) * sin(angleOfDuck) * deltaTimeSeconds;
		}
		if (cos(angleOfDuck) > 0)
		{
			y = y - (300 + increaseSpeed) * cos(angleOfDuck) * deltaTimeSeconds;
		}
		if (cos(angleOfDuck) < 0 )
		{
			y = y + (300 + increaseSpeed) * cos(angleOfDuck) * deltaTimeSeconds;
		}
	}
	if (downRight == true)
	{
		if (sin(angleOfDuck) > 0)
		{
			x = x + (300 + increaseSpeed) * sin(angleOfDuck) * deltaTimeSeconds;
		}
		if (sin(angleOfDuck) < 0)
		{
			x = x - (300 + increaseSpeed) * sin(angleOfDuck) * deltaTimeSeconds;
		}
		if (cos(angleOfDuck) > 0)
		{
			y = y - (300 + increaseSpeed) * cos(angleOfDuck) * deltaTimeSeconds;
		}
		if (cos(angleOfDuck) < 0)
		{
			y = y + (300 + increaseSpeed) * cos(angleOfDuck) * deltaTimeSeconds;
		}
	}


	viewSpace = ViewportSpace(x, y, 2 * resolution.x / 10, 2 * resolution.y / 10);
	SetViewportArea(viewSpace, glm::vec3(0.0f), true);
	visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2D(logicSpace, viewSpace);
	DrawScene(visMatrix, deltaTimeSeconds, angleOfDuck);
		
}

void Tema1::duckDown(float deltaTimeSeconds) {
	glm::ivec2 resolution = window->GetResolution();
	if (cos(angleOfDuck) > 0)
	{
		y = y - 300 * cos(angleOfDuck) * deltaTimeSeconds;
	}
	if (cos(angleOfDuck) < 0)
	{
		y = y + 300 * cos(angleOfDuck) * deltaTimeSeconds;
	}
	viewSpace = ViewportSpace(x, y, 2 * resolution.x / 10, 2 * resolution.y / 10);
	SetViewportArea(viewSpace, glm::vec3(0.0f), true);
	visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2D(logicSpace, viewSpace);
	DrawScene(visMatrix, deltaTimeSeconds, M_PI);
}

void Tema1::duckUp(float deltaTimeSeconds) {
	glm::ivec2 resolution = window->GetResolution();
	if (cos(angleOfDuck) > 0)
	{
		y = y + 300 * cos(angleOfDuck) * deltaTimeSeconds;
	}
	if (cos(angleOfDuck) < 0)
	{
		y = y - 300 * cos(angleOfDuck) * deltaTimeSeconds;
	}
	viewSpace = ViewportSpace(x, y, 2 * resolution.x / 10, 2 * resolution.y / 10);
	SetViewportArea(viewSpace, glm::vec3(0.0f), true);
	visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2D(logicSpace, viewSpace);
	DrawScene(visMatrix, deltaTimeSeconds, 0);
}

void Tema1::drawLife(glm::mat3 visMatrix) {
	// head
	if (countLife == 3 || countLife == 2 || countLife == 1) {
		modelMatrix = visMatrix * transform::Translate(0.0f, 2.0f);
		modelMatrix = modelMatrix * transform::Scale(2.3f, 2.3f);
		RenderMesh2D(meshes["life"], shaders["VertexColor"], modelMatrix);
	}

	if (countLife == 3 || countLife == 2 ) {
		modelMatrix = visMatrix * transform::Translate(2.3f, 2.0f);
		modelMatrix = modelMatrix * transform::Scale(2.3f, 2.3f);
		RenderMesh2D(meshes["life"], shaders["VertexColor"], modelMatrix);
	}

	if (countLife == 3 ) {
		modelMatrix = visMatrix * transform::Translate(4.6f, 2.0f);
		modelMatrix = modelMatrix * transform::Scale(2.3f, 2.3f);
		RenderMesh2D(meshes["life"], shaders["VertexColor"], modelMatrix);
	}

}

void Tema1::life(float deltaTimeSeconds) {
	glm::ivec2 resolution = window->GetResolution();

	viewSpace = ViewportSpace(0, 9 * resolution.y / 10, resolution.x / 10, resolution.y / 10);
	SetViewportArea(viewSpace, glm::vec3(0.0f), true);
	visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);
	drawLife(visMatrix);
}

void Tema1::drawBullet(glm::mat3 visMatrix) {
	// head
	if (countBullet == 3 || countBullet == 2 || countBullet == 1)
	{
		modelMatrix = visMatrix * transform::Translate(0.0f, 1.7f);
		modelMatrix = modelMatrix * transform::Scale(2.0f, 2.0f);
		RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);
	}
	if (countBullet == 3 || countBullet == 2)
	{
		modelMatrix = visMatrix * transform::Translate(2.0f, 1.7f);
		modelMatrix = modelMatrix * transform::Scale(2.0f, 2.0f);
		RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);
	}
	if (countBullet == 3)
	{
		modelMatrix = visMatrix * transform::Translate(4.0f, 1.7f);
		modelMatrix = modelMatrix * transform::Scale(2.0f, 2.0f);
		RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);
	}

}

void Tema1::bullet(float deltaTimeSeconds) {
	glm::ivec2 resolution = window->GetResolution();

	viewSpace = ViewportSpace(0, 8 * resolution.y / 10, resolution.x / 10, resolution.y / 10);
	SetViewportArea(viewSpace, glm::vec3(0.0f), true);
	visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);
	drawBullet(visMatrix);
}

void Tema1::drawScore(glm::mat3 visMatrix){
	modelMatrix = visMatrix * transform::Translate(0.1f, 0.0f);
	modelMatrix = modelMatrix * transform::Scale(7.5f, 3.0f);
	RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);

	modelMatrix = visMatrix * transform::Translate(0.1f, 0.0f);
	modelMatrix = modelMatrix * transform::Scale(countScore, 3.0f);
	RenderMesh2D(meshes["score"], shaders["VertexColor"], modelMatrix);
}

void Tema1::score(float deltaTimeSeconds){
	glm::ivec2 resolution = window->GetResolution();

	viewSpace = ViewportSpace(8 * resolution.x / 10, 9 * resolution.y / 10, 2 * resolution.x / 10, resolution.y / 10);
	SetViewportArea(viewSpace, glm::vec3(0.0f), true);
	visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2D(logicSpace, viewSpace);
	drawScore(visMatrix);
}

void Tema1::Update(float deltaTimeSeconds)
{
	glm::ivec2 resolution = window->GetResolution();
	viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);


	SetViewportArea(viewSpace, glm::vec3(0), true);
	visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2D(logicSpace, viewSpace);
	visSaveMatrix = visMatrix;

	duckSurvival += deltaTimeSeconds;
	if (duckGetShot == false && duckSurvival <= 10.0f && countMiss != 3)
	{
		duck(deltaTimeSeconds);
	}
	if (duckSurvival >= 10.0f ||  countMiss  == 3)
	{
		if (y <= 8 * resolution.y / 10)
		{
			duckUp(deltaTimeSeconds);
		}
		if (y >= 8 * resolution.y / 10)
		{
			startPosition = support::position(resolution.x);
			angleOfDuck = support::angle();
			upLeft = false;
			upRight = false;
			downLeft = false;
			downRight = false;
			if (angleOfDuck > 0)
			{
				upLeft = true;
			}
			if (angleOfDuck < 0)
			{
				upRight = true;
			}
			x = startPosition;
			y = 0;
			duckGetShot = false;
			duckSurvival = 0.0f;
			countMiss = 0;
			countScore = countScore - 0.5f;
			countBullet = 3;
			countLife--;
		}
	}
	if (duckGetShot == true )
	{
		if (y > 0)
		{
			duckDown(deltaTimeSeconds);
		}
		if (y <= 0)
		{
			startPosition = support::position(resolution.x);
			angleOfDuck = support::angle();
			upLeft = false;
			upRight = false;
			downLeft = false;
			downRight = false;
			if (angleOfDuck > 0)
			{
				upLeft = true;
			}
			if (angleOfDuck < 0)
			{
				upRight = true;
			}
			x = startPosition;
			y = 0;
			duckGetShot = false;
			duckSurvival = 0.0f;
			countDuckGetShot++;
			if (countDuckGetShot == 5)
			{
				countDuckGetShot = 0;
				increaseSpeed = increaseSpeed + 150.0f;
			}
			countMiss = 0;
			countBullet = 3;
		}
	}

	life(deltaTimeSeconds);

	bullet(deltaTimeSeconds);

	score(deltaTimeSeconds);
}

void Tema1::FrameEnd()
{
}

void Tema1::DrawScene(glm::mat3 visMatrix, float deltaTimeSeconds, float angleOfDuck)
{
	angularDemo += deltaTimeSeconds;
	if (angularStep >= M_PI_4 / 2)
	{
		up = false;
	}
	else if (angularStep <= -M_PI_4 / 2)
	{
		up = true;
	}
	if (up == true)
	{
		angularStep += deltaTimeSeconds;
	}
	if (up == false)
	{
		angularStep -= deltaTimeSeconds;
	}

	// head
	modelMatrix = visMatrix * transform::Translate(2, 1.8f);
	modelMatrix = modelMatrix * transform::Rotate(angleOfDuck);
	modelMatrix = modelMatrix * transform::Translate(0, 1.6f);
	modelMatrix = modelMatrix * transform::Scale(0.8f, 0.8f);
	RenderMesh2D(meshes["head"], shaders["VertexColor"], modelMatrix);


	// body
	modelMatrix = visMatrix * transform::Translate(2, 1.8f);
	modelMatrix = modelMatrix * transform::Scale(1.5f, 1.5f);
	modelMatrix = modelMatrix * transform::Rotate(angleOfDuck);
	RenderMesh2D(meshes["body"], shaders["VertexColor"], modelMatrix);


	// mouth
	modelMatrix = visMatrix * transform::Translate(2, 1.8f);
	modelMatrix = modelMatrix * transform::Scale(2, 2);
	modelMatrix = modelMatrix * transform::Rotate(angleOfDuck);
	modelMatrix = modelMatrix * transform::Translate(0, 0.8f);
	RenderMesh2D(meshes["mouth"], shaders["VertexColor"], modelMatrix);


	// left wing
	modelMatrix = visMatrix * transform::Translate(2, 1.8f);
	modelMatrix = modelMatrix * transform::Scale(2, 2);
	modelMatrix = modelMatrix * transform::Rotate(angleOfDuck);
	modelMatrix = modelMatrix * transform::Translate(0, 0.5f);
	modelMatrix = modelMatrix * transform::Rotate(2 * angularStep);
	RenderMesh2D(meshes["left wing"], shaders["VertexColor"], modelMatrix);


	// right wing
	modelMatrix = visMatrix * transform::Translate(2, 1.8f);
	modelMatrix = modelMatrix * transform::Scale(2, 2);
	modelMatrix = modelMatrix * transform::Rotate(angleOfDuck);
	modelMatrix = modelMatrix * transform::Translate(0, 0.5f);
	modelMatrix = modelMatrix * transform::Rotate(-2 * angularStep);
	RenderMesh2D(meshes["right wing"], shaders["VertexColor"], modelMatrix);
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
}

void Tema1::OnKeyPress(int key, int mods)
{
}

void Tema1::OnKeyRelease(int key, int mods)
{
	// Add key release event
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// Add mouse move event
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button press event
	glm::ivec2 resolution = window->GetResolution();
	if ( mouseX < (x + 2 * resolution.x / 10) && mouseX > x && (resolution.y - mouseY) < (y + 8 * resolution.y / 10) && (resolution.y - mouseY) > y)
	{
		cout << "shot" << endl;
		duckGetShot = true;
		countScore = countScore + 0.5f;
		countBullet--;
	}
	else {
		countMiss++;
		cout << "miss" << endl;
		countBullet--;
	}
	positionXMouse = mouseX;
	positionYMouse = mouseY;
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button release event
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}
