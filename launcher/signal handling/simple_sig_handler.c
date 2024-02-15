//============================================================
// simple_sig_handler.c
//
// Naive but common example!
//
// example code from:
//      https://www.jmoisio.eu/en/blog/2020/04/20/handling-signals-correctly-in-a-linux-application/
//
// The signal() function is used to set the signal handler.
// It has various shortcomings: inconsistent behavior between
// different UNIXes, unsuitability when used in multi‐threaded
// programs, and lack of functionality used to fine‐tune the
// behavior of signal handling.
//
// There is a race condition in checking the signal flag.
// If a signal is delivered after the flag is read, but
// before the call to poll(), the program will not exit
// until it has served the next client.
//
// Pedantic note for those aiming at maximum portability:
// The flag should have type volatile sig_atomic_t to
// ensure that reads and writes are atomic. This would
// mainly be an issue in CPU architectures where loading
// an int value to a register is not atomic.
//============================================================
int signal_received = 0;

void handle_signal(int signum)
{
    signal_received = signum;
}

int main()
{
    int server_fd, socket_fd;
    struct pollfd pollfds[1];

    signal(SIGTERM, &handle_signal);

    server_fd = create_server();
    pollfds[0].fd = server_fd;
    pollfds[0].events = POLLIN;

    /* Check if a signal was received. */
    while (!signal_received)
    {
        /* But what if we receive signal here? */

        /* Poll the incoming events. This may be interrupted by a signal. */
        pollfds[0].revents = 0;
        if (poll(pollfds, 1, -1) < 0 && errno != EINTR)
        {
            handle_error("poll");
        }

        /* Handle an incoming connection. */
        if (pollfds[0].revents & POLLIN)
        {
            if ((socket_fd = accept(server_fd, NULL, NULL)) < 0)
            {
                handle_error("accept");
            }
            handle_connection(socket_fd);
            close(socket_fd);
        }
    }

    fprintf(stderr, "Exiting via %s\n", strsignal(signal_received));
    close(server_fd);
    return 0;
}
