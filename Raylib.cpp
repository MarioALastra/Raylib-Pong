#include "raylib.h"
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

static int playerScore = 0;
static int enemyScore = 0;

static bool ballHit = false;

enum GameState
{
	MAIN,
	GAME,
	WIN,
	LOSE
};

class Player
{
public:
	Vector2 position;
	Vector2 speed;
	Vector2 size;
	float y_vel;
	void InitPlayer();
	void UpdatePlayer();
	void DrawPlayer();
	Rectangle colRect;
};

class Enemy
{
public:
	Vector2 position;
	Vector2 speed;
	Vector2 size;
	float y_vel;
	void Init();
	void Update();
	void Draw();
	Rectangle colRect;
};

class Ball
{
public:
	Vector2 position;
	Vector2 speed;
	const float SIZE = 20.0f;
	void Init();
	void Update();
	void Draw();
	Rectangle ballRect;
};

template <class T>
T Clamp(T value, T min, T max)
{
	if (value <= min)
		return min;
	else if (value >= max)
		return max;
	else
		return value;
}

template <class T>
T Sign(T value)
{
	if (value > 0)
		return 1;
	else if (value < 0)
		return -1;
	else
		return 0;
}

int main()
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PONG!");
	SetTargetFPS(60);

	GameState state = MAIN;

	Player* player = new Player();
	Ball* ball = new Ball();
	Enemy* enemy = new Enemy();

	int frameCounter = 0;
	float ballSpeed = 0.0f;

	while (!WindowShouldClose())
	{
		switch (state)
		{
			case MAIN:
				if (IsKeyPressed(KEY_ENTER))
				{
					player->InitPlayer();
					ball->Init();
					enemy->Init();
					state = GAME;
					playerScore = 0;
					enemyScore = 0;
				}

				BeginDrawing();
				ClearBackground(WHITE);

				DrawText("PONG!", 250, 200, 100, BLACK);
				DrawText("[Press Enter To Start]", 150, 400, 40, BLACK);

				EndDrawing();
			break;
			case GAME:
				if (IsKeyPressed(KEY_ESCAPE))
					state = MAIN;

				ball->Update();
				player->UpdatePlayer();
				enemy->y_vel = enemy->position.y;
				enemy->position.y += Clamp<float>(((ball->position.y+(ball->SIZE/2)) - (enemy->position.y +(enemy->size.y/2)))*0.2,-2000.0f,2000.0f);
				enemy->Update();
				enemy->y_vel -= enemy->position.y;
				enemy->y_vel += 0.4f;

				if (playerScore >= 3)
				{
					state = WIN;
					frameCounter = 0;
				}
				if (enemyScore >= 3)
				{
					state = LOSE;
					frameCounter = 0;
				}

				if (frameCounter < 15 && ballHit)
					frameCounter++;
				else
				{
					frameCounter = 0;
					ballHit = false;
				}

				if (CheckCollisionRecs(ball->ballRect, player->colRect) && !ballHit)
				{
					ballSpeed += 0.5f;
					ball->speed.y = Clamp<float>(-player->y_vel,-8.0f-ballSpeed,8.0f+ballSpeed);
					ball->speed.x = -ball->speed.x;
					ball->speed.x += Sign<float>(ball->speed.x) * 1.0f;

					ballHit = true;
				}

				if (CheckCollisionRecs(ball->ballRect, enemy->colRect) && !ballHit)
				{
					ballSpeed += 0.5f;
					ball->speed.y = Clamp<float>(-enemy->y_vel, -8.0f-ballSpeed, 8.0f+ ballSpeed);
					ball->speed.x = -ball->speed.x;
					ball->speed.x += Sign<float>(ball->speed.x) * 1.0f;

					ballHit = true;
				}

				BeginDrawing();
				ClearBackground(WHITE);
				DrawText(FormatText("%i | %i",playerScore,enemyScore), (SCREEN_WIDTH/2)-200, (SCREEN_HEIGHT/2)-100, 200, BLACK);
				player->DrawPlayer();
				enemy->Draw();
				ball->Draw();


				EndDrawing();
			break;
			case WIN:
				if (frameCounter < 180)
					frameCounter++;
				else
				{
					frameCounter = 0;
					state = MAIN;
				}
				BeginDrawing();
				ClearBackground(WHITE);
				DrawText("WIN!", (SCREEN_WIDTH / 2) - 200, (SCREEN_HEIGHT / 2) - 100, 200, BLACK);
				EndDrawing();
			break;
			case LOSE:
				if (frameCounter < 180)
					frameCounter++;
				else
				{
					frameCounter = 0;
					state = MAIN;
				}
				BeginDrawing();
				ClearBackground(WHITE);
				DrawText("LOSE", (SCREEN_WIDTH / 2) - 250, (SCREEN_HEIGHT / 2) - 100, 200, BLACK);
				EndDrawing();
			break;
		}
	}

	CloseWindow();
	delete player;
	delete enemy;
	delete ball;
	return 0;
}

void Player::InitPlayer()
{
	position = {100,100};
	size = { 20,80 };
	colRect = { position.x,position.y,size.x,size.y };
	y_vel = 0;
}

void Player::UpdatePlayer()
{
	position.x = 40;
	y_vel = position.y;
	position.y = Clamp<int>(GetMouseY()-(size.y/2),0,SCREEN_HEIGHT-size.y);
	y_vel = y_vel - position.y;
	colRect.x = position.x;
	colRect.y = position.y;
}

void Player::DrawPlayer()
{
	DrawRectangleV(position, size, BLACK);
}

void Ball::Init()
{
	position = { (SCREEN_WIDTH/2)-SIZE,(SCREEN_HEIGHT/2)-SIZE};
	speed = { -5,5 };
	ballRect = { position.x,position.y,SIZE,SIZE };
}

void Ball::Update()
{
	position.x += Clamp<float>(speed.x,-120.0f,120.0f);
	position.y += Clamp<float>(speed.y, -15.0f, 15.0f);
	ballRect.x = position.x;
	ballRect.y = position.y;

	if (position.x <= 0)
	{
		position = { (SCREEN_WIDTH / 2) - SIZE,(SCREEN_HEIGHT / 2) - SIZE };
		speed = { -5,5 };
		speed.x = -speed.x;
		enemyScore++;
	}

	if (position.x >= SCREEN_WIDTH - SIZE)
	{
		position = { (SCREEN_WIDTH / 2) - SIZE,(SCREEN_HEIGHT / 2) - SIZE };
		speed = { -5,5 };
		speed.x = -speed.x;
		playerScore++;
	}


	if (position.y <= 0 || position.y >= SCREEN_HEIGHT - SIZE)
		speed.y = -speed.y;
}

void Ball::Draw()
{
	DrawRectangle(position.x,position.y, SIZE, SIZE, BLACK);
}

void Enemy::Init()
{
	position = { SCREEN_WIDTH-100,100 };
	size = { 20,80 };
	colRect = { position.x,position.y,size.x,size.y };
	y_vel = 0;
}

void Enemy::Update()
{
	position.x = SCREEN_WIDTH - 60;
	position.y = Clamp<float>(position.y,0.0f,SCREEN_HEIGHT-size.y);
	colRect.x = position.x;
	colRect.y = position.y;
}

void Enemy::Draw()
{
	DrawRectangleV(position, size, BLACK);
}
