Options = -std=c++2a

# qwerty1337

# -Wpedantic -Wall 

# Godric's-Hollow factorial

# imperio fact n
# alohomora 
#     revelio legilimens n greater tria flipendo duo
#     alohomora
#         - reverte n geminio fact protego n flipendo tria epoximise duo protego
#     colloportus
#     otherwise
#     alohomora
#         - reverte tria epoximise duo
#     colloportus
# colloportus 

# imperio love horcrux
# alohomora
#     - avenseguim n carpe-retractum horcrux
#     - n carpe-retractum accio
#     - avenseguim result carpe-retractum fact protego colloshoo protego n protego protego
#     - flagrate legilimens result
# colloportus

# Privet-Drive

SrcDir = src
BinDir = bin
IntDir = $(BinDir)/intermediates
LibDir = libs

LIBS = $(wildcard $(LibDir)/*.a)
DEPS = $(wildcard $(SrcDir)/*.h) $(wildcard $(LibDir)/*.h)
OBJS = $(IntDir)/main.o $(IntDir)/syntax.o $(IntDir)/tokenizer.o $(IntDir)/expression_tree.o $(IntDir)/parser.o $(IntDir)/symbol_table.o $(IntDir)/compiler.o 

$(BinDir)/compiler.exe: $(OBJS) $(LIBS) $(DEPS)
	g++ -o $(BinDir)/compiler.exe $(OBJS) $(LIBS)

$(IntDir)/main.o: $(SrcDir)/main.cpp $(DEPS)
	g++ -o $(IntDir)/main.o -c $(SrcDir)/main.cpp $(Options)

$(IntDir)/syntax.o: $(SrcDir)/syntax.cpp $(DEPS)
	g++ -o $(IntDir)/syntax.o -c $(SrcDir)/syntax.cpp $(Options)

$(IntDir)/tokenizer.o: $(SrcDir)/tokenizer.cpp $(DEPS)
	g++ -o $(IntDir)/tokenizer.o -c $(SrcDir)/tokenizer.cpp $(Options)

$(IntDir)/expression_tree.o: $(SrcDir)/expression_tree.cpp $(DEPS)
	g++ -o $(IntDir)/expression_tree.o -c $(SrcDir)/expression_tree.cpp $(Options)

$(IntDir)/parser.o: $(SrcDir)/parser.cpp $(DEPS)
	g++ -o $(IntDir)/parser.o -c $(SrcDir)/parser.cpp $(Options)

$(IntDir)/symbol_table.o: $(SrcDir)/symbol_table.cpp $(DEPS)
	g++ -o $(IntDir)/symbol_table.o -c $(SrcDir)/symbol_table.cpp $(Options)

$(IntDir)/compiler.o: $(SrcDir)/compiler.cpp $(DEPS)
	g++ -o $(IntDir)/compiler.o -c $(SrcDir)/compiler.cpp $(Options)