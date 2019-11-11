
The flow of data

1) The ML determines its IP address and Mac.  It also grabs the current timestamp, the target image ids, the target image coordinants.
2) It posts this information to the system.

3) The system logs contact.   
4) The system determines if the device is in the database.
  a) It locates the school where the device is - and class..
  b) If it isn't in the Db, the user enters this info.
      As an alternative, we could edit this later


5) The system looks for other devices that have checked in last 30 minutes.
  a) it determines which devices are at the same school
  b) It determines if they have the same target images.
 
  c) If the target images are new in the last 30 minutes, create
  a new student group with this log as the key user.
  d) set a timestamp for the start of the group and the calculated time delay from the first contact to help map latency.  
  e) Return the target port range to listen to.

  e) If there is an existing student group with the sames images + school, join it.
  f) Return the IP addresses of the other group members and their target port range.
  g) Return the time stamp and latency data.
  h) Return the order that this unit checked into this student group.

6) The units receives the data.
  a) The unit sets the global clock using the timing data.
  
7) The unit opens up a port to listen for new members.
8) The unit sents an intro message with timing data, IP address, MacID, home port ID

