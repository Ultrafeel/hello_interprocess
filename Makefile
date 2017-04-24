#.SUFFIXES:
#.SUFFIXES: .a .c .o
CC=gcc
CFLAGS= -D_GNU_SOURCE -D__USE_POSIX2 -D__USE_POSIX -I/usr/include -c -Wall -std=c11 -g
LDFLAGS=
LIBSRCH=libhello.h libgoodbye.h 
  
SOURCES=main_a.c main_b.c
OBJDIR:=objdir
LIBDIR:=libraries
OUT_TARG_DIR := target_bin/bin
OBJS=$(OBJDIR)/main_a.o $(OBJDIR)/main_b.o
LIBS_FILENAMES=$(LIBSRCH:.h=.a)
LIBS1=$(addprefix $(LIBDIR)/,$(LIBS_FILENAMES))
LIBS_O=$(LIBSRCH:.h=.o)
LIBS_OB=$(addprefix $(OBJDIR)/,$(LIBS_O))
EXECUTABLE=main_a
RM := rm

.PHONY: clean main libs do-target

all:  main
main: $(OUT_TARG_DIR)/$(EXECUTABLE) $(OUT_TARG_DIR)/main_b

	
$(OUT_TARG_DIR)/$(EXECUTABLE): $(OBJDIR)/main_a.o  | $(OUT_TARG_DIR) 
	$(CC) $(OBJDIR)/main_a.o  $(LDFLAGS)  -o $(OUT_TARG_DIR)/$(EXECUTABLE)
	
$(OUT_TARG_DIR)/main_b: $(OBJDIR)/main_b.o $(OBJDIR)/vector.o | $(OUT_TARG_DIR) 
	$(CC) $(OBJDIR)/main_b.o $(OBJDIR)/vector.o  -lpthread $(LDFLAGS)  -o $@
#:$@

#echo libso!: $(LIBS_OB)

# -Wl,-trace-symbol=bye  

# %.h $(LIBS_OB) $(LIBSRCH)
#$(OBJS):$(EXECUTABLE).c | $(OBJDIR)
$(OBJDIR)/%.o:%.c | $(OBJDIR)
	$(CC) -I. $(CFLAGS) -c -o $@ $<

#	gcc -D_GNU_SOURCE -D__USE_POSIX2 -D__USE_POSIX -I/usr/include -Wall -std=c11  -E -C  $< > $<.ip

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

