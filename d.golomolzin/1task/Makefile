CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = 1task
TEST_DIR = tests

.PHONY: all build test clean

all: build test

build:
	$(CC) $(CFLAGS) -o $(TARGET) 1task.c

test: build
	@echo "=== Тестирование программы ==="
	@mkdir -p $(TEST_DIR)
	
	@echo "1. Тест без аргументов:"
	@./$(TARGET) 2>&1 | tee $(TEST_DIR)/test1_no_args.txt
	@echo ""
	
	@echo "2. Тест недопустимой опции:"
	@./$(TARGET) -x 2>&1 | tee $(TEST_DIR)/test2_invalid_option.txt
	@echo ""
	
	@echo "3. Тест опций через минус:"
	@./$(TARGET) -d -i -p 2>&1 | tee $(TEST_DIR)/test3_multiple_options.txt
	@echo ""
	
	@echo "4. Тест идентификаторов (-i -p):"
	@./$(TARGET) -i -p 2>&1 | tee $(TEST_DIR)/test4_ids.txt
	@echo ""
	
	@echo "5. Тест лидера группы (-s):"
	@./$(TARGET) -s 2>&1 | tee $(TEST_DIR)/test5_leader.txt
	@echo ""
	
	@echo "6. Тест ulimit (-u):"
	@./$(TARGET) -u 2>&1 | tee $(TEST_DIR)/test6_ulimit.txt
	@echo ""
	
	@echo "7. Тест изменения ulimit (-U):"
	@./$(TARGET) -U 1000 2>&1 | tee $(TEST_DIR)/test7_set_ulimit.txt
	@echo ""
	
	@echo "8. Тест core limit (-c):"
	@./$(TARGET) -c 2>&1 | tee $(TEST_DIR)/test8_core_limit.txt
	@echo ""
	
	@echo "9. Тест изменения core limit (-C):"
	@./$(TARGET) -C 500 2>&1 | tee $(TEST_DIR)/test9_set_core.txt
	@echo ""
	
	@echo "10. Тест директории (-d):"
	@./$(TARGET) -d 2>&1 | tee $(TEST_DIR)/test10_directory.txt
	@echo ""
	
	@echo "11. Тест переменных среды (-v):"
	@./$(TARGET) -v | head -10 2>&1 | tee $(TEST_DIR)/test11_env.txt
	@echo "    (выведены первые 10 переменных)"
	@echo ""
	
	@echo "12. Тест добавления переменной (-V):"
	@./$(TARGET) -V MY_TEST_VAR=hello_world -v | grep MY_TEST_VAR 2>&1 | tee $(TEST_DIR)/test12_add_var.txt
	@echo ""
	
	@echo "13. Тест неудачного значения ulimit:"
	@./$(TARGET) -U -100 2>&1 | tee $(TEST_DIR)/test13_bad_ulimit.txt
	@echo ""
	
	@echo "14. Комплексный тест:"
	@./$(TARGET) -d -i -u -c -v | head -5 2>&1 | tee $(TEST_DIR)/test14_complex.txt
	@echo ""
	
	@echo "=== Все тесты завершены ==="
	@echo "Результаты сохранены в папке $(TEST_DIR)/"
	@echo "Проверьте файлы:"
	@ls -la $(TEST_DIR)/

clean:
	rm -f $(TARGET)
	rm -rf $(TEST_DIR)

test-quick: build
	@echo "Быстрое тестирование основных функций:"
	@./$(TARGET) -d
	@./$(TARGET) -i
	@./$(TARGET) -p
	@./$(TARGET) -u
	@./$(TARGET) -c
	@echo "Основные функции работают!"

test-env: build
	@echo "Тестирование переменных среды:"
	@./$(TARGET) -V TEST_VAR=test_value -v | grep TEST_VAR

test-limits: build
	@echo "Тестирование лимитов:"
	@./$(TARGET) -U 2000 -u
	@./$(TARGET) -C 1000 -c

test-error: build
	@echo "Тестирование обработки ошибок:"
	@./$(TARGET) -x
	@./$(TARGET) -U -500
	@./$(TARGET) -V invalid_format

