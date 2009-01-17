
CC      := gcc
INSTALL := ginstall
ECHO    := echo
RM      := /bin/rm
CD      := cd
TAR     := tar

CFLAGS := $(CFLAGS) -W -Wall -g -DDEBUG -I.

LIBS   :=  -lm

EXE_SRC := rbtree.c csv.c section.c opcodes.c labels.c irq.c options.c etripator.c
EXE_OBJ := $(EXE_SRC:.c=.o)
EXE     := etripator

DEPEND := .depend

all :: $(EXE)

dep :: $(DEPEND)

$(DEPEND) ::
	@$(ECHO) "  MKDEP"
	@$(CC) -MM -MG $(CFLAGS) $(EXE_SRC) > $(DEPEND)

$(EXE) :: $(EXE_OBJ)
	@$(ECHO) "  LD        $@"
	@$(CC) -o $(EXE) $^ $(LIBS)

%.o :: %.c
	@$(ECHO) "  CC        $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

install ::

clean :: FORCE
	@$(ECHO) "  CLEAN     object files"
	@$(RM) -f $(EXE_OBJ) 

realclean :: clean
	@$(ECHO) "  CLEAN     $(EXE)"
	@$(RM) -f $(EXE)
	@$(ECHO) "  CLEAN     noise files"
	@$(RM) -f `find . -name "*~" -o -name "\#*"`

c :: clean

rc :: realclean

archive :: realclean
	@$(ECHO) "  TBZ2      `date +"%Y/%m/%d"`"
	@$(CD) ..; $(TAR) jcf etripator-`date +"%Y%m%d"`.tar.bz2 same

FORCE :
ifeq (.depend,$(wildcard .depend))
include .depend
endif
