# WILL NOT BE PUBLISHED

OBJDIR := $(CUSTOM_BUILD_TYPE)-$(CUSTOM_PLATFORM)
LUNA_STAGING ?= $(HOME)/Local


# ------------------------------------------------------------------
# libserviceinstall.a
# ------------------------------------------------------------------

STATICLIB			= $(OBJDIR)/libserviceinstall.a

STATICLIB_OBJS		=
STATICLIB_OBJS 		+= $(OBJDIR)/serviceinstall.o


# ------------------------------------------------------------------
# tools
# ------------------------------------------------------------------

INSTALLER				= $(OBJDIR)/test-serviceinstaller

INSTALLER_OBJS			=
INSTALLER_OBJS			= $(OBJDIR)/test-serviceinstaller.o

INSTALLER_LIBS			=
INSTALLER_LIBS			+= -lserviceinstall
INSTALLER_LIBS			+= -lpbnjson_cpp
INSTALLER_LIBS			+= -lglib-2.0
INSTALLER_LIBS			+= -llunaservice
INSTALLER_LIBS			+= -lrolegen

INSTALLER_LDFLAGS	=
INSTALLER_LDFLAGS	+= -L$(OBJDIR)

# ------------------------------------------------------------------
# common
# ------------------------------------------------------------------

LOCAL_CFLAGS		+= -I$(INCLUDE_DIR)		
LOCAL_CFLAGS        += -I$(INCLUDE_DIR)/glib-2.0  		
LOCAL_CFLAGS		+= -Wall -fPIC -fvisibility=hidden -g

# ------------------------------------------------------------------
# build rules
# ------------------------------------------------------------------
all: setup $(STATICLIB) $(INSTALLER)

setup:
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CFLAGS) $(LOCAL_CFLAGS) -c $< -o $@

$(INSTALLER): $(INSTALLER_OBJS)
	$(CXX) $(INSTALLER_LDFLAGS) $(LDFLAGS) -o $(INSTALLER) $(INSTALLER_OBJS)  $(INSTALLER_LIBS)
	
$(STATICLIB) : $(STATICLIB_OBJS)
	$(AR) -rcs $(STATICLIB_LDFLAGS) $(STATICLIB) $(STATICLIB_OBJS)
	$(STRIP) --strip-unneeded $(STATICLIB)

	
clean:
	rm -rf $(OBJDIR)
	
install:
	mkdir -p $(LUNA_STAGING)/include
	mkdir -p $(LUNA_STAGING)/lib
	install -m 0644 serviceinstall.h $(LUNA_STAGING)/include
	install -m 0755 $(STATICLIB) $(LUNA_STAGING)/lib

stage:
	echo "nothing to do"
		
