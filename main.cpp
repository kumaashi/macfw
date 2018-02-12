#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

using namespace std;
struct Graphics {
	Graphics() {
	}
	~Graphics() { } 
	struct Data {
		enum {
			TYPE_NONE = 0,
			TYPE_VIEW = 0,
			TYPE_VERTEX = 0,
			TYPE_SHADER = 0,
		};
		int type = TYPE_NONE;
		string name;
		void SetName(string n) { name = n; }
		string GetName() { return name; }
		void SetType(int n) { type = n; }
		int GetType() { return type; }
	};
	struct View : public Data {
		int width, height;
		float color[4];
		View(string name, int w, int h) {
			SetName(name);
			SetType(Data::TYPE_VIEW);
			width = w;
			height = h;
		}
		~View() { }
		void SetClearColor(float r, float g, float b, float a) {
			color[0] = r;
			color[1] = g;
			color[2] = b;
			color[3] = a;
		}
		void GetClearColor(float *buf) {
			buf[0] = color[0];
			buf[1] = color[1];
			buf[2] = color[2];
			buf[3] = color[3];
		}
		int GetWidth() { return width; };
		int GetHeight() { return height; };
	};
	map<string, Data *> vMapData;

	View *CreateView(string name, int w, int h) {
		auto v = new View(name, w, h);
		vMapData[name] = v;
		return v;
	}

	void Update() {
	}

	void SubmitView(string name) {
		auto view = static_cast<View *>(vMapData[name]);
		if(view) {
			float color[4];
			view->GetClearColor(color);
			glViewport(0, 0, view->GetWidth(), view->GetHeight());
			glClearColor( color[0], color[1], color[2], color[3]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}
};

int main(int argc, char *argv[]) {
	enum {
		Width = 1280,
		Height = 720,
	};
	SDL_Init(SDL_INIT_VIDEO );
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,  8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1 );
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4 );
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1 );
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1 );
	SDL_SetVideoMode(Width, Height, 32, SDL_OPENGL);
	SDL_Event event;
	Graphics graphics;
	auto view0 = graphics.CreateView("main", Width, Height);
	view0->SetClearColor(0, 0, 1, 1);
	bool isDone = false;
	while(!isDone) {
		graphics.SubmitView(view0->GetName());
		SDL_GL_SwapBuffers();
		while(SDL_PollEvent(&event)) {
			if(event.type & SDL_KEYDOWN) {
				isDone = true;
			}
		}
	}

	SDL_Quit();
	return 0;
}


