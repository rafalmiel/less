#include <setjmp.h>
#include "lesstest.h"

extern TermInfo terminfo;

int verbose = 0;
int explore = 0;
int less_quit = 0;
int details = 0;
char* details_file = NULL;
int err_only = 0;
char* lt_screen = "./lt_screen";
char* lt_screen_opts = NULL;

static char* testfile = NULL;

static int usage(void) {
	fprintf(stderr, "usage: lesstest -o file.lt [-w#] [-h#] [-eEdv] [-D detail-file] [-S lt_screen-opts] [--] less.exe [flags] textfile\n");
	fprintf(stderr, "   or: lesstest -t file.lt less.exe\n");
	fprintf(stderr, "   or: lesstest -x file.lt [-D detail-file]\n");
	return 0;
}

static int setup(int argc, char* const* argv) {
	char* logfile = NULL;
	int ch;
	while ((ch = getopt(argc, argv, "dD:eEo:s:S:t:vx:")) != -1) {
		switch (ch) {
		case 'd':
			details = 1;
			break;
		case 'D':
			details_file = optarg;
			break;
		case 'e':
			err_only = 1;
			break;
		case 'E':
			err_only = 2;
			break;
		case 'o':
			logfile = optarg;
			break;
		case 's':
			lt_screen = optarg;
			break;
		case 'S':
			lt_screen_opts = optarg;
			break;
		case 't':
			testfile = optarg;
			break;
		case 'v':
			verbose = 1;
			break;
		case 'x':
			explore = 1;
			testfile = optarg;
			break;
		default:
			return usage();
		}
	}
	if (logfile != NULL && !log_open(logfile)) {
		fprintf(stderr, "cannot create %s: %s\n", logfile, strerror(errno));
		return 0;
	}
	return 1;
}

int main(int argc, char* const* argv, char* const* envp) {
	if (!setup(argc, argv))
		return RUN_ERR;
	int ok = 0;
	if (testfile != NULL) { // run existing test
		if (explore) {
			if (optind != argc) {
				usage();
			} else {
				ok = explore_testfile(testfile);
			}
		} else {
			if (optind+1 != argc) {
				usage();
			} else {
				ok = run_testfile(testfile, argv[optind]);
			}
		}
	} else { // gen; create new test
		if (optind+2 > argc) {
			usage();
		} else {
			log_file_header();
			ok = run_interactive(argv+optind, argc-optind, envp);
			log_close();
		}
	}
	return ok ? RUN_OK : RUN_ERR;
}
