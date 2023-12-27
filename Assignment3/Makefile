compile : socket_server_p socket_server_t select poll epoll

socket_server_p : socket_server_p.c
	gcc -g -o socket_server_p socket_server_p.c

socket_server_t : socket_server_t.c
	gcc -g -o socket_server_t socket_server_t.c -lpthread

select : select.c
	gcc -g -o select select.c

poll : poll.c
	gcc -g -o poll poll.c

epoll : epoll.c
	gcc -g -o epoll epoll.c
