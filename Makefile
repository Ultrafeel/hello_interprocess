#.SUFFIXES:
#.SUFFIXES: .a .c .o
CC=gcc
CFLAGS=-c -Wall -std=c11 -g
LDFLAGS=
LIBSRCH=libhello.h libgoodbye.h 
  
SOURCES=main_a.c
OBJDIR:=objdir
LIBDIR:=libraries
OUT_TARG_DIR := target_bin/bin
OBJS=$(OBJDIR)/main_a.o
LIBS_FILENAMES=$(LIBSRCH:.h=.a)
LIBS1=$(addprefix $(LIBDIR)/,$(LIBS_FILENAMES))
LIBS_O=$(LIBSRCH:.h=.o)
LIBS_OB=$(addprefix $(OBJDIR)/,$(LIBS_O))
EXECUTABLE=main_a
RM := rm

.PHONY: clean main libs do-target

all:  main
main: $(OUT_TARG_DIR)/$(EXECUTABLE)

	
$(OUT_TARG_DIR)/$(EXECUTABLE): $(OBJS)  | $(OUT_TARG_DIR) 
	$(CC) $(OBJS)  $(LDFLAGS)  -o $(OUT_TARG_DIR)/$(EXECUTABLE)
#:$@

#echo libso!: $(LIBS_OB)

# -Wl,-trace-symbol=bye  

# %.h $(LIBS_OB) $(LIBSRCH)
#$(OBJDIR)/%.o:%.c
$(OBJS):$(EXECUTABLE).c | $(OBJDIR)
	$(CC) -I. $(CFLAGS) -c -o $@ $<

$(LIBDIR)/%.a:$(OBJDIR)/%.o  |$(LIBDIR)
	ar rcsv $@ $<

#$(OBJS): 

$(LIBDIR):
	@mkdir $(LIBDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OUT_TARG_DIR):
	mkdir -p $(OUT_TARG_DIR)
#$(addprefix $(OBJDIR)/,hello.c)
#make --trace -w

clean:
	@echo clean : $(OBJS)  $(EXECUTABLE) $(LIBS1)
	-$(RM)  -rfv   $(OBJS) ./$(OUT_TARG_DIR)/$(EXECUTABLE) $(EXECUTABLE).o $(EXECUTABLE)  $(LIBS1) $(LIBS_FILENAMES) $(LIBS_OB) $(LIBSRCH:.h=.a) $(LIBSRCH:.h=.so)

