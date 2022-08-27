#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <array>

// == Задание ==
// Необходимо разработать программу согласно варианту задания. Язык реализации любой. Пользовательский интерфейс или GUI, или CLI

// == Модель животного мира ==
// - Предметная область : взаимодействие жителей леса(равнины, океана, и пр), в формате игровой симуляции
//
// - Важные сущности : животное, передвижение, еда, питание, размножение, умирание, старость, голод,
// хищники, травоядные, растения, местопребывание(клетка на поле)
//
// - Моделировать в виде пошаговой симуляции, с возможностью добавления животных или растений на клетку поля

enum ANIMAL_STATE { MOVING, EATING, REPRODUCTION, DYING };
enum FOOD_STATE { FRESH, CORRUPTING, CORRUPTED };
enum PLANT_STATE { BEGINING_LIFE, MIDPART_LIFE, ENDING_LIFE };

// Сущности: животное, еда, растения
// Состояние: передвижение, питание, размножение, умирание
// Местоположение

// Три основных класса: Entity, State, Position
// - Entity: Animal -> PlantEatingAnimal, PredatorAnimal; Food, Plant
// - State: AnimalState, FoodState, PlantState
// - Position

// Поле:
// - Позиции: массив 30x30 = Position
// - Каждая позиция: 5 живых существ(хищники и травоядные), еда; растение(препятствие)

// ** Взаимодействие исключительно между Entity?
// ** У каждой сущности указатель на позицию?
// ** Состояние у каждой сущности?

// Базовые классы
class Entity
{
	int id;
	int x;
	int y;
	int old_level;

public:
	Entity()
	{
		id = x = y = old_level = 0;
	}
	Entity(int id, int x, int y, int old) : id(id), x(x), y(y), old_level(old) {}

	int get_x() { return x; }
	int get_y() { return y; }

	void set_x(int x) { x = x; }
	void set_y(int y) { y = y; }

	virtual bool is_animal() { return false; }
	virtual bool is_food() { return false; }
	virtual bool is_plant() { return false; }

	virtual bool move_to(int x, int y) { return false; }
	virtual bool update_state() { return false; }
};

class State
{
	short int state;

public:
	State(short int state) : state(state) {}
};

class Map
{
	int x_size;
	int y_size;

public:
	Map(int x, int y) : x_size(x), y_size(y) {}

	bool is_position_valid(int x, int y)
	{
		return (x >= 1 && x <= x_size) &&
			(y >= 1 && y <= y_size);
	}

	int get_xsize() { return x_size; }
	int get_ysize() { return y_size; }
};

// Состояния
class AnimalState : State
{
	enum ANIMAL_STATE { MOVING, EATING, REPRODUCTION, DYING };

public:
	AnimalState(ANIMAL_STATE state) : State(int(state)) {}

	static ANIMAL_STATE get_moving() { return MOVING; }
	static ANIMAL_STATE get_eating() { return EATING; }
	static ANIMAL_STATE get_reproduction() { return REPRODUCTION; }
	static ANIMAL_STATE get_dying() { return DYING; }
};

class FoodState : State
{
	enum FOOD_STATE { FRESH, CORRUPTING, CORRUPTED };

public:
	FoodState(FOOD_STATE state) : State(state) {}


	static FOOD_STATE get_fresh() { return FRESH; }
	static FOOD_STATE get_corrupting() { return CORRUPTING; }
	static FOOD_STATE get_corrupted() { return CORRUPTED; }
};

class PlantState : State
{
	enum PLANT_STATE { BEGINING_LIFE, MIDPART_LIFE, ENDING_LIFE };

public:
	PlantState(PLANT_STATE state) : State(state) {}


	static PLANT_STATE get_begin() { return BEGINING_LIFE; }
	static PLANT_STATE get_mid() { return MIDPART_LIFE; }
	static PLANT_STATE get_end() { return ENDING_LIFE; }
};

// Сущности(Entity)
class Animal : public Entity
{
	bool is_predator_type;
	int starve_level;

	AnimalState state;

public:
	Animal(int id, int x, int y, bool predator, int old, int starve, AnimalState state) : Entity(id, x, y, old),
		is_predator_type(predator), starve_level(starve), state(state) {}

	bool is_animal() override { return true; }
	bool is_predator() { return is_predator_type; }
	bool move_to(int x, int y) override
	{
		set_x(x);
		set_y(y);

		return true;
	}
};

class Food : public Entity
{
	FoodState state;

public:
	Food(int id, int x, int y, int old, FoodState state) : Entity(id, x, y, old), state(state) {}

	bool is_food() override { return true; }
};

class Plant : public Entity
{
	PlantState state;

public:
	Plant(int id, int x, int y, int old, PlantState state) : Entity(id, x, y, old), state(PlantState(state)) {}

	bool is_plant() override { return true; }
};

// MVC (TEST)

class Model
{
	std::vector<Entity *> entities;
	Map map;

	int last_id = 0;

	int animals_count;
	int food_count;
	int plants_count;

public:
	Model(int width, int height, int a_count, int f_count, int p_count) : map(Map(height, width)),
		animals_count(a_count), food_count(f_count), plants_count(p_count)
	{
		srand(time(0));

		entities = std::vector<Entity *>(a_count + f_count + p_count);
		for (int i = 0; i < animals_count; i++)
		{
			Entity* ent = new Animal(last_id, rand() % map.get_xsize() + 1,
				rand() % map.get_ysize() + 1,
				(bool)(rand() % 2),
				3, 0, AnimalState(AnimalState::get_moving()));

			entities[i] = ent;

			last_id++;
		}

		for (int i = 0; i < food_count; i++)
		{
			Entity* ent = new Food(last_id, rand() % map.get_xsize() + 1,
				rand() % map.get_ysize() + 1,
				1, FoodState(FoodState::get_fresh()));

			entities[i + animals_count] = ent;

			last_id++;
		}

		for (int i = 0; i < plants_count; i++)
		{
			Entity* ent = new Plant(last_id, rand() % map.get_xsize() + 1,
				rand() % map.get_ysize() + 1,
				3, PlantState(PlantState::get_begin()));

			entities[i + animals_count + food_count] = ent;

			last_id++;
		}
	}

	Map get_map() { return map; }

	std::vector<Entity *> get_entities() { return entities; }
};

class View
{
	Model& model;
	std::string map_representation;

public:
	View(Model& model) : model(model)
	{
		int x_dim = model.get_map().get_xsize();
		int y_dim = model.get_map().get_ysize();

		map_representation = "";

		char** positions = new char* [x_dim];

		for (int i = 0; i < x_dim; i++)
		{
			positions[i] = new char[y_dim];

			for (int j = 0; j < y_dim; j++)
				positions[i][j] = '-';
		}


		std::vector<Entity *> entities = model.get_entities();

		for (int i = 0; i < entities.size(); i++)
		{
			char symbol = ' ';

			if (entities[i]->is_animal())
			{
				Animal* animal = dynamic_cast<Animal *>(entities[i]);

				if (animal->is_predator())
					symbol = '&';

				else
					symbol = '@';
			}

			else if (entities[i]->is_food())
				symbol = '*';

			else
				symbol = 'X';

			positions[entities[i]->get_x() - 1][entities[i]->get_y() - 1] = symbol;
		}

		for (int i = 0; i < x_dim; i++)
		{
			for (int j = 0; j < y_dim; j++)
			{
				map_representation += std::string(1, positions[i][j]) + " ";
			}

			map_representation += "\n";
		}

		for (int i = 0; i < x_dim; i++)
		{
			delete positions[i];
		}
	}

	std::string get_representation()
	{
		return map_representation;
	}
};

int main()
{
	Model model = Model(70, 100, 10, 5, 6);

	View view = View(model);

	std::cout << view.get_representation();

	return 0;
}