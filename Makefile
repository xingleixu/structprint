# Makefile for struct_print example
# 用于快速编译和测试示例代码

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = example
PYTHON = python3

# 源文件
SOURCES = example.c
HEADERS = struct_print.h

# 目标：编译示例
all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	@echo "正在编译示例程序..."
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)
	@echo "编译完成！运行 ./$(TARGET) 查看效果"

# 运行示例
run: $(TARGET)
	@echo "运行示例程序..."
	@echo ""
	./$(TARGET)

# 测试Python脚本
test-python:
	@echo "测试Python脚本生成描述符..."
	$(PYTHON) gen_descriptor.py test_structs.h test_descriptors.h
	@echo ""
	@echo "生成的描述符文件："
	@cat test_descriptors.h

# 清理
clean:
	@echo "清理生成的文件..."
	rm -f $(TARGET)
	rm -f test_descriptors.h
	rm -f *.o
	@echo "清理完成！"

# 帮助
help:
	@echo "可用的目标："
	@echo "  make         - 编译示例程序"
	@echo "  make run     - 编译并运行示例程序"
	@echo "  make test-python - 测试Python脚本"
	@echo "  make clean   - 清理生成的文件"
	@echo "  make help    - 显示此帮助信息"

.PHONY: all run test-python clean help

