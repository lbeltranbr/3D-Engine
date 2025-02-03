#pragma once
#include "Window/Window.h"
#include "Auxiliaries/ECS.h"
#include "Graphics/Renderer.h"

namespace SP {

	struct AppInterface;

	struct AppContext {

		SP_INLINE AppContext() {
			Window = std::make_unique<AppWindow>(&Dispatcher, 1280, 720, "SP ENGINE");
			Renderer = std::make_unique<GraphicsRenderer>(1280, 720);
		}
		SP_INLINE ~AppContext() {

			for (auto& layer : Layers) 
				SP_DELETE(layer);
		}

		std::unique_ptr<GraphicsRenderer> Renderer;
		std::vector<AppInterface*> Layers;
		std::unique_ptr<AppWindow> Window;
		EventDispatcher Dispatcher;
		EntityRegistry Scene;
	};
}
