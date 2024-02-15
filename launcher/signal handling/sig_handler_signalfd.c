//============================================================
// sig_handler_signalfd.c
//
// A much better but still imperfect example!
//
// example code from:
//      https://www.jmoisio.eu/en/blog/2020/04/20/handling-signals-correctly-in-a-linux-application/
//
// A system call is named signalfd() is available. It accepts
// a signal mask as an argument and gives you a file descriptor
// that can be polled, read and otherwise handled much like
// a regular file.
//
// Note that using signalfd to handle signals isn’t free from
// problems, but for this use case, it’s an elegant solution.
// See: https://ldpreload.com/blog/signalfd-is-useless
//
// Something is still missing…
//
// This example is better, but it explicitly allowes the server
// to finish serving the current client. Thanks to this lax
// attitude toward basic network security (what if the client
// does something stupid like hangs indefinitely?) I was
// able to block signals and only handle them at one point
// in the program.
//
// There was no need to deal with multiple threads, or child
// processes. Things like signal masks get hairy when there is.
// Much more hairy.
// Real servers need to handle issue 1 somehow.
//
// Some alternatives are:
//
// Introduce timeouts to blocking reads and writes. This
// ensures that the control gets back to poll() eventually.
// Don’t block signals. Let the blocking calls be interrupted,
// and check for the return value. And do that every time you
// invoke a blocking system call…
//
// Use non‐blocking IO. Return to the main loop as soon as
// there is nothing to read or write.
//============================================================
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main()
{
    int server_fd, socket_fd, signal_fd;
    struct pollfd pollfds[2];
    sigset_t sigset;
    struct signalfd_siginfo siginfo;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGTERM);
    sigprocmask(SIG_SETMASK, &sigset, NULL);

    server_fd = create_server();
    pollfds[0].fd = server_fd;
    pollfds[0].events = POLLIN;

    signal_fd = signalfd(-1, &sigset, 0);
    pollfds[1].fd = signal_fd;
    pollfds[1].events = POLLIN;

    while (1)
    {
        /* Poll the incoming events. The signals remain blocked. */
        pollfds[0].revents = 0;
        pollfds[1].revents = 0;
        if (poll(pollfds, 2, -1) < 0)
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

        /* Check if a signal was received. */
        if (pollfds[1].revents & POLLIN)
        {
            if (read(signal_fd, &siginfo, sizeof(siginfo)) != sizeof(siginfo))
            {
                handle_error("read siginfo");
            }
            break;
        }
    }

    fprintf(stderr, "Exiting via %s\n", strsignal(siginfo.ssi_signo));
    close(signal_fd);
    close(server_fd);
    return 0;
}
