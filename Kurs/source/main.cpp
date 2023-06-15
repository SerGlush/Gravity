#include <EvoNDZ/app/scene.h>
#include <EvoNDZ/app/application.h>
#include <EvoNDZ/math/math.h>
#include <EvoNDZ/input/input.h>
#include <EvoNDZ/util/timer.h>
#include <EvoNDZ/math/vector2.h>
#include <EvoNDZ/graphics/simple2d/renderer.h>
#include <imgui/imgui.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <execution>
	
int window_width = 1440;
int window_heigth = 768;
float border = 100.0f;
const float G = 0.01f;

struct body
{
	evo::Vector2f position;
	evo::Vector2f velocity = evo::Vector2f(0, 0);
	float mass;
	float r;
	// при создании тела обязательно указываем координаты и массу тела
	body(evo::Vector2f pos, float received_mass) { position = pos; mass = received_mass; r = sqrt(mass) * 0.01; }
};

class MyScene final : public evo::Scene {
public:

	bool pause = false;
	bool cam_mov_left = false;
	bool cam_mov_right = false;
	bool cam_mov_up = false;
	bool cam_mov_down = false;
	bool linedraw = false;

	std::vector<body> bodies;
	evo::Camera2D<float> camera;
	evo::Vector2f mousepos;
	int chosen_ind = -1;
	float creation_mass = 1;

	void initialize() override {

		// Инициализация нужных переменных
		camera.set_aspect_ratio(float(window_width) / float(window_heigth));
		evo::input::InputMap::Current = &inputMap;
		auto key = [this](evo::input::EventID id, evo::input::Key k, bool press, std::function<void()> f) 
		{
			inputMap.add_key_event(id);
			inputMap.key_event(id) += f;
			inputMap.bind(id, k, press);
		};	
		srand(frameTimer.time<unsigned int, std::nano>());

		// Обьявление функций кнопок

		// создание тела
		key(1, evo::input::Key::D, true, [this]()
			{
				double x, y;
				evo::input::mouse_position_normalized(x, y);
				bodies.emplace_back(camera.screen_to_world({float(x), float(y)}), creation_mass);
			});

		// генерация 1000 тел
		key(11, evo::input::Key::S, true, [this]() {
			for (int и = 0; и < 1000; и++)
			bodies.emplace_back(evo::Vector2f((rand() / (float)RAND_MAX) * border - (border / 2), (rand() / (float)RAND_MAX) * border - (border / 2)), creation_mass);
			});

		// остановка и продолжение времени
		key(2, evo::input::Key::Space, true, [this]() { pause = !pause; });

		// управление мышью
		key(12, evo::input::Key::MouseLeft, true, [this]()
			{
				linedraw = true;
				for (int a = 0; a < bodies.size(); a++) if ((bodies[a].position - mousepos).sqrlen() < bodies[a].r * bodies[a].r) chosen_ind = a;
			});

		key(14, evo::input::Key::MouseLeft, false, [this]()
			{
				linedraw = false;
				if (chosen_ind != -1)
				{
					evo::Vector2f velocitychg = mousepos - bodies[chosen_ind].position;
					if ((mousepos - bodies[chosen_ind].position).sqrlen() > bodies[chosen_ind].r * bodies[chosen_ind].r) bodies[chosen_ind].velocity += velocitychg;
				}
			});

		inputMap.scroll_event() += [this](double x, double y)
		{
			evo::Vector2f scl(pow(camera.scale().x + 0.01f, 1.0f + (float(y) / 20.0f)), pow(camera.scale().y + 0.01f, 1.0f + (float(y) / 20.0f)));
			camera.set_scale(scl);
		};

		// удаление тел
		key(13, evo::input::Key::F, true, [this]() { bodies.clear(); bodies.shrink_to_fit(); chosen_ind = -1; });

		// управление массы создаваемых тел
		key(15, evo::input::Key::T, true, [this]() { creation_mass *= 2; });
		key(16, evo::input::Key::Y, true, [this]() { if (creation_mass > 0.9f) creation_mass /= 2; });

		// управление камерой
		inputMap.simple_switch(3, 4, evo::input::Key::Left, [this]() {cam_mov_left = true; }, [this]() {cam_mov_left = false; });
		inputMap.simple_switch(5, 6, evo::input::Key::Right, [this]() {cam_mov_right = true; }, [this]() {cam_mov_right = false; });
		inputMap.simple_switch(7, 8, evo::input::Key::Up, [this]() {cam_mov_up = true; }, [this]() {cam_mov_up = false; });
		inputMap.simple_switch(9, 10, evo::input::Key::Down, [this]() {cam_mov_down = true; }, [this]() {cam_mov_down = false; });

		// обьявление переменной для отрисовки кадров
		batch = new evo::s2d::Renderer();

		frameTimer.reset();
	}
	void update() override {

		// получаем время и позицию мыши
		float dt = frameTimer.time<float>();
		frameTimer.reset();
		double xmpn, ympn;
		evo::input::mouse_position_normalized(xmpn, ympn);
		mousepos = camera.screen_to_world({ float(xmpn), float(ympn) });

		if (pause)
		{
			// проверка столкновений
			for (int a = 0; a < bodies.size(); a++) for (int b = 0; b < bodies.size(); b++)
				if (a != b) if ((bodies[a].position - bodies[b].position).sqrlen() < evo::math::sqr(bodies[a].r + bodies[b].r))
				{
					bool swapwas = false;
					if (bodies[a].mass < bodies[b].mass) { std::swap(a, b); swapwas = true; }
					bodies[a].velocity = bodies[a].velocity / ((bodies[a].mass + bodies[b].mass) / bodies[a].mass);
					bodies[a].mass += bodies[b].mass;
					bodies[a].r = sqrt(bodies[a].mass) * 0.01;
					bodies.erase(bodies.begin() + b);
					if (a >= b) a--;
					if (chosen_ind == b) chosen_ind = a;
					else if (chosen_ind > b) chosen_ind--;
					b--;
					if (swapwas) std::swap(a, b);
				}

			// просчет сил притяжения и перемещений тел
			std::for_each(std::execution::par_unseq, bodies.begin(), bodies.end(), [this, dt](body& a)
				{
					for (int b = &a - bodies.data() + 1; b < bodies.size(); b++)
					{
						evo::Vector2f gravity = (0.01f + (bodies[b].position - a.position).normalized()) * G / (bodies[b].position - a.position).sqrlen();
						a.velocity += gravity * (dt * bodies[b].mass);
						bodies[b].velocity -= gravity * (dt * a.mass);
					}
				});
			for (body& a : bodies) a.position += a.velocity * dt;

			// проверка границы
			for (body& a : bodies) if (a.position.x >= border || a.position.x <= -border || a.position.y >= border || a.position.y <= -border)
			{
				a.position += -a.velocity * dt;
				a.velocity = -a.velocity / 10.0f;
			}
		}

		// перемещение камеры
		if (cam_mov_left) camera.move_on(evo::Vector2f::X(-dt * camera.scale().x));
		if (cam_mov_right) camera.move_on(evo::Vector2f::X(dt * camera.scale().x));
		if (cam_mov_up) camera.move_on(evo::Vector2f::Y(dt * camera.scale().y));
		if (cam_mov_down) camera.move_on(evo::Vector2f::Y(-dt * camera.scale().y));
	}
	void render() override {

		// рисуем линию вектора скорости полученую мышкой
		if (linedraw && chosen_ind != -1) batch->line(mousepos, bodies[chosen_ind].position, bodies[chosen_ind].r / 2);
		
		// рисуем все тела и отдельно выделеный
		for (const body& a : bodies) batch->circle(a.position, a.r);
		if (chosen_ind != -1)
		{
			batch->circle(bodies[chosen_ind].position, bodies[chosen_ind].r, evo::Color3f(1.0f, 0.0f, 0.0f));
			batch->line(bodies[chosen_ind].position, bodies[chosen_ind].velocity + bodies[chosen_ind].position, bodies[chosen_ind].r / 2);
		}

		// рисуем границу
		batch->line(evo::Vector2f(-border, border), evo::Vector2f(border, border), 0.05f);
		batch->line(evo::Vector2f(border, border), evo::Vector2f(border, -border), 0.05f);
		batch->line(evo::Vector2f(-border, border), evo::Vector2f(-border, -border), 0.05f);
		batch->line(evo::Vector2f(-border, -border), evo::Vector2f(border, -border), 0.05f);

		batch->render(camera);
	}
	 
	void gui() override { 

		// gui
		ImGui::Begin("Info");
		//ImGui::Text("%.1f", 1 / frameTimer.time<double>());
		ImGui::Text("Amount of bodies: %i", bodies.size());
		if (chosen_ind != -1) ImGui::Text("Chosen body mass: %.f", bodies[chosen_ind].mass);
		ImGui::Text("Mass of a spawned body: %.f", creation_mass);
		ImGui::End();
	}
	void terminate() override {

		// terminate
		delete batch;
	}

private:
	evo::s2d::Renderer* batch = nullptr;
	evo::Timer frameTimer;
	evo::input::InputMap inputMap;
};

int main() {
	evo::app::run(window_width, window_heigth, "Kurs", std::make_unique<MyScene>());
	return 0;
}