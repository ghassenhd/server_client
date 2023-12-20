build_dir = build
CFLAGS    = -lpthread

compile:
	#create build_dir
	mkdir $(build_dir)
	#compile client
	$(CC) client.c -o $(build_dir)/client
	#compile server and linking pthread library
	$(CC) server.c -o $(build_dir)/server $(CFLAGS)

clean:
	rm -rf build
