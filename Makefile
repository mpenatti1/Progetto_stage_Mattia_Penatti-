CXX = g++
CXXFLAGS = -g -O0 -Iinclude

SRC = src/Main.cpp src/Anchor.cpp src/ChainSolver.cpp src/KDnode.cpp src/KDpoint.cpp src/KDtree.cpp src/ReadAnchors.cpp
OBJDIR = build
OBJ = $(patsubst src/%.cpp,$(OBJDIR)/src/%.o,$(SRC))

TARGET = maximalColinearChaining

all: $(TARGET)

release: CXXFLAGS = -g -O0 -DNDEBUG -Iinclude
release : $(TARGET)


$(OBJDIR):
	mkdir -p $(OBJDIR)


$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

$(OBJDIR)/src/%.o: src/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)