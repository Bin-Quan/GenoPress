# 编译器设置
CXX := g++
CXXFLAGS := -Wall -Wextra -g -MMD -MP -O2

# 目录设置
SRC_DIR := src
INC_DIR := include
LIB_DIR := lib
OBJ_DIR := obj
BIN_DIR := bin

# 源文件、目标文件和可执行文件
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)
TARGET := $(BIN_DIR)/main

# 包含目录（递归查找所有子目录）
INC_FLAGS := $(addprefix -I,$(shell find $(INC_DIR) -type d))
INC_FLAGS += -I$(SRC_DIR)

# 库文件
LDFLAGS := -L$(LIB_DIR) -Wl,-rpath,$(LIB_DIR)
LDLIBS := -lhts -lbsc

# 默认目标
all: $(TARGET)

# 编译规则
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

# 链接规则
$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

# 包含依赖文件
-include $(DEPS)

# 清理规则
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# 防止与同名文件冲突
.PHONY: all clean
