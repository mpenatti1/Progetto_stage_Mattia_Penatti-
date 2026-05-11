CXX = g++

SRC = src/Main.cpp src/Anchor.cpp src/ChainSolver.cpp src/ReadAnchors.cpp src/Fenwick.cpp
OBJDIR = build

TARGET_BASE = maximalColinearChaining
TARGET_OPT  = maximalColinearChaining_opt

# -----------------------
# DEFAULT SETTINGS
# -----------------------
BUILD ?= base
MODE ?= debug

# -----------------------
# FLAGS
# -----------------------
CXXFLAGS_DEBUG = -g -O0 -Iinclude
CXXFLAGS_RELEASE = -O3 -Iinclude -DNDEBUG

ifeq ($(MODE),debug)
    CXXFLAGS = $(CXXFLAGS_DEBUG)
else
    CXXFLAGS = $(CXXFLAGS_RELEASE)
endif

ifeq ($(BUILD),opt)
    CXXFLAGS += -DUSE_MAX_PRIORITY
    TARGET = $(TARGET_OPT)
    OBJDIR_BUILD = $(OBJDIR)/opt
else
    TARGET = $(TARGET_BASE)
    OBJDIR_BUILD = $(OBJDIR)/base
endif

OBJ = $(patsubst src/%.cpp,$(OBJDIR_BUILD)/%.o,$(SRC))

# -----------------------
# RULES
# -----------------------
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

$(OBJDIR_BUILD)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET_BASE) $(TARGET_OPT)