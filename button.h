#ifndef BUTTON_H
#define BUTTON_H

#include <graphics.h>
#include <vector>
#include <functional>

#pragma comment(lib, "MSIMG32.LIB")

namespace BUTTON {
	typedef std::function<void()> T;

	void putimageAlpha(int x, int y, IMAGE* img) {
		int w = img->getwidth();
		int h = img->getheight();
		AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
	}

	class button {
	public:
		button(T F, int x, int y, IMAGE* IDLE, IMAGE* HOVER, IMAGE* ACTIVE) {
			f = F;
			w = IDLE->getwidth();
			h = IDLE->getheight();
			actived = false;
			hovered = false;
			clicked = false;
			idle = IDLE;
			hover = HOVER;
			active = ACTIVE;

			xm = x - w / 2;
			ym = y - h / 2;
			xM = x + w / 2;
			yM = y + h / 2;
		}

		button() : f([]()->void {}), xm(0), ym(0), xM(0), yM(0), w(0), h(0), actived(false), hovered(false), clicked(false), idle(nullptr), hover(nullptr), active(nullptr) {}

		void enable() {
			actived = true;
		}

		void disable() {
			actived = false;
		}

		void draw() {
			if (actived) {
				if (clicked) {
					BUTTON::putimageAlpha(xm, ym, active);
				}
				else if (hovered) {
					BUTTON::putimageAlpha(xm, ym, hover);
				}
				else {
					BUTTON::putimageAlpha(xm, ym, idle);
				}
			}
		}

		void move(int x, int y) {
			if (x >= xm && x <= xM && y >= ym && y <= yM) {
				hovered = true;
			}
			else {
				hovered = false;
			}
		}

		void click(int x, int y) {
			if (actived) {
				if (x >= xm && x <= xM && y >= ym && y <= yM) {
					clicked = true;
				}
				else {
					clicked = false;
				}
			}
		}

		void release() {
			if (actived && clicked) {
				f();
				clicked = false;
			}
		}

	private:
		T f;
		int xm, ym, xM, yM;
		int w, h;
		bool actived, hovered, clicked;
		IMAGE* idle;
		IMAGE* hover;
		IMAGE* active;
	};

	void move(int x, int y, const std::vector<button*>& buttons) {
		for (button* b : buttons) {
			b->move(x, y);
		}
	}

	void move(int x, int y, const std::vector<button>& buttons) {
		for (button b : buttons) {
			b.move(x, y);
		}
	}

	void draw(const std::vector<button*>& buttons) {
		for (button* b : buttons) {
			b->draw();
		}
	}

	void draw(const std::vector<button>& buttons) {
		for (button b : buttons) {
			b.draw();
		}
	}

	void click(int x, int y, const std::vector<button*>& buttons) {
		for (button* b : buttons) {
			b->click(x, y);
		}
	}

	void click(int x, int y, const std::vector<button>& buttons) {
		for (button b : buttons) {
			b.click(x, y);
		}
	}

	void release(const std::vector<button*>& buttons) {
		for (button* b : buttons) {
			b->release();
		}
	}

	void release(const std::vector<button>& buttons) {
		for (button b : buttons) {
			b.release();
		}
	}

	void enable(const std::vector<button*>& buttons) {
		for (button* b : buttons) {
			b->enable();
		}
	}

	void enable(const std::vector<button>& buttons) {
		for (button b : buttons) {
			b.enable();
		}
	}

	void disable(const std::vector<button*>& buttons) {
		for (button* b : buttons) {
			b->disable();
		}
	}

	void disable(const std::vector<button>& buttons) {
		for (button b : buttons) {
			b.disable();
		}
	}
}

#endif