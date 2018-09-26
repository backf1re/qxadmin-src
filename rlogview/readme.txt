[Copied from QxAdmin.txt]

=== Using rlogview/rlogviewd ===

o Setting-up the server(rlogviewd):
  
  I assume you have the rlogviewd executable (see above).

  Open 'rlogviewd.cfg'(included in this archive). There's 5 options:
  'password': password required to log in rlogviewd
  'port': TCP/IP port that rlogviewd listen to
  'quiet': if "1": reduced somewhat screen output
  'logfile': the actual file that rlogviewd will monitor for update
  'key': encoding key used to 'encrypt' data sent to clients (and to decode data
         coming from clients). See setting-up the client below.

o Setting-up the client(QxAdmin/rlogview)
  
  Start QxAdmin.exe. Click on the 'Show rlogview' button.
  Then click the 'Generate' button. Select the hash key and copy it to the clipboard.
  Paste it(or type it) in the rlogviewd.cfg on the server side.
  For example: If in the hash key edit box there is: "3F87A6B9", you must have a line
  in rlogviewd.cfg like this: key = "3F87A6B9"

  Setup the port, password edit-boxes with the same values as the ones in the
  rlogviewd.cfg file.

* After setting-up both the client and the server, start the server and if
  there's no error you can click the 'Connect' button on the client. You should
  see a message box saying that everything is allright.
