//============================================================
// better_sig_handler.c
//
// Naive but common example!
//
// example code from:
//      https://www.jmoisio.eu/en/blog/2020/04/20/handling-signals-correctly-in-a-linux-application/
//
// Thiscode snippet sets the handler for SIGTERM just like
// before. It specified an empty signal mask and no flags. The
// signal mask controls which other signals are blocked when
// SIGTERM is being handled. The signal mask is for cases when
// the signal handler itself must not be interrupted by another
// signal. In this case, the signal handler sets a one‐way flag,
// so no need to set the mask. The flags control various
// aspects of signal handling, for example, whether or not
// system calls interrupted by the signal are automatically
// restarted.
//
// The race condition between checking the signal flag and
// polling for the incoming connections is solved by blocking
// the SIGTERM signal, and only unblocking it when we’re
// waiting for incoming connections.
//============================================================
int main()
{
    int server_fd, socket_fd;
    struct pollfd pollfds[1];
    sigset_t sigset;
    struct sigaction sa;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGTERM);
    sigprocmask(SIG_SETMASK, &sigset, NULL);

    sigemptyset(&sigset);
    sa.sa_handler = &handle_signal;
    sa.sa_mask = sigset;
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);

    server_fd = create_server();
    pollfds[0].fd = server_fd;
    pollfds[0].events = POLLIN;

    /* Check if a signal was received. */
    while (!signal_received)
    {
        /* Poll the incoming events. This may be interrupted by a signal. */
        pollfds[0].revents = 0;
        sigemptyset(&sigset);
        if (ppoll(pollfds, 1, NULL, &sigset) < 0 && errno != EINTR)
        {
            handle_error("ppoll");
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
