#include <graphics.h>
#include <string>
#include <random>
#include <algorithm>
#include <set>
#include <cmath>
#include <vector>
#include <iostream>
#include <irrKlang.h>

#pragma comment(lib, "MSIMG32.LIB")
#pragma comment(lib, "Winmm.lib")

void putimageAlpha(int x, int y, IMAGE* img, unsigned char alpha = 255) {
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA });
}

double PI = std::acos(-1);

namespace MUSIC {
	irrklang::ISoundEngine* engine;
	irrklang::ISoundSource* bgm_s;
	irrklang::ISound* bgm;
	irrklang::ISoundSource* menu_s;
	irrklang::ISound* menu;
	irrklang::ISoundSource* hit_s;
	irrklang::ISound* hit;
	irrklang::ISoundSource* lose_s;
	irrklang::ISound* lose;
	irrklang::ISoundSource* levelup_s;
	irrklang::ISound* levelup;

	void load() {
		engine = irrklang::createIrrKlangDevice();
	}

	void end() {
		engine->drop();
	}
}

namespace BACKGROUND {
	int width, height;
	bool paused = false;
	const std::string score_s = "SCORE: ";
	int frame = 0;
	int score = 0;
	IMAGE bk;
	HWND hwnd;

	void load() {
		loadimage(&bk, _T("img/background.png"));
		width = bk.getwidth();
		height = bk.getheight();

		hwnd = initgraph(width, height);
		BeginBatchDraw();
		SetWindowPos(hwnd, NULL, 15, 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

		setbkmode(TRANSPARENT);
		setbkcolor(WHITE);

		MUSIC::bgm_s = MUSIC::engine->addSoundSourceFromFile("msc/bgm.mp3");
	}

	void initialize() {
		frame = 0;
		score = 0;
		paused = false;

		MUSIC::menu->stop();
		MUSIC::bgm = MUSIC::engine->play2D(MUSIC::bgm_s, true, false, true);
	}

	void update(int F) {
		frame++;
	}

	void draw(int frame) {
		putimageAlpha(0, 0, &bk);
		std::string text = score_s + std::to_string(score);
		std::wstring text_o(text.begin(), text.end());
		settextcolor(RGB(255, 165, 0));
		outtextxy(6, 6, &text_o[0]);
	}

	void peek(ExMessage* msg) {
		if (msg->message == WM_KEYDOWN) {
			if (msg->vkcode == 0x20) { // SPACE
				paused = true;
			}
		}
		if (msg->message == WM_KEYUP) {
			if (msg->vkcode == 0x20) { // SPACE
				paused = false;
			}
		}
	}

	void end() {
		EndBatchDraw();
		closegraph();
	}
}

namespace CHARACTER {
	const int flash = 4;
	const int number = 6;
	IMAGE image_left[number];
	IMAGE image_right[number];
	IMAGE shade;
	const int period = flash * number;
	bool left = true;
	int width, height;
	int shade_width, shade_height;
	int x, y, x0;
	int xM, yM;
	bool LBD = false, RBD = false;
	int speed = 5;
	bool m_left = false, m_right = false, m_up = false, m_down = false;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis_w, dis_h;

	void load() {
		std::wstring path;
		for (int i = 0; i < number; i++) {
			path = L"img/player_left_" + std::to_wstring(i) + L".png";
			loadimage(&image_left[i], path.c_str());
			path = L"img/player_right_" + std::to_wstring(i) + L".png";
			loadimage(&image_right[i], path.c_str());
		}
		width = image_left[0].getwidth();
		height = image_left[0].getheight();

		path = L"img/shadow_player.png";
		loadimage(&shade, path.c_str());
		shade_width = shade.getwidth();
		shade_height = shade.getheight();

		xM = BACKGROUND::width - width;
		yM = BACKGROUND::height - height;
		dis_w = std::uniform_int_distribution<int>(1, xM - 1);
		dis_h = std::uniform_int_distribution<int>(1, yM - 1);
	}

	void initialize() {
		x = dis_w(gen);
		y = dis_h(gen);
		LBD = false;
		RBD = false;
		m_left = false;
		m_right = false;
		m_up = false;
		m_down = false;
		int speed = 5;
	}

	void draw(int frame) {
		putimageAlpha(x + width / 2 - shade_width / 2, y + height - shade_height / 2, &shade);

		frame %= period;
		frame /= flash;
		if (left) {
			putimageAlpha(x, y, &image_left[frame]);
		}
		else {
			putimageAlpha(x, y, &image_right[frame]);
		}
	}

	void move() {

		x0 = x;

		int speed;
		if (!(LBD ^ RBD)) {
			speed = CHARACTER::speed;
		}
		else if (LBD) {
			speed = CHARACTER::speed * 2;
		}
		else {
			speed = CHARACTER::speed / 2;
		}
		if ((m_left ^ m_right) && (m_up ^ m_down)) {
			speed = static_cast<int>(std::round(speed * 0.7071));
		}

		if (m_left) { x -= speed; }
		if (m_right) { x += speed; }
		if (m_up) { y -= speed; }
		if (m_down) { y += speed; }

		x = std::clamp(x, 0, xM);
		y = std::clamp(y, 0, yM);

		if (x > x0) { left = false; }
		else if (x0 > x) { left = true; }
	}

	void update(int frame) {
		move();
	}

	void peek(ExMessage* msg) {
		if (msg->message == WM_KEYDOWN) {
			if (msg->vkcode == 0x57) { // w
				m_up = true;
			}
			if (msg->vkcode == 0x53) { // s
				m_down = true;
			}
			if (msg->vkcode == 0x41) { // a
				m_left = true;
			}
			if (msg->vkcode == 0x44) { // d
				m_right = true;
			}
		}

		if (msg->message == WM_KEYUP) {
			if (msg->vkcode == 0x57) { // w
				m_up = false;
			}
			if (msg->vkcode == 0x53) { // s
				m_down = false;
			}
			if (msg->vkcode == 0x41) { // a
				m_left = false;
			}
			if (msg->vkcode == 0x44) { // d
				m_right = false;
			}
		}

		if (msg->message == WM_LBUTTONDOWN) {
			LBD = true;
		}
		if (msg->message == WM_LBUTTONUP) {
			LBD = false;
		}
		if (msg->message == WM_RBUTTONDOWN) {
			RBD = true;
		}
		if (msg->message == WM_RBUTTONUP) {
			RBD = false;
		}
	}
}

namespace BULLET {
	int n;
	int size;
	const float sizeShow = 10.0f;
	const float radius = 92.0f;
	float speed = 0.06f;
	const float period = static_cast<float>(2 * PI);
	int x0, y0;

	class bullet {
	public:
		bullet(float a) {
			angle = a;
			updateXY();
		}

		void draw(int frame) {
			solidcircle(x, y, static_cast<int>(size + sizeShow));
		}

		void update(int frame) {
			angle += speed;
			updateXY();
		}

		int getX() { return x; }
		int getY() { return y; }

	private:
		float angle;
		int x, y;

		void updateXY() {
			x = x0 + static_cast<int>(radius * std::cos(angle));
			y = y0 - static_cast<int>(radius * std::sin(angle));
		}
	};

	std::vector<bullet*> bullets;

	void load() {
		setfillcolor(RED);
	}

	void generate(int number) {
		for (bullet* i : bullets) {
			delete i;
		}
		bullets.clear();
		n = number;
		for (int i = 0; i < n; i++) {
			float a = static_cast<float>(i) * period / n;
			bullets.push_back(new bullet(a));
		}
	}

	void initialize() {
		x0 = CHARACTER::x + CHARACTER::width / 2;
		y0 = CHARACTER::y + CHARACTER::height / 2;
		size = 0;
		speed = 0.06f;
		generate(3);
	}

	void draw(int frame) {
		for (bullet* i : bullets) {
			i->draw(frame);
		}
	}

	void update(int frame) {
		x0 = CHARACTER::x + CHARACTER::width / 2;
		y0 = CHARACTER::y + CHARACTER::height / 2;
		for (bullet* i : bullets) {
			i->update(frame);
		}
	}

	void peek(ExMessage* msg) {

	}

	void end() {
		for (bullet* i : bullets) {
			delete i;
		}
		bullets.clear();
	}
}

namespace COVER { bool running = false; bool lost = true; void lose(int wait = 0, bool music = false); void level_up(int score); }

namespace ENEMY {
	int count = 0;
	const int flash = 4;
	const int number = 6;
	IMAGE image_left[number];
	IMAGE image_right[number];
	IMAGE shade;
	const int period = flash * number;
	int width, height;
	int shade_width, shade_height;
	int speed = 4;
	int difficulty = 60;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> edge, rand_x, rand_y, rand_p;

	class enemy {
	public:
		enemy(int frame) {
			start = frame;
			int whichEdge = edge(gen);
			switch (whichEdge) {
			case 0:
				x = -width;
				y = rand_y(gen);
				break;
			case 1:
				x = BACKGROUND::width;
				y = rand_y(gen);
				break;
			case 2:
				x = rand_x(gen);
				y = -height;
				break;
			case 3:
				x = rand_x(gen);
				y = BACKGROUND::height;
				break;
			}
			left = true;
		}

		void draw(int frame) {
			putimageAlpha(x + width / 2 - shade_width / 2, y + height - shade_height, &shade);

			frame -= start;
			frame %= period;
			frame /= flash;
			if (left) {
				putimageAlpha(x, y, &image_left[frame]);
			}
			else {
				putimageAlpha(x, y, &image_right[frame]);
			}
		}

		void move() {
			float dx = static_cast<float>(CHARACTER::x - x);
			float dy = static_cast<float>(CHARACTER::y - y);
			float distance = std::sqrt(dx * dx + dy * dy);
			if (dx > 0) {
				left = false;
			}
			else if (dx < 0) {
				left = true;
			}
			x += static_cast<int>(std::round(dx / distance * speed));
			y += static_cast<int>(std::round(dy / distance * speed));
		}

		int getX() { return x; }
		int getY() { return y; }

	private:
		int x, y;
		bool left;
		int start;
	};

	void load() {
		std::wstring path;
		for (int i = 0; i < number; i++) {
			path = L"img/enemy_left_" + std::to_wstring(i) + L".png";
			loadimage(&image_left[i], path.c_str());
			path = L"img/enemy_right_" + std::to_wstring(i) + L".png";
			loadimage(&image_right[i], path.c_str());
		}
		width = image_left[0].getwidth();
		height = image_left[0].getheight();

		path = L"img/shadow_enemy.png";
		loadimage(&shade, path.c_str());
		shade_width = shade.getwidth();
		shade_height = shade.getheight();

		edge = std::uniform_int_distribution<int>(0, 3);
		rand_x = std::uniform_int_distribution<int>(0, BACKGROUND::width);
		rand_y = std::uniform_int_distribution<int>(0, BACKGROUND::height);

		rand_p = std::uniform_int_distribution<int>(0, 2);

		MUSIC::hit_s = MUSIC::engine->addSoundSourceFromFile("msc/hit.mp3");
	}

	std::set<enemy*> enemies;

	void initialize() {
		for (auto e = enemies.begin(); e != enemies.end(); e++) {
			delete* e;
		}
		enemies.clear();
		difficulty = 60;
		count = difficulty / 2;
		speed = 4;
	}

	void update(int frame) {
		if (count == 0) {
			enemies.insert(new enemy(frame));
			if (BACKGROUND::score >= 50) {
				for (int i = 50; i <= min(1000, BACKGROUND::score); i += 50) {
					if (rand_p(gen) == 0) {
						enemies.insert(new enemy(frame));
					}
				}
			}
		}
		for (auto e = enemies.begin(); e != enemies.end();) {
			(*e)->move();
			bool erased = false;
			int size_b = BULLET::size;
			for (BULLET::bullet* b : BULLET::bullets) { // hit by a bullet
				int x_b = b->getX(), y_b = b->getY();
				if ((x_b + size_b > (*e)->getX()) &&
					(x_b - size_b < ((*e)->getX() + width)) &&
					(y_b + size_b > (*e)->getY()) &&
					(y_b - size_b < ((*e)->getY() + height))) {
					erased = true;
					break;
				}
			}
			if (erased) {
				delete* e;
				e = enemies.erase(e);
				BACKGROUND::score++;
				if (BACKGROUND::score % 5 == 0 && difficulty > 10) { // increase frequency
					difficulty--;
				}
				if (BACKGROUND::score % 24 == 0 && speed < 15) { // increase speed
					speed++;
				}
				COVER::level_up(BACKGROUND::score);
			}
			else { // hit character
				bool lost = false;
				int x_c = CHARACTER::x + CHARACTER::width / 2, y_c = CHARACTER::y + CHARACTER::height / 2;
				if ((x_c > (*e)->getX()) &&
					(x_c < ((*e)->getX() + width)) &&
					(y_c > (*e)->getY()) &&
					(y_c < ((*e)->getY() + height))) {
					lost = true;
				}
				if (lost) {
					COVER::lose(1000, true);
					break;
				}
				else {
					e++;
				}
			}
		}
		count = (++count) % difficulty;
	}

	void draw(int frame) {
		for (enemy* i : enemies) {
			i->draw(frame);
		}
	}

	void peek(ExMessage* msg) {

	}

	void end() {
		for (enemy* e : enemies) {
			delete e;
		}
		enemies.clear();
	}
}

void initialize();

namespace COVER {
	IMAGE cover;
	int frame_display = 0;
	std::wstring text_o;
	const int frame_wait = 40;

	unsigned char alpha(int frame) {
		if (frame <= 120) {
			double f = static_cast<double>(frame) / 120.0;
			return static_cast<unsigned char>(255.0 * f * f * f / 5.0 * 3.0);
		}
		else {
			double f = static_cast<double>(300 - frame) / 180.0;
			return static_cast<unsigned char>(255.0 * (1.0 - f * f * f / 5.0 * 2.0));
		}
	}

	void enter() {
		lost = true;
		running = false;
		text_o = L"";

		frame_display = frame_wait;

		MUSIC::menu = MUSIC::engine->play2D(MUSIC::menu_s, true, false, true);

		int frame = 0;
		while (frame < 300) {
			DWORD start = GetTickCount();
			cleardevice();
			putimageAlpha(0, 0, &cover, alpha(frame));
			FlushBatchDraw();
			frame++;
			DWORD delay = GetTickCount() - start;
			if (delay < 16) {
				Sleep(16 - delay);
			}
		}
	}

	void load() {
		loadimage(&cover, _T("img/cover.png"));

		MUSIC::menu_s = MUSIC::engine->addSoundSourceFromFile("msc/menu.mp3");
		MUSIC::lose_s = MUSIC::engine->addSoundSourceFromFile("msc/lose.mp3");
		MUSIC::levelup_s = MUSIC::engine->addSoundSourceFromFile("msc/levelup.mp3");

		enter();
	}

	void initialize() {

	}

	void update(int frame) {
		if (frame_display < frame_wait) {
			frame_display++;
			if (frame_display == frame_wait) {
				text_o = L"";
			}
		}
	}

	void draw(int frame) {
		if (lost) {
			putimageAlpha(0, 0, &cover);
		}
		else {
			if (text_o.size() > 0) {
				settextcolor(BLUE);
				int x_c = CHARACTER::x + CHARACTER::width / 2, y_c = CHARACTER::y + CHARACTER::height / 2;
				int x, y;
				if (x_c > BACKGROUND::width / 2 && y_c > BACKGROUND::height / 2) {
					x = x_c - CHARACTER::width / 2 - 90;
					y = y_c - CHARACTER::height / 2 - 30;
				}
				else if (x_c < BACKGROUND::width / 2 && y_c > BACKGROUND::height / 2) {
					x = x_c - CHARACTER::width / 2;
					y = y_c - CHARACTER::height / 2 - 30;
				}
				else if (x_c > BACKGROUND::width / 2 && y_c < BACKGROUND::height / 2) {
					x = x_c - CHARACTER::width / 2 - 90;
					y = y_c + CHARACTER::height / 2 + 5;
				}
				else {
					x = x_c - CHARACTER::width / 2;
					y = y_c + CHARACTER::height / 2 + 5;
				}
				outtextxy(x, y, &text_o[0]);
			}
		}
	}

	void peek(ExMessage* msg) {
		if (lost) {
			if (msg->message == WM_LBUTTONDOWN) {
				lost = false;
				running = true;
				frame_display = 0;
				::initialize();
			}
		}
		else {
			if (msg->message == WM_KEYDOWN) {
				if (msg->vkcode == 0x1B) { // ESC
					lose();
				}
			}
		}
	}

	void lose(int wait, bool music) {
		lost = true;
		running = false;
		if (music) {
			MUSIC::lose = MUSIC::engine->play2D(MUSIC::lose_s, false, false, true);
		}
		Sleep(wait);
		frame_display = frame_wait;
		MUSIC::bgm->stop();
		MUSIC::menu = MUSIC::engine->play2D(MUSIC::menu_s, true, false, true);
		text_o = L"";
	}

	void level_up(int score) {
		switch (score) {
		case 10:
			text_o = L"LEVEL UP! Bullet speed ++";
			frame_display = 0;
			BULLET::speed += 0.01f;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 20:
			text_o = L"LEVEL UP! Bullet size ++";
			frame_display = 0;
			BULLET::size++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 30:
			text_o = L"LEVEL UP! Move speed ++";
			frame_display = 0;
			CHARACTER::speed++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 40:
			text_o = L"LEVEL UP! Bullet size ++";
			frame_display = 0;
			BULLET::size++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 50:
			text_o = L"LEVEL UP! Bullet amount ++";
			frame_display = 0;
			BULLET::generate(4);
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 60:
			text_o = L"LEVEL UP! Bullet speed ++";
			frame_display = 0;
			BULLET::speed += 0.01f;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 70:
			text_o = L"LEVEL UP! Bullet size ++";
			frame_display = 0;
			BULLET::size++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 80:
			text_o = L"LEVEL UP! Bullet speed ++";
			frame_display = 0;
			BULLET::speed += 0.01f;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 90:
			text_o = L"LEVEL UP! Move speed ++";
			frame_display = 0;
			CHARACTER::speed++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 100:
			text_o = L"LEVEL UP! Bullet size ++";
			frame_display = 0;
			BULLET::size++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 115:
			text_o = L"LEVEL UP! Bullet speed ++";
			frame_display = 0;
			BULLET::speed += 0.01f;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 130:
			text_o = L"LEVEL UP! Move speed ++";
			frame_display = 0;
			CHARACTER::speed++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 145:
			text_o = L"LEVEL UP! Bullet amount ++";
			frame_display = 0;
			BULLET::generate(5);
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 160:
			text_o = L"LEVEL UP! Bullet speed ++";
			frame_display = 0;
			BULLET::speed += 0.01f;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 175:
			text_o = L"LEVEL UP! Bullet size ++";
			frame_display = 0;
			BULLET::size++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 190:
			text_o = L"LEVEL UP! Bullet speed ++";
			frame_display = 0;
			BULLET::speed += 0.01f;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 210:
			text_o = L"LEVEL UP! Move speed ++";
			frame_display = 0;
			CHARACTER::speed++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 230:
			text_o = L"LEVEL UP! Bullet size ++";
			frame_display = 0;
			BULLET::size++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 250:
			text_o = L"LEVEL UP! Bullet speed ++";
			frame_display = 0;
			BULLET::speed += 0.01f;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 270:
			text_o = L"LEVEL UP! Bullet size ++";
			frame_display = 0;
			BULLET::size++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 300:
			text_o = L"LEVEL UP! Move speed ++";
			frame_display = 0;
			CHARACTER::speed++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 330:
			text_o = L"LEVEL UP! Bullet amount ++";
			frame_display = 0;
			BULLET::generate(6);
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 360:
			text_o = L"LEVEL UP! Bullet size ++";
			frame_display = 0;
			BULLET::size++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 400:
			text_o = L"LEVEL UP! Bullet speed ++";
			frame_display = 0;
			BULLET::speed += 0.01f;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 450:
			text_o = L"LEVEL UP! Move speed ++";
			frame_display = 0;
			CHARACTER::speed++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 500:
			text_o = L"LEVEL UP! Bullet speed ++";
			frame_display = 0;
			BULLET::speed += 0.01f;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 570:
			text_o = L"LEVEL UP! Bullet size ++";
			frame_display = 0;
			BULLET::size++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 650:
			text_o = L"LEVEL UP! Bullet speed ++";
			frame_display = 0;
			BULLET::speed += 0.01f;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		case 750:
			text_o = L"LEVEL UP! Move speed ++";
			frame_display = 0;
			CHARACTER::speed++;
			MUSIC::levelup = MUSIC::engine->play2D(MUSIC::levelup_s, false, false, true);
			break;
		default:
			MUSIC::hit = MUSIC::engine->play2D(MUSIC::hit_s, false, false, true);
			break;
		}
	}
}

void load() {
	MUSIC::load();
	BACKGROUND::load();
	CHARACTER::load();
	BULLET::load();
	ENEMY::load();
	COVER::load();
}

void initialize() {
	BACKGROUND::initialize();
	CHARACTER::initialize();
	BULLET::initialize();
	ENEMY::initialize();
	COVER::initialize();
}

void update(int frame) {
	BACKGROUND::update(frame);
	CHARACTER::update(frame);
	BULLET::update(frame);
	ENEMY::update(frame);
	COVER::update(frame);
}

void draw(int frame) {
	cleardevice();

	BACKGROUND::draw(frame);
	CHARACTER::draw(frame);
	BULLET::draw(frame);
	ENEMY::draw(frame);
	COVER::draw(frame);

	FlushBatchDraw();
}

void peek(ExMessage* msg) {
	BACKGROUND::peek(msg);
	CHARACTER::peek(msg);
	BULLET::peek(msg);
	ENEMY::peek(msg);
	COVER::peek(msg);
}

void end() {
	MUSIC::end();
	BACKGROUND::end();
	BULLET::end();
	ENEMY::end();
}

int main() {

	DWORD start, delta_t;

	load();

	BeginBatchDraw();

	ExMessage msg;

	while (IsWindow(BACKGROUND::hwnd)) {

		start = GetTickCount();

		while (peekmessage(&msg)) {
			peek(&msg);
		}

		if (!BACKGROUND::paused && COVER::running) {
			update(BACKGROUND::frame);
		}

		draw(BACKGROUND::frame);

		delta_t = GetTickCount() - start;

		if (delta_t < 24) {
			Sleep(24 - delta_t);
		}
	}

	end();

	return 0;
}