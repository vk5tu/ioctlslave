/* ioctlsave -- save current terminal settings in ioctl.save format.
 *
 * This is to fix a chicken-and-egg issue with non-9600bps bit rates
 * and Linux serial console.  No idea what the 'official' way to do
 * this is.
 *
 * Copyright © AARNet Pty Ltd (ACN 084 540 518).
 * Australian Academic & Research Network, http://www.aarnet.edu.au/
 * Licensed to the public under the GNU General Public License Version Two,
 * see http://www.gnu.org/licenses/gpl.html. AARNet members may have
 * additional license choices.
 *
 * Written by Glen Turner, glen.turner+howto@aarnet.edu.au.
 *
 * $Id$
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define PROGRAM_NAME "ioctlsave"

static const char *rcs_id="$Id$";

static void
show_version(FILE *output)
{
  fprintf(output,
          PROGRAM_NAME ":\n");
  fprintf(output,
          "Version: " "$Revision: 1.1 $" "\n");
  fprintf(output,
          "Revision control: %s\n",
          rcs_id);

#ifdef __GNUC__
  fprintf(output,
          "Compilation date: " __DATE__ " " __TIME__ "\n");
  fprintf(output,
          "Compiler version: " __VERSION__ "\n");
#endif

  exit(0);
}

static char *help_text[] = {
  PROGRAM_NAME ": Save terminal settings in same format init uses for "
    "/etc/ioctl.save\n",
  "Syntax:\n"
  "  " PROGRAM_NAME " [-t TERMINAL_DEVICE] [FILENAME]\n",
  "    Write current terminal settings to FILENAME\n",
  "      If FILENAME is omitted, stdout is used.\n"
  "      -t  Save settings for TERMINAL_DEVICE\n",
  "          Default is stdin.\n",
  "  " PROGRAM_NAME " -h\n",
  "    Show this help\n",
  "  " PROGRAM_NAME " -V\n",
  "    Show the version of this software for fault reporting\n",
  "Example:\n"
  "  Save serial console terminal settings for single-user mode\n",
  "    " PROGRAM_NAME " -t /dev/ttyS0 /etc/ioctl.save\n",
  NULL
};

static void
show_help(FILE *output)
{
  char **line;

  for (line = help_text;
       *line != NULL;
       line++)
  {
    fputs(*line, output);
  }
}

void
parse_arguments(int argc,
                char **argv,
                char **terminal_name,
                char **file_name)
{
  int option;
  
  *file_name = NULL;
  *terminal_name = NULL;

  while ((option = getopt(argc, argv, "Vht:")) != -1)
  {
    switch (option)
    {
      case 'V':
        show_version(stdout);
        exit(0);
        break;
       
       case 'h':
         show_help(stdout);
         exit(0);
         break;

       case 't':
         *terminal_name = optarg;
         break;
       
       default:
         if (isprint(optopt))
         {
           fprintf(stderr,
                   PROGRAM_NAME ": Unknown option -- %c.\n",
                   optopt);
         }
         else
         {
           fprintf(stderr,
                   PROGRAM_NAME ": Unknown option.\n");
         }
         show_help(stderr);
         exit(1);
    };
  };

  for (option = optind; option < argc; option++)
  {
    if (*file_name)
    {
      fputs(PROGRAM_NAME ": Too many file names, just one is needed.\n",
            stderr);
      show_help(stderr);
      exit(1);
    }
    else
    {
      *file_name = argv[optind];
    }
  }
}

void
get_settings(char *terminal_name,
             struct termios *terminal_settings_p)
{
  int terminal_fd;

  /* Open the terminal */
  if (terminal_name)
  {
    terminal_fd = open(terminal_name, O_RDWR|O_NOCTTY|O_NONBLOCK);
    if (terminal_fd == -1)
    {
      fputs(PROGRAM_NAME ": open(): ",
            stderr);
      perror(terminal_name);
      exit(1);
    }
  }
  else
  {
    terminal_name = "stdin";
    terminal_fd = fileno(stdin);
  }

  /* Get the terminal's settings. */
  if (tcgetattr(terminal_fd, terminal_settings_p))
  {
    fputs(PROGRAM_NAME ": tcgetattr(): ",
          stderr);
    perror(terminal_name);
    exit(1);
  }
  if (terminal_fd != fileno(stdin) && close(terminal_fd))
  {
    fputs(PROGRAM_NAME ": close(): ",
          stderr);
    perror(terminal_name);
    exit(1);
  }
}

void
write_settings(struct termios *terminal_settings_p,
               char *file_name)
{
  int file_fd;

  /* Write the settings to a file. */
  if (file_name == NULL || file_name == "-")
  {
    file_fd = fileno(stdout);
  }
  else
  {
    file_fd = open(file_name,
                   O_WRONLY|O_CREAT|O_TRUNC,
                   0600);
    if (file_fd == -1)
    {

      fputs(PROGRAM_NAME ": tcgetattr(): ",
            stderr);
      perror(file_name);
      exit(1);
    }
  }

  if (write(file_fd,
            terminal_settings_p,
            sizeof(struct termios)) == -1)
  {
      fputs(PROGRAM_NAME ": write(): ",
            stderr);
      perror(file_name);
      exit(1);
    
  }

  if (file_fd != fileno(stdout) && close(file_fd))
  {
      fputs(PROGRAM_NAME ": close(): ",
            stderr);
      perror(file_name);
      exit(1);
  }
}

int
main(int argc,
     char *argv[])
{
  char *terminal_name;
  char *file_name;
  struct termios terminal_settings;
  
  parse_arguments(argc, argv, &terminal_name, &file_name);
  get_settings(terminal_name, &terminal_settings);
  write_settings(&terminal_settings, file_name);

  return 0;
}
