APP=login
SF= ../common/Log.o ../common/timer.o ../common/packet_dump.o ../common/unix.o \
   ../common/Mutex.o ../common/MiscFunctions.o LoginDatabase.o LoginAccount.o \
   ../common/TCPConnection.o ../common/emu_opcodes.o \
   client.o net.o PacketHeaders.o LWorld.o ../common/md5.o ../common/dbcore.o \
   ../common/EQEMuError.o ../common/misc.o ../common/Crypto.o ../common/RC4.o \
   .obj/debug.o .obj/database.o .obj/EQStream.o ../common/xmlParser.o \
   .obj/EQStreamFactory.o .obj/EQPacket.o ../common/CRC16.o ../common/packet_functions.o \
   ../common/Condition.o ../common/opcodemgr.o ../common/PacketStruct.o ../common/ConfigReader.o

CC=g++
LINKER=gcc
DFLAGS=-DEQ2 -DLOGIN
WFLAGS=-Wall -Wuninitialized -Wwrite-strings -Wcast-qual  -Wcomment -Wcast-align -Wno-deprecated
COPTS=$(WFLAGS) -g -march=native -O -pthread -pipe -DFX -D_GNU_SOURCE -DINVERSEXY $(DFLAGS) -I/usr/include/mysql -I/usr/mysql/include
LINKOPTS=-rdynamic -L. -lstdc++ -lm -lz -L/usr/lib/mysql -L/usr/mysql/lib -lmysqlclient -ldl
all: $(APP)

$(APP): $(SF)
	$(LINKER) $(COPTS) $(OBJS) $^ $(LINKOPTS) -o $@

clean:
	rm -f $(SF) $(APP)

%.o: %.cpp
	$(CC) -c $(COPTS) $< -o $@

.obj/%.o: ../common/%.cpp ../common/%.h
	mkdir -p .obj
	$(CC) $(COPTS) -c $< -o $@
