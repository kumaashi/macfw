#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

using namespace std; //wahaha

#ifdef _WIN32

FILE _iob[] = {*stdin, *stdout, *stderr};

extern "C" FILE * __cdecl __imp___iob_func(void)
{
    return _iob;
}

#endif

struct Graphics {
	struct Data {
		Data() {}
		virtual ~Data() {}
		enum {
			TYPE_NONE = 0,
			TYPE_VIEW,
			TYPE_VERTEX,
			TYPE_STATE,
			TYPE_INSTANCE,
		};
		int type = TYPE_NONE;
		string name;
		virtual string GetName() { return name; }
		virtual int GetType() { return type; }
		virtual void SetName(string n) { name = n; }
		virtual void SetType(int n) { type = n; }
	};
	map<string, Data *> vMapData;

	struct State : public Data {
		uint64_t bits = 0x0000000000000000;
		State(string name) {
			SetName(name);
			SetType(Data::TYPE_STATE);
		}
		~State() {
			printf("%s\n", __func__);
		}
	};

	struct Vertex : public Data {
		vector<uint8_t> vbuf;
		Vertex(string name) {
			SetName(name);
			SetType(Data::TYPE_VERTEX);
		}
		~Vertex() {
			printf("%s\n", __func__);
		}
		void SetData(void *src, size_t size) {
			vbuf.resize(size);
			memcpy(vbuf.data(), src, size);
		}
		void * GetData() { return vbuf.data(); }
		size_t GetSize() { return vbuf.size(); }
	};

	struct Instance : public Data {
		bool isAlive = true;
		Vertex *vertex = 0;
		State *state = 0;
		float pos[4] = {0, 0, 0, 1};
		float scale[4] = {1, 1, 1, 1};
		float rot[4] = {0, 0, 0, 0};
		Instance(string name) {
			SetName(name);
			SetType(Data::TYPE_INSTANCE);
		}
		~Instance() { printf("%s\n", __func__); }
		void SetVertex(Vertex *v) { vertex = v; }
		Vertex *GetVertex() { return vertex; }
		void SetState(State *v) { state = v; }
		State *GetState() { return state; }
		bool GetAlive() { return isAlive; }
		void SetAlive(bool value) { isAlive = value; }

		void SetPos(float x, float y, float z) { pos[0] = x; pos[1] = y; pos[2] = z; }
		void SetScale(float x, float y, float z) { scale[0] = x; scale[1] = y; scale[2] = z; }
		void SetRot(float x, float y, float z) { rot[0] = x; rot[1] = y; rot[2] = z; }

		void GetPos(float *x) { for(int i = 0; i < 3; i++) { x[i] = pos[i]; } }
		void GetScale(float *x) { for(int i = 0; i < 3; i++) { x[i] = scale[i]; } }
		void GetRot(float *x) { for(int i = 0; i < 3; i++) { x[i] = rot[i]; } }
	};

	struct View : public Data {
		struct Rect {
			int x, y, w, h;
		} rect;
		float color[4];
		map<string, Instance *> vMapInstance;
		void GetInstance(vector<Instance *> &dest) {
			for(auto &x : vMapInstance) {
				dest.push_back(x.second);
			}
		}
		void AddInstance(Instance *instance) {
			vMapInstance[instance->GetName()] = instance;
		}
		View(string name, int x, int y, int w, int h) {
			SetName(name);
			SetType(Data::TYPE_VIEW);
			rect.x = x;
			rect.y = y;
			rect.w = w;
			rect.h = h;
		}
		~View() {
			printf("%s\n", __func__);
		}
		void SetClearColor(float r, float g, float b, float a) {
			color[0] = r;
			color[1] = g;
			color[2] = b;
			color[3] = a;
		}
		void GetClearColor(float *buf) {
			for(int i = 0 ; i < 4 ; i++) {
				buf[i] = color[i];
			}
		}
		Rect GetViewport() {
			return rect;
		}
	};
	View *CreateView(string name, int x, int y, int w, int h) {
		auto cache = vMapData.find(name);
		if(cache != vMapData.end()) {
			ReleaseData(cache->second);
		}
		auto v = new View(name, x, y, w, h);
		vMapData[name] = v;
		return v;
	}

	Vertex *CreateVertex(string name, void *data = 0, size_t size = 0) {
		auto cache = vMapData.find(name);
		if(cache != vMapData.end()) {
			ReleaseData(cache->second);
		}
		auto v = new Vertex(name);
		if(data) {
			v->SetData(data, size);
		}
		vMapData[name] = v;
		return v;
	}

	State *CreateState(string name) {
		auto cache = vMapData.find(name);
		if(cache != vMapData.end()) {
			ReleaseData(cache->second);
		}
		auto v = new State(name);
		vMapData[name] = v;
		return v;
		return nullptr;
	}

	Instance *CreateInstance(string name) {
		auto cache = vMapData.find(name);
		if(cache != vMapData.end()) {
			ReleaseData(cache->second);
		}
		auto v = new Instance(name);
		vMapData[name] = v;
		return v;
	}

	void Update() {
		for(auto & pair : vMapData) {
			auto data = pair.second;
			auto type = data->GetType();
			if(type == Data::TYPE_NONE) {}
			if(type == Data::TYPE_VIEW) {}
			if(type == Data::TYPE_VERTEX) {}
			if(type == Data::TYPE_STATE) {} 
			if(type == Data::TYPE_INSTANCE) {}
		}
	}

	void SubmitView(string name) {
		auto view = static_cast<View *>(vMapData[name]);
		if(view) {
			float color[4];
			view->GetClearColor(color);
			View::Rect rect = view->GetViewport();
			glViewport(rect.x, rect.y, rect.w, rect.h);
			glClearColor( color[0], color[1], color[2], color[3]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			vector<Instance *> vinstance;
			view->GetInstance(vinstance);
		} else {
			printf("failed find view=%s\n", name.c_str());
		}
	}

	void ReleaseData(Data *data) {
		delete data;
	}

	void ReleaseDatas() {
		printf("%s vMapData.size()=%d\n", __func__, vMapData.size());
		for(auto &x : vMapData) {
			ReleaseData(x.second);
		}
	}

	Graphics() { }

	~Graphics() {
		printf("%s\n", __func__);
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
	
	{
		Graphics graphics;
		auto view0 = graphics.CreateView("main", 0, 0, Width, Height);
		view0->SetClearColor(0, 0, 1, 1);
		
		bool isDone = false;
		while(!isDone) {
			while(SDL_PollEvent(&event)) {
				if(event.type & SDL_KEYDOWN) {
					isDone = true;
				}
			}
			
			graphics.SubmitView(view0->GetName());
			SDL_GL_SwapBuffers();
		}
		printf("ç≤ÅXñÿê≥é˜\n");
	}
		printf("ç≤ÅXñÿê≥é˜ OK \n");
	SDL_Quit();
	return 0;
}


