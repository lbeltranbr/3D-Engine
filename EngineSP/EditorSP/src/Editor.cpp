#include <EngineSP.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


using namespace SP;

struct Editor : AppInterface {
	
	SP_INLINE void OnStart() {}
	SP_INLINE void OnUpdate() {}

};

int32_t main(int32_t argc, char* argv[]) {


//#ifdef ENABLE_SPDLOG
//	
//	std::string command = argv[1];
//	std::string compare = "/disable_comments";
//
//#endif // ENABLE_SPDLOG

	auto app = new Application();

	app->AttachLayer<Editor>();
	app->RunContext();
	return 0;
}