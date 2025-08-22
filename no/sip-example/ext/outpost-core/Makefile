# Copyright (c) 2013-2018, Fabian Greif
# Copyright (c) 2014, Murat Goeksu
# Copyright (c) 2017, Muhammad Bassam
# Copyright (c) 2018, Annika Ofenloch
# Copyright (c) 2019-2020, 2024, Felix Passenberg
# Copyright (c) 2020, 2022-2024, Jan-Gerd Mess
# Copyright (c) 2021, toth_no
# Copyright (c) 2022, Tobias Pfeffer
# Copyright (c) 2023-2024, Pascal Pieper
# Copyright (c) 2023, rein_j1
# Copyright (c) 2024, Pieper, Pascal
# Copyright (c) 2024, Passenberg, Felix Constantin
# Copyright (c) 2024, Cold, Erin Cynthia
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

# Configure the parallel build normal "make -jn" is not possible as SCons
# is used to build the source files. And make has no option the extract the
# number of request parallel processes from the "-j" option.
ifneq ($(JOBS),)
  MAKEJOBS=-j$(JOBS)
else
  # Default is 3 parallel jobs. Use `JOBS=` to disable parallel build.
  NPROCS:=1
  OS:=$(shell uname -s)
  ifeq ($(OS),Linux)
    NPROCS:=$(shell nproc)
  endif
  MAKEJOBS=-j$(NPROCS)
endif

# Check if the terminal supports colors
COLORS := $(shell tput colors 2> /dev/null)

ifeq ($(COLORS),)
# The terminal doesn't support colors, don't output anything colorful
CINFO  =
COK    =
CERROR =
CEND   =
else
# Color definitions (e.g. for bash 'tput colors' returns '8')
CINFO  = \033[;0;33m
COK    = \033[32;01m
CERROR = \033[31;01m
CEND   = \033[0m
endif

OUTPOST_CORE_PATH ?= .

COVERAGE_DIR   := build/coverage
# without --exclude-throw-branches because libraries are built with -fno-exception
COVERAGE_PARAMS := --exclude-unreachable-branches

EXTRA_CONFIGURATIONS ?= sip/test/max_payloadLength_param:outpost_sip \
                        sip/test/zero_payloadLength_param:outpost_sip


help:
	@echo "Please use \`make <target>\` where <target> is one of"
	@echo "  doc        to build all documentation"
	@echo "  doxygen    to build the doxygen documentation"
	@echo "  test       to run all unit tests"
	@echo "  test-full  to run all unit and compilation tests"
	@echo "  clang-tidy to run static analysis"
	@echo "  clean      to remove temporary data (\`build\` folder)"

print-versions:
	uname -a || true
	gcc --version || true
	clang --version || true
	ldd --version || true
	clang-format --version || true
	scons --version || true
	python --version || true
	python3 --version || true
	cppcheck --version || true
	clang-tidy --version || true
	gcovr --version || true
	doxygen --version || true

doc: doxygen

doxygen:
	@doxygen doc/doxygen/doxyfile
	@doxygen doc/doxygen/doxyfile-internal

test-other-configurations:
	- $(foreach EXTRA_CONFIGURATIONS,$(EXTRA_CONFIGURATIONS), \
			$(eval PARAM = $(word 1,$(subst :, ,$(EXTRA_CONFIGURATIONS)))) \
			$(eval MODULE = $(word 2,$(subst :, ,$(EXTRA_CONFIGURATIONS)))) \
		scons -C modules $(MAKEJOBS) use_outpost_asserts=1 param_extra_include=$(PARAM) run-$(MODULE) ; \
	)
	@printf "\n$(COK)[PASS] All extra-configuration unit tests passed!$(CEND)\n"

test: test-other-configurations
	scons -C modules use_outpost_asserts=1 run-unittests $(MAKEJOBS)  # build and run in parallel
	@printf "\n$(COK)[PASS] All unit tests passed!$(CEND)\n"

test-trap:
	scons -C modules trap=1 run-unittests $(MAKEJOBS) # build and run in parallel
	@printf "\n$(COK)[PASS] All unit tests passed!$(CEND)\n"

compile_commands_modular:
	@mkdir -p build/cppcheck
	@scons -C modules -D append_buildpath=cppcheck unittests_db

compile_commands: compile_commands_modular
	@find ./build/ -iname "compile_commands.json" -exec cat {} + | jq '.[]' | jq -s '.'   > build/compile_commands.json

cppcheck : compile_commands_modular
	-find build/cppcheck -type f -name "*.xml" -delete -true;

	@#cppcheck: "-j flag disables 'unusedFunctions'" Warning, so without $(MAKEJOBS)
	@for DB in `find build/cppcheck -iname "compile_commands.json"` ;\
	do \
		MODULE=$$(basename $$(dirname $${DB})) ;\
		DIR=build/cppcheck/$${MODULE} ;\
		XMLFILE=$${DIR}.xml ;\
		echo "$${MODULE} ..." ;\
		cppcheck --quiet --language=c++ --library=googletest --enable=all -USYS_gettid --inconclusive --force --inline-suppr --project=$${DB} --suppressions-list=.cppcheck-excludes --xml --xml-version=2 2> $${XMLFILE} --check-config; \
		cppcheck --quiet --language=c++ --library=googletest --enable=all -USYS_gettid --inconclusive --force --inline-suppr --project=$${DB} --suppressions-list=.cppcheck-excludes --xml --xml-version=2 2> $${XMLFILE}; \
		cppcheck-htmlreport --file $${XMLFILE} --report-dir $${DIR} --title="$${MODULE}";\
	done;
	@printf "\n$(CINFO)all CPPcheck-src results saved to disk$(CEND)\n";

clang-tidy:
	@mkdir -p build/clang-tidy
	@scons -C modules -D append_buildpath=clang-tidy unittests_db

	@for DB in `find build/clang-tidy -iname "compile_commands.json"` ;\
	do \
		DIR=$$(dirname $${DB}) ;\
		MODULE=$$(basename $${DIR}) ;\
		echo $${DB} ;\
		echo $${MODULE} ;\
		run-clang-tidy -p="$${DIR}" -quiet | tee build/clang-tidy/$(MODULE).log ;\
		clang-tidy-html build/clang-tidy/$(MODULE).log -o build/clang-tidy/$(MODULE)/clang.html ;\
	done;

cloc-source:
	@mkdir -p build/cloc
	@for m in `find modules -iname "src"` ; do \
		MODULE=$$(basename $$(realpath $${m}/..)) ;\
		printf "\n$(CINFO)Count Lines of Code for module \"$${MODULE}\":$(CEND)\n" ; \
		cloc --by-file --xml --out=build/cloc/$${MODULE}.xml $${m} ;\
	done

test-full: test
	echo "\n$(CINFO)Run compilation tests:$(CEND)\n"
	$(MAKE) -C test/sparc_rtems --no-print-directory
	@printf "\n$(COK)[PASS] All tests passed!$(CEND)\n"

GLOB_COVERAGE_JSONS = $$(python3 -c "import glob; print(' '.join(['--add-tracefile ' + file for file in glob.glob('$(COVERAGE_DIR)/**/*.json', recursive=True)]))")

coverage:
	@# clean up
	@find $(COVERAGE_DIR) -name "*.gcda" -name "*.gcno" -delete || true;
	# comment out to always force re-run of runners
	#find $(COVERAGE_DIR) -name "*.json" -delete || true
	#find $(COVERAGE_DIR) -name "*.xml"  -delete || true
	@mkdir -p $(COVERAGE_DIR)/report;
	@rm -rf $(COVERAGE_DIR)/report/*
	scons -C modules coverage=1 $(MAKEJOBS)             # builds the runners parallel
	scons -C modules coverage=1 run-unittests-coverage  # but run them sequentially, as multiple gcov runs interfere

	- $(foreach EXTRA_CONFIGURATIONS,$(EXTRA_CONFIGURATIONS), \
			$(eval PARAM = $(word 1,$(subst :, ,$(EXTRA_CONFIGURATIONS)))) \
			$(eval MODULE = $(word 2,$(subst :, ,$(EXTRA_CONFIGURATIONS)))) \
		scons -C modules coverage=1 $(MAKEJOBS) param_extra_include=$(PARAM) run-$(MODULE)-coverage ; \
	)

	@# Line coverage
	@gcovr \
		$(GLOB_COVERAGE_JSONS) \
		--root=modules \
		$(COVERAGE_PARAMS) \
		--txt-metric line \
		--txt $(COVERAGE_DIR)/line-coverage.log

	@# Branch coverage
	@gcovr \
		$(GLOB_COVERAGE_JSONS) \
		--root=modules \
		$(COVERAGE_PARAMS) \
		--txt-metric branch \
		--txt $(COVERAGE_DIR)/branch-coverage.log

	# Decision coverage
	@gcovr \
		$(GLOB_COVERAGE_JSONS) \
		--root=modules \
		$(COVERAGE_PARAMS) \
		--txt-metric decision \
		--txt $(COVERAGE_DIR)/decision-coverage.log

	# html summary
	@gcovr \
		$(GLOB_COVERAGE_JSONS) \
		--root=modules \
		--xml-pretty \
		$(COVERAGE_PARAMS) \
		--print-summary \
		--decisions \
		-o "$(COVERAGE_DIR)/coverage.xml" \
		--html-details $(COVERAGE_DIR)/report/index.html > "$(COVERAGE_DIR)/summary.txt" ;
	@sed -i -E 's/filename="/filename="modules\//' "$(COVERAGE_DIR)/coverage.xml"
	cat $(COVERAGE_DIR)/summary.txt | $(OUTPOST_CORE_PATH)/tools/gcovr_summary_calculator.py | tee $(COVERAGE_DIR)/summary_precise.txt
	@printf "\n$(COK)[PASS] Coverage analysis done!$(CEND)\n"

analyze-clang:
	@for m in $(MODULES); do \
		printf "\n$(CINFO)Run static analysis with clang for module \"$$m\":$(CEND)\n" ; \
		make -C modules/$$m analyze-clang --no-print-directory || return 1 ; \
	done
	@printf "\n$(COK)[PASS] Static analysis with clang done!$(CEND)\n"

metric:
	@for m in `find modules -iname "src"` ; do \
		MODULE=$$(basename $$(realpath $${m}/..)) ;\
		echo $${MODULE} ;\
		printf "\n$(CINFO)Generating code metrics for module \"$${MODULE}\" (sources):$(CEND)\n" ; \
		sloccount --duplicates --wide $${m} ; \
		printf "\n$(CINFO)Generating code metrics for module \"$${MODULE}\" (unittests):$(CEND)\n" ; \
		sloccount --duplicates --wide $${m}/../test ; \
	done

CLANG_FORMAT="clang-format"

# exclude *ext* paths
FORMAT_SOURCE_FILES ?= $(shell find modules -type f -name '*.cpp' -or -name '*.h' -not -path '*/ext/*')

format:
	@$(CLANG_FORMAT) -i --style=file $(FORMAT_SOURCE_FILES)
	@printf "\n$(COK)[PASS] Formatting done!$(CEND)\n"

license-update:
	python3 ../reword -v -u "append"

SCONS_BUILD_TYPES ?= "coverage=1" "trap=1" "-D append_buildpath=clang-tidy" "-D append_buildpath=cppcheck"

clean :
	@for type in $(SCONS_BUILD_TYPES) ; do \
		echo $$type ;\
		scons -C modules -c $$type > /dev/null ;\
	done

.PHONY: doc test test-other-configurations clean

