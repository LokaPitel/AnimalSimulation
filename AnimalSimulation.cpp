#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <chrono>
#include <random>
#include <vector>
#include <array>

#include <windows.h>

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


// Полезные классы
class CoordinateSystem
{
	int x; 
	int y;

public:
	CoordinateSystem() { x = y = 0; }
	CoordinateSystem(int x, int y) : x(x), y(y) {}

	int get_x() { return x; }
	int get_y() { return y; }

	void set_x(int x) { this->x = x; }
	void set_y(int y) { this->y = y; }

	int get_x_index() { return y - 1; }
	int get_y_index() { return x - 1; }
};

class Keyboard
{
public:
	static const int ENTER_KEY = 13;
	static const int ESC_KEY = 27;

	static int handle_keyboard()
	{
		while (true)
		{
			if (_kbhit())
			{
				int key = _getch();

				if (key == ENTER_KEY || key == ESC_KEY)
					return key;
			}
		}
	}
};

class StringUtility
{
public:
	static std::string make_column_right_align(int width, std::string content)
	{
		std::string result = "" + content;

		while (result.size() < width)
			result = " " + result;

		return result;
	}

	static std::string make_column_left_align(int width, std::string content)
	{
		std::string result = content + "";

		while (result.size() < width)
			result = result + " ";

		return result;
	}

	static std::string make_column_center_align(int width, std::string content)
	{
		std::string result = content + "";

		bool even = true;

		while (result.size() < width)
		{
			if (!even)
				result = " " + result;

			else
				result = result + " ";

			even = !even;
		}

		return result;
	}
};

// Состояния
class State
{
protected:
	short int state;

public:
	State(short int state) : state(state) {}
	short int get_state() { return state; }
};

class AnimalState : State
{
public:
	enum ANIMAL_STATE { MOVING, EATING, REPRODUCTION, DYING };

	AnimalState(ANIMAL_STATE state) : State(int(state)) {}

	short int get_state() { return state; }
	void set_state(ANIMAL_STATE state) { this->state = state; }
};

class FoodState : State
{
public:
	enum FOOD_STATE { FRESH, CORRUPTING, CORRUPTED };
	FoodState(FOOD_STATE state) : State(state) {}

	void set_state(FOOD_STATE state) { this->state = state; }

	static FOOD_STATE get_fresh() { return FRESH; }
	static FOOD_STATE get_corrupting() { return CORRUPTING; }
	static FOOD_STATE get_corrupted() { return CORRUPTED; }
};

class PlantState : State
{
public:
	enum PLANT_STATE { BEGINING_LIFE, MIDPART_LIFE, ENDING_LIFE };

	PlantState(PLANT_STATE state) : State(state) {}

	void set_state(PLANT_STATE state) { this->state = state; }

	static PLANT_STATE get_begin() { return BEGINING_LIFE; }
	static PLANT_STATE get_mid() { return MIDPART_LIFE; }
	static PLANT_STATE get_end() { return ENDING_LIFE; }
};

// Сущности(Entity)
class Entity
{
	int id;
	CoordinateSystem coord;

protected:
	int health;
	int old_level;

public:
	static const int MAX_HEALTH = 100;
	static const int AVERAGE_HEALTH = 70;
	static const int LOW_HEALTH = 30;

	Entity() : coord(CoordinateSystem(0, 0))
	{
		id = old_level = 0;

		health = 20;
	}
	Entity(int id, int x, int y, int old) : id(id), coord(CoordinateSystem(x, y)), old_level(old), health(20) {}

	int get_x() { return coord.get_x(); }
	int get_y() { return coord.get_y(); }

	void set_x(int x) { coord.set_x(x); }
	void set_y(int y) { coord.set_y(y); }

	int get_health() { return health; }
	int get_old() { return old_level; }

	bool operator==(const Entity& rhs)
	{
		return id == rhs.id;
	}

	virtual bool is_animal() { return false; }
	virtual bool is_food() { return false; }
	virtual bool is_plant() { return false; }
	virtual bool update_state() { return false; }

	virtual bool update() { return false; }

	virtual void add_health(int number) {}
	virtual void add_old(int number) {}
};

class Animal : public Entity
{
	bool is_predator_type;
	int starve_level;

	AnimalState state;

public:
	static const int MAX_OLD = 20;
	static const int AVERAGE_OLD = 12;
	static const int YOUNG_OLD = 6;

	static const int MAX_STARVE = 20;
	static const int AVERAGE_STARVE = 14;
	static const int LOW_STARVE = 6;

	Animal(int id, int x, int y, bool predator, int old, int starve, AnimalState state) : Entity(id, x, y, old),
		is_predator_type(predator), starve_level(starve), state(state) {}

	int get_starve() { return starve_level; }

	bool is_animal() override { return true; }
	bool is_predator() { return is_predator_type; }
	
	virtual bool update() override
	{ 
		if (starve_level < MAX_STARVE)
			starve_level++;

		if (old_level < MAX_OLD)
			old_level++;

		return true;
	}

	virtual void add_health(int number) override
	{
		health += number;

		if (health > MAX_HEALTH)
			health = MAX_HEALTH;

		if (health < 0)
			health = 0;
	}

	virtual void add_old(int number) override {}

	void add_starve(int number) 
	{
		if (starve_level > MAX_STARVE)
			starve_level = MAX_STARVE;

		if (starve_level < 0)
			starve_level = 0;
	}

	void set_state(AnimalState::ANIMAL_STATE state) { this->state.set_state(state); }
};

class Food : public Entity
{
	FoodState state;

public:
	static const int MAX_OLD = 6;
	static const int AVERAGE_OLD = 4;

	Food(int id, int x, int y, int old, FoodState state) : Entity(id, x, y, old), state(state) {}

	bool is_food() override { return true; }

	void set_state(FoodState::FOOD_STATE state) { this->state.set_state(state); }

	virtual void add_health(int number)
	{
		health += number;

		if (health > 100)
			health = 100;

		if (health < 0)
			health = 0;
	}

	virtual void add_old(int number) {}

	virtual bool update() override
	{
		if (old_level < MAX_OLD)
			old_level++;

		return true;
	}
};

class Plant : public Entity
{
	PlantState state;

public:
	static const int MAX_OLD = 40;
	static const int AVERAGE_OLD = 25;

	Plant(int id, int x, int y, int old, PlantState state) : Entity(id, x, y, old), state(PlantState(state)) {}

	bool is_plant() override { return true; }

	void set_state(PlantState::PLANT_STATE state) { this->state.set_state(state); }

	virtual bool update() override
	{
		if (old_level < 20)
			old_level++;

		return true;
	}

	virtual void add_health(int number) override
	{
		health += number;

		if (health > 100)
			health = 100;

		if (health < 0)
			health = 0;
	}

	virtual void add_old(int number) override {}
};

// Карта & Взаимодействие

class Map
{
	int x_size;
	int y_size;

	int planteating_count;
	int predator_count;
	int food_count;
	int plants_count;

	int last_id = 0;

	std::vector<Entity*> entities;

public:
	Map(int x, int y, int a_count, int f_count, int p_count) : food_count(f_count), plants_count(p_count)
	{
		planteating_count = a_count - 0.3 * a_count;
		predator_count = a_count - planteating_count;



		x_size = x;
		y_size = y;

		std::default_random_engine default_engine;
		std::uniform_int_distribution<int> uniform_distribution_x(1, x);
		std::uniform_int_distribution<int> uniform_distribution_y(1, y);

		entities = std::vector<Entity*>(a_count + f_count + p_count);
		for (int i = 0; i < planteating_count; i++)
		{
			Entity* ent = new Animal(last_id, uniform_distribution_x(default_engine),
				uniform_distribution_y(default_engine),
				false,
				3, 0, AnimalState(AnimalState::MOVING));

			entities[i] = ent;

			last_id++;
		}

		for (int i = 0; i < predator_count; i++)
		{
			Entity* ent = new Animal(last_id, uniform_distribution_x(default_engine),
				uniform_distribution_y(default_engine),
				true,
				3, 0, AnimalState(AnimalState::MOVING));

			entities[i + planteating_count] = ent;

			last_id++;
		}

		for (int i = 0; i < food_count; i++)
		{
			Entity* ent = new Food(last_id, uniform_distribution_x(default_engine),
				uniform_distribution_y(default_engine),
				1, FoodState(FoodState::get_fresh()));

			entities[i + planteating_count + predator_count] = ent;

			last_id++;
		}

		for (int i = 0; i < plants_count; i++)
		{
			Entity* ent = new Plant(last_id, uniform_distribution_x(default_engine),
				uniform_distribution_y(default_engine),
				3, PlantState(PlantState::get_begin()));

			entities[i + planteating_count + predator_count + food_count] = ent;

			last_id++;
		}
	}

	// Валидация позиций
	bool is_position_valid(int x, int y)
	{
		return (x >= 1 && x <= x_size) &&
			(y >= 1 && y <= y_size);
	}
	bool is_position_valid(CoordinateSystem cs)
	{
		return (cs.get_x() >= 1 && cs.get_x() <= x_size) &&
			(cs.get_y() >= 1 && cs.get_y() <= y_size);
	}

	// Занята ли позиция?
	bool is_position_reserved(int x, int y)
	{
		for (int i = 0; i < entities.size(); i++)
		{
			if (entities[i]->get_x() == x && entities[i]->get_y() == y)
				return true;
		}

		return false;
	}

	// Размеры карты
	int get_xsize() { return x_size; }
	int get_ysize() { return y_size; }

	// Размеры карты, но для массивов
	int get_xindex() { return y_size; }
	int get_yindex() { return x_size; }

	std::vector<Entity*> get_entities() { return entities; }


	// Функции движения
	bool move_to(Entity& ent, int x, int y)
	{
		CoordinateSystem cs = CoordinateSystem(x, y);

		if (!is_position_valid(cs))
			return false;

		ent.set_x(cs.get_x());
		ent.set_y(cs.get_y());

		return true;
	}

	// Проверка на занятость, а затем передвижение
	bool move_to_valid(Entity& ent, int x, int y)
	{
		CoordinateSystem cs = CoordinateSystem(x, y);

		if (!is_position_valid(cs) || is_position_reserved(x, y))
			return false;

		ent.set_x(cs.get_x());
		ent.set_y(cs.get_y());

		return true;
	}

	bool move_up(Entity& ent) { return move_to_valid(ent, ent.get_x(), ent.get_y() - 1); }
	bool move_down(Entity& ent) { return move_to_valid(ent, ent.get_x(), ent.get_y() + 1); }
	bool move_right(Entity& ent) { return move_to_valid(ent, ent.get_x() + 1, ent.get_y()); }
	bool move_left(Entity& ent) { return move_to_valid(ent, ent.get_x() - 1, ent.get_y()); }

	bool delete_entity(Entity& entity)
	{ 
		std::vector<Entity*>::iterator to_delete = entities.begin();

		int i;
		for (i = 0; i < entities.size(); i++)
		{
			if (*to_delete[i] == entity)
				break;
		}

		entities.erase(to_delete);
	}

	//bool add_entity(Entity* entity)
	//{
	//	entities.push_back(entity);
	//}

	bool create_new_animal(int x, int y, bool predator)
	{
		std::default_random_engine default_engine;

		std::uniform_int_distribution<int> uniform_distribution_x(1, x_size);
		std::uniform_int_distribution<int> uniform_distribution_y(1, y_size);

		Entity* ent = new Animal(last_id, uniform_distribution_x(default_engine),
			uniform_distribution_y(default_engine),
			predator,
			3, 0, AnimalState(AnimalState::MOVING));

		entities.push_back(ent);

		last_id++;
	}

	int get_predator_count() { return predator_count; }
	int get_planteating_count() { return planteating_count; }
	int get_food_count() { return food_count; }
	int get_plant_count() { return plants_count; }
};

class Interaction
{
	Entity* first;
	Entity* second;

public:
	Interaction(Entity* first, Entity* second) : first(first), second(second) {}

	// Травоядный ест Еду
	bool eat_food()
	{
		if (!first->is_animal() || !second->is_food())
			return false;

		Animal* animal = dynamic_cast<Animal*>(first);

		animal->add_starve(-3);
		animal->add_health(10);
	}
	
	// Плотоядный ест Травоядного
	bool bite_animal()
	{
		if (!first->is_animal() || !second->is_animal())
			return false;

		Animal* first_animal = dynamic_cast<Animal*>(first);

		if (!first_animal->is_predator())
			return false;

		Animal* second_animal = dynamic_cast<Animal*>(second);

		first_animal->add_health(15);
		first_animal->add_starve(-4);

		second_animal->add_health(-20);
	}

	// Травоядный ест растение
	bool bite_plant()
	{
		if (!first->is_animal() || !second->is_plant())
			return false;

		Animal* animal = dynamic_cast<Animal*>(first);

		if (animal->is_predator())
			return false;

		animal->add_health(2);
		animal->add_starve(-1);

		second->add_health(-3);
	}

	// Репродукция Травоядный-Травоядный, Плотоядный-Плотоядный
	bool reproduction()
	{
		if (!first->is_animal() || !second->is_animal())
			return false;

		Animal* first_animal = dynamic_cast<Animal*>(first);
		Animal* second_animal = dynamic_cast<Animal*>(second);

		if (!first_animal->is_predator() && second_animal->is_predator() ||
			first_animal->is_predator() && !second_animal->is_predator())
			return false;

		int first_x = first_animal->get_x();
		int first_y = first_animal->get_y();

		int second_x = first_animal->get_x();
		int second_y = first_animal->get_y();

		int child_x = -1;
		int child_y = -1;

		if (first_x == second_x && first_y > second_y)
		{
			child_x = first_x + 1;
			child_y = first_y;
		}

		else if (first_x == second_x && first_y < second_y)
		{
			child_x = first_x - 1;
			child_y = first_y;
		}

		else if (first_x > second_x && first_y == second_y)
		{
			child_x = first_x;
			child_y = first_y + 1;
		}

		else
		{
			child_x = first_x;
			child_y = first_y - 1;
		}

	}

	bool is_in_danger()
	{
		Animal* first_animal = dynamic_cast<Animal*>(first);
		Animal* second_animal = dynamic_cast<Animal*>(second);

		return !first_animal->is_predator() && second_animal->is_predator() && distance_between() <= 2;
	}

	int distance_between()
	{
		return abs(first->get_x() - second->get_x()) + abs(first->get_y() - second->get_y());
	}
};

//class AIController
//{
//	Map& map;
//	Entity* agent;
//
//public:
//	AIController(Map& map, Entity* agent) : agent(agent) {}
//
//	Entity* get_agent() { return agent; }
//	Map get_map() { return map; }
//	void set_agent(Entity* entity) { agent = entity; }
//};
//
//class AIControllerPlantEating : public AIController
//{
//	enum BEHAVIOUR { EAT, REPRODUCTION, RUN_AWAY, IDLE };
//	BEHAVIOUR target_behavior;
//public:
//	AIControllerPlantEating(Animal* planteating) : AIController(planteating), target_behavior(IDLE) {}
//
//	void choose_target_behavior()
//	{
//		Animal* animal = dynamic_cast<Animal*>(get_agent());
//
//		int health = animal->get_health();
//		int old = animal->get_old();
//		int starve = animal->get_starve();
//
//		if (is_in_danger())
//			target_behavior = RUN_AWAY;
//
//		if (health >= Animal::AVERAGE_HEALTH &&
//			(old >= Animal::YOUNG_OLD && old < Animal::AVERAGE_OLD) &&
//			starve < Animal::LOW_STARVE)
//			target_behavior = REPRODUCTION;
//
//		else if (health < Animal::AVERAGE_HEALTH || starve >= Animal::LOW_STARVE)
//			target_behavior = EAT;
//
//
//	}
//
//	bool is_in_danger()
//	{
//		Animal* first_animal
//		Animal* second_animal
//
//		return !first_animal->is_predator() && second_animal->is_predator() && distance_between() <= 2;
//	}
//};

// MVC (TEST)

class Model
{
	//std::vector<Entity *> entities;
	Map map;

public:
	Model(int width, int height, int a_count, int f_count, int p_count) : 
		map(Map(height, width, a_count, f_count, p_count)) {}

	Map get_map() { return map; }

	std::vector<Entity*> get_entities() { return map.get_entities(); }

	void update()
	{
		std::vector<Entity*> entities = map.get_entities();

		for (int i = 0; i < entities.size(); i++)
		{
			entities[i]->update_state();
		}
	}


};

// Отрисовка поля и интерфейса, внешний вид поля и интерфейса
class View
{
	Model& model;
	std::string map_representation;
	char** positions;

public:
	View(Model& model) : model(model)
	{
		int x_dim = model.get_map().get_xindex();
		int y_dim = model.get_map().get_yindex();

		//std::cout << "test";
		//std::cout << x_dim << " " << y_dim << "\n";

		map_representation = "";

		positions = new char* [x_dim];

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

			CoordinateSystem cs = CoordinateSystem(entities[i]->get_x(), entities[i]->get_y());

			//std::cout << cs.get_x_index() << " " << cs.get_y_index() << "\n";

			positions[cs.get_x_index()][cs.get_y_index()] = symbol;
		}

		for (int i = 0; i < x_dim; i++)
		{
			for (int j = 0; j < y_dim; j++)
			{
				map_representation += std::string(1, positions[i][j]) + " ";
			}

			if (model.get_map().get_xsize() <= 80)
			{
				const std::string divider = "###################################";
				int divider_size = divider.size();

				if (i == 0)
					map_representation += "\t\t" +
					StringUtility::make_column_left_align(25, std::string("Plant-eating animals: ")) +
					std::to_string(model.get_map().get_planteating_count());

				else if (i == 1)
					map_representation += "\t\t" +
					StringUtility::make_column_left_align(25, std::string("Predator animals: ")) +
					std::to_string(model.get_map().get_predator_count());

				else if (i == 2)
					map_representation += "\t\t" +
					StringUtility::make_column_left_align(25, std::string("Food: ")) +
					std::to_string(model.get_map().get_food_count());

				else if (i == 3)
					map_representation += "\t\t" +
					StringUtility::make_column_left_align(25, std::string("Plants: ")) +
					std::to_string(model.get_map().get_plant_count());

				else if (i == 8)
					map_representation += "\t\t" + std::string(divider);

				else if (i == 9)
					map_representation += "\t\t" + StringUtility::make_column_center_align(divider_size, "INFO");

				else if (i == 10)
					map_representation += "\t\t" + StringUtility::make_column_center_align(divider_size, "USE FOLLOWING KEYS");

				else if (i == 12)
					map_representation += "\t\t" + StringUtility::make_column_left_align(divider_size, " ENTER - next step of simulation");

				else if (i == 13)
					map_representation += "\t\t" + StringUtility::make_column_left_align(divider_size, " ESC - terminate program");

				else if (i == 15)
					map_representation += "\t\t" + std::string(divider);

				else if (i == 17)
					map_representation += "\t\t" + StringUtility::make_column_center_align(divider_size, "NOTATION");

				else if (i == 18)
					map_representation += "\t\t" + StringUtility::make_column_left_align(divider_size, " @ - plant-eating animal");

				else if (i == 19)
					map_representation += "\t\t" + StringUtility::make_column_left_align(divider_size, " & - predator animal");

				else if (i == 20)
					map_representation += "\t\t" + StringUtility::make_column_left_align(divider_size, " * - food");

				else if (i == 21)
					map_representation += "\t\t" + StringUtility::make_column_left_align(divider_size, " X - plant");

				else if (i == 23)
					map_representation += "\t\t" + std::string(divider);
			}

			map_representation += "\n";
		}

		if (model.get_map().get_xsize() > 80)
		{
			const std::string divider = "###################################";
			int divider_size = divider.size();

			map_representation += "\n\n";

			map_representation += StringUtility::make_column_left_align(25, std::string("Plant-eating animals: ")) + 
				std::to_string(model.get_map().get_planteating_count()) + "\n";

			map_representation += StringUtility::make_column_left_align(25, std::string("Predator animals: ")) + 
				std::to_string(model.get_map().get_predator_count()) + "\n";

			map_representation += StringUtility::make_column_left_align(25, std::string("Food: ")) + 
				std::to_string(model.get_map().get_food_count()) + "\n";

			map_representation += StringUtility::make_column_left_align(25, std::string("Plants: ")) + 
				std::to_string(model.get_map().get_plant_count()) + "\n";

			map_representation += "\n\n\n\n\n";

			map_representation += " " + std::string(divider) + "\n\n";
			map_representation += " " + StringUtility::make_column_center_align(divider_size, "INFO") + "\n";
			map_representation += " " + StringUtility::make_column_center_align(divider_size, "USE FOLLOWING KEYS") + "\n\n\n";
			map_representation += " " + StringUtility::make_column_left_align(divider_size, " ENTER - next step of simulation") + "\n";
			map_representation += " " + StringUtility::make_column_left_align(divider_size, " ESC - terminate program") + "\n\n";
			map_representation += " " + std::string(divider) + "\n\n";
			map_representation += " " + StringUtility::make_column_center_align(divider_size, "NOTATION") + "\n";
			map_representation += " " + StringUtility::make_column_left_align(divider_size, " @ - plant-eating animal") + "\n";
			map_representation += " " + StringUtility::make_column_left_align(divider_size, " & - predator animal") + "\n";
			map_representation += " " + StringUtility::make_column_left_align(divider_size, " * - food") + "\n";
			map_representation += " " + StringUtility::make_column_left_align(divider_size, " X - plant") + "\n\n";
			map_representation += " " + std::string(divider) + "\n";
		}
	}

	~View()
	{
		for (int i = 0; i < model.get_map().get_xsize(); i ++)
			delete positions[i];

		delete positions;
	}

	void update()
	{
		int x_dim = model.get_map().get_xindex();
		int y_dim = model.get_map().get_yindex();

		map_representation = "";

		for (int i = 0; i < x_dim; i++)
			for (int j = 0; j < y_dim; j++)
				positions[i][j] = '-';

		std::vector<Entity*> entities = model.get_entities();

		for (int i = 0; i < entities.size(); i++)
		{
			char symbol = ' ';

			if (entities[i]->is_animal())
			{
				Animal* animal = dynamic_cast<Animal*>(entities[i]);

				if (animal->is_predator())
					symbol = '&';

				else
					symbol = '@';
			}

			else if (entities[i]->is_food())
				symbol = '*';

			else
				symbol = 'X';

			CoordinateSystem cs = CoordinateSystem(entities[i]->get_x(), entities[i]->get_y());

			positions[cs.get_x_index()][cs.get_y_index()] = symbol;
		}

		for (int i = 0; i < x_dim; i++)
		{
			for (int j = 0; j < y_dim; j++)
			{
				map_representation += std::string(1, positions[i][j]) + " ";
			}

			if (model.get_map().get_xsize() <= 80)
			{
				const std::string divider = "###################################";
				int divider_size = divider.size();

				if (i == 0)
					map_representation += "\t\t" +
					StringUtility::make_column_left_align(25, std::string("Plant-eating animals: ")) +
					std::to_string(model.get_map().get_planteating_count());

				else if (i == 1)
					map_representation += "\t\t" +
					StringUtility::make_column_left_align(25, std::string("Predator animals: ")) +
					std::to_string(model.get_map().get_predator_count());

				else if (i == 2)
					map_representation += "\t\t" +
					StringUtility::make_column_left_align(25, std::string("Food: ")) +
					std::to_string(model.get_map().get_food_count());

				else if (i == 3)
					map_representation += "\t\t" +
					StringUtility::make_column_left_align(25, std::string("Plants: ")) +
					std::to_string(model.get_map().get_plant_count());

				else if (i == 8)
					map_representation += "\t\t" + std::string(divider);

				else if (i == 9)
					map_representation += "\t\t" + StringUtility::make_column_center_align(divider_size, "INFO");

				else if (i == 10)
					map_representation += "\t\t" + StringUtility::make_column_center_align(divider_size, "USE FOLLOWING KEYS");

				else if (i == 12)
					map_representation += "\t\t" + StringUtility::make_column_left_align(divider_size, " ENTER - next step of simulation");

				else if (i == 13)
					map_representation += "\t\t" + StringUtility::make_column_left_align(divider_size, " ESC - terminate program");

				else if (i == 15)
					map_representation += "\t\t" + std::string(divider);

				else if (i == 17)
					map_representation += "\t\t" + StringUtility::make_column_center_align(divider_size, "NOTATION");

				else if (i == 18)
					map_representation += "\t\t" + StringUtility::make_column_left_align(divider_size, " @ - plant-eating animal");

				else if (i == 19)
					map_representation += "\t\t" + StringUtility::make_column_left_align(divider_size, " & - predator animal");

				else if (i == 20)
					map_representation += "\t\t" + StringUtility::make_column_left_align(divider_size, " * - food");

				else if (i == 21)
					map_representation += "\t\t" + StringUtility::make_column_left_align(divider_size, " X - plant");

				else if (i == 23)
					map_representation += "\t\t" + std::string(divider);
			}

			map_representation += "\n";
		}

		if (model.get_map().get_xsize() > 80)
		{
			const std::string divider = "###################################";
			int divider_size = divider.size();

			map_representation += "\n\n";

			map_representation += StringUtility::make_column_left_align(25, std::string("Plant-eating animals: ")) + std::to_string(model.get_map().get_planteating_count()) + "\n";
			map_representation += StringUtility::make_column_left_align(25, std::string("Predator animals: ")) + std::to_string(model.get_map().get_predator_count()) + "\n";
			map_representation += StringUtility::make_column_left_align(25, std::string("Food: ")) + std::to_string(model.get_map().get_food_count()) + "\n";
			map_representation += StringUtility::make_column_left_align(25, std::string("Plants: ")) + std::to_string(model.get_map().get_plant_count()) + "\n";

			map_representation += "\n\n\n\n\n";

			map_representation += " " + std::string(divider) + "\n\n";
			map_representation += " " + StringUtility::make_column_center_align(divider_size, "INFO") + "\n";
			map_representation += " " + StringUtility::make_column_center_align(divider_size, "USE FOLLOWING KEYS") + "\n\n\n";
			map_representation += " " + StringUtility::make_column_left_align(divider_size, " ENTER - next step of simulation") + "\n";
			map_representation += " " + StringUtility::make_column_left_align(divider_size, " ESC - terminate program") + "\n\n";
			map_representation += " " + std::string(divider) + "\n\n";
			map_representation += " " + StringUtility::make_column_center_align(divider_size, "NOTATION") + "\n";
			map_representation += " " + StringUtility::make_column_left_align(divider_size, " @ - plant-eating animal") + "\n";
			map_representation += " " + StringUtility::make_column_left_align(divider_size, " & - predator animal") + "\n";
			map_representation += " " + StringUtility::make_column_left_align(divider_size, " * - food") + "\n";
			map_representation += " " + StringUtility::make_column_left_align(divider_size, " X - plant") + "\n\n";
			map_representation += " " + std::string(divider) + "\n";
		}
	}

	void render() 
	{ 
		system("cls");
		std::cout << get_representation(); 
	}

	std::string get_representation()
	{
		return map_representation;
	}
};

class Control
{
	Model& model;
	View& view;

public:
	Control(Model& model, View& view) : model(model), view(view)
	{
		std::vector<Entity*> entities = model.get_entities();
		auto last_time = std::chrono::system_clock::now();

		while (true)
		{
			auto now = std::chrono::system_clock::now();

			if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count() < 400)
			{
				while (_kbhit())
					_getch();

				continue;
			}

			last_time = now;

			for (auto entity : entities)
			{
				// Change State...
				if (entity->is_animal())
				{
					Animal* animal = dynamic_cast<Animal*>(entity);

					if (animal->get_health() == 0 || animal->get_old() == Animal::MAX_OLD ||
						animal->get_starve() == Animal::MAX_STARVE)
						animal->set_state(AnimalState::DYING);
				}

				else if (entity->is_food())
				{
					Food* food = dynamic_cast<Food*>(entity);

					if (food->get_health() == 0 || food->get_old() == Food::MAX_OLD)
						food->set_state(FoodState::CORRUPTED);

					else if (food->get_old() > Food::AVERAGE_OLD)
						food->set_state(FoodState::CORRUPTING);
				}

				else
				{
					Plant* plant = dynamic_cast<Plant*>(entity);

					if (plant->get_health() == 0 || plant->get_old() == Plant::MAX_OLD)
						plant->set_state(PlantState::ENDING_LIFE);

					else if (plant->get_old() > Food::AVERAGE_OLD)
						plant->set_state(PlantState::MIDPART_LIFE);
				}

				// Action

				entity->update();

				if (!entity->is_animal())
					continue;

				int direction = rand() % 4;

				Map map = model.get_map();

				switch (direction)
				{
				case 0:
					map.move_up(*entity);
					break;

				case 1:
					map.move_down(*entity);
					break;

				case 2:
					map.move_right(*entity);
					break;

				case 3:
					map.move_left(*entity);
					break;
				}

				entity->update();
			}

			view.update();
			view.render();

			int key = Keyboard::handle_keyboard();

			if (key == Keyboard::ESC_KEY)
				exit(0);
		}
	}
};

int main()
{
	//while (true)
	//{
	//	if (_kbhit())
	//		std::cout << _getch() << "\n";
	//}

	Model model = Model(25, 25, 10, 5, 6); // Minimal size is 25x25

	View view = View(model);

	Control control = Control(model, view);

	return 0;
}
