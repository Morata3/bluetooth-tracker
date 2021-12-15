SOURCE_DIR = src
SOURCE_DIR_INFO = $(SOURCE_DIR)/device_info
SOURCE_DIR_LIST = $(SOURCE_DIR)/info_list
SOURCE_DIR_MQTT = $(SOURCE_DIR)/Mqtt

TARGET_DIR = objs
TARGET_DIR_INFO = $(TARGET_DIR)/device_info
TARGET_DIR_LIST = $(TARGET_DIR)/info_list
TARGET_DIR_MQTT = $(TARGET_DIR)/Mqtt


CC = $(COMPILER_PATH)$(GCC_PREFIX)gcc
CFLAGS = -Wall -g `pkg-config --libs --cflags libbson-1.0` 
LDFLAGS = $(ADDITIONAL_LINKER_FLAGS) -Wl,--no-as-needed -g
SYSLIBS = -lmosquitto -lpcap -lbson-1.0 -lpthread -lbluetooth
DEPS = $(SOURCE_DIR_INFO)/bt_device_info.h
RM = rm -r

EXEC_SNIFF = bt_sniffer 
EXEC_SCAN = bt_scanner
INFO_SOURCES = $(notdir $(wildcard $(SOURCE_DIR_INFO)/*.c))
LIST_SOURCES = $(notdir $(wildcard $(SOURCE_DIR_LIST)/*.c))
MQTT_SOURCES = $(notdir $(wildcard $(SOURCE_DIR_MQTT)/*.c))


DIRECTORIES = $(TARGET_DIR).dir $(TARGET_DIR_INFO).dir $(TARGET_DIR_LIST).dir $(TARGET_DIR_MQTT).dir
INFO_OBJS = $(INFO_SOURCES:%.c=$(TARGET_DIR_INFO)/%.o)
LIST_OBJS = $(LIST_SOURCES:%.c=$(TARGET_DIR_LIST)/%.o)
MQTT_OBJS = $(MQTT_SOURCES:%.c=$(TARGET_DIR_MQTT)/%.o)


$(TARGET_DIR) : $(DIRECTORIES) $(EXEC_SNIFF:%=$(TARGET_DIR)/%.o) $(EXEC_SNIFF:%=$(TARGET_DIR)/%) $(EXEC_SCAN:%=$(TARGET_DIR)/%.o) $(EXEC_SCAN:%=$(TARGET_DIR)/%)
$(TARGET_DIR_INFO) : $(INFO_OBJS)
$(TARGET_DIR_LIST) : $(LIST_OBJS)
$(TARGET_DIR_MQTT) : $(MQTT_OBJS)


$(TARGET_DIR)/% : $(TARGET_DIR)/%.o $(INFO_OBJS) $(LIST_OBJS) $(MQTT_OBJS)
	$(CC) $(LDFLAGS) -o $@ $@.o $(INFO_OBJS) $(LIST_OBJS) $(MQTT_OBJS) $(SYSLIBS) 

$(TARGET_DIR)/%.o : $(SOURCE_DIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ -c $< 

$(TARGET_DIR_INFO)/%.o : $(SOURCE_DIR_INFO)/%.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ -c $< 

$(TARGET_DIR_LIST)/%.o : $(SOURCE_DIR_LIST)/%.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ -c $< 

$(TARGET_DIR_MQTT)/%.o : $(SOURCE_DIR_MQTT)/%.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ -c $< 


%.dir : 
	@echo "Checking directory $*"
	@if [ ! -d $* ]; then \
		echo "Making directory $*"; \
		mkdir -p $* ; \
	fi;

.PHONY : clean
clean :
	$(RM) $(TARGET_DIR)
