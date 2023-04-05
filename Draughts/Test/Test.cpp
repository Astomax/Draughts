#include <iostream>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <random>
#include <vector>
#include <math.h>
#include <array>
#include <string>

const float PI = 3.141592f;
const int window_size = 1000;

float color_A[3] = { 1, 0, 0 };

float color_B[3] = { 0, 0, 1 };

float color_A2[3] = { 0.5f, 0, 0 };

float color_B2[3] = { 0, 0, 0.5f };

float selected_color[3] = { 0, 1, 0 };

float background_color[3] = { 0.3f, 0.3f, 0.3f};

float number_color[3] = { 1, 1, 1 };

float number_background_color[3] = { 0, 0, 0 };

int lines[200][2];
int line_count;

const int numbers[10][15] = {
		{ 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1 },
	{ 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 },
	{ 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1 },
	{ 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1 },
	{ 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1 },
	{ 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1 },
	{ 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1 },
	{ 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1 },
	{ 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1 },
	{ 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1 }
};

void DrawCube(int x, int y, int size, float color[3])
{
	glBegin(GL_TRIANGLES);
	glColor3f(color[0], color[1], color[2]);
	glVertex2i(x, y + size);
	glVertex2i(x + size, y);
	glVertex2i(x, y);

	glColor3f(color[0], color[1], color[2]);
	glVertex2i(x + size, y + size);
	glVertex2i(x, y + size);
	glVertex2i(x + size, y);

	glEnd();
}

void DrawNumbers(std::vector<int> indexes, int x_centre, int y_centre, int font_size, float color[3])
{
	int text_size = indexes.size();
	font_size = int(float(font_size) / 4.0f);
	int width = (text_size * 3 + (text_size - 1)) * font_size;
	int height = text_size * 5;

	int x = x_centre - (width / 2);
	int y = y_centre - (height / 2);

	for (int i = 0; i < text_size; i++)
	{
		for (int i2 = 0; i2 < 5; i2++)
		{
			for (int i3 = 0; i3 < 3; i3++)
			{
				int index = (i2 * 3) + i3;
				if (numbers[indexes[i]][index] == 1)
				{
					DrawCube(x + (i3 * font_size), y + (i2 * font_size), font_size, color);
				}
			}
		}

		x += 4 * font_size;
	}
}

int RandomMinPlus()
{
	if (rand() % 2 == 0)
	{
		return -1;
	}

	return 1;
}

float RandomFloat(float Max)
{
	return static_cast<float> (rand()) / (static_cast<float> (RAND_MAX / Max));
}

void DrawCircle(float x_mid, float y_mid, float radius, float color[3])
{
	float angle = 0;

	float previous_x = x_mid + radius;
	float previous_y = y_mid;

	for (int i = 0; i < 100; i++)
	{
		angle += 2 * PI / 99;

		float x = cos(angle) * radius + x_mid;
		float y = sin(angle) * radius + y_mid;

		glColor3f(color[0], color[1], color[2]);
		glVertex2f(x, y);
		glVertex2f(previous_x, previous_y);
		glVertex2f(x_mid, y_mid);

		previous_x = x;
		previous_y = y;
	}
}

struct Board
{
	int search_depth = 8;
	int board[100] = { 0 };

	bool cannot_jump[100][4] = {
		{
			false
		}
	};

	int directions[4] = { 9, 11, -9, -11 };

	bool can_beat = false;

	void DebugThings()
	{
		board[40] = -1;
		board[42] = -1;
		board[71] = 0;
		board[93] = 0;
		board[75] = 0;

		int start_to_end[200][2];
		int remove[200][10];
		int routes_found = 0;

		int removing_array[9];

		HopMovementPiece(31, 1, removing_array, 0, 31, remove, start_to_end, routes_found);
		std::cout << routes_found << " routes found\n";
	}

	void Setup()
	{
		for (int i = 0; i < 10; i++)
		{
			for (int i2 = 0; i2 < 2; i2++)
			{
				// Left of the board
				cannot_jump[i * 10 + i2][0] = true;
				cannot_jump[i * 10 + i2][3] = true;

				// Right of the board
				cannot_jump[i * 10 + 8 + i2][1] = true;
				cannot_jump[i * 10 + 8 + i2][2] = true;

				// Down of the board
				cannot_jump[i + i2 * 10][2] = true;
				cannot_jump[i + i2 * 10][3] = true;

				// up of the board
				cannot_jump[i + i2 * 10 + 80][0] = true;
				cannot_jump[i + i2 * 10 + 80][1] = true;
			}
		}
	}

	void Start()
	{
		// Check where a certain direction can not jump
		// Place all the pieces at the correct place
		int index = 0;
		for (int i = 0; i < 4; i++)
		{
			int start = i % 2;
			for (int i2 = 0; i2 < 5; i2++)
			{
				board[i * 10 + start] = 1;
				board[i * 10 + 60 + start] = -1;

				index++;
				start += 2;
			}
		}
	}

	void QueenAttackMovement(int pos, int value, int removing[29], int removing_index, int old_pos, int(&remove)[200][10], int(&start_to_end)[200][2], int& routes_found)
	{
		bool end = true;

		for (int i = 0; i < 4; i++)
		{
			int new_pos = pos;
			int direction = directions[i];

			while (1 == 1)
			{
				if (cannot_jump[new_pos][i])
				{
					break;
				}

				new_pos += direction;

				int multi_value = board[new_pos] * value;

				if (multi_value < 0 && board[new_pos + direction] == 0)
				{
					bool is_old_pos = false;
					int new_removing_array[29] = { 0 };

					for (int i2 = 0; i2 < removing_index; i2++)
					{
						if (new_pos == removing[i2])
						{
							is_old_pos = true;
							break;
						}

						new_removing_array[i2] = removing[i2];
					}

					if (is_old_pos)
					{
						break;
					}

					int new_index = removing_index + 1;
					new_removing_array[removing_index] = new_pos;

					QueenAttackMovement(new_pos + direction, value, new_removing_array, new_index, old_pos, remove, start_to_end, routes_found);
					end = false;
					break;
				}
				else if (multi_value != 0)
				{
					break;
				}
			}
		}

		if (end == false || removing_index == 0)
		{
			return;
		}

		for (int i = 0; i < removing_index; i++)
		{
			remove[routes_found][i] = removing[i];
		}

		if (routes_found >= 200 || removing_index >= 29)
		{
			std::cout << "error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
		}

		remove[routes_found][removing_index] = 666;

		start_to_end[routes_found][0] = old_pos;
		start_to_end[routes_found][1] = pos;

		routes_found++;
	}

	void HopMovementPiece(int pos, int value, int removing[29], int removing_index, int old_pos, int(&remove)[200][10], int(&start_to_end)[200][2], int& routes_found)
	{
		bool end = true;

		for (int i = 0; i < 4; i++)
		{
			if (cannot_jump[pos][i])
			{
				continue;
			}

			int direction = directions[i];

			int enemy_pos = pos + direction;

			if (board[enemy_pos] * value >= 0)
			{
				continue;
			}

			int new_pos = enemy_pos + direction;

			if (board[new_pos] != 0)
			{
				continue;
			}

			bool is_old_pos = false;
			int new_removing_array[29] = { 0 };

			for (int i2 = 0; i2 < removing_index; i2++)
			{
				if (enemy_pos == removing[i2])
				{
					is_old_pos = true;
					break;
				}

				new_removing_array[i2] = removing[i2];
			}

			if (is_old_pos)
			{
				continue;
			}

			int new_index = removing_index + 1;
			new_removing_array[removing_index] = enemy_pos;

			HopMovementPiece(new_pos, value, new_removing_array, new_index, old_pos, remove, start_to_end, routes_found);
			end = false;
		}

		if (end == false || removing_index == 0)
		{
			return;
		}

		for (int i = 0; i < removing_index; i++)
		{
			remove[routes_found][i] = removing[i];
		}

		remove[routes_found][removing_index] = 666;

		start_to_end[routes_found][0] = old_pos;
		start_to_end[routes_found][1] = pos;

		routes_found++;
	}

	void QueenMovement(int pos, int(&start_to_end)[200][2], int& routes_found)
	{
		for (int i = 0; i < 4; i++)
		{
			int new_pos = pos;
			int direction = directions[i];

			while (1 == 1)
			{
				int div = new_pos % 10;

				if (div == 0 && (direction == 9 || direction == -11))
				{
					break;
				}

				if (div == 9 && (direction == -9 || direction == 11))
				{
					break;
				}

				if (new_pos > 89 && direction > 0)
				{
					break;
				}

				if (new_pos < 10 && direction < 0)
				{
					break;
				}

				new_pos += direction;

				if (board[new_pos] != 0)
				{
					break;
				}

				start_to_end[routes_found][0] = pos;
				start_to_end[routes_found][1] = new_pos;

				routes_found++;
			}
		}
	}

	void NormalMovement(int pos, int direction_index, int(&start_to_end)[200][2], int& routes_found)
	{
		for (int i = 0; i < 2; i++)
		{
			int div = pos % 10;
			int direction = directions[direction_index + i];

			if (div == 0 && (direction == 9 || direction == -11))
			{
				continue;
			}

			if (div == 9 && (direction == -9 || direction == 11))
			{
				continue;
			}

			if (pos > 89 && direction > 0)
			{
				continue;
			}

			if (pos < 10 && direction < 0)
			{
				continue;
			}

			int new_pos = pos + direction;

			if (board[new_pos] != 0)
			{
				continue;
			}

			start_to_end[routes_found][0] = pos;
			start_to_end[routes_found][1] = new_pos;

			routes_found++;
		}
	}

	int total_runs = 0;

	int MiniMax(int side, int depth, int value, int score)
	{
		total_runs++;

		if (depth == search_depth)
		{
			return score;
		}

		can_beat = false;

		bool do_remove = true;
		int new_depth = depth + 1;

		int start_to_end[200][2] = {
		{ 0 }
		};

		int remove[200][10] = {
		{ 0 }
		};

		int routes_found = 0;
		int removing[29];

		int new_value = 666;

		int positions[20] = { 0 };

		int pos_index = 0;

		int norm_index = 0;

		int selected_score = -1000;
		if (side == -1)
		{
			selected_score = 1000;
			norm_index = 2;
		}

		std::vector<int> best_route;

		for (int i = 0; i < 20; i++)
		{
			positions[i] = 666;
		}

		// Get the location of the pieces of the selected side
		for (int i = 0; i < 10; i++)
		{
			int start = i % 2;
			for (int i2 = 0; i2 < 5; i2++)
			{
				int selected_index = i * 10 + start;

				if (board[selected_index] * side > 0)
				{
					positions[pos_index] = selected_index;
					pos_index++;
				}

				start += 2;
			}
		}

		// Check if pieces can be hopped
		for (int i = 0; i < 20; i++)
		{
			int pos = positions[i];

			if (pos == 666)
			{
				continue;
			}

			if (abs(board[pos]) > 1)
			{
				QueenAttackMovement(pos, side, removing, 0, pos, remove, start_to_end, routes_found);
				continue;
			}

			HopMovementPiece(pos, side, removing, 0, pos, remove, start_to_end, routes_found);
		}

		// If no routes where foudn where pieces where hopped, go to basic
		if (routes_found == 0)
		{
			do_remove = false;
			for (int i = 0; i < 20; i++)
			{
				int pos = positions[i];

				if (pos == 666)
				{
					continue;
				}

				if (abs(board[pos]) > 1)
				{
					QueenMovement(pos, start_to_end, routes_found);
					continue;
				}

				NormalMovement(pos, norm_index, start_to_end, routes_found);
			}
		}
		else
		{
			can_beat = true;
		}

		for (int i = 0; i < routes_found; i++)
		{
			// Remove all the pieces if they have to be removed
			// Every piece removed gives extra score to selected side

			int remember[9] = {};

			if (do_remove)
			{
				for (int i2 = 0; i2 < 9; i2++)
				{
					int a = remove[i][i2];

					if (a == 666)
					{
						break;
					}

					score += side;
					remember[i2] = board[a];
					board[a] = 0;
				}
			}

			int index_A = start_to_end[i][1];

			int change_value = board[start_to_end[i][0]];

			// Set the old piece to target position
			board[index_A] = change_value;
			board[start_to_end[i][0]] = 0;

			if (abs(board[index_A]) == 1)
			{
				if (index_A < 10 && side == -1)
				{
					score -= 1;
					board[index_A] = side * 2;
				}
				else if (index_A > 89 && side == 1)
				{
					score++;
					board[index_A] = side * 2;
				}
			}

			// Go 1 depth further, get the score with it
			// The value variable gives the current highest or lowest score.
			int new_score = MiniMax(side * -1, new_depth, new_value, score);

			// If the value was not set before set it here for the first time
			if (value == 666)
			{
				new_value = new_score;
			}

			// If the side == -1, then you want the minimum score
			// Else if the side == 1, then you want the maximum
			// If new best is found set the new value to that value
			if (side == -1)
			{
				if (new_score <= selected_score)
				{
					if (depth == 0)
					{
						if (new_score != selected_score)
						{
							best_route.erase(best_route.begin(), best_route.end());
						}

						best_route.insert(best_route.begin(), i);
					}

					selected_score = new_score;
					new_value = new_score;
				}
			}
			else if (side == 1)
			{
				if (new_score >= selected_score)
				{
					if (depth == 0)
					{
						if (new_score != selected_score)
						{
							best_route.erase(best_route.begin(), best_route.end());
						}

						best_route.insert(best_route.begin(), i);
					}

					selected_score = new_score;
					new_value = new_score;
				}
			}

			// Set the board back to how it was
			if (do_remove)
			{
				for (int i2 = 0; i2 < 9; i2++)
				{
					int a = remove[i][i2];

					if (a == 666)
					{
						break;
					}

					score -= side;

					board[a] = remember[i2];
				}
			}

			if (board[start_to_end[i][1]] != change_value)
			{
				if (side == -1)
				{
					score++;
				}
				else if (side == 1)
				{
					score -= 1;
				}
			}

			board[start_to_end[i][0]] = change_value;
			board[start_to_end[i][1]] = 0;

			if (depth > 0 && value != 666)
			{
				// If side == 1, then you have to get the maximum.
				// If the next node in the tree is smaller then max elliminate
				if (side == -1 && new_score < value)
				{
					return -100000000;
				}

				// If side == 1, then you have to get the minimum.
				// If the next node in the tree is smaller then max elliminate
				if (side == 1 && new_score > value)
				{
					return 10000000;
				}
			}
		}

		if (depth == 0)
		{
			if (best_route.size() == 0)
			{
				return 123456;
			}

			int random_best_route = best_route[rand() % best_route.size()];

			int begin_index = start_to_end[random_best_route][0];

			line_count = 0;
			lines[0][0] = begin_index;

			if (do_remove)
			{
				for (int i2 = 0; i2 < 9; i2++)
				{
					int a = remove[random_best_route][i2];

					if (a == 666)
					{
						break;
					}

					int direction = a - lines[line_count][0];

					if (abs(direction) % 9 == 0)
					{
						if (direction < 0)
						{
							direction = -9;
						}
						else
						{
							direction = 9;
						}
					}

					if (abs(direction) % 11 == 0)
					{
						if (direction < 0)
						{
							direction = -11;
						}
						else
						{
							direction = 11;
						}
					}

					lines[line_count][1] = a + direction;
					line_count++;
					lines[line_count][0] = a + direction;

					score += side;

					board[a] = 0;
				}
			}

			int index_A = start_to_end[random_best_route][1];

			lines[line_count][1] = index_A;

			board[index_A] = board[start_to_end[random_best_route][0]];

			if (abs(board[index_A]) == 1)
			{
				if (index_A < 10 && side == -1)
				{
					board[index_A] = side * 2;
				}
				else if (index_A > 89 && side == 1)
				{
					board[index_A] = side * 2;
				}
			}

			board[start_to_end[random_best_route][0]] = 0;
		}

		// Return the found score
		return selected_score;
	}
};

int Score(Board selected)
{
	int score = 0;
	for (int i = 0; i < 10; i++)
	{
		int start = i % 2;
		for (int i2 = 0; i2 < 5; i2++)
		{
			int index = i * 10 + start;

			if (selected.board[index] > 0)
			{
				score++;
			}
			else if (selected.board[index] < 0)
			{
				score -= 1;
			}

			start += 2;
		}
	}

	return score;
}

int main(void)
{
	std::srand(std::time(NULL));

	std::cout << "beginning\n";
	GLFWwindow* window;

	/*Initialize the library */
	if (!glfwInit())
		return -1;

	/*Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(window_size + 300, window_size, "enge dingen", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/*Make the window's context current */
	glfwMakeContextCurrent(window);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0.0, window_size + 300, window_size, 0.0);

	Board test_board;

	test_board.Start();
	test_board.Setup();

	int pressed = 0;

	bool clicked = false;
	int clicked_x = -1;
	int clicked_y = -1;
	int clicked_index = -1;

	int start_to_end[200][2] = {
		{ 0 }
	};

	int remove[200][10] = {
		{ 0 }
	};

	int routes_found = 0;
	int removing[29];

	int make_move = 0;
	bool move = false;

	int show_lines = 0;
	int selected_side = -1;

	int difference = 0;

	/*Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/*Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(selected_color[0], selected_color[1], selected_color[2], 1);

		if (make_move < 0 && move)
		{
			int score = Score(test_board);
			difference = test_board.MiniMax(-1, 0, 666, score);

			move = false;
			show_lines = 100;
		}

		/*
		if (make_move < 0) {
			if (selected_side == -1) {
				test_board.search_depth = 6;
			}
			else {
				test_board.search_depth = 6;
			}

			int score = Score(test_board);
			difference = test_board.MiniMax(selected_side, 0, 666, score);

			if (difference == 123456) {
				test_board.Start();
			}
			else {
				selected_side *= -1;

				make_move = 70;
				show_lines = 69;
			}
		}

		*/

		make_move -= 1;

		glBegin(GL_TRIANGLES);

		glColor3f(background_color[0], background_color[1], background_color[2]);
		glVertex2i(1000, 0);
		glVertex2i(1300, 0);
		glVertex2i(1000, 1000);

		glColor3f(background_color[0], background_color[1], background_color[2]);
		glVertex2i(1300, 1000);
		glVertex2i(1300, 0);
		glVertex2i(1000, 1000);

		int white_pieces = 0;
		int black_pieces = 0;

		for (int i = 0; i < 10; i++)
		{
			int start = i % 2;

			for (int i2 = 0; i2 < 10; i2++)
			{
				float color = 0;
				if (start % 2 == 0)
				{
					color = 1;
				}

				bool good = true;

				if (clicked_index != -1)
				{
					for (int i3 = 0; i3 < routes_found; i3++)
					{
						if (start_to_end[i3][1] == ((9 - i2) * 10 + i))
						{
							good = false;
							break;
						}
					}
				}

				if (good == false)
				{
					start++;
					continue;
				}

				if (clicked_x == i && clicked_y == i2)
				{
					start++;
					continue;
				}

				glColor3f(color, color, color);
				glVertex2f(i * 100, i2 * 100);
				glVertex2f(i * 100 + 100, i2 * 100);
				glVertex2f(i * 100, i2 * 100 + 100);

				glColor3f(color, color, color);
				glVertex2f(i * 100 + 100, i2 * 100 + 100);
				glVertex2f(i * 100 + 100, i2 * 100);
				glVertex2f(i * 100, i2 * 100 + 100);

				start++;
			}
		}

		for (int i = 0; i < 10; i++)
		{
			int start = i % 2;
			for (int i2 = 0; i2 < 5; i2++)
			{
				int index = i * 10 + start;
				if (test_board.board[index] == -1)
				{
					black_pieces++;
					DrawCircle(start * 100 + 50, (9 - i) * 100 + 50, 35, color_B);
				}
				else if (test_board.board[index] == 1)
				{
					white_pieces++;
					DrawCircle(start * 100 + 50, (9 - i) * 100 + 50, 35, color_A);
				}
				else if (test_board.board[index] == 2)
				{
					white_pieces++;
					DrawCircle(start * 100 + 50, (9 - i) * 100 + 50, 35, color_A2);
				}
				else if (test_board.board[index] == -2)
				{
					black_pieces++;
					DrawCircle(start * 100 + 50, (9 - i) * 100 + 50, 35, color_B2);
				}

				start += 2;
			}
		}

		glEnd();

		std::vector<int> white_vector;
		for (char item : std::to_string(white_pieces))
		{
			white_vector.insert(white_vector.end(), item - "0"[0]);
		}

		std::vector<int> black_vector;
		for (char item : std::to_string(black_pieces))
		{
			black_vector.insert(black_vector.end(), item - "0"[0]);
		}

		std::vector<int> difference_vector;

		for (char item : std::to_string(abs(difference)))
		{
			difference_vector.insert(difference_vector.end(), item - "0"[0]);
		}

		DrawCube(1090, 110, 120, number_background_color);
		DrawCube(1090, 760, 120, number_background_color);

		DrawNumbers(white_vector, 1150, 1000 - 200, 50, number_color);
		DrawNumbers(black_vector, 1150, 150, 50, number_color);

		float green[3] = { 0, 1, 0 };

		float red[3] = { 1, 0, 0 };

		if (difference > 0)
		{
			DrawNumbers(difference_vector, 1150, 500, 50, green);
		}
		else if (difference < 0)
		{
			DrawNumbers(difference_vector, 1150, 500, 50, red);
		}
		else
		{
			DrawNumbers(difference_vector, 1150, 500, 50, number_color);
		}

		glBegin(GL_LINES);

		if (show_lines > 0)
		{
			for (int i = 0; i < line_count + 1; i++)
			{
				int Xa = (lines[i][0] % 10) * 100 + 50;
				int Ya = (9 - lines[i][0] / 10) * 100 + 50;

				int Xb = (lines[i][1] % 10) * 100 + 50;
				int Yb = (9 - lines[i][1] / 10) * 100 + 50;

				glColor3f(selected_color[0], selected_color[1], selected_color[2]);
				glVertex2i(Xa, Ya);
				glVertex2i(Xb, Yb);
			}
		}

		glEnd();

		show_lines -= 1;

		if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) && pressed < 0 && move == false)
		{
			double x_pos;
			double y_pos;

			glfwGetCursorPos(window, &x_pos, &y_pos);

			x_pos = int(x_pos);
			y_pos = int(y_pos);

			int x = x_pos / 100;
			int y = (y_pos) / 100;

			if (clicked_index != -1)
			{
				int new_index = x + (9 - y) * 10;

				for (int i = 0; i < routes_found; i++)
				{
					if (start_to_end[i][1] == new_index)
					{
						test_board.board[new_index] = test_board.board[clicked_index];
						test_board.board[clicked_index] = 0;

						if (new_index > 89)
						{
							test_board.board[new_index] = 2;
						}

						for (int i2 = 0; i2 < 9; i2++)
						{
							if (remove[i][i2] == 666)
							{
								break;
							}

							test_board.board[remove[i][i2]] = 0;
						}

						move = true;
						make_move = 50;

						break;
					}
				}

				clicked_index = -1;

				clicked_x = -1;
				clicked_y = -1;
			}
			else
			{
				routes_found = 0;
				for (int i = 0; i < 200; i++)
				{
					for (int i2 = 0; i2 < 10; i2++)
					{
						remove[i][i2] = 666;
					}
				}

				clicked_index = x + (9 - y) * 10;

				clicked_x = x;
				clicked_y = y;

				if (test_board.board[clicked_index] > 0)
				{
					test_board.MiniMax(1, test_board.search_depth - 1, 666666, 0);

					if (test_board.board[clicked_index] > 1)
					{
						test_board.QueenAttackMovement(clicked_index, 1, removing, 0, clicked_index, remove, start_to_end, routes_found);

						if (routes_found == 0 && test_board.can_beat == false)
						{
							test_board.QueenMovement(clicked_index, start_to_end, routes_found);
						}
					}
					else
					{
						test_board.HopMovementPiece(clicked_index, 1, removing, 0, clicked_index, remove, start_to_end, routes_found);

						if (routes_found == 0 && test_board.can_beat == false)
						{
							test_board.NormalMovement(clicked_index, 0, start_to_end, routes_found);
						}
					}
				}
			}

			pressed = 200;
		}

		pressed -= 1;

		if (clicked)
		{
			int score = Score(test_board);
			test_board.MiniMax(-1, 0, 666, score);
		}

		/*Swap front and back buffers */
		glfwSwapBuffers(window);

		/*Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}