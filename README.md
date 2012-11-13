BeagleBoardVNS
==============
Created By: Tom Atnip, Kevin Geisler, Ruffin White

BeagleBoard Bone Visitor Notification System (VNS)

This Project is a system utilizing the BeagleBone, Playstaion Eye.  The Goal was to design an application so that students would be able leave audio messages for professors when they are out of their office. Students will be able to walk up to the device and press a button to record their message which will then be available for the professor by email.
Overall we have a functioning project that consist of a C program, python script, bash script, and nodejs webserver. The C program controls how the audio is recorded. The python script controls sending emails with attachments. The bash script is in charge of moving the audio file, converting it to mp3, and firing the python script. The nodejs server controlls the preference file for the python script.

For more information, you can view the project webpage at http://elinux.org/ECE497_Beagle_VNS#Software