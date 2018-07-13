svr:
	g++ main.cpp mainctrl.cpp common/*.cpp -o svr 
clean:
	rm svr -rf