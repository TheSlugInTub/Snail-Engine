cd src

clang++ -o main.exe -std=c++20 main.cpp Animation.cpp Camera.cpp Canvas.cpp Console.cpp glad.c Object.cpp ObjectManager.cpp Renderer.cpp SnailFunctions.cpp SoundBuffer.cpp SoundDevice.cpp SoundSource.cpp stb.cpp Tilemap.cpp Window.cpp -I../Libraries/include -I../Scripts -L../Libraries/lib -lglfw3 -lbox2d -lfreetype -lOpenAL32 -lalcommon -lal-excommon --target=x86_64-pc-windows-msvc -g

cmd /k 