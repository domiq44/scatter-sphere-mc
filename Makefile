# Répertoires
SRC_DIR     = src
INC_DIR     = include
OBJ_DIR     = obj
TEST_DIR    = tests
RESULTS_DIR = results

# Exécutables
SIM_EXEC  = simulate
PLOT_EXEC = plot
TEST_EXEC = tests_runner

# Compilateur et options
CC      = cc
CFLAGS  = -O3 -std=c11 -Wall -Wextra -I$(INC_DIR)
CFLAGS += -fno-builtin-sincos
CFLAGS += -D_DEFAULT_SOURCE

# Flags spécifiques pour les tests
TEST_CFLAGS = $(CFLAGS) -Wno-unused-parameter -Wno-unused-variable

LDFLAGS = -lm

# cmocka (via pkg-config)
CMOCKA_CFLAGS  = $(shell pkg-config --cflags cmocka 2>/dev/null)
CMOCKA_LDFLAGS = $(shell pkg-config --libs cmocka 2>/dev/null)

# Modules sources du simulateur
SIM_MODULES = vec3.c rng.c basis.c hg.c config.c geometry.c simulate.c
SIM_SRC     = $(addprefix $(SRC_DIR)/, $(SIM_MODULES))
SIM_OBJ     = $(addprefix $(OBJ_DIR)/, $(SIM_MODULES:.c=.o))

# Modules sources du plot
PLOT_MODULES = plot_main.c plot_csv.c plot_utils.c plot_gnuplot.c
PLOT_SRC     = $(addprefix $(SRC_DIR)/, $(PLOT_MODULES))
PLOT_OBJ     = $(addprefix $(OBJ_DIR)/, $(PLOT_MODULES:.c=.o))

# Tests unitaires
TEST_SRC = $(filter-out $(TEST_DIR)/test_main.c, $(wildcard $(TEST_DIR)/test_*.c))
TEST_OBJ = $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%.o,$(TEST_SRC))
TEST_MAIN_OBJ = $(OBJ_DIR)/test_main.o

# Règle par défaut
all: $(RESULTS_DIR) $(SIM_EXEC) $(PLOT_EXEC)

# Création des dossiers
$(RESULTS_DIR):
	@mkdir -p $(RESULTS_DIR)
	@echo "[OK] Dossier results créé"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@echo "[OK] Dossier obj créé"

# Compilation du simulateur
$(SIM_EXEC): $(OBJ_DIR) $(SIM_OBJ)
	@$(CC) $(CFLAGS) -o $@ $(SIM_OBJ) $(LDFLAGS)
	@echo "[OK] Binaire simulate compilé"

# Compilation du programme plot
$(PLOT_EXEC): $(OBJ_DIR) $(PLOT_OBJ) $(OBJ_DIR)/config.o
	@$(CC) $(CFLAGS) -o $@ $(PLOT_OBJ) $(OBJ_DIR)/config.o $(LDFLAGS)
	@echo "[OK] Binaire plot compilé"

# Compilation générique des .o (modules C)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "[CC] $<"

# Compilation générique des .o (tests C)
$(OBJ_DIR)/test_%.o: $(TEST_DIR)/test_%.c | $(OBJ_DIR)
	@$(CC) $(TEST_CFLAGS) $(CMOCKA_CFLAGS) -c $< -o $@
	@echo "[CC test] $<"

# Binaire unique des tests cmocka
$(TEST_EXEC): $(TEST_MAIN_OBJ) $(TEST_OBJ) \
			  $(OBJ_DIR)/vec3.o $(OBJ_DIR)/rng.o $(OBJ_DIR)/basis.o \
			  $(OBJ_DIR)/hg.o $(OBJ_DIR)/config.o $(OBJ_DIR)/geometry.o \
			  $(OBJ_DIR)/plot_csv.o $(OBJ_DIR)/plot_utils.o
	@$(CC) -o $@ $^ \
		-Wl,--wrap=popen \
		-Wl,--wrap=pclose \
		$(CMOCKA_LDFLAGS) $(LDFLAGS)
	@echo "[OK] Binaire de tests compilé"

# Compiler les tests
build_tests: $(TEST_EXEC)
	@echo "[OK] Compilation des tests"

# Exécuter les tests
tests: $(OBJ_DIR) $(TEST_EXEC)
	@./$(TEST_EXEC)

# Nettoyer les fichiers générés
clean:
	@rm -f $(SIM_EXEC) $(PLOT_EXEC) $(TEST_EXEC)
	@rm -rf $(OBJ_DIR)
	@echo "[OK] Nettoyage effectué"

# Nettoyage complet
distclean: clean
	@rm -rf $(RESULTS_DIR)
	@find . -maxdepth 1 -name "config.*.toml" \
		! -name "config.template.toml" \
		! -name "config.toml" \
		-delete
	@echo "[OK] distclean terminé"

# Formatage du code
FORMAT = clang-format
FORMAT_FLAGS = -i
FORMAT_SRC = $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*.h \
						 $(TEST_DIR)/*.c $(TEST_DIR)/*.h)

format:
	@$(FORMAT) $(FORMAT_FLAGS) $(FORMAT_SRC)
	@echo "[OK] Formatage du code terminé"

.PHONY: all clean distclean tests build_tests format
