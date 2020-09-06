OBJS = main.o
BIN = ./bin/
OBJS := $(addprefix $(BIN), $(OBJS))

CXFLAGS = -Wall -m32
LDFLAGS = -lstdc++

all: $(BIN)apov.exe

$(BIN)apov.exe: $(OBJS)
	gcc -o $@ $^ $(LDFLAGS)

$(BIN)%.o: %.cpp
	gcc -o $@ -c $< $(CXFLAGS)
    
clean:
	powershell " Remove-Item ./bin/*.o"
	powershell " Remove-Item ./bin/*.exe"
    