// $Id: cppstrtok.cpp,v 1.7 2016-09-30 11:45:04-07 - - $

// Use cpp to scan a file and print line numbers.
// Print out each input line read in, then strtok it for
// tokens.

#include <string>
using namespace std;

#include <unistd.h>
#include "string_set.h" // angle brackets for system headers
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <iostream>
#include <cstdio>
#include "auxlib.h"

string_set strset;

const string CPP = "/usr/bin/cpp -nostdinc"; //2>errors.log
constexpr size_t LINESIZE = 1024;


// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
  size_t len = strlen (string);
  if (len == 0) return;
  char* nlpos = string + len - 1;
  if (*nlpos == delim) *nlpos = '\0';
}

// Print the meaning of a signal.
static void eprint_signal (const char* kind, int signal) {
  fprintf (stderr, ", %s %d", kind, signal);
  const char* sigstr = strsignal (signal);
  if (sigstr != NULL) fprintf (stderr, " %s", sigstr);
}

// Print the status returned from a subprocess.
void eprint_status (const char* command, int status) {
  if (status == 0) return; 
  fprintf (stderr, "%s: status 0x%04X", command, status);
  if (WIFEXITED (status)) {
    fprintf (stderr, ", exit %d", WEXITSTATUS (status));
  }
  if (WIFSIGNALED (status)) {
    eprint_signal ("Terminated", WTERMSIG (status));
      #ifdef WCOREDUMP
    if (WCOREDUMP (status)) fprintf (stderr, ", core dumped");
      #endif
  }
  if (WIFSTOPPED (status)) {
    eprint_signal ("Stopped", WSTOPSIG (status));
  }
  if (WIFCONTINUED (status)) {
    fprintf (stderr, ", Continued");
  }
  fprintf (stderr, "\n");
}


// Run cpp against the lines of the file.
void cpplines (FILE* pipe, const char* filename) {
  int linenr = 1;
  char inputname[LINESIZE];
  strcpy (inputname, filename);
  for (;;) {
    char buffer[LINESIZE];
    char* fgets_rc = fgets (buffer, LINESIZE, pipe);
    if (fgets_rc == NULL) break;
    chomp (buffer, '\n');
    //printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
    // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
    int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
			    &linenr, inputname);
    if (sscanf_rc == 2) {
      // printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, inputname);
      continue;
    }
    char* savepos = NULL;
    char* bufptr = buffer;
    for (int tokenct = 1;; ++tokenct) {
      char* token = strtok_r (bufptr, " \t\n", &savepos);
      bufptr = NULL;
      if (token == NULL) break;
      strset.intern(token); // inserts tokenized strings into string set
      
      
//printf ("token %d.%d: [%s]\n",
      //linenr, tokenct, token);
    }
    ++linenr;
  }
}

char allAT[] = "@";
int main (int argc, char** argv) {
  const char* execname = basename (argv[0]);
  int exit_status = EXIT_SUCCESS;
  int c;
  while ((c=getopt(argc,argv,"lyD@:")) !=-1){
    DEBUGF('D',"option %c %d",c,c); // option o calls the set_debugflags 
    switch(c){
    case 'D':
    case 'l':
    case 'y':
      break;
    case '@':
      if (strchr(optarg, '-')) {
	set_debugflags(allAT);
      }
      set_debugflags(optarg);
      DEBUGF('D', "calling set_debugflags %s\n",optarg);
      break;
    default:
      continue;
    }
  }


  for (int argi = optind; argi < argc; ++argi) {
    char* filename = argv[argi];
    string command = CPP + " " + filename;
    //printf ("command=\"%s\"\n", command.c_str());
    FILE* pipe = popen (command.c_str(), "r");
    DEBUGF('c', "compiling %s\n", argv[argi]); // option c compiles 
    if (pipe == NULL) {
      exit_status = EXIT_FAILURE;
      fprintf (stderr, "%s: %s: %s\n",
	       execname, command.c_str(), strerror (errno));
    }else {
      cpplines (pipe, filename);
      int pclose_rc = pclose (pipe);
      eprint_status (command.c_str(), pclose_rc);
      if (pclose_rc != 0){ // 0 means exit was successful
	exit_status = EXIT_FAILURE; // we shouldnt dump here, because its a fail
      }
      else{
	char first = argv[argc-1]+ "str";
	FILE *fp = freopen(first,"w",stdout); //
	  strset.dump(fp);
	fclose(fp);
      }
      
    }
  }
  return exit_status;
   
}
