# GNU Make workspace makefile autogenerated by Premake

.NOTPARALLEL:

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug)
  CTools_config = debug
  Test_config = debug
endif
ifeq ($(config),release)
  CTools_config = release
  Test_config = release
endif

PROJECTS := CTools Test

.PHONY: all clean help $(PROJECTS) 

all: $(PROJECTS)

CTools:
ifneq (,$(CTools_config))
	@echo "==== Building CTools ($(CTools_config)) ===="
	@${MAKE} --no-print-directory -C . -f CTools.make config=$(CTools_config)
endif

Test: CTools
ifneq (,$(Test_config))
	@echo "==== Building Test ($(Test_config)) ===="
	@${MAKE} --no-print-directory -C . -f Test.make config=$(Test_config)
endif

clean:
	@${MAKE} --no-print-directory -C . -f CTools.make clean
	@${MAKE} --no-print-directory -C . -f Test.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug"
	@echo "  release"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   CTools"
	@echo "   Test"
	@echo ""
	@echo "For more information, see http://industriousone.com/premake/quick-start"