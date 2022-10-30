#pragma once

#ifdef WINDOWS
#define SDL_MAIN int SDL_main(int argc, char* argv[])
#else
#define SDL_MAIN int main(int argc, char* argv[])
#endif
