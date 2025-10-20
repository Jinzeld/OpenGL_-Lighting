sample:		sample.cpp
		g++ -framework OpenGL -framework GLUT sample.cpp bmptotexture.cpp loadobjmtlfiles.cpp -o sample -I. -Wno-deprecated
