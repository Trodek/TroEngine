#include <stdlib.h>
#include "Application.h"
#include "Globals.h"
#include "SDL/include/SDL.h"

#pragma comment( lib, "Engine/SDL/libx86/SDL2.lib" )
#pragma comment( lib, "Engine/SDL/libx86/SDL2main.lib" )

//Brofiler implementation
#include "Brofiler/Brofiler.h"
#pragma comment( lib, "Engine/Brofiler/ProfilerCore32.lib" )

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = NULL;

int main(int argc, char ** argv)
{
	EDITOR_LOG("Starting Engine'...");

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;

	while (state != MAIN_EXIT)
	{
		BROFILER_FRAME("Main");
		switch (state)
		{
		case MAIN_CREATION:

			EDITOR_LOG("-------------- Application Creation --------------");
			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			EDITOR_LOG("-------------- Application Awake --------------");
			if (App->Init() == false)
			{
				EDITOR_LOG("Application Awake exits with ERROR");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				EDITOR_LOG("-------------- Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				EDITOR_LOG("Application Update exits with ERROR");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			EDITOR_LOG("-------------- Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				EDITOR_LOG("Application CleanUp exits with ERROR");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}

	delete App;
	EDITOR_LOG("Exiting Engine...\n");
	return main_return;
}