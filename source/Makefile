# If you don't have freetype, use this:

LDFLAGS  = -L. -Llib32 -lglfw -lGL -ldl
CXXFLAGS = -O2 -Wall -Wno-write-strings -Wno-parentheses -Wno-unused-result -DLINUX

# If you have installed the freetype package, use this:

#LDFLAGS  = -L. -Llib32 -lglfw -lGL -ldl -lfreetype
#CXXFLAGS = -O2 -I/usr/include/freetype2 -Wall -Wno-write-strings -Wno-parentheses -Wno-unused-result -DLINUX -DHAVE_FREETYPE

# Other stuff

PROG = shader

OBJS = shader.o gpuProgram.o linalg.o wavefront.o renderer.o gbuffer.o font.o glad/src/glad.o 

$(PROG): $(OBJS)
	$(CXX) -o $(PROG) $(OBJS) $(LDFLAGS) 

clean:
	rm -f *.o *~ $(PROG)

depend:	
	makedepend -Y *.h *.cpp

# DO NOT DELETE

gpuProgram.o: headers.h glad/include/glad/glad.h
gpuProgram.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
headers.o: glad/include/glad/glad.h glad/include/KHR/khrplatform.h
headers.o: include/GLFW/glfw3.h linalg.h
renderer.o: wavefront.h headers.h glad/include/glad/glad.h
renderer.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
renderer.o: seq.h shadeMode.h gpuProgram.h gbuffer.h
seq.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
seq.o: include/GLFW/glfw3.h linalg.h
wavefront.o: headers.h glad/include/glad/glad.h
wavefront.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
wavefront.o: seq.h shadeMode.h gpuProgram.h
font.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
font.o: include/GLFW/glfw3.h linalg.h gpuProgram.h
gbuffer.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
gbuffer.o: include/GLFW/glfw3.h linalg.h gbuffer.h
gpuProgram.o: gpuProgram.h headers.h glad/include/glad/glad.h
gpuProgram.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
linalg.o: linalg.h
renderer.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
renderer.o: include/GLFW/glfw3.h linalg.h renderer.h wavefront.h seq.h
renderer.o: shadeMode.h gpuProgram.h gbuffer.h shader.h
shader.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
shader.o: include/GLFW/glfw3.h linalg.h wavefront.h seq.h shadeMode.h
shader.o: gpuProgram.h renderer.h gbuffer.h font.h
wavefront.o: headers.h glad/include/glad/glad.h
wavefront.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
wavefront.o: gpuProgram.h wavefront.h seq.h shadeMode.h
