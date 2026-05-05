all:
	g++ main.cpp -o final_coordinate.exe -lfreeglut -lopengl32 -lglu32
	final_coordinate.exe

clean:
	del final_coordinate.exe