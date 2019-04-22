#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/inotify.h>

#define BUFSZ   16384

static void errexit(char *s) {
        fprintf(stderr, "%s\n", s);
        exit(1);
}

int main(void) {
        int ifd, wd, i, n;
        char buf[BUFSZ];

        ifd = inotify_init();
        if (ifd < 0)
                errexit("cannot obtain an inotify instance");

        wd = inotify_add_watch(ifd, ".", IN_MODIFY|IN_CREATE|IN_DELETE);
        if (wd < 0)
                errexit("cannot add inotify watch");

        while (1) {
                n = read(ifd, buf, sizeof(buf));
                if (n <= 0)
                        errexit("read problem");

                i = 0;
                while (i < n) {
                        struct inotify_event *ev;

                        ev = (struct inotify_event *) &buf[i];
                        if (ev->len)
                                printf("file %s %s\n", ev->name,
                                       (ev->mask & IN_CREATE) ? "created" :
                                       (ev->mask & IN_DELETE) ? "deleted" :
                                       "modified");
                        else
                                printf("unexpected event - wd=%d mask=%d\n",
                                       ev->wd, ev->mask);

                        i += sizeof(struct inotify_event) + ev->len;
                }
                printf("---\n");
        }
}
