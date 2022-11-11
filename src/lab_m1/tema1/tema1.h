#pragma once

#include "components/simple_scene.h"
#include <cmath>

namespace m1
{
	class Tema1 : public gfxc::SimpleScene
	{
	public:
		struct ViewportSpace
		{
			ViewportSpace() : x(0), y(0), width(1), height(1) {}
			ViewportSpace(int x, int y, int width, int height)
				: x(x), y(y), width(width), height(height) {}
			int x;
			int y;
			int width;
			int height;
		};

		struct LogicSpace
		{
			LogicSpace() : x(0), y(0), width(1), height(1) {}
			LogicSpace(float x, float y, float width, float height)
				: x(x), y(y), width(width), height(height) {}
			float x;
			float y;
			float width;
			float height;
		};

	public:
		Tema1();
		~Tema1();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void DrawScene(glm::mat3 visMatrix, float deltaTimeSeconds, float angularOfDuck);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

		// Sets the logic space and view space
		// logicSpace: { x, y, width, height }
		// viewSpace: { x, y, width, height }
		glm::mat3 VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
		glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);

		void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);
		Mesh* Create(const std::string& name, const std::vector<unsigned int>& indices, const std::vector<VertexFormat>& vertices);
		Mesh* Circle(const std::string& name, glm::vec3 color);

		void duck(float deltaTimeSeconds);
		void duckDown(float deltaTimeSeconds);
		void duckUp(float deltaTimeSeconds);

		void life(float deltaTimeSeconds);
		void drawLife(glm::mat3 visMatrix);
		void bullet(float deltaTimeSeconds);
		void drawBullet(glm::mat3 visMatrix);
		void score(float deltaTimeSeconds);
		void drawScore(glm::mat3 visMatrix);
		Mesh* Square(const std::string& name, glm::vec3 leftBottomCorner, glm::vec3 color, bool fill = false);

	protected:
		float length;
		ViewportSpace viewSpace;
		LogicSpace logicSpace;
		glm::mat3 modelMatrix, visMatrix;
		glm::mat3 visSaveMatrix;


		//
		bool up = false;
		bool down = false;
		float angularStep;
		float angularDemo;

		bool duckAppear;
		float speed;
		int startPosition;
		float angleOfDuck;

		bool upLeft;
		bool upRight;
		bool downLeft;
		bool downRight;

		float x;
		float y;

		int positionXMouse;
		int positionYMouse;

		bool duckGetShot;
		float duckSurvival;

		int countMiss;
		float countScore;

		int countDuckGetShot;
		int countLife;
		int countBullet;
		float increaseSpeed;
	};
}   // namespace m1
