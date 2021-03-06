MPCVERSION=1.1.0
MAKEFLAGS =

#GMPINCLUDEDIR=$(abs_builddir)/../i/GMP/include/
#GMPLIBDIR=$(abs_builddir)/../i/GMP/lib/
#MPFRINCLUDEDIR=$(abs_builddir)/../i/MPFR/include/
#MPFRLIBDIR=$(abs_builddir)/../i/MPFR/lib/
MPLIBRARY_PATH=@GMP_LIBDIR@:@MPFR_LIBDIR@

CONFIGURE_ARGS=--prefix=$(abs_builddir)/../i/MPC/ --host=$(host) --with-gmp-include=@GMP_INCLUDEDIR@ --with-gmp-lib=@GMP_LIBDIR@ --with-mpfr-include=@MPFR_INCLUDEDIR@ --with-mpfr-lib=@MPFR_LIBDIR@
CONFIGURE_ARGS2=--prefix=$(prefix) --host=$(host) --with-gmp-include=@GMP_INCLUDEDIR@ --with-gmp-lib=@GMP_LIBDIR@ --with-mpfr-include=@MPFR_INCLUDEDIR@ --with-mpfr-lib=@MPFR_LIBDIR@

EXTRACT_STAMP=.mpc_extract_done
PATCH_STAMP=.mpc_patch_done
CONFIGURE_STAMP=.mpc_configure_done
BUILD_STAMP=.mpc_build_done
INSTALL_STAMP=.mpc_install_done
INSTALL_INTERNAL_STAMP=.mpc_install_internal_done
CHECK_STAMP=.mpc_check_done

all: extract config build check install_internal

work/$(EXTRACT_STAMP):
	rm -rf work ; mkdir -p work/internal  ; mkdir -p work/install
	cd $(abs_builddir)/work/internal; tar xvfz ../../download/mpc-$(MPCVERSION).tar.gz
	cd $(abs_builddir)/work/install; tar xvfz ../../download/mpc-$(MPCVERSION).tar.gz
	touch $(abs_builddir)/work/$(EXTRACT_STAMP)

work/$(PATCH_STAMP): work/$(EXTRACT_STAMP)
	touch $(abs_builddir)/work/$(PATCH_STAMP)

work/$(CONFIGURE_STAMP): work/$(PATCH_STAMP)
	cd $(abs_builddir)/work/internal/mpc-$(MPCVERSION) ; env "CC=$(CC)" "CXX=$(CXX)" "CPPFLAGS=$(CPPFLAGS)" "FC=$(FC)" "LDFLAGS=$(LDFLAGS)" "NM=$(NM)" "RANLIB=$(RANLIB)" ./configure $(CONFIGURE_ARGS)
	cd $(abs_builddir)/work/install/mpc-$(MPCVERSION) ; env "CC=$(CC)" "CXX=$(CXX)" "CPPFLAGS=$(CPPFLAGS)" "FC=$(FC)" "LDFLAGS=$(LDFLAGS)" "NM=$(NM)" "RANLIB=$(RANLIB)" ./configure $(CONFIGURE_ARGS2)
	touch $(abs_builddir)/work/$(CONFIGURE_STAMP)

work/$(BUILD_STAMP): work/$(CONFIGURE_STAMP)
	cd $(abs_builddir)/work/internal/mpc-$(MPCVERSION) ; env "LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(MPLIBRARY_PATH)" ${MAKE}
	cd $(abs_builddir)/work/install/mpc-$(MPCVERSION) ; env "LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(MPLIBRARY_PATH)" ${MAKE}
	touch $(abs_builddir)/work/$(BUILD_STAMP)

work/$(CHECK_STAMP): work/$(BUILD_STAMP)
if IS_MINGW
	cd $(abs_builddir)/work/internal/mpc-$(MPCVERSION) ; env "LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(MPLIBRARY_PATH)" TESTS_ENVIRONMENT=wine ${MAKE} check
	cd $(abs_builddir)/work/install/mpc-$(MPCVERSION) ; env "LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(MPLIBRARY_PATH)" TESTS_ENVIRONMENT=wine ${MAKE} check
else
	cd $(abs_builddir)/work/internal/mpc-$(MPCVERSION) ; env "LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(MPLIBRARY_PATH)" ${MAKE} check
	cd $(abs_builddir)/work/install/mpc-$(MPCVERSION) ; env "LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(MPLIBRARY_PATH)" ${MAKE} check
endif
	touch $(abs_builddir)/work/$(CHECK_STAMP)

work/$(INSTALL_INTERNAL_STAMP): work/$(CHECK_STAMP)
	cd $(abs_builddir)/work/internal/mpc-$(MPCVERSION) ; ${MAKE} install
	touch $(abs_builddir)/work/$(INSTALL_INTERNAL_STAMP)

work/$(INSTALL_STAMP): work/$(CHECK_STAMP)
	cd $(abs_builddir)/work/install/mpc-$(MPCVERSION) ; ${MAKE} install
	touch $(abs_builddir)/work/$(INSTALL_STAMP)

extract: work/$(EXTRACT_STAMP)

patch: work/$(PATCH_STAMP)

config: work/$(CONFIGURE_STAMP)

build: work/$(BUILD_STAMP)

check: work/$(CHECK_STAMP)

install_internal: work/$(INSTALL_INTERNAL_STAMP)

install: work/$(INSTALL_STAMP)

distdir:

clean:
	rm -rf work

distclean:
	rm -rf work
