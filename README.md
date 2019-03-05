# cs3113sp19-project1
This project contains a single executable (oush). 
OUSH is an interactive bash interpreter. It supports 2 special symbols:
PIPE(analogous to bash input redirection) and BG, which runs a process in the background.
It accepts 1 character per line, plus one of these special symbols.

Additionally, it will support batch processing. It will execute a batchfile given as an argument via command line
(eg ./oush batch.oush)


#Resources
https://www.quora.com/What-are-macros-in-C
helped me understand C Preprocessors

The TLPI-DIST files, especially the fork-exec pattern section
The linux manpages
