#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

using namespace std; //wahaha

struct Graphics {
	struct Data {
		enum {
			TYPE_NONE = 0,
			TYPE_VIEW,
			TYPE_VERTEX,
			TYPE_SHADER,
		};
		int type = TYPE_NONE;
		string name;
		string GetName() { return name; }
		int GetType() { return type; }
		void SetName(string n) { name = n; }
		void SetType(int n) { type = n; }
	};
	map<string, Data *> vMapData;

	struct View : public Data {
		int width, height;
		float color[4];
		View(string name, int w, int h) {
			SetName(name);
			SetType(Data::TYPE_VIEW);
			width = w;
			height = h;
		}
		~View() {
			printf("%s", __func__);
		}
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

	struct Vertex : public Data {
		vector<uint8_t> vbuf;
		Vertex(string name) {
			SetName(name);
			SetType(Data::TYPE_VERTEX);
		}
		~Vertex() {
			printf("%s", __func__);
		}
		void SetData(void *src, size_t size) {
			vbuf.resize(size);
			memcpy(vbuf.data(), src, size);
		}
		void * GetData() { return vbuf.data(); }
		size_t GetSize() { return vbuf.size(); }
	};

	View *CreateView(string name, int w, int h) {
		auto v = new View(name, w, h);
		vMapData[name] = v;
		return v;
	}

	Vertex *CreateVertex(string name, void *data = 0, size_t size = 0) {
		auto v = new Vertex(name);
		if(data) {
			v->SetData(data, size);
		}
		vMapData[name] = v;
		return v;
	}

	void Update() {
		for(auto &pair : vMapData) {
			auto data = pair.data;
			auto type = data->GetType();
		}
	}

	void SubmitView(string name) {
		auto view = static_cast<View *>(vMapData[name]);
		if(view) {
			float color[4];
			view->GetClearColor(color);
			glViewport(0, 0, view->GetWidth(), view->GetHeight());
			glClearColor( color[0], color[1], color[2], color[3]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			printf("clear");
		} else {
			printf("failed find view=%s", name.c_str());
		}
	}

	void ReleaseDatas() {
		for(auto &x : vMapData) {
			delete x.second;
		}
	}

	Graphics() { }

	~Graphics() {
		ReleaseDatas();
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
	//SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1 );
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1 );
	SDL_SetVideoMode(Width, Height, 32, SDL_OPENGL);
	SDL_Event event;
	Graphics graphics;
	auto view0 = graphics.CreateView("main", Width, Height);
	view0->SetClearColor(0, 0, 1, 1);
	bool isDone = false;
	while(!isDone) {
		printf("update");
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


