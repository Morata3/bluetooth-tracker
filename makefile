SOURCE_DIR = src
SOURCE_DIR_INFO = $(SOURCE_DIR)/device_info
SOURCE_DIR_LIST = $(SOURCE_DIR)/info_list

TARGET_DIR = objs
TARGET_DIR_INFO = $(TARGET_DIR)/device_info
TARGET_DIR_LIST = $(TARGET_DIR)/info_list


CC = $(COMPILER_PATH)$(GCC_PREFIX)gcc
CFLAGS = -Wall -g `pkg-config --libs --cflags libbson-1.0` 
LDFLAGS = $(ADDITIONAL_LINKER_FLAGS) -Wl,--no-as-needed -g
SYSLIBS = -lpcap -lbson-1.0
DEPS = $(SOURCE_DIR_INFO)/bt_device_info.h
RM = rm -r

EXEC = bt_sniffer 
INFO_SOURCES = $(notdir $(wildcard $(SOURCE_DIR_INFO)/*.c))
LIST_SOURCES = $(notdir $(wildcard $(SOURCE_DIR_LIST)/*.c))


DIRECTORIES = $(TARGET_DIR).dir $(TARGET_DIR_INFO).dir $(TARGET_DIR_LIST).dir
INFO_OBJS = $(INFO_SOURCES:%.c=$(TARGET_DIR_INFO)/%.o)
LIST_OBJS = $(LIST_SOURCES:%.c=$(TARGET_DIR_LIST)/%.o)


$(TARGET_DIR) : $(DIRECTORIES) $(EXEC:%=$(TARGET_DIR)/%.o) $(EXEC:%=$(TARGET_DIR)/%)
$(TARGET_DIR_INFO) : $(INFO_OBJS)
$(TARGET_DIR_LIST) : $(LIST_OBJS)

$(TARGET_DIR)/% : $(TARGET_DIR)/%.o $(INFO_OBJS) $(LIST_OBJS)
	$(CC) $(LDFLAGS) -o $@ $@.o $(INFO_OBJS) $(LIST_OBJS) $(SYSLIBS)

$(TARGET_DIR)/%.o : $(SOURCE_DIR)/%.c $(DEPS)
	$(CC) $(CFLAGS)  -o $@ -c $< 

$(TARGET_DIR_INFO)/%.o : $(SOURCE_DIR_INFO)/%.c $(DEPS)
	$(CC) $(CFLAGS)  -o $@ -c $< 

$(TARGET_DIR_LIST)/%.o : $(SOURCE_DIR_LIST)/%.c $(DEPS)
	$(CC) $(CFLAGS)  -o $@ -c $< 

%.dir : 
	@echo "Checking directory $*"
	@if [ ! -d $* ]; then \
		echo "Making directory $*"; \
		mkdir -p $* ; \
	fi;

.PHONY : clean
clean :
	$(RM) $(TARGET_DIR)

