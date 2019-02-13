all: build 

build: ksender kreceiver

ksender: ksender.o link_emulator/lib.o utils.o protocol_kermit_sender.o
	gcc -g ksender.o link_emulator/lib.o utils.o protocol_kermit_sender.o -o ksender

kreceiver: kreceiver.o link_emulator/lib.o utils.o protocol_kermit_receiver.o
	gcc -g kreceiver.o link_emulator/lib.o utils.o protocol_kermit_receiver.o -o kreceiver

.c.o: 
	gcc -Wall -g -Werror -c $? 

clean:
	-rm -f *.o ksender kreceiver 
